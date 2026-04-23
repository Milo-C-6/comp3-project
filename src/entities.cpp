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
#include "entities.hpp"

Player::Player(float x, float y, KeyboardKey left, KeyboardKey right, KeyboardKey jump)
{
    this->pos = {x, y};
    this->vel = {0, 0};
    this->onGround = false;
    controls[0] = left; // there has got to be a better way to do this
    controls[1] = right;
    controls[2] = jump;
}

void Player::updatePosition() {
    pos = Vector2Add(pos, vel);
    vel.y += 0.1; // Apply gravity
}
void Player::checkCollision(Rectangle rect) {
    if (CheckCollisionRecs(Rectangle {pos.x+1, pos.y+1 + vel.y, 23, 23}, rect)) { // plus 1 to center the collision
        if (vel.y > 0.1)
            onGround = true;
        vel.y = 0;
    }
    else if (vel.y != 0)
        onGround = false;
    
    if (CheckCollisionRecs(Rectangle {pos.x+1  + vel.x, pos.y+1, 23, 23}, rect))
        vel.x = 0;
}
void Player::checkControls() {
    if (IsKeyDown(controls[0]))
        vel.x = -2;
    else if (IsKeyDown(controls[1]))
        vel.x = 2;
    else
        vel.x = 0;

    if (IsKeyDown(controls[2]) && onGround)
        vel.y = -4;
}