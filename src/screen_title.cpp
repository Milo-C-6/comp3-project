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
#include <iostream>
using namespace std;

// #define RAYGUI_IMPLEMENTATION
#include "raygui.h"

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;
static int selection = 1;
static int menu = 1; // 1: Main screen, 2: Play sub menu
static int buttonPressed = 0; // 1: Play game, 2: Editor, 3: Local, 4: Online
static Rectangle loadGameRec = {200, screenHeight/2.0f, 100, 50};
static Rectangle loadEditorRec = {400, screenHeight/2.0f, 100, 50};
static Rectangle backButtonRec = {screenWidth-100.0f, 0, 100, 50};
static Vector2 selectionPos = {260, (screenHeight/2.0f)-20};
//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
static void UpdateMainMenus(void); // Updates variables for before any selections with sub menus are chosen
static void DrawPlayerSelect(void); // Draws the player select screen, for adding players and setting their color
//----------------------------------------------------------------------------------
// Title Screen Functions Definition
//----------------------------------------------------------------------------------
// Title Screen Initialization logic
void InitTitleScreen(int imenu)
{
    framesCounter = 0;
    finishScreen = 0;
    buttonPressed = 0;
    menu = imenu;
}

// Title Screen Update logic
void UpdateTitleScreen(void)
{
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D) || CheckCollisionPointRec(GetMousePosition(), loadEditorRec))
    {
        selectionPos.x = 460;
        selection = 2;
    }
    else if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A) || CheckCollisionPointRec(GetMousePosition(), loadGameRec))
    {
        selectionPos.x = 260;
        selection = 1;
    }
    UpdateMainMenus();
    buttonPressed = 0;
}

// Title Screen Draw logic
void DrawTitleScreen(void)
{
    ClearBackground(LIME);
    
    if (menu == 3)
    {
        DrawPlayerSelect();
        return;
    }

    DrawTextEx(font, "COMP3-PROJECT", {20, 10}, font.baseSize*3.0f, 4, BLACK);
    DrawText("A multiplayer platforming and puzzle\n solving game by Milo and Trace!", 20, 50, 20, BLACK);
    DrawText("Created using Raylib", 20, 95, 20, BLACK);
    DrawTriangle(selectionPos, Vector2Subtract(selectionPos, Vector2 {20, 0}), Vector2Add(selectionPos, Vector2 {-10, 10}), BLACK);
    if (menu == 1)
    {
        if (GuiButton(loadGameRec, "START GAME")) buttonPressed = 1;
        else if (GuiButton({400, screenHeight/2.0f, 100, 50}, "STAGE EDITOR")) buttonPressed = 2;
    }
    else if (menu == 2)
    {
        if (GuiButton(loadGameRec, "LOCAL")) buttonPressed = 1;
        else if (GuiButton({400, screenHeight/2.0f, 100, 50}, "ONLINE")) buttonPressed = 2;

        if (GuiButton(backButtonRec, "BACK")) menu = 1;
    }
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

void UpdateMainMenus(void)
{
    // Press enter or tap to change to GAMEPLAY screen
    if (IsKeyPressed(KEY_ENTER))
    {
        if (selection == 1)
            goto firstButton;
        else if (selection == 2)
            goto secondButton; // gotos are fine, trustt
    }
    if (buttonPressed == 1)
    {
        firstButton:
        menu++;
        PlaySound(fxCoin);
    }
    else if (buttonPressed == 2)
    {
        secondButton:
        switch (menu)
        {
            case 1:
                finishScreen = 3;
                break;
            case 2:
                finishScreen = 2;
                break;
        }
    }
}

void DrawPlayerSelect(void)
{
    float bgHeight = screenHeight-screenHeight/8;
    float bgY = (screenHeight - bgHeight)/2;
    DrawRectangle(0, bgY, screenWidth, bgHeight, DARKGRAY); 
    for (int i = 0; i < 8; i++)
    {
        DrawRectangle(100+i%4*110, bgY+100+i/4*110, 100, 100, GRAY); // Height and width should be based on screen height and width
    }
}