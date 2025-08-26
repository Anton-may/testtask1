#include <iostream>
#include "logger.h"
#include <optional>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>

struct Task
{
    std::string msg;
    std::optional<Level> lvl;
};

std::queue<Task> tasks;
std::mutex mtx;
std::condition_variable cv;
bool stop = false;

void worker(Logger & logger)
{
    while (true)
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] {return stop || !tasks.empty(); });

        if (stop && tasks.empty()) break;

        Task t = std::move(tasks.front());
        tasks.pop();
        lock.unlock();

        logger.log(t.msg, t.lvl.value_or(logger.getLevel()));
    }
}

std::optional<Level> parseLevel(const std::string &s)
{
    if ((s == "regular") || (s == "reg") || (s == "r")) return Level::Regular;
    if ((s == "warning") || (s == "warn") || (s == "w")) return Level::Warning;
    if ((s == "sanction") || (s == "sanc") || (s == "s")) return Level::Sanction;
    return std::nullopt;
}

int main (int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cerr << "Usage: app <logfile> <level>\n";
        return 1;
    }
    std::string logfile = argv[1];
    auto defaultLvl = parseLevel(argv[2]).value_or(Level::Regular);

    Logger logger(std::make_unique<outFile>(logfile), defaultLvl);
    
    std::thread t(worker, std::ref(logger));

    std::string line;
    std::cout << "Enter message: (press \"quit\" to end)\n";
    while (true)
    {
        if(!std::getline(std::cin, line)) break;
        if (line == "quit") break;
        if (line.rfind("setlevel", 0) == 0)
        {
            auto lvl = parseLevel(line.substr(9));
            if (lvl)
            {
                logger.setLevel(*lvl);
                std::cout << "Base log level changed to: " << logger.getStrLevel() << std::endl;
            }
            continue;
        }

        // работа расшифровки отправки сообщения
        // базовая обработка
        auto pos = line.find(':');
        std::optional<Level> lvl;
        std::string msg = line;

        // обработка по формату
        if(pos != std::string::npos)
        {
            auto prefix = line.substr(0, pos);
            auto parsed = parseLevel(prefix);
            if (parsed)
            {
                // готовый уровень
                lvl = parsed;
                // готовое сообщение
                msg = line.substr(pos + 1);
                if (!msg.empty() && msg[0] == ' ') msg.erase(0,1);
            }
        }

        {
            std::lock_guard<std::mutex> lock(mtx);
            tasks.push({msg, lvl});
        }
        cv.notify_one();
    }

    
    {
        std::lock_guard<std::mutex> lock(mtx);
        stop = true;
    }
    
    cv.notify_one();
    t.join();


    return 0;
}