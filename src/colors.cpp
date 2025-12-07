#include "common.hpp"

sf::Color enumToColor(Color choice)
{
    switch (choice)
    {
    case EMPTY:
        return sf::Color(66, 73, 82);
    case CYAN:
        return sf::Color::Cyan;
    case BLUE:
        return sf::Color::Blue;
    case ORANGE:
        return sf::Color(255, 127, 0);
    case YELLOW:
        return sf::Color::Yellow;
    case GREEN:
        return sf::Color::Green;
    case PURPLE:
        return sf::Color(128, 0, 128);
    case RED:
        return sf::Color::Red;
    case DARK_PURPLE:
        return sf::Color(49, 49, 80);
    case TRANSPARENT:
        return sf::Color(255, 255, 255, 64);
    default:
        return sf::Color::Black;
    }
}