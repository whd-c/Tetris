#pragma once
#include "common.hpp"

class Tetromino
{
public:
    uint8_t squareSize;
    Color color;
    char id;

    Position pos{};
    int8_t rotationIndex{};

    std::vector<std::vector<Color>> piece;

    Tetromino() : squareSize{},
                  color{},
                  id{},
                  pos{0, 0},
                  rotationIndex{},
                  piece{} {}
    Tetromino(int _squareSize, char _id, Color _color)
        : squareSize(_squareSize), id(_id), color(_color), piece(_squareSize, std::vector<Color>(_squareSize, EMPTY))
    {
    }

    void initializePosition();
    Tetromino rotatedCCW();
    Tetromino rotatedCW();
};