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
#include <iostream>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;
static Rectangle panelRec;
static Rectangle panelContentRec;
static Rectangle panelView;
static Vector2 panelScroll;
//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
static void DrawStyleEditControls(void);  // Draw and process scroll bar style edition controls

//----------------------------------------------------------------------------------
// Ending Screen Functions Definition
//----------------------------------------------------------------------------------

void InitEditorScreen(void)
{
    framesCounter = 0;
    finishScreen = 0;
    panelRec = { 0, 0, static_cast<float>(GetScreenWidth()), 150 };
    panelContentRec = {0, 0, 1000, 140 };
    panelView = { 0 };
    panelScroll = { 0, 0 };
    GuiSetStyle(LISTVIEW, SCROLLBAR_WIDTH, 8);
}
void UpdateEditorScreen(void)
{

}
void DrawEditorScreen(void)
{
    // DrawText(TextFormat("[%f, %f]", panelScroll.x, panelScroll.y), 200, 200, 20, RED);

    GuiScrollPanel(panelRec, NULL, panelContentRec, &panelScroll, &panelView);

    BeginScissorMode(panelView.x, panelView.y, panelView.width, panelView.height);
        DrawRectangleGradientH(panelRec.x + panelScroll.x, panelRec.y + panelScroll.y, 1000, 141, RED, BLUE);
    EndScissorMode();

}
void UnloadEditorScreen(void)
{

}
int FinishEditorScreen(void)
{
    return finishScreen;
}