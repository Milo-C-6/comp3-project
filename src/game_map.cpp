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
#include "game_map.hpp"
#include "map_parts.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

GameMap::GameMap()
{
    this->spawn = (Vector2){0,0};
    this->levelSize = (Vector2){500,500};
}

int GameMap::size()
{
    return this->mapParts.size();
}

ostream& operator<<(ostream& os, const GameMap& gameMap)
{
    os << "s " << gameMap.title << endl;
    os << "s " << gameMap.description << endl;
    os << "s " << gameMap.author << endl;
    os << "v " << gameMap.spawn.x << " " << gameMap.spawn.y << endl;
    os << "v " << gameMap.levelSize.x << " " << gameMap.levelSize.y << endl;
    for (MapPart mapPart : gameMap.mapParts)
        os << "m " << mapPart << endl;
    return os;
}

ifstream& operator>>(ifstream& is, GameMap& gameMap)
{
    gameMap.mapParts.clear();
    int counter = 0;
    string line;
    while (getline(is, line))
    {
        istringstream iss(line);
        char identifier;
        iss >> identifier;
        if (identifier == '#')
            continue; // It's a comment, skip
        else if (identifier == 's') 
        {
            switch (counter)
            {
                case 0:
                    gameMap.title = line.substr(2);
                    break;
                case 1:
                    gameMap.description = line.substr(2);
                    break;
                case 2:
                    gameMap.author = line.substr(2);
                    break;
                default:
                    break;
            }
        }
        else if (identifier == 'v')
        {
            switch (counter)
            {
            case 3:
                iss >> gameMap.spawn.x >> gameMap.spawn.y;
                break;
            case 4:
                iss >> gameMap.levelSize.x >> gameMap.levelSize.y;
                break;
            default:
                break;
            }
        }
        else if (identifier == 'm')
        {
            MapPart newMP;
            line >> newMP;
            gameMap.mapParts.push_back(newMP);
        }
        counter++;
    }

    return is;
}