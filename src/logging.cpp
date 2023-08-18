#include "logging.hpp"

Logger::~Logger() {}

Logger &Logger::instance() {
    static ConsoleLogger logger_instance;
    // static FileLogger logger_instance("logs/" + getCurrentTimestamp() + ".log");
    return logger_instance;
}

void ConsoleLogger::log(const std::string &message) {
    std::cout << "ConsoleLogger: " << message << std::endl;
}

FileLogger::FileLogger(const std::string &filename) {
    log_file_.open(filename, std::ios::out);
    if (!log_file_.is_open()) {
        std::cerr << "Failed to open log file" << std::endl;
    }
}

FileLogger::~FileLogger() {
    if (log_file_.is_open()) {
        log_file_.close();
    }
}

void FileLogger::log(const std::string &message) {
    if (log_file_.is_open()) {
        log_file_ << message << std::endl;
    }
}

std::string getCurrentTimestamp() {
    std::time_t       t   = std::time(0);
    std::tm          *now = std::localtime(&t);
    std::stringstream ss;

    ss << std::setfill('0') << (now->tm_year + 1900) << std::setw(2) << (now->tm_mon + 1)
       << std::setw(2) << now->tm_mday << "-" << std::setw(2) << now->tm_hour << "-" << std::setw(2)
       << now->tm_min << "-" << std::setw(2) << now->tm_sec;

    return ss.str();
}
