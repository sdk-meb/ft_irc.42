/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utilsServer.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: blind-eagle <blind-eagle@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/23 03:22:18 by blind-eagle       #+#    #+#             */
/*   Updated: 2023/02/24 17:30:49 by blind-eagle      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./Server.hpp"

std::string     Server::getCurrentDate(){
    // Get the current time
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

    // Convert to a time_t object
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);

    // Convert to a string representation of the local time
    char buf[80];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", std::localtime(&now_c));

    // Return the current date as a string
    return std::string(buf);
}



bool        Server::checkIfNickNameAlreadyUsed(std::string nickName) const{
    std::vector<User>::const_iterator it;

    for (it = _users.begin() ; it != _users.end(); it++){
        if (it->getNickName() == nickName)
            return (true);
    }
    return (false);
}