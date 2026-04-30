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

// #define RAYGUI_IMPLEMENTATION
#include "raygui.h"

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;
static int selection = 1;
bool loadGame = false;
bool loadEditor = false;
static Rectangle loadGameRec = {200, screenHeight/2.0f, 100, 50};
static Rectangle loadEditorRec = {400, screenHeight/2.0f, 100, 50};
static Vector2 selectionPos = {260, (screenHeight/2.0f)-20};
//----------------------------------------------------------------------------------
// Title Screen Functions Definition
//----------------------------------------------------------------------------------

// Title Screen Initialization logic
void InitTitleScreen(void)
{
    // TODO: Initialize TITLE screen variables here!
    framesCounter = 0;
    finishScreen = 0;
}

// Title Screen Update logic
void UpdateTitleScreen(void)
{
    // TODO: Update TITLE screen variables here!
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D) || CheckCollisionPointRec(GetMousePosition(), loadEditorRec))
    {
        selectionPos.x = 460;
        selection = 2; //Editor
    }
    else if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A) || CheckCollisionPointRec(GetMousePosition(), loadGameRec))
    {
        selectionPos.x = 260;
        selection = 1; //Gameplay
    }
    // Press enter or tap to change to GAMEPLAY screen
    if (IsKeyPressed(KEY_ENTER))
    {
        if (selection == 1)
        {
            finishScreen = 2;
        }
        else if (selection == 2)
        {
            finishScreen = 3;
        }
    }
    if (loadGame)
    {
        //finishScreen = 1;   // OPTIONS
        finishScreen = 2;   // GAMEPLAY
        PlaySound(fxCoin);
    }
    else if (loadEditor)
    {
        finishScreen = 3;
    }
}

// Title Screen Draw logic
void DrawTitleScreen(void)
{
    // TODO: Draw TITLE screen here!
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), GREEN);
    Vector2 pos = { 20, 10 };
    DrawTextEx(font, "COMP3-PROJECT", pos, font.baseSize*3.0f, 4, BLACK);
    DrawText("A multiplayer platforming and puzzle\n solving game by Milo and Trace!", 20, 50, 20, BLACK);
    DrawText("Created using Raylib", 20, 95, 20, BLACK);
    // DrawTriangle(Vector2 {260, (screenHeight/2.0f)-20}, Vector2 {240, (screenHeight/2.0f)-20}, Vector2 {250, (screenHeight/2.0f)-10}, BLACK);
    DrawTriangle(selectionPos, Vector2Subtract(selectionPos, Vector2 {20, 0}), Vector2Add(selectionPos, Vector2 {-10, 10}), BLACK);
    loadGame = GuiButton(loadGameRec, "START GAME");
    loadEditor = GuiButton({400, screenHeight/2.0f, 100, 50}, "STAGE EDITOR");
}

// Title Screen Unload logic
void UnloadTitleScreen(void)
{
    // TODO: Unload TITLE screen variables here!
}

// Title Screen should finish?
int FinishTitleScreen(void)
{
    return finishScreen;
}