#include "raylib.h"
#include "raymath.h"
#include "entities.hpp"

Player::Player()
{
    pos = {50, 50};
    vel = {0, 0};
}
void Player::applyMovement() {
    pos = Vector2Add(pos, vel);
}