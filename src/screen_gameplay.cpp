/**********************************************************************************************
*
*   raylib - Advance Game template
*
*   Gameplay Screen Functions Definitions (Init, Update, Draw, Unload)
*
*   Copyright (c) 2014-2022 Ramon Santamaria (@raysan5)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#include "raylib.h"
#include "screens.h"
#include "entities.hpp"
#include <iostream>

using namespace std;
//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0; // maybe later remove?
static Player players[] = { Player() };
static Rectangle mapRects[] = { {0,150,200,50}, {200,120,200,50} };

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------

// Gameplay Screen Initialization logic
void InitGameplayScreen(void)
{
    // TODO: Initialize GAMEPLAY screen variables here!
    framesCounter = 0;
    finishScreen = 0;
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
    for (Player &plr : players)
    {
        // Handle input
        if (IsKeyDown(KEY_D))
            plr.vel.x = 2;
        else if (IsKeyDown(KEY_A))
            plr.vel.x = -2;
        else
            plr.vel.x = 0;
        // Handle collision
        for (Rectangle mapRect : mapRects)
        {
            if (CheckCollisionRecs(Rectangle {plr.pos.x, plr.pos.y + plr.vel.y, 25, 25}, mapRect)) {
                plr.vel.y = 0;
            }
            else
            {
                plr.pos.x += plr.vel.y;
            }
            if (CheckCollisionRecs(Rectangle {plr.pos.x  + plr.vel.x, plr.pos.y, 25, 25}, mapRect)) {
                plr.vel.x = 0;
            }
            else
            {
                plr.pos.x += plr.vel.x;
            }
        }
    }
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
    // Draw background
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), WHITE);
    // Draw map
    for (Rectangle rect : mapRects) 
    {
        DrawRectangle(rect.x,rect.y,rect.width,rect.height, ORANGE);
    }
    // Draw players
    for (Player plr : players) {
        DrawRectangle(plr.pos.x, plr.pos.y, 25, 25, BLUE);
    }
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