#ifndef LOGGER_H
#define LOGGER_H

#include "blockedqueue.hpp"
#include <string>
#include <atomic>

enum class LOG_LEVEL
{
    INFO,
    ERROR,
};

// @brief Singleton class to log
class Logger
{
public:
    void set_log_level(LOG_LEVEL);

    void log(std::string);

    static Logger &get_instance();
    ~Logger();

    Logger(const Logger &) = delete;
    Logger(Logger &&) = delete;

    Logger &operator=(const Logger &) = delete;
    Logger &operator=(Logger &&) = delete;

private:
    Logger();
    static Logger *_ptr;
    std::atomic<LOG_LEVEL>  _log_level;
    BlockedQ<std::string> _blocked_q;
};

#define LOG_INFO(logmsgformat, ...)                     \
    do                                                  \
    {                                                   \
        Logger &logger = Logger::get_instance();        \
        logger.set_log_level(LOG_LEVEL::INFO);          \
        char c[1024] = {0};                             \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
        logger.log(c);                                  \
    } while (0);

#define LOG_ERROR(logmsgformat, ...)                    \
    do                                                  \
    {                                                   \
        Logger &logger = Logger::get_instance();        \
        logger.set_log_level(LOG_LEVEL::ERROR);         \
        char c[1024] = {0};                             \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
        logger.log(c);                                  \
    } while (0);

#endif