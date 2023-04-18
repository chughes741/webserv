/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: flahoud <flahoud@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/14 14:31:22 by malord            #+#    #+#             */
/*   Updated: 2023/04/18 13:12:13 by flahoud          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_HPP
#define HTTP_HPP

#include <iostream>
#include <vector>

class HTTP {
    public:
        HTTP(void);
        HTTP(const HTTP &copy);
        HTTP &operator=(const HTTP &rhs);
        virtual ~HTTP(void);

        bool isSetting(std::string setting);
    protected:
    private: 
        std::vector<std::string> _settings;
};
#endif