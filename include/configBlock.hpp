
#pragma once

#include <iostream>
#include <vector>

// Template
class ConfigBlock {
   public:
    // Default constructor (Required)
    ConfigBlock();
    // Copy constructor (Required)
    ConfigBlock(const ConfigBlock &copy);
    // Constructors
    // Default Destructor (Required)
    ~ConfigBlock();
    // Destructors
    // Copy assignment operator (Required)
    ConfigBlock &operator=(const ConfigBlock &copy);
    // Operators
    // Swap function
    // Setters & Getters
    // Other functions
    bool validateType();  // from enum launch pointer function to validate type
   protected:
   private:
    // Enum blockType? Events;http;server;location
    std::string              blockType;
    std::vector<ConfigBlock> asda

    // All configurations available
    // server_name ...;
};

/**
 * @brief Configuration options for a single location block
 */
struct LocationConfig {
    std::string path;       /**< Path to match requests against */
    std::string root;       /**< Root directory for serving files */
    std::string index_file; /**< Name of the index file */
    std::string cgi_path;   /**< Path to the CGI program */
};

/**
 * @brief Configuration for a single server block
 */
struct ServerConfig {
    int                         port;           /**< Port number */
    std::string                 server_name;    /**< Server name */
    std::string                 error_pages[5]; /**< Error pages */
    std::vector<LocationConfig> locations;      /**< List of location blocks */
};

/**
 * @brief Configuration for an events block
 */
struct EventsConfig {
    int worker_connections; /**< Maximum number of connections per worker */
    std::string use;        /**< I/O multiplexing method */
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
    int          worker_processes; /**< Number of worker processes to spawn */
    std::string  error_log;        /**< Path to the error log file */
    std::string  pid_file;         /**< Path to the PID file */
    EventsConfig events;           /**< Events block */
    std::vector<ServerConfig> servers; /**< List of server blocks */
};
