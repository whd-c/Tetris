#pragma once
#include "common.hpp"

class Tetromino
{
public:
    Tetromino() : squareSize{},
                  id{},
                  color{},
                  piece{},
                  pos{0, 0},
                  rotationIndex{} {}
    Tetromino(int _squareSize, char _id, Color _color)
        : squareSize(_squareSize), id(_id), color(_color), piece(_squareSize, std::vector<Color>(_squareSize, EMPTY))
    {
    }

    uint8_t squareSize;
    char id;
    Color color;
    std::vector<std::vector<Color>> piece;
    Position pos{};
    int8_t rotationIndex{};

    void initializePosition();
    Tetromino rotatedCCW();
    Tetromino rotatedCW();
};