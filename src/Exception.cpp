/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Exception.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flahoud <flahoud@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/14 13:52:11 by malord            #+#    #+#             */
/*   Updated: 2023/04/17 10:21:19 by flahoud          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Exception.hpp"

const char *WebExcep::WrongParameter::what() const throw() {
    return ("Wrong parameter");
}

WebExcep::FileError::FileError(const std::string& filename) :
	message("Error: cannot open file " + filename) {}

const char *WebExcep::FileError::what() const throw() {
	return message.c_str();
}