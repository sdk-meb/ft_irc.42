/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: blind-eagle <blind-eagle@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/12 02:53:28 by blind-eagle       #+#    #+#             */
/*   Updated: 2023/02/23 18:30:29 by blind-eagle      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./Server.hpp"

//! - Commands :

void    Server::quit(User* user,std::string message){
    // - first version - :
    std::string quitMessage;

    if (message.empty())
        quitMessage = "Alert : " + user->getNickName() + " Is quitting the server now!" 
        ;
    else 
        quitMessage = message;
    std::cout << quitMessage << std::endl;
}

std::string     Server::repliesMessage(std::string errorStatus, User const *user) const {

    std::string  errorMsg;
    if (user->getNickName().empty())
        errorMsg = errorStatus + " ";
    else
        errorMsg = errorStatus + " " + user->getNickName() + " ";
    return (errorMsg);
}

void        Server::buildResponseToSend(User const * senderUser, User const * recipientUser, std::string data){
    std::string  dataToSend;

    dataToSend = generatePrefix(senderUser) + data;
    sendResponse(recipientUser->getPollFds(),dataToSend);
}

void    Server::welcome(User * user){
    buildResponseToSend(NULL, user , repliesMessage("001", user) + "Welcome to the Internet Relay Network " + user->getNickName() + "!" + user->getUserName() + "@" + user->getHostName());
    buildResponseToSend(NULL, user, repliesMessage("002", user) + "Your host is " + _serverName + ", running version 13.37");
    buildResponseToSend(NULL, user, repliesMessage("003", user) + "This server was created " + getCurrentDate());
    buildResponseToSend(NULL, user, repliesMessage("004", user) + _serverName + " 13.37 i k nlov");
}

bool    Server::checkIfNickNameValid(std::string nickName) const{
    std::string str = nickName;
    int i;
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

// & ------------- Command : + PASS + ---------------------- 
    
void    Server::pass(User* user, std::string password){
    if (user->isAuthenticated())
       buildResponseToSend(NULL, user, repliesMessage("462", user) + ":You may not reregister");
    else if (password.empty())
        buildResponseToSend(NULL, user, repliesMessage("461", user) + "PASS :Not enough parameters");
    else if (password != this->_password)
        buildResponseToSend(NULL, user, repliesMessage("464", user) + ":Password incorrect");
    else{
            std::cout << "i Should do that !" << std::endl;
        user->setPassword(password);
        if (user->Authenticate(this->_password)){
            welcome(user);
        }
    }
}

// & ------------- Command : + NICK + ---------------------- 

void    Server::updateNickNameInChannels(std::string currentNickName, std::string newNickName){
    std::vector<Channel>::iterator  channel_it;
    for (channel_it = _channels.begin(); channel_it != _channels.end(); ++channel_it)
        channel_it->changeUserNickName(currentNickName, newNickName);
}

void    Server::nick(User* user, std::string nickName){
    std::vector<User>::const_iterator   it;
    
    if (nickName.empty())
        buildResponseToSend(NULL, user, repliesMessage("431", user) + ":No nickname given");
    else if (checkIfNickNameAlreadyUsed(nickName))
        buildResponseToSend(NULL, user, repliesMessage("433", user) + nickName + " :Nickname is already in use");
    else if (!checkIfNickNameValid(nickName))
        buildResponseToSend(NULL, user, repliesMessage("432", user) + nickName + " :Erroneous nickname");
    else{
        for (it = _users.begin(); it != _users.end(); ++it){
            if (user->isAuthenticated())
                buildResponseToSend(user, &*it, "Changed his nickname to " + nickName); // inform All the users that you changed your nickname
            }
        if (user->isAuthenticated())
            updateNickNameInChannels(user->getNickName(), nickName); // update the old nickname in the channels
        user->setNickName(nickName);
        if (!(user->isAuthenticated()) && user->Authenticate(_password))
            welcome(user);
    }
}

// & ------------- Command : + USER + ---------------------- 

void    Server::user(User* user, std::string userName, std::string hostName, std::string serverName, std::string realName){
    (void)serverName;
    if (user->isAuthenticated())
        buildResponseToSend(NULL, user, repliesMessage("462", user) + ":You may not reregister");
    else if (userName.empty() || hostName.empty() || serverName.empty() || realName.empty())
        buildResponseToSend(NULL, user, repliesMessage("461", user) + "USER :Not enough parameters");
    else{
        if (hostName == "*")
            user->setHostName("localhost");
        else
            user->setHostName(hostName);
        user->setUserName(userName);
        user->setRealName(realName);
        if (user->Authenticate(_password))
            welcome(user);
    }
}