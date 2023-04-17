/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malord <malord@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/14 14:49:03 by malord            #+#    #+#             */
/*   Updated: 2023/04/17 09:14:43 by malord           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

Location::Location(void) : _settings() {
    //std::cout << "Location default constructor called" << std::endl;
    // THIS will need to be reviewed
    _settings.push_back("proxy_pass");
    _settings.push_back("root");
    _settings.push_back("alias");
    _settings.push_back("try_files");
    _settings.push_back("rewrite");
    _settings.push_back("proxy_set_header");
    _settings.push_back("proxy_pass_header");
    _settings.push_back("proxy_redirect");
    _settings.push_back("expires");
    _settings.push_back("auth_basic");
    _settings.push_back("allow and deny");
    _settings.push_back("fastcgi_pass");
    _settings.push_back("uwsgi_pass");
    _settings.push_back("scgi_pass");
    _settings.push_back("grpc_pass");
    
}

Location::Location(const Location &copy) // : To update for the initializer list
{
    //std::cout << "Location copy constructor called" << std::endl;
    *this = copy;
}

Location &Location::operator=(const Location &rhs)
{
    //std::cout << "Location assignment operator called" << std::endl;
    if (this != &rhs)
    {
        // do what needs to be done
    }
    return (*this);
}

Location::~Location(void)
{
    //std::cout << "Location default destructor called" << std::endl;
}



/*
KEY WORDS FOR LOCATION : 

proxy_pass: Specifies the backend server to which requests should be forwarded when using NGINX as a reverse proxy.

root: Sets the root directory for serving files for the location.

alias: Specifies an alternative path for serving files for the location, which can be used to hide the actual file system path.

try_files: Defines a series of files or URIs to try in case of a request that matches the location pattern, allowing for fallbacks or redirects.

rewrite: Specifies URL rewriting rules, allowing for manipulation of the requested URL before it is processed.

proxy_set_header: Sets custom HTTP headers to be passed to the backend server when proxying requests.

proxy_pass_header: Specifies which HTTP headers from the backend server should be passed to the client in the response.

proxy_redirect: Configures URL redirection rules for responses from the backend server.

expires: Sets the caching behavior and expiration time for responses from the location.

auth_basic: Enables basic HTTP authentication for requests that match the location pattern.

allow and deny: Specifies IP addresses or address ranges that are allowed or denied access to the location.

fastcgi_pass: Specifies the backend FastCGI server to which requests should be forwarded for processing.

uwsgi_pass: Specifies the backend uWSGI server to which requests should be forwarded for processing.

scgi_pass: Specifies the backend SCGI server to which requests should be forwarded for processing.

grpc_pass
*/