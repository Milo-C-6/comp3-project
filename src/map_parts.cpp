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
#include "map_parts.hpp"
#include <vector>

MapPart::MapPart(PartType partType, Color color, vector<Vector2> points, vector<PartAttributes> attributes)
{
    this->partType = partType;
    this->color = color;
    this->points = points;
    this->attributes = attributes;
}
MapPart::MapPart(PartType partType, Color color, vector<Vector2> points)
{
    this->partType = partType;
    this->color = color;
    this->points = points;
}