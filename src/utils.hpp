#pragma once
#include "components.hpp"

Velocity computeVelocity(double speed, Direction direction);
Velocity computeVelocity(double speed, double angle);
double directionToRadians(DIRECTION direction);
bool isWeakTo(ElementType t1, ElementType t2);

