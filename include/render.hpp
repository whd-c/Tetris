#pragma once
#include "tetromino.hpp"

class Render
{
public:
    float startX, startY;
    Render(sf::RenderWindow &_window, sf::Font _roboto) : window(_window), roboto(_roboto) {};

    void drawHeldTetromino(const Tetromino &tetromino);
    void drawTetromino(const Tetromino &tetromino);
    void drawNextTetromino(const Tetromino &tetromino);
    void drawText(sf::Text &text, std::string content, float posX, float posY);
    void drawGrid(std::array<std::array<Color, GRID_WIDTH>, GRID_HEIGHT> screenState);

private:
    sf::RenderWindow &window;
    const sf::Font roboto;
};