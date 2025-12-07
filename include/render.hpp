#pragma once
#include "tetromino.hpp"

extern std::array<std::array<Color, GRID_WIDTH>, GRID_HEIGHT> screenState;
extern sf::Font roboto;
extern Tetromino heldTetromino;

class Render
{
public:
    float startX, startY;
    Render(sf::RenderWindow &_window) : window(_window) {};

    void drawHeldTetromino();
    void drawTetromino(const Tetromino &tetromino);
    void drawNextTetromino(const Tetromino &tetromino);
    void drawGrid();

private:
    sf::RenderWindow &window;
};