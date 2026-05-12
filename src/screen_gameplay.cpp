/**********************************************************************************************
*
*   Copyright (C) 2026 Milo Cummings
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <https://www.gnu.org/licenses/>.
*
**********************************************************************************************/

#include "raylib.h"
#include "raymath.h"
#include "screens.h"
#include "entities.hpp"
#include "map_parts.hpp"
#include "game_map.hpp"
#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>

using namespace std;

//----------------------------------------------------------------------------------
// Shared Variables Definition (global)
// NOTE: Those variables are shared between modules through screens.h
//----------------------------------------------------------------------------------
bool plrKeyPressed = false;
static int LastKeyPressed = 0;
// int screenWidth;
// int screenHeight;
// bool hasScreenResized;

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0; // maybe later remove?
static vector<Player> players = { Player(50, 50, 0), Player(50, 80, 1) };
static GameMap gameMap;
static Camera2D camera = { 0 };
static float dropInAlpha = 0;
static int dropInHintI = 1;
static bool switchHint = true;
static int plrsInWin = 0;
static string world = "base";
static int lvl = 0;

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
// static void LoadLevel(void);  // Create the variables for the gameMap
static void StartLevel(void); // Starts level based on lvl and world variables
// void RestartLevel(GameMap gMap, vector<Player> *plrs);  // Move players to spawn of level, and reset any moving parts back to original place.

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------

// Gameplay Screen Initialization logic
void InitGameplayScreen(int lvl, string world)
{
    framesCounter = 0;
    finishScreen = 0;
    // Camera
    camera.target =  players[0].pos;
    // camera.offset = (Vector2){ static_cast<float>(screenWidth)/2.0f, static_cast<float>(screenHeight)/2.0f };
    camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    dropInAlpha = 0;
    ::lvl = lvl;
    ::world = world;
    StartLevel();
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
    UpdateLevel(&gameMap, &players, &camera);
    // Check win
    if (plrsInWin >= players.size())
    {
        lvl++;
        StartLevel();
    }
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
    BeginMode2D(camera);

        DrawMap(gameMap);

        // Draw players
        for (Player plr : players) 
            DrawRectangle(plr.pos.x, plr.pos.y, 25, 25, BLUE);

    EndMode2D();

    DrawGameplayUi();
}
// Gameplay Screen Unload logic
void UnloadGameplayScreen(void)
{
    // TODO: Unload GAMEPLAY screen variables here!
    // what just delete everything?
    // probably just set gamemap, players, and camera to nothing.
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
    return finishScreen;
}

// void LoadLevel(void)
// {
//     // TODO: We need a proper file level format that we can use to load from, rather than hard coded levels.
//     gameMap.title = "0: Debug";
//     gameMap.description = "For debbugging the game";
//     gameMap.author = "Milo";
//     gameMap.spawn = Vector2{50, 50};
//     gameMap.levelSize = Vector2{2500, 500};

//     gameMap.mapParts = {
//         MapPart(SLOPE, GOLD, vector<Vector2>{ {399, 119}, {399, 155}, {500, 155} }), // Slopes have to be drawn first, and slightly overlapped into other things
//         MapPart(RECTANGLE, ORANGE, vector<Vector2>{ {0, 150}, {200, 50} }),
//         MapPart(RECTANGLE, ORANGE, vector<Vector2>{ {200, 120}, {200, 50} }),
//         MapPart(RECTANGLE, ORANGE, vector<Vector2>{ {400, 150}, {500, 50} }),
//         MapPart(RECTANGLE, ORANGE, vector<Vector2>{ {400, 0}, {500, 50} }),
//         MapPart(RECTANGLE, RED, vector<Vector2>{ {300, 0}, {50, 50} }, unordered_map<PartAttributes, int>{ {KILL, 1} }),
//         MapPart(RECTANGLE, ORANGE, vector<Vector2>{ {-250, 200}, {200, 50} }),
//         MapPart(RECTANGLE, ORANGE, vector<Vector2>{ {-50, 150}, {25, 50} }),
//         MapPart(RECTANGLE, BLACK, vector<Vector2>{ {-85, 150}, {25, 50} }, unordered_map<PartAttributes, int>{ {LAUNCHER, -5} }),
//         MapPart(RECTANGLE, LIME, vector<Vector2>{ {-120, 150}, {25, 50} }, unordered_map<PartAttributes, int>{ {BOUNCY, 1} }),
//         MapPart(RECTANGLE, VIOLET, vector<Vector2>{ {-155, 180}, {25, 50} }, unordered_map<PartAttributes, int>{ {MOVING, 1} })
//     };

//     gameMap.mapParts[10].formulaY = "180+20*sin(t)";
// }

void UpdateLevel(GameMap *gMap, vector<Player> *plrs, Camera2D *cam2d)
{
    // Reset values
    plrKeyPressed = false;
    plrsInWin = 0;
    // Update camera
    Vector2 minV2 = cam2d->target;
    Vector2 maxV2 = cam2d->target;

    for (Player &plr : *plrs)
    {   
        // Check if they're out of bounds
        if (!CheckCollisionPointRec(plr.pos, {-gMap->levelSize.x/2, -gMap->levelSize.y/2, gMap->levelSize.x, gMap->levelSize.y}))
        {
            RestartLevel(gMap, plrs);
            return;
        }

        minV2 = Vector2Min(minV2, Vector2AddValue(plr.pos, -10));
        maxV2 = Vector2Max(maxV2, Vector2AddValue(plr.pos, 35));
        // Update players
        plr.CheckControls();

        // Handle collision
            // Player collision
        for (Player plr2 : *plrs)
        {
            if (plr.id == plr2.id)
                continue;
            plr.CheckCollision(MapPart(RECTANGLE, BLACK, vector<Vector2>{plr2.pos, {25, 25}}));
        }
        // Map collision
        int counter = 0;
        for (MapPart part : gMap->mapParts)
        {
            if (plr.CheckCollision(part))
            {
                if (part.attributes.count(KILL) && part.attributes.at(KILL) > 0)
                    RestartLevel(gMap, plrs);
                else if (part.attributes.count(WIN) && part.attributes.at(WIN) > 0)
                    plrsInWin++;
                else if (part.partType == MP_KEY)
                {
                    for (MapPart &part2 : gMap->mapParts)
                        if (part2.attributes.count(LOCK) > 0 && part2.attributes.at(LOCK) == part.attributes.at(LOCK))
                        {
                            part2.attributes.at(LOCK) = -1;
                            part2.color = Fade(part2.color, 0.5);
                        }

                    gMap->mapParts.erase(gMap->mapParts.begin() + counter);
                }
            } 
            counter++;
        }
        plr.UpdatePosition();
    }

    // Make sure camera encompasses all players
    Vector2 widthV2 = Vector2Subtract(maxV2, minV2);
    Vector2 centerV2 = Vector2Add(minV2, Vector2Scale(widthV2, 0.5));
    cam2d->target = centerV2;

    Vector2 zoomScaleV2 = Vector2Divide({static_cast<float>(screenWidth), static_cast<float>(screenHeight)}, Vector2Max(widthV2, {static_cast<float>(screenWidth), static_cast<float>(screenHeight)})); // Maps will probably have a max, so the Vector2Min will eventually be replaced with Vector2Clamp
    cam2d->zoom = min(zoomScaleV2.x, zoomScaleV2.y);

    // Update level
    for (MapPart &mapPart : gMap->mapParts)
        mapPart.ExecuteFormulas(plrs->size());

    // Check other players keyboard keys to add a player
    for (int i=1;i<8;i++)
    {
        for (Player plr2 : *plrs)
            if (i == plr2.id)
                goto controlsContiue; // Skip players already in the game

        for (int i2=0;i2<3;i2++)
        {
            if (IsKeyPressed(controls[i][i2]))
            {
                plrs->push_back(Player(gMap->spawn.x, gMap->spawn.y, i));
                break;
            }
        }
        controlsContiue: // so you can double continue
    }
    // Reset last pressed key
    int keyPressed = GetKeyPressed();
    if (!plrKeyPressed && keyPressed != 0)
    {
        LastKeyPressed = keyPressed;
    }
    else if (plrKeyPressed)
        LastKeyPressed = 0;

    if (LastKeyPressed != 0)
    {
        dropInAlpha = sin(4*GetTime())/2+0.5;
        if (switchHint && dropInAlpha < 0.1)
        {
            if (dropInHintI < 7)
                dropInHintI += 1;
            else
                dropInHintI = 1;
            
            switchHint = false;
        }
        else if (dropInAlpha > 0.9)
            switchHint = true;
    }
}

void RestartLevel(GameMap *gMap, vector<Player> *plrs)
{
    int i = 0;
    for (Player &plr : *plrs)
    {
        plr.pos = Vector2Add(gMap->spawn, {float(i/4*30), float(-i%4*30)} );
        plr.vel = Vector2Zero();
        i++;
    }
}

void DrawMap(GameMap gMap)
{
    for (MapPart part : gMap.mapParts)
    {
        switch (part.partType)
        {
            case RECTANGLE: // make a switch statement stupid
                DrawRectangle(part.points[0].x, part.points[0].y, part.points[1].x, part.points[1].y, part.color);
                break;
            case SLOPE:
                DrawTriangle(part.points[0], part.points[1], part.points[2], part.color);
                break;
            case MP_TEXT:
                DrawText(part.label.c_str(), part.points[0].x, part.points[0].y, 16, part.color);
                break;
            case MP_KEY:
                DrawTexture(txKey, part.points.at(0).x, part.points.at(0).y, part.color);
                break;
        } 
    }
}
void DrawGameplayUi(void)
{
    if (LastKeyPressed != 0)
    {
        string label = "Press ";
        if (dropInHintI == 1) // GetKeyName(KEY_LEFT) returns null for some reason, atleast on linux
            label.append("Left arrow");
        else
            label.append(GetKeyName(controls[dropInHintI][0]));
        label.append(" to drop in player");
        DrawText(label.c_str(), 250, screenHeight-32, 16, Fade(BLACK, dropInAlpha));
    }
}
void StartLevel(void)
{
    ifstream inputGameMapFile("resources/levels/" + world + to_string(lvl) + ".cm");

    if (!inputGameMapFile.good())
    {
        finishScreen = 2;
        return;
    }

    inputGameMapFile >> gameMap;
    inputGameMapFile.close();

    gameMap.mapParts.push_back(MapPart(RECTANGLE, BLACK, vector<Vector2>{{0, 120}, {20, 20}}, unordered_map<PartAttributes, int>{{LOCK, 1}}));
    gameMap.mapParts.push_back(MapPart(MP_KEY, WHITE, vector<Vector2>{{80, 120}}, unordered_map<PartAttributes, int>{{LOCK, 1}}));
    RestartLevel(&gameMap, &players);
}