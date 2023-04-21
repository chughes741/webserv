#pragma once

#include <sys/socket.h>
#include <unistd.h>

#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "config.hpp"
#include "events.hpp"
#include "socket.hpp"

#define OUT 0
#define EVENTS 1
#define HTTP 2
#define SERVER 3
