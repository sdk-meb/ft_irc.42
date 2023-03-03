/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: blind-eagle <blind-eagle@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/05 12:28:52 by blind-eagle       #+#    #+#             */
/*   Updated: 2023/03/03 17:03:19 by blind-eagle      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef  SERVER_HPP
# define SERVER_HPP

#include "./User.hpp"
#include "./Channel.hpp"
#include <chrono>
#include <ctime>
#include <string>
#include <unistd.h>
#include <string>
#include <iostream>
#include <vector>
#include <poll.h>
#include <sys/socket.h>
#include <sstream>
#include <netinet/in.h>
#include <fcntl.h>
#include <limits>
#include <cstdlib>
#include <cstring>
#include <cctype>

#define MAXUSERS 500

class   Server{
    private :
        std::string  _serverName;
        std::string  _password;
        int          _port;
        int          _AddressSize;
        sockaddr_in  _ServerAddress;
        
        std::vector<pollfd>  _fds;
        std::vector<User>    _users;
        std::vector<Channel> _channels;
        
        //! - Commands: 
        void    quit(User* user, std::string message);
        void    pass(User* user, std::string password);
        void    nick(User* user, std::string nickName);
        void    user(User* user, std::string userName, std::string hostName, std::string serverName, std::string realName);
        void    join(User* user, std::vector<std::string> & argsVector);
        void    part(User* user, std::vector<std::string> & chanArgs, std::string partReasonMessage);
        void    pong(User* user, std::string response) const;
        void    privmsg(User* user, std::vector<std::string> & targets, std::string message) const;
        void    notice(User* user, std::vector<std::string> & targets, std::string noticeMessage) const;
        void    list(User* user, std::vector<std::string> & channels) const;
        void    mode(User* user, std::string target, std::vector<std::string> modes);
        void    topic(User* user, std::string channel, std::string topic);
        void    kick(User *user, std::string channel, std::string target, std::string reason);
        void    names(User * user, std::string channel);

    public  :
        Server();
        Server(std::string serverName, int port, char *password);
        Server(Server const &server);
        ~Server();

        Server& operator=(Server const &obj);
    // -getters- :
    
    int                     getPort() const;
    std::string             getServerName() const;
    std::string             getPassword() const;
    std::vector<pollfd>     getFds();


    // -Main Methods- :

    void    establishListening();
    void    handlingEvents();
    int     itsCommand(std::string line);
    int     parser(std::string buffer, int index);
    
    std::string     getWordInLine(std::string line,int* posPointer) const;
    std::vector<std::string>     getVectorOfArgs(std::string line, int* posPointer);
    bool            checkCommandValidation(std::string  command);
    void            userNotLoginIn(User* user) const;
    void            buildResponseToSend(User const * senderUser, User const * recipientUser, std::string data) const;
    void            buildResponseToSendToChanMembers(User const * senderUser, Channel const & channel, std::string data) const;
    void            sendResponse(pollfd fds, std::string data) const;
    std::string     repliesMessage(std::string errorMsg, User const *user) const;
    std::string     generatePrefix(User const * user) const;
    
    void            welcome(User *user);
    std::string     getCurrentDate();
    
    //* - Nick Utils - :
    bool            checkIfNickNameAlreadyUsed(std::string nickName) const;
    bool            checkIfNickNameValid(std::string nickName) const;
    void            updateNickNameInChannels(std::string currentNickName, std::string newNickName);

    //* - Join utils - :
    bool    checkIfChannelNameValid(std::string channelName);
    const User *          getUser(std::string nickName) const;
    void                  listOfChannelMembers(Channel  & channel, User const * user) const;

    User    *            getUser(std::string nickName);
    //* - mode utils - :

    void    showChannelModes(User* user, Channel* channel);
    void    userModes(User* user, User* target, std::vector<std::string> modes);
    void    channelModes(User* user, Channel* channel, std::vector<std::string> modes);
};

#endif