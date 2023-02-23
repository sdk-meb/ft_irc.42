/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: blind-eagle <blind-eagle@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/05 02:16:12 by blind-eagle       #+#    #+#             */
/*   Updated: 2023/02/22 01:26:16 by blind-eagle      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./User.hpp"

User::User(){
    this->_userName = "";
    this->_realName = "";
    this->_hostName = "";
    this->_defaultPasswd = "";
    this->_isAuthenticated = false;
}

User::User(pollfd newUserByFd){
    this->_userName = "";
    this->_realName = "";
    this->_hostName = "";
    this->_defaultPasswd = "";
    this->_isAuthenticated = false;
    this->_pollFds.events  = newUserByFd.events;
    this->_pollFds.fd      = newUserByFd.fd;
    this->_pollFds.revents = newUserByFd.revents;
}

User::User(const User& user){
    this->_userName = user._userName;
    this->_realName = user._realName;
    this->_hostName = user._hostName;
    this->_nickName = user._nickName;
    this->_defaultPasswd = user._defaultPasswd;
    this->_isAuthenticated = user._isAuthenticated;
    this->_pollFds = user._pollFds;
    this->_pollFds.events = user._pollFds.events;
    this->_pollFds.fd = user._pollFds.fd;
    this->_pollFds.revents = user._pollFds.revents; 
}

User::~User(){};

User&   User::operator=(const User& obj){
    this->_userName = obj._userName;
    this->_realName = obj._realName;
    this->_hostName = obj._hostName;
    this->_nickName = obj._nickName;
    this->_defaultPasswd = obj._defaultPasswd;
    this->_isAuthenticated = obj._isAuthenticated;
    this->_pollFds = obj._pollFds;
    this->_pollFds.events = obj._pollFds.events;
    this->_pollFds.fd = obj._pollFds.fd;
    this->_pollFds.revents = obj._pollFds.revents; 
    return (*this);
}

// -setters- :

void    User::setUserName(std::string userName){
    this->_userName = userName;
}

void    User::setRealName(std::string realName){
    this->_realName = realName;
}

void    User::setHostName(std::string hostName){
    this->_hostName = hostName;
}

void    User::setNickName(std::string nickName){
    this->_nickName = nickName;
}

void    User::setPassword(std::string passwd){
    this->_password = passwd;
}

void    User::setPollExtractedData(std::string msg){
    this->_pollExtractedData = msg;
}
// -getters- :

std::string    User::getUserName() const{
    return(this->_userName);
}

std::string    User::getRealName() const{
    return (this->_realName);
}

std::string    User::getHostName() const{
    return (this->_hostName);
}

std::string    User::getNickName() const{
    return (this->_nickName);
}

std::string    User::getDefaultPasswd() const{
    return (this->_defaultPasswd);
}

std::string    User::getPollExtractedData() const{
    return (this->_pollExtractedData);
}

pollfd      User::getPollFds() const{
    return (this->_pollFds);
}

void    User::joinPollExtractedData(char *data){
    
    this->_pollExtractedData += data;
}

bool      User::isAuthenticated() const{
    return (this->_isAuthenticated);
}

bool     User::Authenticate(std::string serverPasswd){
    if (!(this->_userName.empty()) && !(this->_realName.empty()) && !(this->_nickName.empty()) && this->_password == serverPasswd){
        this->_isAuthenticated = true;
        return (true);
    }
    return (false);
}