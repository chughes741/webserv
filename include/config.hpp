
#pragma once

#include <string>
#include <vector>

#define CONFIG_FILE "../config/server.conf"

struct Location {
    bool        is_cgi;
    std::string path;
};

/**
 * @brief Configuration for a single server block
 */
struct ServerConfig {
    int                             port;           /**< Port number */
    std::string                     server_name;    /**< Server name */
    std::string                     error_pages[5]; /**< Error pages */
    std::map<std::string, Location> locations;      /**< List of locations */
};

/**
 * @brief Configuration for an events block
 */
struct EventsConfig {
    std::string use;   /**< I/O multiplexing method */
    bool multi_accept; /**< Whether to accept multiple connections at once */
    int  accept_mutex_delay; /**< Delay before accepting new connections */
    bool debug_connection;   /**< Whether to print debug information */
    bool use_poll;           /**< Whether to use poll() instead of select() */
    bool deferred_accept;    /**< Whether to defer accepting new connections */
};

/**
 * @brief Configuration for an HTTP server
 */
struct HttpConfig {
    std::string               error_log; /**< Path to the error log file */
    std::string               pid_file;  /**< Path to the PID file */
    EventsConfig              events;    /**< Events block */
    std::vector<ServerConfig> servers;   /**< List of server blocks */
};

bool parseConfig(std::string);