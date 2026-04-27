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
#include "game_map.hpp"
#include "map_parts.hpp"
#include <iostream>
#include <vector>
using namespace std;

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

//----------------------------------------------------------------------------------
// Shared Variables Definition (global)
// NOTE: Those variables are shared between modules through screens.h
//----------------------------------------------------------------------------------
// int screenWidth;
// int screenHeight;
// bool hasScreenResized;

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;
static GameMap gameMap;

// Actual level editing
static int editScroll;
static int editMode;
static int partScroll;
static int partIndex;
static Camera2D camera = { 0 };
// GUIS
static int configStatus;
static bool showInfo;
static Rectangle messageBoxRect;
// Config
static char titleInput[64];
static char descriptionInput[512];
static char authorInput[64];
static int panelInputEditIndex;
static int levelWidth;
static int levelHeight;
static Vector2 mouseDistance;
static int draggingBox;


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
    editScroll = 0;
    editMode = 0;
    partScroll = 0;
    partIndex = 0;
    configStatus = 0;
    showInfo = false;
    messageBoxRect = Rectangle{50, 50, 400, 270};
    titleInput[64] = { 0 };
    authorInput[64] = { 0 };
    panelInputEditIndex = 0;
    draggingBox = 0;

    GuiSetStyle(LISTVIEW, SCROLLBAR_WIDTH, 8);
    GuiSetStyle(VALUEBOX, TEXT_PADDING, 5);
}

void UpdateEditorScreen(void)
{
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) 
    {
        // Check textbox buttons
        if (configStatus && CheckCollisionPointRec(GetMousePosition(), messageBoxRect))
        {   
            // For dragging the config box 
            if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){messageBoxRect.x, messageBoxRect.y, 375, 25}))
            {
                mouseDistance = Vector2Subtract(GetMousePosition(), (Vector2){messageBoxRect.x, messageBoxRect.y});
                draggingBox = 1;
            } 
            // Check keyboard input
            if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){messageBoxRect.x+35, messageBoxRect.y+100, 340, 25})) panelInputEditIndex = 2;
            if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){messageBoxRect.x+55, messageBoxRect.y+50, 320, 25})) panelInputEditIndex = 1;
            if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){messageBoxRect.x+75, messageBoxRect.y+130, 300, 25})) panelInputEditIndex = 3;
            if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){messageBoxRect.x+117, messageBoxRect.y+165, 146, 25})) panelInputEditIndex = 4;
            if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){messageBoxRect.x+117, messageBoxRect.y+195, 146, 25})) panelInputEditIndex = 5;
        }
    }
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        // Allow dragging the config box
        if (draggingBox == 1)
        {
            Vector2 newPos = Vector2Subtract(GetMousePosition(), mouseDistance);
            messageBoxRect.x = newPos.x;
            messageBoxRect.y = newPos.y;
        }
    }
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
    {
        draggingBox = 0;

        // Acutal level edits should only happen if we're pressing in the level area
        if (!CheckCollisionPointRec(GetMousePosition(), (Rectangle){0, 0, float(screenWidth), 125})
            && !configStatus)
        {
            switch(editMode)
            {
                case 0: // Place mode
                    switch(partIndex)
                    {
                        case 0: // Rectangle
                            gameMap.mapParts.push_back(MapPart(RECTANGLE, ORANGE, vector<Vector2>{GetMousePosition(), (Vector2){50, 50}}));
                            break;
                        case 1: // Slope
                            gameMap.mapParts.insert(gameMap.mapParts.begin(), MapPart(SLOPE, ORANGE, vector<Vector2>{GetMousePosition(), Vector2Add(GetMousePosition(), (Vector2){0, 50}), Vector2Add(GetMousePosition(), (Vector2){50, 50})}));
                            break;
                        case 2: // Spawn
                            gameMap.spawn = GetMousePosition();
                            break;
                        default: // idfk what to do here
                            break;
                    }
                case 1: // Select mode
                    break;
                case 2: // Move mode
                    break;
                case 3: // Copy mode
                    break;
                case 4: // Delete mode
                    break;
                default: // i really dfk what to do here
                    break;
            }
            
        }
    }
    // Apply the config
    if (configStatus == 1)
    {
        gameMap.title = titleInput;
        gameMap.description = descriptionInput;
        gameMap.author = authorInput;
        gameMap.size.x = levelWidth;
        gameMap.size.y = levelHeight;
    }
}

void DrawEditorScreen(void)
{
    DrawMap(gameMap);
    for (int i = 0; i < 8; i++)
        DrawRectangle(gameMap.spawn.x+i/4*30, gameMap.spawn.y-i%4*30, 25, 25, Fade(BLUE, 0.6));
    DrawText("Spawn", gameMap.spawn.x, gameMap.spawn.y-120, 16, BLACK);

    if (editMode == 0)
    {
        // Draw hologram block place
        switch(partIndex)
        {
            case 0: // Rectangle
                DrawRectangle(GetMouseX(), GetMouseY(), 50, 50, Fade(ORANGE, 0.1));
                break;
            case 1: // Slope
                DrawTriangle(GetMousePosition(), Vector2Add(GetMousePosition(), (Vector2){0, 50}), Vector2Add(GetMousePosition(), (Vector2){50, 50}), Fade(GOLD, 0.1));
                break;
            case 2: // Spawn
                for (int i = 0; i < 8; i++)
                    DrawRectangle(GetMouseX()+i/4*30, GetMouseY()-i%4*30, 25, 25, Fade(BLUE, 0.1));
                DrawText("Spawn", GetMouseX(), GetMouseY()-120, 16, Fade(BLACK, 0.1));
                break;
            default: // idfk what to do here
                break;
        }
    }

    // Draw GUI over map
    GuiListView((Rectangle){0,0,150,125}, "Place;Select;Move;Copy;Delete", &editScroll, &editMode);
    
    GuiListView((Rectangle){150, 0, float(screenWidth)-200, 125}, "Rectangle;Slope;Spawn", &partScroll, &partIndex);

    if (GuiButton((Rectangle){float(screenWidth)-50, 0, 50, 62}, "Config")) configStatus = 1;
    if (GuiButton((Rectangle){float(screenWidth)-50, 63, 50, 62}, "Info")) showInfo = true;

    if (configStatus)
    {
        configStatus = GuiMessageBox(messageBoxRect, "Configuration", "", "Apply");
        
        GuiLabel((Rectangle){messageBoxRect.x+25, messageBoxRect.y+50, 50, 25}, "Title:");
        GuiTextBox((Rectangle){messageBoxRect.x+55, messageBoxRect.y+50, 320, 25}, titleInput, 64, panelInputEditIndex == 1);
        
        GuiLabel((Rectangle){messageBoxRect.x+25, messageBoxRect.y+80, 100, 25}, "Description:");
        GuiTextBox((Rectangle){messageBoxRect.x+35, messageBoxRect.y+100, 340, 25}, descriptionInput, 512, panelInputEditIndex == 2);
        
        GuiLabel((Rectangle){messageBoxRect.x+25, messageBoxRect.y+130, 50, 25}, "Author:");
        GuiTextBox((Rectangle){messageBoxRect.x+75, messageBoxRect.y+130, 300, 25}, authorInput, 64, panelInputEditIndex == 3);
        
        GuiLabel((Rectangle){messageBoxRect.x+25, messageBoxRect.y+162, 50, 25}, "Size:");
        GuiSpinner((Rectangle){messageBoxRect.x+90, messageBoxRect.y+165, 200, 25}, "Width", &levelWidth, 500, 500000, panelInputEditIndex == 4);
        GuiSpinner((Rectangle){messageBoxRect.x+90, messageBoxRect.y+195, 200, 25}, "Height", &levelHeight, 500, 500000, panelInputEditIndex == 5);
    }

}
void UnloadEditorScreen(void)
{

}
int FinishEditorScreen(void)
{
    return finishScreen;
}