#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <mutex>

enum class LogLevel {Regular, Warning, Error};

struct ILogSink
{
    virtual ~ILogSink() = default;
    virtual bool write(LogLevel level, const std::string& msg, const std::string & timestamp) = 0;
};


class FileLogSink : public ILogSink 
{
    public:
    explicit FileLogSink(const std::string & filename);
    bool write(LogLevel level, const std::string & msg, const std::string & timestamp) override;



    private:
    std::ofstream out_;
    static std::string toString(LogLevel lvl);
};

class Logger
{
    public:
    Logger(std::unique_ptr<ILogSink> sink, LogLevel level);
    
    bool log(const std::string & msg, LogLevel level);
    void setLevel(LogLevel level);
    LogLevel getLevel() const;

    private:
    std::unique_ptr<ILogSink> sink_;
    LogLevel threshold_;
    mutable std::mutex m_;

    static std::string currentTime();


};