#include "raylib.h"
#include "raymath.h"
#include "entities.hpp"

Player::Player()
{
    this->pos = {50, 50};
    this->vel = {0, 0};
    this->onGround = false;
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