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

void parseConfig(string, HttpConfig &);

class Parser {
   public:
    Parser(HttpConfig &);
    ~Parser();

    void tokenizeConfig(string);
    void validateFirstToken(string);
    void validateLastToken(string);
    void initSettings();
    bool contextSwitchCase(int);
    bool setContext();
    int  getSetting(string settingsList[], int size);

    bool   setGlobalSetting();
    bool   setEventsContext();
    bool   setEventsSetting();
    bool   setWorkerConnections();
    bool   setHttpContext();
    bool   setHttpSetting();
    bool   setServerContext();
    bool   setServerSetting();
    bool   setListen();
    int    retrievePort(string);
    bool   setServerName();
    bool   setAccessLog();
    bool   setRoot();
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
