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
static Player p1;

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
    if (IsKeyDown(KEY_D)) 
        p1.vel.x = 2;
    else if (IsKeyDown(KEY_A))
        p1.vel.x = -2;
    else
        p1.vel.x = 0;

    p1.applyMovement();
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), WHITE);
    DrawRectangle(p1.pos.x, p1.pos.y, 25, 25, RED);
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