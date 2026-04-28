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
static int selectedPart;
static Camera2D camera = { 0 };
// GUIS
static int configStatus;
static bool showInfo;
static Rectangle messageBoxRect;
static Rectangle partInfoRect;
static int panelInputEditIndex; // 1-5: Config inputs, 6-11: Part info inputs
// Part Info
static int cPartValues[6];
// Config
static char titleInput[64];
static char descriptionInput[512];
static char authorInput[64];
static int levelWidth;
static int levelHeight;
static Vector2 mouseDistance;
static int draggingBox; // 1 = Config box, 2 = Part info box


//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
static int PlacePart(int partI, GameMap *gMap); // Insert a block into the map parts, returns selected part.
static void DrawGui(void); // guess what it does :)
static int getClickedPart(GameMap gMap); // Gets the part clicked by mouse
static void CheckEditorWindows(void); // Make dragging/keyboard input for windows work
static bool isMouseNotOverGui(void); // Returns bool if mouse is not over any guis

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
    selectedPart = -1;
    partInfoRect = Rectangle{float(screenWidth)-315, 150, 140, 215};
    fill_n(cPartValues, 6, 50);

    GuiSetStyle(LISTVIEW, SCROLLBAR_WIDTH, 8);
    GuiSetStyle(VALUEBOX, TEXT_PADDING, 5);
}

void UpdateEditorScreen(void)
{
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) 
    {
        CheckEditorWindows();
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
        else if (draggingBox == 2)
        {
            Vector2 newPos = Vector2Subtract(GetMousePosition(), mouseDistance);
            partInfoRect.x = newPos.x;
            partInfoRect.y = newPos.y;
        }
    }
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
    {
        draggingBox = 0;

        // Acutal level edits should only happen if we're pressing in the level area
        if (isMouseNotOverGui())
        {
            switch(editMode)
            {
                case 0: // Place mode
                    selectedPart = PlacePart(partIndex, &gameMap);
                case 1: // Select mode
                    selectedPart = getClickedPart(gameMap);
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
        goto applyPartInfo; // The previous things should only happen if the mouse button is released, but this should happen if the left or enter is released 
    }
    // Apply partInfo box
    if (IsKeyReleased(KEY_ENTER))
    {
        applyPartInfo:
        if (selectedPart > -1)
        {
            int end = 2;
            if (gameMap.mapParts.at(selectedPart).partType == SLOPE) end = 3;
            for (int i = 0; i < end; i++)
            {
                gameMap.mapParts.at(selectedPart).points[i].x = cPartValues[2*i];
                gameMap.mapParts.at(selectedPart).points[i].y = cPartValues[2*i+1];
            }
        }
    }
    // Apply the config
    if (configStatus == 1)
    {
        gameMap.title = titleInput;
        gameMap.description = descriptionInput;
        gameMap.author = authorInput;
        gameMap.levelSize.x = levelWidth;
        gameMap.levelSize.y = levelHeight;
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
    DrawGui();

}
void UnloadEditorScreen(void)
{

}
int FinishEditorScreen(void)
{
    return finishScreen;
}
void DrawGui(void)
{
    GuiListView((Rectangle){0,0,150,125}, "Place;Select;Move;Copy;Delete", &editScroll, &editMode); // edit types
    
    GuiListView((Rectangle){150, 0, float(screenWidth)-200, 125}, "Rectangle;Slope;Spawn", &partScroll, &partIndex); // part types

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

    if (selectedPart != -1)
    {
        if (GuiWindowBox(partInfoRect, "Part info")) selectedPart = -1;
        string inputLabels[4] = {"X:", "Y:","Width:","Height:"};
        
        if (selectedPart > -1 && gameMap.mapParts[selectedPart].partType == SLOPE)
        {
            inputLabels[0] = "X1"; inputLabels[1] = "Y1";
            inputLabels[2] = "X2"; inputLabels[3] = "Y2";
            GuiSpinner((Rectangle){partInfoRect.x+45, partInfoRect.y+150, 80, 25}, "X3:", &cPartValues[4], 0, gameMap.levelSize.x, panelInputEditIndex == 10);
            GuiSpinner((Rectangle){partInfoRect.x+45, partInfoRect.y+180, 80, 25}, "Y3:", &cPartValues[5], 0, gameMap.levelSize.y, panelInputEditIndex == 11);
        }
        
        GuiSpinner((Rectangle){partInfoRect.x+45, partInfoRect.y+30, 90, 25}, inputLabels[0].c_str(), &cPartValues[0], -gameMap.levelSize.x+gameMap.levelSize.x/2, gameMap.levelSize.x/2, panelInputEditIndex == 6);
        GuiSpinner((Rectangle){partInfoRect.x+45, partInfoRect.y+60, 90, 25}, inputLabels[1].c_str(), &cPartValues[1], -gameMap.levelSize.x+gameMap.levelSize.x/2, gameMap.levelSize.x/2, panelInputEditIndex == 7);
        GuiSpinner((Rectangle){partInfoRect.x+45, partInfoRect.y+90, 90, 25}, inputLabels[2].c_str(), &cPartValues[2], 0, gameMap.levelSize.x, panelInputEditIndex == 8);
        GuiSpinner((Rectangle){partInfoRect.x+45, partInfoRect.y+120, 90, 25}, inputLabels[3].c_str(), &cPartValues[3], 0, gameMap.levelSize.y, panelInputEditIndex == 9);
    }
}
int PlacePart(int partI, GameMap *gMap)
{
    cPartValues[0] = GetMouseX(); cPartValues[1] = GetMouseY();
    switch(partIndex)
    {
        case 0: // Rectangle
            gMap->mapParts.push_back(MapPart(RECTANGLE, ORANGE, vector<Vector2>{GetMousePosition(), (Vector2){50, 50}}));
            return gMap->mapParts.size()-1;
        case 1: // Slope
            gMap->mapParts.insert(gMap->mapParts.begin(), MapPart(SLOPE, ORANGE, vector<Vector2>{GetMousePosition(), Vector2Add(GetMousePosition(), (Vector2){0, 50}), Vector2Add(GetMousePosition(), (Vector2){50, 50})}));
            return 0; // since slopes go to the start
        case 2: // Spawn
            gMap->spawn = GetMousePosition();
            return -2;
        default: // idfk what to do here
        break;
    }
    return -1;
}
int getClickedPart(GameMap gMap)
{
    if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){gMap.spawn.x, gMap.spawn.y-120, 55, 120}))
        return -2;
    
    int counter = 0;
    for (MapPart mapPart : gMap.mapParts)
    {
        if ((mapPart.partType == RECTANGLE && CheckCollisionPointRec(GetMousePosition(), (Rectangle){mapPart.points[0].x, mapPart.points[0].y, mapPart.points[1].x, mapPart.points[1].y}))
            || (mapPart.partType == SLOPE && CheckCollisionPointTriangle(GetMousePosition(), mapPart.points[0], mapPart.points[1], mapPart.points[2])))
        {
            cPartValues[0] = mapPart.points[0].x; cPartValues[1] = mapPart.points[0].y;
            cPartValues[2] = mapPart.points[1].x; cPartValues[2] = mapPart.points[1].y;
            return counter;
        }
        
        counter++;
    }
    return -1;
}
void CheckEditorWindows(void)
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
        if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){messageBoxRect.x+55, messageBoxRect.y+50, 320, 25})) panelInputEditIndex = 1;
        else if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){messageBoxRect.x+35, messageBoxRect.y+100, 340, 25})) panelInputEditIndex = 2;
        else if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){messageBoxRect.x+75, messageBoxRect.y+130, 300, 25})) panelInputEditIndex = 3;
        else if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){messageBoxRect.x+117, messageBoxRect.y+165, 146, 25})) panelInputEditIndex = 4;
        else if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){messageBoxRect.x+117, messageBoxRect.y+195, 146, 25})) panelInputEditIndex = 5;
    }
    // Check part info buttons
    else if (selectedPart != -1 && CheckCollisionPointRec(GetMousePosition(), partInfoRect))
    {   
        // For dragging the config box 
        if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){partInfoRect.x, partInfoRect.y, 115, 25}))
        {
            mouseDistance = Vector2Subtract(GetMousePosition(), (Vector2){partInfoRect.x, partInfoRect.y});
            draggingBox = 2;
        } 
        // Check keyboard input
        if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){partInfoRect.x+72, partInfoRect.y+150, 36, 25})) panelInputEditIndex = 10;
        else if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){partInfoRect.x+72, partInfoRect.y+180, 36, 25})) panelInputEditIndex = 11;
        else if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){partInfoRect.x+72, partInfoRect.y+30, 36, 25})) panelInputEditIndex = 6;
        else if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){partInfoRect.x+72, partInfoRect.y+60, 36, 25})) panelInputEditIndex = 7;
        else if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){partInfoRect.x+72, partInfoRect.y+90, 36, 25})) panelInputEditIndex = 8;
        else if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){partInfoRect.x+72, partInfoRect.y+120, 36, 25})) panelInputEditIndex = 9;
    }
}
bool isMouseNotOverGui(void)
{
    return (!configStatus 
            && !CheckCollisionPointRec(GetMousePosition(), (Rectangle){0, 0, float(screenWidth), 125})
            && !(selectedPart != -1 && CheckCollisionPointRec(GetMousePosition(), partInfoRect)));
}