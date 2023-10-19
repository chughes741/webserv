#pragma once

#include <stdio.h>

#include <ctime>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <string>

class Logger {
   public:
    virtual ~Logger();
    virtual void   log(const std::string &message) = 0;
    static Logger &instance();
};

class ConsoleLogger : public Logger {
   public:
    void log(const std::string &message);
};

class FileLogger : public Logger {
   public:
    FileLogger(const std::string &filename);
    ~FileLogger();

    void log(const std::string &message);

   private:
    std::ofstream log_file_;
};

std::string getTime();
std::string getCurrentTimestamp();
