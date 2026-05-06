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
        GuiButton((Rectangle){0, controlsRec.y+63, 120, 24 }, "MOVE LEFT"); 
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