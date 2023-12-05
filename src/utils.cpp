#include "utils.hpp"
#include <cmath>
#include <random>

/*
        HELPER FUNCTIONS TO DO WITH MOVEMENT:
            - Velocities
            - Positions
            - Directions
*/

// Function to compute the the x and y components of the normalized velocity given speed + direction
Velocity computeVelocity(double speed, Direction direction) {
    Velocity velocity_component;
    double angle = directionToRadians(direction.direction);
    velocity_component.velocity.x = speed * cos(angle);
    velocity_component.velocity.y = speed * -sin(angle);

    return velocity_component;
}

// Function to compute the the x and y components of the normalized velocity given speed + angle
Velocity computeVelocity(double speed, double angle) {
    Velocity velocity_component;
    velocity_component.velocity.x = speed * cos(angle);
    velocity_component.velocity.y = speed * sin(angle);

    return velocity_component;
}


// Function to convert a direction to an angle in radians
double directionToRadians(DIRECTION direction) {
    switch (direction) {
    case E:
        return 0.0;              // 0 degrees
    case NE:
        return M_PI / 4.0;       // 45 degrees
    case N:
        return M_PI / 2.0;       // 90 degrees
    case NW:
        return 3.0 * M_PI / 4.0; // 135 degrees
    case W:
        return M_PI;             // 180 degrees
    case SW:
        return 5.0 * M_PI / 4.0; // 225 degrees
    case S:
        return 3.0 * M_PI / 2.0; // 270 degrees
    case SE:
        return 7.0 * M_PI / 4.0; // 315 degrees
    default:
        // Invalid direction, might need to catch error somewhere
        throw std::invalid_argument("Invalid direction");
    }
}

// Function that determines elemental weaknesses
bool isWeakTo(ElementType t1, ElementType t2) {
    // Elemental Hierarchy:
        // Water > Fire > Earth > Lightning, where Lightning > Water
           
    // If t1 is weaker than t2, return true
    switch (t1) {
    case ElementType::WATER:
        return t2 == ElementType::LIGHTNING;
    case ElementType::FIRE:
        return t2 == ElementType::WATER;
    case ElementType::EARTH:
        return t2 == ElementType::FIRE;
    case ElementType::LIGHTNING:
        return t2 == ElementType::EARTH;
    case ElementType:: COMBO:
        return t2 == getRandomElementType(); // randomize weakness
    default:
        return false;
    }
}

// Helper function to get a random element type
ElementType getRandomElementType() {
    static const std::vector<ElementType> elementTypes = { ElementType::WATER, ElementType::FIRE, ElementType::EARTH, ElementType::LIGHTNING };

    // Use C++11 random number generation
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(0, elementTypes.size() - 1);

    return elementTypes[distribution(gen)];
}

