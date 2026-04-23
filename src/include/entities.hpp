#ifndef ENTITIES_HPP
#define ENTITIES_HPP
#include "raylib.h"
#include "raymath.h"

class Player {
    public:
        Player();
        Vector2 pos;
        Vector2 vel;
        bool onGround;

        void updatePosition();
        void checkCollision(Rectangle rect);
};
#endif