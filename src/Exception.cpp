/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Exception.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malord <malord@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/14 13:52:11 by malord            #+#    #+#             */
/*   Updated: 2023/04/14 14:01:58 by malord           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Exception.hpp"

const char *WebExcep::WrongParameter::what() const throw() {
    return ("Wrong parameter");
}