#include "tetromino.hpp"

void Tetromino::initializePosition()
{
    pos.x = (GRID_WIDTH - squareSize) / 2;
    pos.y = 0;
    if (id == 'I')
        pos.y--;
}

Tetromino Tetromino::rotatedCCW()
{
    Tetromino rotatedTetrominoCCW{*this};
    if (--rotatedTetrominoCCW.rotationIndex == -1)
        rotatedTetrominoCCW.rotationIndex = 3;
    for (int i = 0; i < rotatedTetrominoCCW.squareSize; i++)
    {
        for (int j = 0; j < rotatedTetrominoCCW.squareSize; j++)
        {
            rotatedTetrominoCCW.piece[rotatedTetrominoCCW.squareSize - 1 - j][i] = this->piece[i][j];
        }
    }

    return rotatedTetrominoCCW;
}
Tetromino Tetromino::rotatedCW()
{
    Tetromino rotatedTetrominoCW{*this};
    if (++rotatedTetrominoCW.rotationIndex == 4)
        rotatedTetrominoCW.rotationIndex = 0;
    for (int i = 0; i < rotatedTetrominoCW.squareSize; i++)
    {
        for (int j = 0; j < rotatedTetrominoCW.squareSize; j++)
        {
            rotatedTetrominoCW.piece[j][rotatedTetrominoCW.squareSize - 1 - i] = this->piece[i][j];
        }
    }

    return rotatedTetrominoCW;
}