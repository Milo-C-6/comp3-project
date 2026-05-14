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
#include "exprtk.hpp"
#include <vector>
#include <unordered_map>
#include <sstream>

MapPart::MapPart(PartType partType, Color color, vector<Vector2> points, unordered_map<PartAttributes, int> attributes)
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

    if (partType == MP_TEXT)
        this->label = "Placeholder";
}
MapPart::MapPart()
{
    this->partType = RECTANGLE;
    this->color = ORANGE;
}

void MapPart::ExecuteFormulas(float numPlrs)
{
    if (!this->attributes.count(MOVING) > 0)
        return;

    typedef exprtk::symbol_table<float> symbol_table_t;
    typedef exprtk::expression<float>   expression_t;
    typedef exprtk::parser<float>       parser_t;

    float time = GetTime();

    symbol_table_t symbol_table;
    symbol_table.add_variable("t", time);
    symbol_table.add_variable("n", numPlrs);
    symbol_table.add_constants();

    expression_t expression;
    expression.register_symbol_table(symbol_table);

    parser_t parser;
    
    if (!formulaX.empty())
    {
        parser.compile(formulaX, expression);
        points[0].x = expression.value();
    }

    if (!formulaY.empty())
    {
        parser.compile(formulaY, expression);
        points[0].y = expression.value();
    }
        
}

std::ostream& operator<<(std::ostream& os, const MapPart& mapPart)
{
    os << mapPart.partType << " ";
    os << mapPart.color.r << " ";
    os << mapPart.color.g << " ";
    os << mapPart.color.b << " ";
    int end = 2;
    if (mapPart.partType == SLOPE) end = 3;
    else if (mapPart.partType == MP_TEXT || mapPart.partType == MP_KEY) end = 1;
    for (int i = 0; i < end; i++)
    {
        os << mapPart.points[i].x << " ";
        os << mapPart.points[i].y << " ";
    }

    for (const auto & [key, value] : mapPart.attributes)
        os << key << ":" << value << ";";
    
    if (mapPart.partType == MP_TEXT) 
        os << " t\"" << mapPart.label << "\"";

    if (!mapPart.formulaX.empty())
    {
        os << " x\"" << mapPart.formulaX << "\"";
    }

    if (!mapPart.formulaY.empty())
    {
        os << " y\"" << mapPart.formulaY << "\"";
    }
    return os;
}

string& operator>>(string& line, MapPart& mapPart) 
{
    istringstream iss(line);
    char identifier; // unused
    int partType;
    // Get part type and color
    iss >> identifier >> partType >> mapPart.color.r >> mapPart.color.g >> mapPart.color.b;
    mapPart.partType = static_cast<PartType>(partType);

    // Get points
    mapPart.points.clear();
    int end = 2;
    if (mapPart.partType == SLOPE) end = 3;
    else if (mapPart.partType == MP_TEXT || mapPart.partType == MP_KEY) end = 1;
    for (int i = 0; i < end; i++)
    {
        Vector2 newPoint;
        iss >> newPoint.x >> newPoint.y;
        mapPart.points.push_back(newPoint);
    }
    
    // Get attributes
    string attrLine;
    iss >> attrLine;
    size_t attrPos = -1;
    while ((attrPos = attrLine.find(';')) != string::npos) 
    {
        string token = attrLine.substr(0, attrPos);
        attrLine = attrLine.substr(attrPos+1, attrLine.length());
        
        string key = token.substr(0, token.find(":"));
        string value = token.substr(token.find(":")+1, token.length() - key.length());
        mapPart.attributes[static_cast<PartAttributes>(stoi(key))] = stoi(value);
    }

    // Get label
    size_t pos = -1;
    if ((pos = line.find("t\"")) != string::npos) {
        string label = line.substr(pos+2, line.length());
        label = label.substr(0, label.find('\"'));
        mapPart.label = label;
    }

    // Get formula
    pos = -1;
    if ((pos = line.find("x\"")) != string::npos) {
        string xFormula = line.substr(pos+2, line.length());
        xFormula = xFormula.substr(0, xFormula.find('\"'));
        mapPart.formulaX = xFormula;
    }

    pos = -1;
    if ((pos = line.find("y\"")) != string::npos) {
        string yFormula = line.substr(pos+2, line.length());
        yFormula = yFormula.substr(0, yFormula.find('\"'));
        mapPart.formulaY = yFormula;
    }
    
    return line;
}