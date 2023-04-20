/**
 * @file httpServer.cpp
 * @author Francis L.
 * @author Marc-André L.
 * @author Cole H.
 * @version 0.1
 * @date 2023-04-19
 * @brief
 */

#include "httpServer.hpp"

/**
 * @brief Constructor
 *
 * @param config Configuration for the server
 */
HttpServer::HttpServer(ServerConfig config) : socket_(), config_(config) {}

/**
 * @brief Destructor
 */
HttpServer::~HttpServer() throw() {}

/**
 * @brief Start the server
 */
void HttpServer::start() {}
