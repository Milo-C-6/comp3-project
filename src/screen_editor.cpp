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
#include <algorithm>
using namespace std;

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#undef RAYGUI_IMPLEMENTATION            // Avoid including raygui implementation again
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
static int editMode; // 0: Place, 1: Select, 2: Move, 3: Copy, 4: Delete
static int partScroll;
static int partIndex; // 0: Rectangle, 1: Slope, 2: Spawn 
static int selectedPart;
// Camera
static Camera2D camera = { 0 };
static Vector2 cameraTarg = Vector2Zero();
static Vector2 mouseWorldPos;
static bool isPanning;
// GUIS
static int configStatus;
static bool showInfo;
static Rectangle messageBoxRect;
static Rectangle partInfoRect;
static int panelInputEditIndex; // 1-5: Config inputs, 6-14: Part info inputs
// Part Info
static int cPartValues[6];
static bool cPartChecks[4];
const PartAttributes attributes[4] = {KILL, BOUNCY, WIN, MOVING};
static int cPartLauncher = 0;
static int pColorScroll = 0;
static int colorIndex = 2;
static char cPartFormulaX[128];
static char cPartFormulaY[128];
const Color colors[21] = {
        DARKGRAY, MAROON, ORANGE, DARKGREEN, DARKBLUE, DARKPURPLE, DARKBROWN,
        GRAY, RED, GOLD, LIME, BLUE, VIOLET, BROWN, LIGHTGRAY, PINK, YELLOW,
        GREEN, SKYBLUE, PURPLE, BEIGE };
// Config
static char titleInput[64];
static char descriptionInput[512];
static char authorInput[64];
static int levelWidth;
static int levelHeight;
static Vector2 mouseDistance[3];
static int draggingBox; // 1 = Config box, 2 = Part info box, 


//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
static int PlacePart(int partI, GameMap *gMap); // Insert a block into the map parts, returns selected part.
static void DrawGui(void); // guess what it does :)
static int getClickedPart(GameMap gMap); // Gets the part clicked by mouse
static void CheckEditorWindows(void); // Make dragging/keyboard input for windows work
static bool isMouseNotOverGui(void); // Returns bool if mouse is not over any guis
static void InitialMovePart(void); // Sets the mousedistance for MapParts
static void CopyPart(void);
static void cameraMovements(void); // Adds camera panning and zooming
static void setCPartInfo(MapPart mapPart); // Set the CpartInfo variable to the current selected/placed object.
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
    partInfoRect = Rectangle{float(screenWidth)-315, 150, 295, 255};
    fill_n(cPartValues, 6, 50);
    camera.target = cameraTarg;
    camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    isPanning = false;

    GuiSetStyle(LISTVIEW, SCROLLBAR_WIDTH, 8);
    GuiSetStyle(VALUEBOX, TEXT_PADDING, 5);
}

void UpdateEditorScreen(void)
{
    cameraMovements();
    mouseWorldPos = Vector2Scale(Vector2Subtract(GetMousePosition(), camera.offset), 1/camera.zoom);
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) 
    {
        CheckEditorWindows();
        InitialMovePart();
        CopyPart();
        cout << GetMouseX() << ", " << GetMouseY() << endl;
    }
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        Vector2 newPos = Vector2Subtract(GetMousePosition(), mouseDistance[0]);
        // Allow dragging the config box
        if (draggingBox == 1)
        {
            messageBoxRect.x = newPos.x;
            messageBoxRect.y = newPos.y;
        }
        else if (draggingBox == 2)
        {
            partInfoRect.x = newPos.x;
            partInfoRect.y = newPos.y;
        }

        // Dragging map parts
        if ((editMode == 2 || editMode == 3) && selectedPart != -1)
        {
            if (selectedPart == -2)
                gameMap.spawn = newPos;
            else
            {
                gameMap.mapParts.at(selectedPart).points[0] = Vector2Subtract(mouseWorldPos, mouseDistance[0]);
                if (gameMap.mapParts.at(selectedPart).partType == SLOPE)
                {
                    gameMap.mapParts.at(selectedPart).points[1] = Vector2Subtract(mouseWorldPos, mouseDistance[1]);
                    gameMap.mapParts.at(selectedPart).points[2] = Vector2Subtract(mouseWorldPos, mouseDistance[2]);
                }
            }
        }
    }
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
    {
        draggingBox = 0;

        // Acutal level edits should only happen if we're pressing in the level area
        if (isMouseNotOverGui())
        {
            if (editMode != 0 && editMode != 2)
                selectedPart = getClickedPart(gameMap);
            switch(editMode)
            {
                case 0: // Place mode
                    selectedPart = PlacePart(partIndex, &gameMap);
                    break;
                case 2: // Move mode
                case 3: // Copy mode
                    selectedPart = -1;
                    break;
                case 4: // Delete mode
                    if (selectedPart > -1)
                        gameMap.mapParts.erase(gameMap.mapParts.begin()+selectedPart);
                    selectedPart = -1;
                    break;
                default: // idfk what to do here
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
            MapPart& sPart = gameMap.mapParts.at(selectedPart);
            int end = 2;
            if (sPart.partType == SLOPE) end = 3;
            for (int i = 0; i < end; i++)
            {
                sPart.points[i].x = cPartValues[2*i];
                sPart.points[i].y = cPartValues[2*i+1];
            }
            sPart.color = colors[colorIndex];
            for (int aIndex : cPartChecks)
            {
                if (aIndex)
                    sPart.attributes[attributes[aIndex]] = true;
            }
            sPart.formulaX = (cPartFormulaX);
            sPart.formulaY = (cPartFormulaY);
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
    BeginMode2D(camera);
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
                    DrawRectangle(mouseWorldPos.x, mouseWorldPos.y, 50, 50, Fade(ORANGE, 0.1));
                    break;
                case 1: // Slope
                    DrawTriangle(mouseWorldPos, Vector2Add(mouseWorldPos, (Vector2){0, 50}), Vector2Add(mouseWorldPos, (Vector2){50, 50}), Fade(GOLD, 0.1));
                    break;
                case 2: // Spawn
                    for (int i = 0; i < 8; i++)
                        DrawRectangle(mouseWorldPos.x+i/4*30, mouseWorldPos.y-i%4*30, 25, 25, Fade(BLUE, 0.1));
                    DrawText("Spawn", mouseWorldPos.x, mouseWorldPos.y-120, 16, Fade(BLACK, 0.1));
                    break;
                default: // idfk what to do here
                    break;
            }
        }
        DrawRectangleLines(-gameMap.levelSize.x/2, -gameMap.levelSize.y/2, gameMap.levelSize.x, gameMap.levelSize.y, RED); // Map border
    EndMode2D();
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

    if (selectedPart != -1 && editMode < 2)
    {
        if (GuiWindowBox(partInfoRect, "Part info")) selectedPart = -1;
        string inputLabels[4] = {"X:", "Y:","Width:","Height:"};
        
        if (selectedPart > -1 && gameMap.mapParts[selectedPart].partType == SLOPE)
        {
            inputLabels[0] = "X1"; inputLabels[1] = "Y1";
            inputLabels[2] = "X2"; inputLabels[3] = "Y2";
            GuiSpinner((Rectangle){partInfoRect.x+45, partInfoRect.y+150, 90, 25}, "X3:", &cPartValues[4], 0, gameMap.levelSize.x, panelInputEditIndex == 10);
            GuiSpinner((Rectangle){partInfoRect.x+45, partInfoRect.y+180, 90, 25}, "Y3:", &cPartValues[5], 0, gameMap.levelSize.y, panelInputEditIndex == 11);
        }
        
        GuiSpinner((Rectangle){partInfoRect.x+45, partInfoRect.y+30, 90, 25}, inputLabels[0].c_str(), &cPartValues[0], -gameMap.levelSize.x+gameMap.levelSize.x/2, gameMap.levelSize.x/2, panelInputEditIndex == 6);
        GuiSpinner((Rectangle){partInfoRect.x+45, partInfoRect.y+60, 90, 25}, inputLabels[1].c_str(), &cPartValues[1], -gameMap.levelSize.x+gameMap.levelSize.x/2, gameMap.levelSize.x/2, panelInputEditIndex == 7);
        GuiSpinner((Rectangle){partInfoRect.x+45, partInfoRect.y+90, 90, 25}, inputLabels[2].c_str(), &cPartValues[2], 0, gameMap.levelSize.x, panelInputEditIndex == 8);
        GuiSpinner((Rectangle){partInfoRect.x+45, partInfoRect.y+120, 90, 25}, inputLabels[3].c_str(), &cPartValues[3], 0, gameMap.levelSize.y, panelInputEditIndex == 9);
        // Attributes
        GuiCheckBox((Rectangle){partInfoRect.x+145, partInfoRect.y+30, 20, 20}, "Kill", &cPartChecks[0]);
        GuiCheckBox((Rectangle){partInfoRect.x+145, partInfoRect.y+55, 20, 20}, "Bouncy", &cPartChecks[1]);
        GuiSpinner((Rectangle){partInfoRect.x+200, partInfoRect.y+80, 90, 20}, "Launcher", &cPartLauncher, 0, 255, panelInputEditIndex == 12);
        GuiCheckBox((Rectangle){partInfoRect.x+145, partInfoRect.y+105, 20, 20}, "Win", &cPartChecks[2]);
        GuiCheckBox((Rectangle){partInfoRect.x+145, partInfoRect.y+130, 20, 20}, "Moving", &cPartChecks[3]);
        GuiLabel((Rectangle){partInfoRect.x+10, partInfoRect.y+160, 55, 20}, "X Formula");
        GuiTextBox((Rectangle){partInfoRect.x+70, partInfoRect.y+160, 200, 20}, cPartFormulaX, 64, panelInputEditIndex == 13);
        GuiLabel((Rectangle){partInfoRect.x+10, partInfoRect.y+190, 55, 20}, "Y Formula");
        GuiTextBox((Rectangle){partInfoRect.x+70, partInfoRect.y+190, 200, 20}, cPartFormulaY, 64, panelInputEditIndex == 14);
        GuiLabel((Rectangle){partInfoRect.x+10, partInfoRect.y+220, 50, 30}, "Color");
        GuiListView((Rectangle){partInfoRect.x+60, partInfoRect.y+220, 200, 30}, "DARKGRAY;MAROON;ORANGE;DARKGREEN;DARKBLUE;DARKPURPLE;DARKBROWN;GRAY;RED;GOLD;LIME;BLUE;VIOLET;BROWN;LIGHTGRAY;PINK;YELLOW;GREEN;SKYBLUE;PURPLE;BEIGE", &pColorScroll, &colorIndex);
    }
}
int PlacePart(int partI, GameMap *gMap)
{
    fill(begin(cPartChecks), begin(cPartChecks)+3, false);
    cPartValues[0] = mouseWorldPos.x; cPartValues[1] = mouseWorldPos.y;
    colorIndex = 2;
    // memset(&partPosFormulas,0,2);
    switch(partIndex)
    {
        case 0: // Rectangle
            gMap->mapParts.push_back(MapPart(RECTANGLE, ORANGE, vector<Vector2>{mouseWorldPos, (Vector2){50, 50}}));
            cPartValues[2] = 50; cPartValues[3] = 50;
            return gMap->mapParts.size()-1;
        case 1: // Slope
            gMap->mapParts.insert(gMap->mapParts.begin(), MapPart(SLOPE, ORANGE, vector<Vector2>{mouseWorldPos, Vector2Add(mouseWorldPos, (Vector2){0, 50}), Vector2Add(mouseWorldPos, (Vector2){50, 50})}));
            cPartValues[2] = mouseWorldPos.x; cPartValues[3] = mouseWorldPos.y+50;
            cPartValues[4] = mouseWorldPos.x+50; cPartValues[5] = mouseWorldPos.y+50;
            return 0; // since slopes go to the start
        case 2: // Spawn
            gMap->spawn = mouseWorldPos;
            // return -2;
            return -1;
        default: // idfk what to do here
        break;
    }
    return -1;
}
int getClickedPart(GameMap gMap)
{
    // if (CheckCollisionPointRec(mouseWorldPos, (Rectangle){gMap.spawn.x, gMap.spawn.y-120, 55, 120}))
    //     return -2;
    
    int counter = 0;
    for (MapPart mapPart : gMap.mapParts)
    {
        if ((mapPart.partType == RECTANGLE && CheckCollisionPointRec(mouseWorldPos, (Rectangle){mapPart.points[0].x, mapPart.points[0].y, mapPart.points[1].x, mapPart.points[1].y}))
            || (mapPart.partType == SLOPE && CheckCollisionPointTriangle(mouseWorldPos, mapPart.points[0], mapPart.points[1], mapPart.points[2])))
        {
            setCPartInfo(mapPart);
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
            mouseDistance[0] = Vector2Subtract(GetMousePosition(), (Vector2){messageBoxRect.x, messageBoxRect.y});
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
        if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){partInfoRect.x, partInfoRect.y, 270, 25}))
        {
            mouseDistance[0] = Vector2Subtract(GetMousePosition(), (Vector2){partInfoRect.x, partInfoRect.y});
            draggingBox = 2;
        } 
        // Check keyboard input
        if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){partInfoRect.x+72, partInfoRect.y+150, 36, 25})) panelInputEditIndex = 10;
        else if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){partInfoRect.x+72, partInfoRect.y+180, 36, 25})) panelInputEditIndex = 11;
        else if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){partInfoRect.x+72, partInfoRect.y+30, 36, 25})) panelInputEditIndex = 6;
        else if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){partInfoRect.x+72, partInfoRect.y+60, 36, 25})) panelInputEditIndex = 7;
        else if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){partInfoRect.x+72, partInfoRect.y+90, 36, 25})) panelInputEditIndex = 8;
        else if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){partInfoRect.x+72, partInfoRect.y+120, 36, 25})) panelInputEditIndex = 9;
        else if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){partInfoRect.x+225, partInfoRect.y+80, 45, 20})) panelInputEditIndex = 12;
        else if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){partInfoRect.x+60, partInfoRect.y+160, 210, 20})) panelInputEditIndex = 13;
        else if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){partInfoRect.x+60, partInfoRect.y+190, 210, 20})) panelInputEditIndex = 14;
    }
}
bool isMouseNotOverGui(void)
{
    return (!configStatus 
            && !CheckCollisionPointRec(GetMousePosition(), (Rectangle){0, 0, float(screenWidth), 125})
            && !(selectedPart != -1 && editMode < 2 && CheckCollisionPointRec(GetMousePosition(), partInfoRect)));
}
void InitialMovePart(void)
{
    if (editMode != 2)
        return;
    
    selectedPart = getClickedPart(gameMap);

    if (selectedPart == -1)
        return;

    MapPart sPart = gameMap.mapParts.at(selectedPart);
    int end = 1;
    if (sPart.partType == SLOPE) end = 3;
    for (int i=0; i<end; i++)
        mouseDistance[i] = Vector2Subtract(mouseWorldPos, sPart.points[i]);
}
void CopyPart(void)
{
    if (editMode != 3)
        return;
    int partI = getClickedPart(gameMap);
    
    if (partI < 0)
        return;
        
    partI = gameMap.mapParts.at(partI).partType;
    selectedPart = PlacePart(partI, &gameMap);
}
void cameraMovements(void)
{
    if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE))
    {
        isPanning = true;
        mouseDistance[0] = Vector2Subtract(Vector2Subtract(GetMousePosition(), camera.offset), camera.offset);
    }
    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE))
    {
        camera.offset = Vector2Subtract(Vector2Subtract(GetMousePosition(), camera.offset), mouseDistance[0]); // This makes a weird jitter effect I don't know how to fix, and also dont plan to fix
    }
    if (IsMouseButtonReleased(MOUSE_BUTTON_MIDDLE))
        isPanning = false;
    if (isMouseNotOverGui())
        camera.zoom = expf(logf(camera.zoom) + ((float)GetMouseWheelMove()*0.1f)); //thanks raylib docs
    if (camera.zoom < 0.1f) camera.zoom = 0.1f;
}
void setCPartInfo(MapPart mapPart)
{
    int end = 2;
    if (mapPart.partType == SLOPE) end = 3;
    for (int i = 0; i < end; i++)
    {
        cPartValues[2*i] = mapPart.points[i].x;
        cPartValues[2*i+1] = mapPart.points[i].y;
    }
    // Check attributes
    for (int i = 0; i < 4; i++)
    {
        if (mapPart.attributes.count(attributes[i]) > 0 && mapPart.attributes.at(attributes[i]) > 0)
        {
            cPartChecks[i] = 1;
            if (attributes[i] == MOVING)
            {
                strcpy(cPartFormulaX, mapPart.formulaX.c_str());
                strcpy(cPartFormulaY, mapPart.formulaY.c_str());
            }
        }

    }
    if (mapPart.attributes.count(LAUNCHER) > 0)
        cPartLauncher = mapPart.attributes.at(LAUNCHER);
}