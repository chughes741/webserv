/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Exception.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flahoud <flahoud@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/14 13:52:13 by malord            #+#    #+#             */
/*   Updated: 2023/04/15 14:39:02 by flahoud          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include <iostream>
#include <stdexcept>
#include <string>

class WebExcep {
   public:
    class WrongParameter : public std::exception {
       public:
        virtual const char *what() const _NOEXCEPT;
		virtual ~WrongParameter() _NOEXCEPT {}
    };
	class FileError : public std::exception {
	public:
		FileError(const std::string& filename) :
			message("Error: cannot open file " + filename) {}
		virtual const char *what() const _NOEXCEPT {
			return message.c_str();
		}
		virtual ~FileError() _NOEXCEPT  {}
		
	private:
		std::string message;
	};
};
#endif