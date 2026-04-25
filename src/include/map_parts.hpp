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

#ifndef MAP_PART_HPP
#define MAP_PART_HPP
#include "raylib.h"
#include "raymath.h"
#include <vector>

using namespace std;

typedef enum PartType { RECTANGLE, SLOPE, TRAMPOLINE } PartType;
typedef enum PartAttributes { KILL } PartAttributes;

class MapPart {
    public:
        PartType partType;
        Color color;
        vector<Vector2> points; // For rectangles first v2 is pos, second is width. Slopes (triangles) have 3 v2 points. *maybe a array with a set value of 3 would be better?*
        vector<PartAttributes> attributes; // If KILL stays the only attribute, then this will probably be changed with a kill bool
        
        MapPart(PartType partType, Color color, vector<Vector2> points, vector<PartAttributes> attributes);
        MapPart(PartType partType, Color color, vector<Vector2> points);
};
#endif