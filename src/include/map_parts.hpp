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
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

typedef enum PartType { RECTANGLE, SLOPE, MP_TEXT } PartType;
typedef enum PartAttributes { WIN, KILL, BOUNCY, LAUNCHER, MOVING } PartAttributes;

class MapPart 
{
    public:
        PartType partType;
        Color color;
        vector<Vector2> points; // For rectangles first v2 is pos, second is width. Slopes (triangles) have 3 v2 points. *maybe a array with a set value of 3 would be better?*
        unordered_map<PartAttributes, int> attributes;
        string formulaX;
        string formulaY;
        string label;
        
        MapPart(PartType partType, Color color, vector<Vector2> points, unordered_map<PartAttributes, int> attributes);
        MapPart(PartType partType, Color color, vector<Vector2> points);
        MapPart();
        void ExecuteFormulas();

        friend ostream& operator<<(std::ostream& os, const MapPart& mapPart);
        friend string& operator>>(string& line, MapPart& mapPart);
};
#endif