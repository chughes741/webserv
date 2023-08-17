#pragma once

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <regex>
#include <stdexcept>
#include <vector>

#include "webserv.hpp"

// Context Settings
#define GLOBAL 0
#define EVENTS 1
#define HTTP   2
#define SERVER 3
// Global Settings
#define ERROR_LOG        0
#define PID              1
#define WORKER_PROCESSES 2
// Events Settings
#define WORKER_CONNECTIONS 0
// Http Settings
#define INDEX           0
#define ERROR_PAGE_H    1
// Server Settings
#define LISTEN          0
#define SERVER_NAME     1
#define ERROR_PAGE_S    2
#define ROOT            3
#define LOCATION        4
// Location Settings
#define PATH            0
#define FASTCGI         1
#define ERROR_PAGE_L    2

/** @todo figure out why this aren't working with includes from webserv.hpp */
struct HttpConfig;

/**
 * @brief Parse a config file. Read the file line by line and split into tokens.
 *
 * @param config_file	[in] Path to the config file
 * @param httpConfig	[in][out] Struct to store the config settings
 */
void parseConfig(std::string config_file, HttpConfig &httpConfig);

/**
 * @brief Parse the config file
 */
class Parser {
   public:
    Parser(HttpConfig &);
    ~Parser();

    void tokenizeConfig(std::string);
    /**
     * @brief 	 Increment iterator and validate first token of settings (not ;)
     */
    void validateFirstToken(std::string);
    /**
     * @brief 	 Increment iterator and validate last token of settings (is ;)
     */
    void validateLastToken(std::string);
    void initSettings();

    /**
     * @brief Call the appropriate context setting function
     */
    bool contextSwitchCase(int context);

    /**
     * @brief define in which context the token is and initialize the context
     */
    bool setContext();
    int getSetting(std::string settingsList[], int size);

    bool setGlobalSetting();
    bool setWorkerProcesses();
    bool setErrorLog();

    /** @todo determine if we accept/need a pid value in global context */
    bool setPid();
    bool setEventsContext();
    bool setEventsSetting();

    /** @todo determine where we put the worker connections setting
     * @brief Set the Worker Connections object
     */
    bool setWorkerConnections();
    bool setHttpContext();
    bool setHttpSetting();
    bool setHttpErrorPage();

    /** @todo determine where we put the index setting
     * @brief Set the index pages of the server
     */
    bool setIndex();

    bool setServerContext();
    bool setServerSetting();
    bool setListen();
    int retrievePort(std::string);
    bool isValidIPAddress(const std::string &ip);
    bool setServerName();
    bool setServerErrorPage();
    bool setRoot();

    bool setLocationSetting(std::string uri);
    bool setLocationUri();
    bool setPath(std::string &);
    bool setFastCGI(std::string &);
    bool setLocationErrorPage(std::string &);

   private:
    std::vector<std::string>           tokens;
    std::vector<int>                   context;
    std::vector<std::string>::iterator it;
    HttpConfig                         &httpConfig;
};

