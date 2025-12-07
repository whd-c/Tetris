#include "game_manager.hpp"

void GameManager::initializeTetrominoes()
{
    for (Tetromino &tetromino : tetrominoes)
    {
        switch (tetromino.id)
        {
        case 'O':
            tetromino.piece[0][0] = tetromino.color;
            tetromino.piece[0][1] = tetromino.color;
            tetromino.piece[1][0] = tetromino.color;
            tetromino.piece[1][1] = tetromino.color;
            break;
        case 'I':
            tetromino.piece[1][0] = tetromino.color;
            tetromino.piece[1][1] = tetromino.color;
            tetromino.piece[1][2] = tetromino.color;
            tetromino.piece[1][3] = tetromino.color;
            break;
        case 'S':
            tetromino.piece[0][1] = tetromino.color;
            tetromino.piece[0][2] = tetromino.color;
            tetromino.piece[1][0] = tetromino.color;
            tetromino.piece[1][1] = tetromino.color;
            break;
        case 'Z':
            tetromino.piece[0][0] = tetromino.color;
            tetromino.piece[0][1] = tetromino.color;
            tetromino.piece[1][1] = tetromino.color;
            tetromino.piece[1][2] = tetromino.color;
            break;
        case 'L':
            tetromino.piece[0][2] = tetromino.color;
            tetromino.piece[1][0] = tetromino.color;
            tetromino.piece[1][1] = tetromino.color;
            tetromino.piece[1][2] = tetromino.color;
            break;
        case 'J':
            tetromino.piece[0][0] = tetromino.color;
            tetromino.piece[1][0] = tetromino.color;
            tetromino.piece[1][1] = tetromino.color;
            tetromino.piece[1][2] = tetromino.color;
            break;
        case 'T':
            tetromino.piece[0][1] = tetromino.color;
            tetromino.piece[1][0] = tetromino.color;
            tetromino.piece[1][1] = tetromino.color;
            tetromino.piece[1][2] = tetromino.color;
            break;
        }
    }
}

std::vector<Tetromino> GameManager::generateBag()
{
    std::vector<Tetromino> bag{tetrominoes};

    std::random_device rd;
    std::mt19937 mt(rd());
    std::shuffle(bag.begin(), bag.end(), mt);
    return bag;
}

bool GameManager::tryRotate(Tetromino &currentTetromino, const Tetromino &rotatedPiece)
{
    const int startRot = currentTetromino.rotationIndex;
    const int endRot = rotatedPiece.rotationIndex;
    if (rotatedPiece.id == 'I')
    {
        const int nextRotCW = (startRot + 1) % 4;
        const bool isCW = (endRot == nextRotCW);
        const auto &kickTable = isCW ? kickTableICW : kickTableICCW;
        for (int i = 0; i < 5; i++)
        {
            const int deltaX = kickTable[startRot][i].x;
            const int deltaY = kickTable[startRot][i].y;

            if (isValidPosition(rotatedPiece, deltaX, deltaY))
            {
                currentTetromino = rotatedPiece;
                currentTetromino.pos.x += deltaX;
                currentTetromino.pos.y += deltaY;
                return true;
            }
        }
    }
    else
    {
        for (int i = 0; i < 5; i++)
        {
            const int deltaX = offsetData[startRot][i].x - offsetData[endRot][i].x;
            const int deltaY = offsetData[startRot][i].y - offsetData[endRot][i].y;
            if (isValidPosition(rotatedPiece, deltaX, deltaY))
            {
                currentTetromino = rotatedPiece;
                currentTetromino.pos.x += deltaX;
                currentTetromino.pos.y += deltaY;
                return true;
            }
        }
    }
    return false;
}

std::optional<Tetromino> GameManager::newTetromino(const Tetromino &tetromino)
{
    Tetromino temp{tetromino};
    temp.initializePosition();

    for (int i = 0; i < temp.squareSize; i++)
    {
        for (int j = 0; j < temp.squareSize; j++)
        {
            int gridX = temp.pos.x + j;
            int gridY = temp.pos.y + i;
            if (gridY >= 0 && gridY < GRID_HEIGHT && gridX >= 0 && gridX < GRID_WIDTH)
            {
                if (screenState[gridY][gridX] != EMPTY)
                {
                    return std::nullopt;
                }
            }
        }
    }

    return temp;
}

bool GameManager::isValidPosition(const Tetromino &tetromino, int8_t deltaX, int8_t deltaY)
{
    for (int i = 0; i < tetromino.squareSize; i++)
    {
        for (int j = 0; j < tetromino.squareSize; j++)
        {
            if (tetromino.piece[i][j] != EMPTY)
            {
                const int8_t gridX = tetromino.pos.x + j + deltaX;
                const int8_t gridY = tetromino.pos.y + i + deltaY;

                if (gridX < 0 || gridX >= GRID_WIDTH)
                {
                    return false;
                }
                if (gridY >= GRID_HEIGHT)
                {
                    return false;
                }
                if (gridY >= 0 && screenState[gridY][gridX] != EMPTY)
                {
                    return false;
                }
            }
        }
    }
    return true;
}
bool GameManager::isGrounded(const Tetromino &tetromino)
{
    Tetromino next = tetromino;
    next.pos.y++;
    return !isValidPosition(next);
}

void GameManager::handleCollision(const Tetromino &tetromino)
{
    for (int i = 0; i < tetromino.squareSize; i++)
    {
        for (int j = 0; j < tetromino.squareSize; j++)
        {
            if (tetromino.piece[i][j] != EMPTY)
            {
                int gridX{tetromino.pos.x + j};
                int gridY{tetromino.pos.y + i};

                if (gridY >= 0 && gridY < GRID_HEIGHT && gridX >= 0 && gridX < GRID_WIDTH)
                    screenState[gridY][gridX] = tetromino.piece[i][j];
            }
        }
    }
    canHold = true;
}

void GameManager::handleWreck(Tetromino &tetromino, std::vector<Tetromino> &bag)
{
    std::optional<Tetromino> nextTetromino{newTetromino(bag[0])};
    if (!nextTetromino)
    {
        for (int i = 0; i < GRID_HEIGHT; i++)
        {
            for (int j = 0; j < GRID_WIDTH; j++)
            {
                screenState[i][j] = EMPTY;
            }
        }
        bag = generateBag();
        nextTetromino = newTetromino(bag[0]);
        score = 0;
        level = 1;
        canHold = true;
        hasHeld = false;
        themeMusic.setPlayingOffset(sf::seconds(1.0f));
        heldTetromino = Tetromino();
    }
    if (nextTetromino)
    {
        tetromino = *nextTetromino;
        bag.erase(bag.begin());
    }
}

bool GameManager::holdTetromino(Tetromino &tetromino, std::vector<Tetromino> &bag)
{
    if (!canHold)
        return false;
    canHold = false;

    if (!hasHeld)
    {
        heldTetromino = tetromino;
        tetromino = *newTetromino(*bag.begin());
        bag.erase(bag.begin());
        hasHeld = true;
    }
    else
    {
        Tetromino temp = heldTetromino;
        heldTetromino = tetromino;
        tetromino = temp;
    }
    heldTetromino.initializePosition();
    // rotate till its at the default rotation
    while (heldTetromino.rotationIndex != 0)
    {
        heldTetromino = heldTetromino.rotatedCCW();
    }
    return true;
}

void GameManager::clearRows()
{
    int writeRow{GRID_HEIGHT - 1};
    int rowsCleared{};
    for (int i = GRID_HEIGHT - 1; i >= 0; i--)
    {
        bool fullRow{true};
        for (int j = 0; j < GRID_WIDTH; j++)
        {
            if (screenState[i][j] == EMPTY)
            {
                fullRow = false;
                break;
            }
        }

        if (!fullRow)
        {

            if (i != writeRow)
            {
                for (int j = 0; j < GRID_WIDTH; j++)
                {
                    screenState[writeRow][j] = screenState[i][j];
                }
            }
            writeRow--;
        }
        else
        {
            rowsCleared++;
        }
    }
    for (int i = writeRow; i >= 0; i--)
    {
        for (int j = 0; j < GRID_WIDTH; j++)
        {
            screenState[i][j] = EMPTY;
        }
    }
    switch (rowsCleared)
    {
    case 1:
        score += 40;
        break;
    case 2:
        score += 100;
        break;
    case 3:
        score += 300;
        break;
    case 4:
        score += 1200;
        break;
    }
    if (rowsCleared > 0)
        level = (score / 500) + 1;
}
