/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTP.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malord <malord@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/14 14:43:38 by malord            #+#    #+#             */
/*   Updated: 2023/04/14 15:18:38 by malord           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Http.hpp"

HTTP::HTTP(void) : settings() {
    //std::cout << "Default HTTP constructor called" << std::endl;
    //THIS will need to be reviewed
    _settings.push_back("include");
    _settings.push_back("default_type");
    _settings.push_back("log_format");
    _settings.push_back("access_log");
    _settings.push_back("sendfile");
    _settings.push_back("tcp_nopush");
    _settings.push_back("tcp_nodelay");
    _settings.push_back("keepalive_timeout");
    _settings.push_back("server");
    _settings.push_back("location");
    _settings.push_back("proxy_pass");
    _settings.push_back("gzip");
    _settings.push_back("resolver");
}

HTTP::HTTP(const HTTP &copy) : _settings(copy._settings)
{
    //std::cout << "Copy HTTP constructor called";
    *this = copy;
}

HTTP &HTTP::operator=(const HTTP &rhs) // : To be updated for initializer list
{
    //std::cout << "HTTP Operator assignment called" << std::endl;
    if (this != &rhs) {
        // do the assignment 
    }
    return (*this);
}
HTTP::~HTTP(void)
{
    //std::cout << "Default HTTP destructor called" << std::endl;
}

bool HTTP::isSetting(std::string setting)
{
    for (std::vector<std::string>::const_iterator it = _settings.begin(); it != _settings.end(); ++it)
    {
        if (setting == *it)
            return (true);
    }
    return (false);
}