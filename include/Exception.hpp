/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Exception.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malord <malord@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/14 13:52:13 by malord            #+#    #+#             */
/*   Updated: 2023/04/14 14:02:53 by malord           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include <iostream>
#include <stdexcept>
#include <string>

class WebExcep : public std::exception {
   public:
    class WrongParameter : public std::exception {
       public:
        virtual const char *what() const throw();
    };
};
#endif