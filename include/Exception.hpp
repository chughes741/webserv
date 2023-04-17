/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Exception.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flahoud <flahoud@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/14 13:52:13 by malord            #+#    #+#             */
/*   Updated: 2023/04/17 10:21:11 by flahoud          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include <iostream>
#include <stdexcept>
#include <string>

class WebExcep : public std::exception {
   public:
    class WrongParameter {
       public:
        virtual const char *what() const throw();
		virtual ~WrongParameter() throw() {}
    };
	class FileError {
	public:
		FileError(const std::string& filename);
		virtual const char *what() const throw();
		virtual ~FileError() throw()  {}
		
	private:
		std::string message;
	};
};
#endif