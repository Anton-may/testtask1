#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <mutex>

enum class Level
{
    Regular = 0,
    Warning = 1,
    Sanction = 2
};

struct outDevice
{
    virtual ~outDevice() = default;
    virtual bool write(Level level, const std::string &msg, const std::string &timestamp) = 0;
};

class outFile : public outDevice
{
public:
    explicit outFile(const std::string &filename);
    bool write(Level level, const std::string &msg, const std::string &timestamp) override;

private:
    //output
    std::ofstream out_;
    static std::string toString(Level lvl);
};

class Logger
{
public:
    Logger(std::unique_ptr<outDevice> device, Level level);

    bool log(const std::string &msg, Level level);
    void setLevel(Level level);
    Level getLevel() const;
    std::string getStrLevel();

private:
    std::unique_ptr<outDevice> device_;
    Level threshold_;
    mutable std::mutex m_;

    static std::string currentTime();
};