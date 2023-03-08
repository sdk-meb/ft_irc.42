/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utilsServer.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bben-aou <bben-aou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/23 03:22:18 by blind-eagle       #+#    #+#             */
/*   Updated: 2023/03/08 11:45:03 by bben-aou         ###   ########.fr       */
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

bool    Server::checkIfNickNameValid(std::string nickName) const{
    std::string str = nickName;
    size_t i;
    if ( str.size() > 9 || str.empty())
        return (false);
    if (!(isalpha(str[0]) || ispunct(str[0])))
        return (false);
    for (i = 1 ; i < str.length() ; i++){
        if (!(isalpha(str[i]) || isdigit(str[i]) || ispunct(str[i]) || str[i] == '_' || str[i] == '-'))
            return (false);
    }
    return (true);
}


bool        Server::checkIfNickNameAlreadyUsed(std::string nickName) const{
    std::vector<User>::const_iterator it;

    for (it = _users.begin() ; it != _users.end(); it++){
        if (it->getNickName() == nickName)
            return (true);
    }
    return (false);
}
void    Server::sendResponse(pollfd fds, std::string data) const{
    std::string     prefix;
    std::string     response;
    int             prefixPos;
    prefixPos = 0;
    if (data[0] == ':'){
        prefixPos = 1;
        prefix = getWordInLine(data, &prefixPos);
        prefix = ":" + prefix; 
    }
    int i = prefixPos;
    while (data[i]){
        response += prefix;
        while (response.size() < 510 && data[i]){
            response += data[i];
            i++;
        }
        response += '\r';
        response += '\n';
        send(fds.fd, response.c_str(), response.size(),0);
        response.clear();
    }
}

std::string     Server::generatePrefix(User const *user) const{
    std::string  prefix = "";
    if (user == NULL){
        prefix = ":" + _serverName + " ";
        return (prefix);
    }
    prefix.append(":" + user->getNickName() + "!" + user->getUserName());
    prefix.append("@" + user->getHostName() + " ");
    return (prefix);
}

std::string     Server::repliesMessage(std::string errorStatus, User const *user) const {

    std::string  errorMsg;
    if (user->getNickName().empty())
        errorMsg = errorStatus + " ";
    else
        errorMsg = errorStatus + " " + user->getNickName() + " ";
    return (errorMsg);
}

void        Server::buildResponseToSend(User const * senderUser, User const * recipientUser, std::string data) const{
    std::string  dataToSend;
    dataToSend = generatePrefix(senderUser) + data;
    sendResponse(recipientUser->getPollFds(), dataToSend);
}

void    Server::buildResponseToSendToChanMembers(User const * senderUser, Channel const  & channel, std::string data) const{
    std::vector<std::string>::const_iterator it;
    for (it = channel.beginMem(); it != channel.endMem() ; ++it){
        if (senderUser->getNickName() != *it)
            sendResponse(getUser(*it)->getPollFds(),data);
    }
}

User *          Server::getUser(std::string nickName){
    std::vector<User>::iterator it;

    for (it = _users.begin() ; it != _users.end(); ++it){
        if (it->getNickName() == nickName)
            return (&*it);
    }
    return (NULL);
}

const User *    Server::getUser(std::string nickName) const{
    std::vector<User>::const_iterator it;

    for (it = _users.begin() ; it != _users.end(); ++it){
        if (it->getNickName() == nickName)
            return (&*it);
    }
    return (NULL);
}

void    Server::welcome(User * user){
    buildResponseToSend(NULL, user , repliesMessage("001", user) + "Welcome to the Internet Relay Network " + user->getNickName() + "!" + user->getUserName() + "@" + user->getHostName());
    buildResponseToSend(NULL, user, repliesMessage("002", user) + "Your host is " + _serverName + ", running version 13.37");
    buildResponseToSend(NULL, user, repliesMessage("003", user) + "This server was created " + getCurrentDate());
    buildResponseToSend(NULL, user, repliesMessage("004", user) + _serverName + " 13.37 i k nlov");
}

