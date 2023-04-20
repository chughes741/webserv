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

#include <map>
#include <string>
#include <vector>

#include "Exception.hpp"
#include "webserv.hpp"

#define PRINT 0

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

void parseConfig(string);
