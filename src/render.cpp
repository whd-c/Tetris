#include "render.hpp"

void Render::drawHeldTetromino(const Tetromino &tetromino)
{
    const float previewBoxX{startX + GRID_WIDTH * CELL_SIZE - CELL_SIZE * 19};
    const float previewBoxY{startY + CELL_SIZE * 5};
    const float previewBoxSize{CELL_SIZE * 6};

    auto boxBg{sf::RectangleShape({previewBoxSize, previewBoxSize})};
    boxBg.setPosition({previewBoxX, previewBoxY});
    boxBg.setFillColor(enumToColor(EMPTY));
    boxBg.setOutlineColor(sf::Color::White);
    boxBg.setOutlineThickness(3.0f);
    window.draw(boxBg);

    auto label{sf::Text(roboto, "HOLD", 36)};
    label.setPosition({previewBoxX + 75, previewBoxY - 50});
    window.draw(label);

    auto rectangle{sf::RectangleShape({COLOR_SIZE, COLOR_SIZE})};

    const float pieceWidth{tetromino.squareSize * CELL_SIZE};
    const float pieceHeight{tetromino.squareSize * CELL_SIZE};

    const float offsetX{previewBoxX + (previewBoxSize - pieceWidth) / 2.0f};
    const float offsetYDenominator{(tetromino.id != 'O') ? 1.5f : 2.0f};
    const float offsetY{previewBoxY + (previewBoxSize - pieceHeight) / offsetYDenominator};

    for (int i = 0; i < tetromino.squareSize; i++)
    {
        for (int j = 0; j < tetromino.squareSize; j++)
        {
            if (tetromino.piece[i][j] == EMPTY)
                continue;
            const float posX{offsetX + j * CELL_SIZE};
            const float posY{offsetY + i * CELL_SIZE};

            rectangle.setPosition({posX, posY});
            rectangle.setFillColor(enumToColor(tetromino.color));
            rectangle.setOutlineThickness(RECTANGLE_OUTLINE_SIZE);
            rectangle.setOutlineColor(enumToColor(DARK_PURPLE));
            window.draw(rectangle);
        }
    }
}

void Render::drawTetromino(const Tetromino &tetromino)
{
    auto rectangle{sf::RectangleShape({COLOR_SIZE, COLOR_SIZE})};
    for (int i = 0; i < tetromino.squareSize; i++)
    {
        for (int j = 0; j < tetromino.squareSize; j++)
        {
            if (tetromino.piece[i][j] == EMPTY || tetromino.pos.y + i < 0)
                continue;
            const float posX{startX + (tetromino.pos.x + j) * CELL_SIZE};
            const float posY{startY + (tetromino.pos.y + i) * CELL_SIZE};
            rectangle.setPosition({posX, posY});
            rectangle.setFillColor(enumToColor(tetromino.color));
            if (tetromino.color != TRANSPARENT)
            {
                rectangle.setOutlineThickness(RECTANGLE_OUTLINE_SIZE);
                rectangle.setOutlineColor(enumToColor(DARK_PURPLE));
            }

            window.draw(rectangle);
        }
    }
}

void Render::drawNextTetromino(const Tetromino &tetromino)
{
    const float previewBoxX{startX + GRID_WIDTH * CELL_SIZE + CELL_SIZE * 3};
    const float previewBoxY{startY + CELL_SIZE * 5};
    const float previewBoxSize{CELL_SIZE * 6};

    auto boxBg{sf::RectangleShape({previewBoxSize, previewBoxSize})};
    boxBg.setPosition({previewBoxX, previewBoxY});
    boxBg.setFillColor(enumToColor(EMPTY));
    boxBg.setOutlineColor(sf::Color::White);
    boxBg.setOutlineThickness(3.0f);
    window.draw(boxBg);

    auto label{sf::Text(roboto, "NEXT", 36)};
    label.setPosition({previewBoxX + 75, previewBoxY - 50});
    window.draw(label);

    auto rectangle{sf::RectangleShape({COLOR_SIZE, COLOR_SIZE})};

    const float pieceWidth{tetromino.squareSize * CELL_SIZE};
    const float pieceHeight{tetromino.squareSize * CELL_SIZE};

    const float offsetX{previewBoxX + (previewBoxSize - pieceWidth) / 2.0f};
    const float offsetYDenominator{(tetromino.id != 'O') ? 1.5f : 2.0f};
    const float offsetY{previewBoxY + (previewBoxSize - pieceHeight) / offsetYDenominator};

    for (int i = 0; i < tetromino.squareSize; i++)
    {
        for (int j = 0; j < tetromino.squareSize; j++)
        {
            if (tetromino.piece[i][j] == EMPTY)
                continue;
            const float posX{offsetX + j * CELL_SIZE};
            const float posY{offsetY + i * CELL_SIZE};

            rectangle.setPosition({posX, posY});
            rectangle.setFillColor(enumToColor(tetromino.color));
            rectangle.setOutlineThickness(RECTANGLE_OUTLINE_SIZE);
            rectangle.setOutlineColor(enumToColor(DARK_PURPLE));
            window.draw(rectangle);
        }
    }
}

void Render::drawText(sf::Text &text, std::string content, float posX, float posY)
{
    text.setPosition({posX, posY});
    text.setString(content);
    window.draw(text);
}

void Render::drawGrid(std::array<std::array<Color, GRID_WIDTH>, GRID_HEIGHT> screenState)
{
    constexpr float TOTAL_GRID_WIDTH{GRID_WIDTH * CELL_SIZE};
    constexpr float TOTAL_GRID_HEIGHT{GRID_HEIGHT * CELL_SIZE};

    static const float START_X{(TARGET_WIDTH - TOTAL_GRID_WIDTH) / 2.0f};
    static const float START_Y{(TARGET_HEIGHT - TOTAL_GRID_HEIGHT) / 2.0f};

    startX = START_X;
    startY = START_Y;

    auto gridBg{sf::RectangleShape({TOTAL_GRID_WIDTH, TOTAL_GRID_HEIGHT})};
    gridBg.setPosition({START_X, START_Y});
    gridBg.setFillColor(enumToColor(EMPTY));
    gridBg.setOutlineColor(sf::Color::White);
    gridBg.setOutlineThickness(3.0f);
    window.draw(gridBg);

    auto rectangle{sf::RectangleShape({COLOR_SIZE, COLOR_SIZE})};
    for (int i = 0; i < GRID_HEIGHT; i++)
    {
        for (int j = 0; j < GRID_WIDTH; j++)
        {
            if (screenState[i][j] == EMPTY)
                continue;
            const float posX{START_X + j * CELL_SIZE};
            const float posY{START_Y + i * CELL_SIZE};

            rectangle.setPosition({posX, posY});
            rectangle.setFillColor(enumToColor(screenState[i][j]));
            rectangle.setOutlineThickness(RECTANGLE_OUTLINE_SIZE);
            rectangle.setOutlineColor(enumToColor(DARK_PURPLE));
            window.draw(rectangle);
        }
    }
}