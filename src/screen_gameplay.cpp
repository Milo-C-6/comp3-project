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
#include "screens.h"
#include "entities.hpp"
#include "map_parts.hpp"
#include <iostream>
#include <vector>

using namespace std;
//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static float screenWidth = 800; // TODO make this use the raylib_game.cpp screen sizes
static float screenHeight = 450;
static int framesCounter = 0;
static int finishScreen = 0; // maybe later remove?
static Player players[] = { Player(50, 50, KEY_A, KEY_D, KEY_W), Player(150, 50, KEY_LEFT, KEY_RIGHT, KEY_UP) };
static vector<MapPart> map;
static Camera2D camera = { 0 };

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------

void LoadLevel(void)
{
    // TODO: We need a proper file level format that we can use to load from, rather than hard coded levels.
    map = {
        MapPart(SLOPE, GOLD, vector<Vector2>{ {399, 119}, {399, 155}, {500, 155} }), // Slopes have to be drawn first, and slightly overlapped into other things
        MapPart(RECTANGLE, ORANGE, vector<Vector2>{ {0, 150}, {200, 50} }),
        MapPart(RECTANGLE, ORANGE, vector<Vector2>{ {200, 120}, {200, 50} }),
        MapPart(RECTANGLE, ORANGE, vector<Vector2>{ {400, 150}, {900, 50} }),
        MapPart(RECTANGLE, ORANGE, vector<Vector2>{ {400, 0}, {900, 50} })
    };
}

// Gameplay Screen Initialization logic
void InitGameplayScreen(void)
{
    framesCounter = 0;
    finishScreen = 0;
    // Camera
    camera.target =  players[0].pos;
    camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    LoadLevel();
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
    // Update camera
    Vector2 minV2 = camera.target;
    Vector2 maxV2 = camera.target;

    for (Player &plr : players)
    {   
        minV2 = Vector2Min(minV2, Vector2AddValue(plr.pos, -10));
        maxV2 = Vector2Max(maxV2, Vector2AddValue(plr.pos, 35));
        // Update players
        plr.CheckControls();

        // Handle collision
        // for (Player plr2 : players)
        // {
        //     if (plr == plr2)
        //         continue;
        //     plr.checkCollision()
        // }
        for (MapPart part : map)
            plr.CheckCollision(part);
        plr.UpdatePosition();
    }

    // Make sure camera encompasses all players
    Vector2 widthV2 = Vector2Subtract(maxV2, minV2);
    Vector2 centerV2 = Vector2Add(minV2, Vector2Scale(widthV2, 0.5));
    camera.target = centerV2;

    Vector2 zoomScaleV2 = Vector2Divide({screenWidth, screenHeight}, Vector2Max(widthV2, {screenWidth, screenHeight})); // Maps will probably have a max, so the Vector2Min will eventually be replaced with Vector2Clamp
    camera.zoom = min(zoomScaleV2.x, zoomScaleV2.y);
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
    // Draw background
    ClearBackground(RAYWHITE);

    BeginMode2D(camera);

        // Draw map
        for (MapPart part : map) 
            if (part.partType == RECTANGLE)
                DrawRectangle(part.points[0].x, part.points[0].y, part.points[1].x, part.points[1].y, part.color);
            else if (part.partType == SLOPE)
                DrawTriangle(part.points[0], part.points[1], part.points[2], part.color);

        // Draw players
        for (Player plr : players) 
            DrawRectangle(plr.pos.x, plr.pos.y, 25, 25, BLUE);

    EndMode2D();
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void)
{
    // TODO: Unload GAMEPLAY screen variables here!
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
    return finishScreen;
}