#include "logger.h"

outFile::outFile(const std::string &filename) : out_(filename, std::ios::app)
{
}

bool outFile::write(Level level, const std::string &msg, const std::string &timestamp)
{
    if (!out_.is_open()) return false;
    out_ << "[" << timestamp << "]" << "[" << toString(level) << "]" << msg << "\n";
    return true;
}

std::string outFile::toString(Level lvl)
{
    switch (lvl)
    {
    case Level::Regular: return "REGULAR";
    case Level::Warning: return "WARNING";
    case Level::Sanction: return "ERROR";
    }
    return "UNKNOWN";
}

Logger::Logger(std::unique_ptr<outDevice> device, Level level) : device_(std::move(device)), threshold_(level)
{

}

bool Logger::log(const std::string &msg, Level level)
{
    std::lock_guard<std::mutex> lock(m_);
    if (level < threshold_) return false;
    return device_->write(level, msg, currentTime());
}

void Logger::setLevel(Level level)
{
    threshold_ = level;
}

Level Logger::getLevel() const
{
    return threshold_;
}

std::string Logger::getStrLevel()
{
    if (threshold_ == Level::Regular) return "REGULAR";
    if (threshold_ == Level::Warning) return "WARNING";
    if (threshold_ == Level::Sanction) return "ERROR";
    
    return "UNKNOWN";
}

std::string Logger::currentTime()
{
    auto now = std::chrono::system_clock::now();
    auto tt = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&tt);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}
