#include "game.hpp"

int main()
{
    try
    {
        Game game;
        game.run();
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << "Runtime error: " << e.what() << "\n";
        return 1;
    }
    catch (...)
    {
        std::cerr << "An unknown error occurred.\n";
        return 1;
    }
    return 0;
}
