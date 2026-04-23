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

        if (IsKeyDown(KEY_SPACE) && plr.onGround)
            plr.vel.y = -4;

        // Handle collision
        for (Rectangle mapRect : mapRects)
           plr.checkCollision(mapRect);
        plr.updatePosition();
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