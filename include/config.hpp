#pragma once

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "http.hpp"

/**
 * @brief Configuration options for a single location block
 */
struct LocationConfig {
    /** Constructor, initializes to default values */
    LocationConfig()
        : client_max_body_size("1m"),
          error_page(),
          root("html"),
          index_file("index.html"),
          limit_except(GET | POST | DELETE),
          autoindex(false),
          cgi_enabled(false),
          cgi_path("") {}

    std::string                client_max_body_size; /**< Maximum size of a request body */
    std::map<int, std::string> error_page;           /**< Default error page */
    std::string                root;                 /**< Root directory for serving files */
    std::string                index_file;           /**< Name of the index file */
    unsigned short             limit_except;         /**< Allowed methods */
    bool                       autoindex;            /**< Enable autoindex */
    bool                       cgi_enabled;          /**< Enable CGI */
    std::string                cgi_path;             /**< Path to CGI programs */
};

/**
 * @brief Configuration for a single server block
 */
struct ServerConfig {
    ServerConfig()
        : server_names(),
          listen("", 80),
          root("html"),
          error_page(),
          client_max_body_size("1m"),
          locations() {}

    std::vector<std::string>    server_names;         /**< Server name */
    std::pair<std::string, int> listen;               /**< Address and port to listen on */
    std::string                 root;                 /**< Root directory for serving files */
    std::map<int, std::string>  error_page;           /**< Default error page */
    std::string                 client_max_body_size; /**< Maximum size of a request body */
    std::map<std::string, LocationConfig> locations;  /**< List of locations (path, location)*/
};

/** Configuration for an HTTP server */
struct HttpConfig {
    HttpConfig()
        : servers(),
          error_page(),
          error_log("error.log"),
          root("html"),
          client_max_body_size("1m") {}

    std::vector<ServerConfig>  servers;              /**< List of server blocks */
    std::map<int, std::string> error_page;           /**< Default error page */
    std::string                error_log;            /**< Path to the error log file */
    std::string                root;                 /**< Root directory for serving files */
    std::string                client_max_body_size; /**< Maximum size of a request body */
};

extern HttpConfig httpConfig;
