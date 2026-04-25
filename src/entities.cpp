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
#include "map_parts.hpp"

Player::Player(float x, float y, KeyboardKey left, KeyboardKey right, KeyboardKey jump)
{
    this->pos = {x, y};
    this->vel = {0, 0};
    this->onGround = false;
    controls[0] = left; // there has got to be a better way to do this
    controls[1] = right;
    controls[2] = jump;
}

void Player::UpdatePosition() {
    pos = Vector2Add(pos, vel);
    vel.y += 0.1; // Apply gravity
}
bool Player::CheckCollision(MapPart part) {
    // TODO: Make the player *glide* on the slope, rather than just simple triangle collision

    bool yCollide = false;
    bool xCollide = false;
    
    if (part.partType == RECTANGLE)
    {
        Rectangle partRect = {part.points[0].x, part.points[0].y, part.points[1].x, part.points[1].y};
        yCollide = CheckCollisionRecs(Rectangle {pos.x+1, pos.y+1 + vel.y, 23, 23}, partRect); // plus 1 to center the collision
        xCollide = CheckCollisionRecs(Rectangle {pos.x+1  + vel.x, pos.y+1, 23, 23}, partRect);
    }

    if (part.partType == SLOPE) // Will not account if the player is INSIDE the triangle, only the lines around them. Would love a better solution
    {
        if (vel.y < 0) 
        {
            for (int i = 0; i < 3; i++) {
                int i2 = (i+1) % 3; // To get the Vector2 after I, loop back to start
                // Top line of box
                yCollide = CheckCollisionLines( {pos.x+1, pos.y-1+vel.y}, {pos.x+22, pos.y-1+vel.y}, part.points.at(i), part.points.at(i2), NULL);
                if (yCollide)
                    break;
            }
        }
        else if (vel.y > 0) 
        {
            for (int i = 0; i < 3; i++) {
                int i2 = (i+1) % 3;
                // Bottom line of box
                yCollide = CheckCollisionLines( {pos.x+1, pos.y+22+vel.y}, {pos.x+22, pos.y+22+vel.y}, part.points.at(i), part.points.at(i2), NULL);
                if (yCollide)
                    break;
            }
        }

        for (int i = 0; i < 3; i++) {
            int i2 = (i+1) % 3; 
            // Top line of box
            xCollide = CheckCollisionLines( {pos.x+1+vel.x, pos.y-1}, {pos.x+22+vel.x, pos.y-1}, part.points.at(i), part.points.at(i2), NULL);
            if (xCollide)
                break;

            if (vel.x < 0) 
            {
                // Left line of box
                xCollide = CheckCollisionLines( {pos.x+1+vel.x, pos.y-1}, {pos.x+22+vel.x, pos.y+22}, part.points.at(i), part.points.at(i2), NULL);
                if (xCollide)
                    break;
            }
            else if (vel.x > 0)
            {
                // Right line of box
                xCollide = CheckCollisionLines( {pos.x+22+vel.x, pos.y-1}, {pos.x+22+vel.x, pos.y+22}, part.points.at(i), part.points.at(i2), NULL);
                if (xCollide)
                    break;
            }
            // Bottom line of box
            xCollide = CheckCollisionLines( {pos.x+1+vel.x, pos.y+22}, {pos.x+22+vel.x, pos.y+22}, part.points.at(i), part.points.at(i2), NULL);
            if (xCollide)
                break;
        }
    }

    if (yCollide)
    {
        if (vel.y > 0.1)
            onGround = true;
        vel.y = 0;
    }
    else if (vel.y != 0)
        onGround = false;
    
    if (xCollide)
        vel.x = 0;

    return xCollide || yCollide;
}
void Player::CheckControls() {
    if (IsKeyDown(controls[0]))
        vel.x = -2;
    else if (IsKeyDown(controls[1]))
        vel.x = 2;
    else
        vel.x = 0;

    if (IsKeyDown(controls[2]) && onGround)
        vel.y = -4;
}