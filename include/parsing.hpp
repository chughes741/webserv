#pragma once

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <regex>
#include <stdexcept>
#include <vector>

#include "config.hpp"

// Context Settings
#define GLOBAL 0
#define EVENTS 1
#define HTTP   2
#define SERVER 3

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

    bool setPid();
    bool setEventsContext();
    bool setEventsSetting();
    bool createOrValidateDirectory();
    bool setErrorPages(std::map<int, std::string> &);

    bool setWorkerConnections();
    bool setHttpContext();
    bool setHttpSetting();
    bool setHttpClientBodySize();
    bool setHttpUploadDirectory();

    bool setIndex();

    bool setServerContext();
    bool setServerSetting();
    bool setListen();
    bool setServerRedirect();
    int retrievePort(std::string);
    bool isValidIPAddress(const std::string &ip);
    bool setServerName();
    bool setServerRoot();
    bool setServerClientBodySize();
    bool setServerUploadDirectory();

    bool setLocationSetting(std::string uri);
    bool setLocationUri();
    bool setLocationRoot(std::string &);
    bool setFastCGI(std::string &);
    bool setAutoIndex(std::string &);
    bool setLocationRedirect(std::string &);
    bool setLimitExcept(std::string &);
    bool setLocationClientBodySize(std::string &);
    bool setLocationUploadDirectory(std::string &);

   private:
    std::vector<std::string>           tokens;
    std::vector<int>                   context;
    std::vector<std::string>::iterator it;
    HttpConfig                         &httpConfig;
};

