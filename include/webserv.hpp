#pragma once

#include "http.hpp"  // Needs to be included before config.hpp
#include "config.hpp"
#include "events.hpp"
#include "parsing.hpp"
#include "server.hpp"
#include "socket.hpp"
#include "logging.hpp"

#define OUT    0
#define EVENTS 1
#define HTTP   2
#define SERVER 3

#ifndef CONFIG_FILE
#define CONFIG_FILE "./config/server.conf"
#endif
