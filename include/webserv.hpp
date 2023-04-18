#pragma once

#include <sys/socket.h>
#include <unistd.h>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <exception>

#include "Exception.hpp"
#include "config.hpp"
#include "events.hpp"

#define OUT 0
#define EVENTS 1
#define HTTP 2
#define SERVER 3
