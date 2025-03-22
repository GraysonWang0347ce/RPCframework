#include "logger.hpp"
#include <time.h>
#include <iostream>

Logger *Logger::_ptr = nullptr;

Logger &Logger::get_instance()
{
    if (_ptr == nullptr)
    {
        _ptr = new Logger();
    }
    return *_ptr;
}

Logger::~Logger()
{
    if (_ptr)
    {
        delete _ptr;
        _ptr = nullptr;
    }
}

Logger::Logger()
{
    std::thread write_log([this]() -> void
                          {
                              while (true)
                              {
                                  // Get crrent date and write log into file
                                  time_t now = time(nullptr);
                                  tm *date = localtime(&now);
                                  int day = date->tm_mday;
                                  int month = date->tm_mon + 1;
                                  int year = date->tm_year + 1900;
                                  int hour = date->tm_hour;
                                  int min = date->tm_min;                         
                                  int second = date->tm_sec;
                                  char filename[512];
                                  sprintf(filename, "./log/%d-%d-%d-log.txt",
                                                    year, month, day);
                                  FILE *pf = fopen(filename, "a+");

                                  if (pf == nullptr)
                                  {
                                      std::cerr << "Logger opening file error! got " << filename << std::endl;
                                      exit(EXIT_FAILURE);
                                  }
                                  
                                  char time[1024] = {0};
                                  sprintf(time,"%d:%d:%d -> [%s] ",
                                    hour,min,second,this->_log_level.load() == LOG_LEVEL::INFO? "INFO":"ERROR");

                                  std::string msg = this->_blocked_q.pop_front();
                                  msg.insert(0,time);
                                  msg.append("\n");

                                  fputs(msg.c_str(), pf);
                                  fclose(pf);
                              } });

    write_log.detach();
}

void Logger::set_log_level(LOG_LEVEL l)
{
    _log_level.store(l);
}

void Logger::log(std::string info)
{
    _blocked_q.push_back(info);
}