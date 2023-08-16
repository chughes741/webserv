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
#define INDEX 0
// Server Settings
#define LISTEN      0
#define SERVER_NAME 1
#define ACCESS_LOG  2
#define ROOT        3
#define LOCATION    4
// Location Settings
#define PATH    0
#define FASTCGI 1

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

    /**
     * @brief Tokenize a line of a config file.
     *
     * @param line		[in] Line to tokenize
     */
    void tokenizeConfig(std::string);
    /**
     * @brief 	 Increment iterator and validate first token of settings (not ;)
     */
    void validateFirstToken(std::string);
    /**
     * @brief 	 Increment iterator and validate last token of settings (is ;)
     */
    void validateLastToken(std::string);

    /**
     * @brief Initialize settings from token vector
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
     * @brief define in which context the token is and initialize the context
     */
    bool setContext();

    /**
     * @brief Get the Setting object
     *
     * @param settingsList Array of settings
     * @param size Size of settings array
     * @return int Index of setting in settings array
     */
    int getSetting(std::string settingsList[], int size);

    /**
     * @brief Set global context setting through a switch case of available
     * settings
     */
    bool setGlobalSetting();

    /**
     * @brief Set the Worker Processes setting
     */
    bool setWorkerProcesses();

    /**
     * @brief Set the error log setting for any setting
     */
    bool setErrorLog();

    /** @todo determine if we accept/need a pid value in global context */
    bool setPid();

    /**
     * @brief Set the Events Context and increment the token iterator
     */
    bool setEventsContext();

    /**
     * @brief Calls the approriate setter depending on the setting
     */
    bool setEventsSetting();

    /** @todo determine where we put the worker connections setting
     * @brief Set the Worker Connections object
     */
    bool setWorkerConnections();

    /**
     * @brief Set http context and increment the token iterator
     */
    bool setHttpContext();

    /**
     * @brief Set http setting
     */
    bool setHttpSetting();

    /** @todo determine where we put the index setting
     * @brief Set the index pages of the server
     */
    bool setIndex();

    /**
     * @brief Set the Server Context and increment the token iterator
     */
    bool setServerContext();

    /**
     * @brief Set server setting
     */
    bool setServerSetting();

    /**
     * @brief Transform the token into the listening port of the server
     * @return true or throw an exception
     */
    bool setListen();

    /**
     * @brief validate the value for port setting in listen
     */
    int retrievePort(std::string);

    /**
     * @brief validate the ip address for listen
     * @param ip the ip address to validate in string format
     */
    bool isValidIPAddress(const std::string &ip);

    /**
     * @brief Set the server name
     * @return true or throw invalid_argument
     */
    bool setServerName();

    bool setAccessLog();

    /**
     * @brief Set the Root setting for the server context
     */
    bool setRoot();

    /**
     * @brief Set http setting
     */
    bool setLocationSetting(std::string uri);
    bool setLocationUri();
    void setPath(std::string &);
    void setFastCGI(std::string &);

   private:
    std::vector<std::string>           tokens;
    std::vector<int>                   context;
    std::vector<std::string>::iterator it;
    HttpConfig                        &httpConfig;
};

