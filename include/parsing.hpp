#pragma once

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "webserv.hpp"

// Context Settings
#define GLOBAL 0
#define EVENTS 1
#define HTTP 2
#define SERVER 3
// Global Settings
#define WORKER_PROCESSES 0
#define ERROR_LOG 1
#define PID 2
// Events Settings
#define WORKER_CONNECTIONS 0
// Http Settings
#define INDEX 0
// Server Settings
#define LISTEN 0
#define SERVER_NAME 1
#define ACCESS_LOG 2
#define ROOT 3
#define LOCATION 4
// Location Settings
#define PATH 0
#define FASTCGI 1

using std::string;
using std::vector;

/** @todo figure out why this aren't working with includes from webserv.hpp */
struct HttpConfig;

/**
 * @brief Parse a config file. Read the file line by line and split into tokens.
 *
 * @param config_file	[in] Path to the config file
 * @param httpConfig	[out] Struct to store the config settings
 */
void parseConfig(string config_file, HttpConfig &httpConfig);

/**
 * @brief Parse the config file
 */
class Parser {
   public:
    Parser(HttpConfig &);
    ~Parser();

    /**
     * @brief Tokenize a line of a config file.
     *
     * @param line		[in] Line to tokenize
     */
    void tokenizeConfig(string);
    void validateFirstToken(string);
    void validateLastToken(string);

    /**
     * @brief Initialize settings from token vector
     *
     * @param tokens [in] Vector of tokens
     */
    void initSettings();

    /**
     * @brief Call the appropriate context setting function
     *
     * @param context to represent which context
     * @return true or throw logic_error
     */
    bool contextSwitchCase(int context);

    /**
     * @brief define in which context the token is
     */
    bool setContext();

    /**
     * @brief Get the Setting object
     *
     * @param settingsList Array of settings
     * @param size Size of settings array
     * @return int Index of setting in settings array
     */
    int getSetting(string settingsList[], int size);

    /**
     * @brief Set global context settings
     */
    bool setGlobalSetting();
    bool setEventsContext();
    bool setEventsSetting();
    bool setWorkerConnections();
    bool setHttpContext();

    /**
     * @brief Set http setting
     */
    bool setHttpSetting();
    bool setServerContext();

    /**
     * @brief Set server setting
     */
    bool setServerSetting();

    /**
     * @brief Transform the token into the listening port of the server
     *
     * @return true or throw an exception
     */
    bool setListen();

    /**
     * @brief validate the value for port setting in listen
     *
     * @param num the port value in string format
     * @return int if port value is all integer
     */
    int retrievePort(string);

    /**
     * @brief Set the server name
     *
     * @return true or throw invalid_argument
     */
    bool setServerName();

    bool setAccessLog();
    bool setRoot();

    /**
     * @brief Set http setting
     */
    bool   setLocationSetting();
    string setLocationUri();
    void   setPath(string &);
    void   setFastCGI(string &);

   private:
    vector<string>           tokens;
    vector<int>              context;
    vector<string>::iterator it;
    HttpConfig              &httpConfig;
};
