/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malord <malord@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/14 14:31:19 by malord            #+#    #+#             */
/*   Updated: 2023/04/14 14:41:26 by malord           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <algorithm>
#include <iostream>
#include <vector>

class Location {
   public:
    Location(void);
    Location(const Location &copy);
    Location& operator=(const Location &rhs);
    virtual ~Location(void);

   protected:
   private:
};
#endif