#include "logger.h"

FileLogSink::FileLogSink(const std::string &filename) : out_(filename, std::ios::app)
{
}

bool FileLogSink::write(LogLevel level, const std::string &msg, const std::string &timestamp)
{
    if (!out_.is_open()) return false;
    out_ << "[" << timestamp << "]" << "[" << toString(level) << "]" << msg << "\n";
    return true;
}

std::string FileLogSink::toString(LogLevel lvl)
{
    switch (lvl)
    {
    case LogLevel::Regular: return "REGULAR";
    case LogLevel::Warning: return "WARNING";
    case LogLevel::Error: return "ERROR";
    }
    return "UNKNOWN";
}

Logger::Logger(std::unique_ptr<ILogSink> sink, LogLevel level) : sink_(std::move(sink)), threshold_(level)
{

}

bool Logger::log(const std::string &msg, LogLevel level)
{
    std::lock_guard<std::mutex> lock(m_);
    if (level < threshold_) return false;
    return sink_->write(level, msg, currentTime());
}

void Logger::setLevel(LogLevel level)
{
    threshold_ = level;
}

LogLevel Logger::getLevel() const
{
    return threshold_;
}

std::string Logger::currentTime()
{
    auto now = std::chrono::system_clock::now();
    auto tt = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&tt);
    std::ostringstream oss;
    oss << std::put_time(&tm, "&Y-%m-%d %H:%M:%S");
    return oss.str();
}
