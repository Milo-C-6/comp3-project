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

#ifndef ENTITIES_HPP
#define ENTITIES_HPP
#include "raylib.h"
#include "raymath.h"
#include "map_parts.hpp"

class Player {
    public:
        Vector2 pos;
        Vector2 vel;
        bool onGround;
        bool onSlope;
        KeyboardKey controls[3];
        
        Player(float x, float y, KeyboardKey left, KeyboardKey right, KeyboardKey jump);
        
        void UpdatePosition();
        void CheckCollision(MapPart part);
        void CheckControls();
};
#endif