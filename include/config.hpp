/**
 * @file config.hpp
 * @author Francis L.
 * @author Marc-André L.
 * @author Cole H.
 * @version 0.1
 * @date 2023-04-19
 * @brief
 *
 * Configuration options for the HTTP server:
 *
 * - syntax: http { ... }
 * - default:
 * - context: global
 *
 * - syntax: server { ... }
 * - default:
 * - context: http
 *
 * - syntax: server_name name ...;
 * - default: server_name "";
 * - context: server
 *
 * - syntax: listen [address][:port];
 * - default: listen *:80;
 * - context: server
 *
 * - syntax: location uri { ... }
 * - default:
 * - context: server
 *
 * - syntax: error_page code ... uri;
 * - default:
 * - context: http, server, location
 *
 * - syntax: client_max_body_size size;
 * - default: client_max_body_size 1m;
 * - context: http, server, location
 *
 * - syntax: root path;
 * - default: root html;
 * - context: http, server, location
 *
 * - syntax: limit_except method ...;
 * - default: limit_except GET POST DELETE;
 * - context: location
 *
 * - HTTP redirections?
 *
 * - Turn on or off directory listing?
 *
 * - Default file if request is a directory?
 *
 * - CGI based on certain file extensions?
 *
 * - Save location of uploaded files?
 *
 * Potential configuration options:
 * - aio (asynchronous I/O)
 * - alias (alias for a location)
 * - auth_delay (delay for 401 response, helps prevent brute force attacks)\
 * - internal (location can only be used for internal requests)
 * - output_buffers (number and size of buffers for reading response from disk)
 */

#pragma once

#define CONFIG_FILE "./config/server.conf"

#define BLOCK 1
#define SETTING 2

using std::map;
using std::pair;
using std::string;
using std::vector;

/**
 * @brief Configuration options for a single location block
 */
struct LocationConfig {
    /** Constructor, initializes to default values */
    LocationConfig()
        : client_max_body_size("1m"),
          error_page(404, ""),
          root("html"),
          index_file("index.html"),
          limit_except(GET | POST | DELETE),
          cgi_enabled(false),
          cgi_path("") {
    }

    string client_max_body_size;    /**< Maximum size of a request body */
    pair<int, string> error_page;   /**< Default error page */
    string            root;         /**< Root directory for serving files */
    string            index_file;   /**< Name of the index file */
    unsigned short    limit_except; /**< Allowed methods */
    bool              cgi_enabled;  /**< Enable CGI */
    string            cgi_path;     /**< Path to CGI programs */
};

/**
 * @brief Configuration for a single server block
 */
struct ServerConfig {
    /** Constructor, initializes to default values */
    ServerConfig()
        : server_name(""),
          listen("", 80),
          root("html"),
          error_page(404, ""),
          client_max_body_size("1m"),
          locations() {
    }

    string            server_name; /**< Server name */
    pair<string, int> listen;      /**< Address and port to listen on */
    string            root;        /**< Root directory for serving files */
    pair<int, string> error_page;  /**< Default error page */
    string client_max_body_size;   /**< Maximum size of a request body */
    map<string, LocationConfig>
        locations; /**< List of locations (path, location)*/
};

/**
 * @brief Configuration for an HTTP server
 */
struct HttpConfig {
    /** Constructor, initializes to default values */
    HttpConfig()
        : servers(),
          error_page(404, ""),
          error_log("error.log"),
          root("html"),
          client_max_body_size("1m") {
    }

    vector<ServerConfig> servers;    /**< List of server blocks */
    pair<int, string>    error_page; /**< Default error page */
    string               error_log;  /**< Path to the error log file */
    string               root;       /**< Root directory for serving files */
    string client_max_body_size;     /**< Maximum size of a request body */
};

extern HttpConfig httpConfig;


// void parseConfig(string);

// int	getNumCores();
// void setWorkerProcessses();
