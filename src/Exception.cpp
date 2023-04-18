/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Exception.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flahoud <flahoud@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/14 13:52:11 by malord            #+#    #+#             */
/*   Updated: 2023/04/17 11:09:49 by flahoud          ###   ########.fr       */
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

WebExcep::UnknownDirective::UnknownDirective(const std::string& directive) :
	directive("Error: unknown directive \"" + directive + "\"") {}

const char *WebExcep::UnknownDirective::what() const throw() {
	return directive.c_str();
}

WebExcep::WrongSettingValue::WrongSettingValue(const std::string& setting) :
	setting("Error: wrong setting value for \"" + setting + "\"") {}

const char *WebExcep::WrongSettingValue::what() const throw() {
	return setting.c_str();
}


WebExcep::WrongContext::WrongContext(const std::string& context) :
	context("Error: wrong context block \"" + context + "\"") {}

const char *WebExcep::WrongContext::what() const throw() {
	return context.c_str();
}