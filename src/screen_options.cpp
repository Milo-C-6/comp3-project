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

// #define RAYGUI_IMPLEMENTATION
#include "raygui.h"

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;
static Rectangle backButtonRec = {screenWidth-100.0f, 0, 100, 50};
static Rectangle controlsRec = {0, 20, 120, 40};
bool windowControlsActive = false;
//----------------------------------------------------------------------------------
// Options Screen Functions Definition
//----------------------------------------------------------------------------------

// Options Screen Initialization logic
void InitOptionsScreen(void)
{
    // TODO: Initialize OPTIONS screen variables here!
    framesCounter = 0;
    finishScreen = 0;
}

// Options Screen Update logic
void UpdateOptionsScreen(void)
{
    // TODO: Update OPTIONS screen variables here!
}

// Options Screen Draw logic
void DrawOptionsScreen(void)
{
    // TODO: Draw OPTIONS screen here!
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), LIGHTGRAY);
    DrawText("OPTIONS:", 0, 0, 20, BLACK);
    if (GuiButton(backButtonRec, "BACK"));

    if (GuiButton(controlsRec, "EDIT CONTROLS")) windowControlsActive = true;

    if (windowControlsActive)
    {
        if (GuiWindowBox({0, controlsRec.y+40, 120, 93}, "CONTROLS:")) windowControlsActive = false;

        if (GuiButton((Rectangle){0, controlsRec.y+63, 120, 24 }, "MOVE LEFT"));
        {
            GuiTextBox((Rectangle){screenWidth/2.0f, screenHeight/2.0f, 100, 100}, "W", 50, true);
        } 

        GuiButton((Rectangle){0, controlsRec.y+86, 120, 24 }, "MOVE RIGHT"); 
        GuiButton((Rectangle){0, controlsRec.y+109, 120, 24 }, "JUMP"); 
    }
    
}

// Options Screen Unload logic
void UnloadOptionsScreen(void)
{
    // TODO: Unload OPTIONS screen variables here!
}

// Options Screen should finish?
int FinishOptionsScreen(void)
{
    return finishScreen;
}

void SetControls(void)
{
    KeyboardKey defaultControls[8][3] = { 
        {KEY_Q, KEY_E, KEY_W}, // Each {} is a different player slot, 0: Left 1: Right 2: Jump
        {KEY_LEFT, KEY_RIGHT, KEY_UP},
        {KEY_R, KEY_Y, KEY_T},
        {KEY_COMMA, KEY_SLASH, KEY_PERIOD},
        {KEY_U, KEY_O, KEY_I},
        {KEY_KP_4, KEY_KP_6, KEY_KP_8},
        {KEY_Z, KEY_C, KEY_X},
        {KEY_LEFT_BRACKET, KEY_BACKSLASH, KEY_EQUAL}
    };
    for (int i=0;i<8;i++)
    {
        for (int i2=0;i<3;i++)
        {
            controls[i][i2] = defaultControls[i][i2];
        }
    }
}