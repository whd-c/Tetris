#pragma once
#include <SFML/Audio.hpp>
#include <tetromino.hpp>
#include <algorithm>
#include <random>

class GameManager
{
public:
    GameManager(sf::Music &_themeMusic) : themeMusic(_themeMusic) {};

    void initializeTetrominoes();
    std::vector<Tetromino> generateBag();
    bool tryRotate(Tetromino &currentTetromino, const Tetromino &rotatedPiece);
    std::optional<Tetromino> newTetromino(const Tetromino &tetromino);
    bool isValidPosition(const Tetromino &tetromino, int8_t deltaX = 0, int8_t deltaY = 0);
    bool isGrounded(const Tetromino &tetromino);
    void handleCollision(const Tetromino &tetromino);
    void handleWreck(Tetromino &tetromino, std::vector<Tetromino> &bag);
    bool holdTetromino(Tetromino &tetromino, std::vector<Tetromino> &bag);
    void clearRows();

    std::array<std::array<Color, GRID_WIDTH>, GRID_HEIGHT> screenState{};

    int getScore() const { return score; }
    unsigned int getLevel() const { return level; }
    Tetromino getHeldTetromino() const { return heldTetromino; }

    void setScore(int _score) { score = _score; }
    void setLevel(unsigned int _level) { level = _level; }
    void setCanHold(bool _canHold) { canHold = _canHold; }
    void setHasHeld(bool _hasHeld) { hasHeld = _hasHeld; }
    void setHeldTetromino(const Tetromino &_heldTetromino) { heldTetromino = _heldTetromino; }

private:
    std::vector<Tetromino> tetrominoes = {
        {2, 'O', YELLOW},
        {4, 'I', CYAN},
        {3, 'S', GREEN},
        {3, 'Z', RED},
        {3, 'L', ORANGE},
        {3, 'J', BLUE},
        {3, 'T', PURPLE},
    };

    sf::Music &themeMusic;
    Tetromino heldTetromino;

    bool canHold{true};
    bool hasHeld{false};

    uint16_t level{1};
    uint32_t score{};
};