#include "game.hpp"

#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <ctime>

int main()
{
    std::ofstream g_errorLog("error_log.log", std::ios::app);

    if (g_errorLog.is_open())
    {
        std::cerr.rdbuf(g_errorLog.rdbuf());
    }
    else
    {
        std::cerr << "WARNING! Unable to open error log file.\n";
    }
    try
    {
        Game game;
        game.run();
    }
    catch (const std::runtime_error &e)
    {
        auto except_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::cerr << std::put_time(std::localtime(&except_time), "%Y-%m-%d\t%H:%M:%S") << "\tRUNTIME ERROR: " << e.what();
        return 1;
    }
    catch (...)
    {
        std::cerr << "An unknown error occurred.\n";
        return 1;
    }
    return 0;
}
