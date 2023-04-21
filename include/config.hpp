#pragma once

#include <map>
#include <string>
#include <vector>

#define CONFIG_FILE "./config/server.conf"

#define BLOCK 1
#define SETTING 2

using std::map;
using std::string;
using std::vector;

/**
 * @brief Configuration options for a single location block
 */
struct Location {
    string root;       /**< Root directory for serving files */
    string index_file; /**< Name of the index file */
    string cgi_path;   /**< Path to the CGI program */
};

/**
 * @brief Configuration for a single server block
 */
struct ServerConfig {
    int                   port;           /**< Port number */
    string                server_name;    /**< Server name */
    string                error_pages[5]; /**< Error pages */
    map<string, Location> locations; /**< List of locations (path, location)*/
};

/**
 * @brief Configuration for an events block
 */
struct EventsConfig {
    string use;          /**< I/O multiplexing method */
    bool   multi_accept; /**< Whether to accept multiple connections at once */
    int    accept_mutex_delay; /**< Delay before accepting new connections */
    bool   debug_connection;   /**< Whether to print debug information */
    bool   use_poll;           /**< Whether to use poll() instead of select() */
    bool   deferred_accept; /**< Whether to defer accepting new connections */
};

/**
 * @brief Configuration for an HTTP server
 */
struct HttpConfig {
    string               error_log; /**< Path to the error log file */
    string               pid_file;  /**< Path to the PID file */
    EventsConfig         events;    /**< Events block */
    vector<ServerConfig> servers;   /**< List of server blocks */
};

extern HttpConfig httpConfig;


// void parseConfig(string);

// int	getNumCores();
// void setWorkerProcessses();
