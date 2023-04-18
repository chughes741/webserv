/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Exception.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flahoud <flahoud@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/14 13:52:13 by malord            #+#    #+#             */
/*   Updated: 2023/04/18 12:09:34 by flahoud          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include <iostream>
#include <stdexcept>
#include <string>

class FileError : public std::exception {
public:
	FileError(const std::string& config_file) : 
		message("Error: cannot open file " + config_file) {}
	virtual const char *what() const throw() {
		return message.c_str();}
	virtual ~FileError() throw() {}
		
private:
	std::string message;
};


/* Old Exceptions */
class WebExcep : public std::exception {
   public:
    class WrongParameter : public std::exception{
       public:
        virtual const char *what() const throw();
		virtual ~WrongParameter() throw() {}
    };
	class FileError : public std::exception {
	public:
		FileError(const std::string&);
		virtual const char *what() const throw();
		virtual ~FileError() throw()  {}
		
	private:
		std::string message;
	};

	class UnknownDirective : public std::exception {
	public:
		UnknownDirective(const std::string&);
		virtual const char *what() const throw();
		virtual ~UnknownDirective() throw()  {}
		
	private:
		std::string directive;
	};

	class WrongSettingValue : public std::exception {
	public:
		WrongSettingValue(const std::string&);
		virtual const char *what() const throw();
		virtual ~WrongSettingValue() throw()  {}
		
	private:
		std::string setting;
	};

	class WrongContext : public std::exception {
	public:
		WrongContext(const std::string&);
		virtual const char *what() const throw();
		virtual ~WrongContext() throw()  {}
		
	private:
		std::string context;
	};
};
#endif