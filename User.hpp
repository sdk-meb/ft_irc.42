/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: blind-eagle <blind-eagle@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/05 02:16:10 by blind-eagle       #+#    #+#             */
/*   Updated: 2023/02/22 01:05:23 by blind-eagle      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef  USER_HPP
# define USER_HPP

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <poll.h>


class   User{
    private :
        std::string     _userName;
        std::string     _realName;
        std::string     _hostName;
        std::string     _nickName;
        std::string     _password;
        std::string     _defaultPasswd;
        pollfd          _pollFds;
        bool            _isAuthenticated;
        std::string     _pollExtractedData;
    public  :
        User();
        User(pollfd newUserByFd);
        User(const User& user);
        ~User();
        
        User& operator=(const User& obj);

    // -setters-:

    void  setUserName(std::string userName);
    void  setRealName(std::string realName);
    void  setHostName(std::string hostName);
    void  setNickName(std::string nickName);
    void  setPassword(std::string passwd);
    void  setDefaultPasswd(std::string passwd);
    void  setPollExtractedData(std::string msg);

    // -getters- :

    std::string     getUserName() const;
    std::string     getRealName() const;
    std::string     getHostName() const;
    std::string     getNickName() const;
    std::string     getDefaultPasswd() const;
    std::string     getPollExtractedData() const;
    pollfd          getPollFds() const; 
    
    void    joinPollExtractedData(char *data);
    
    bool    isAuthenticated() const;
    bool    Authenticate(std::string serverPasswd);
};


#endif