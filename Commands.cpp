/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: blind-eagle <blind-eagle@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/12 02:53:28 by blind-eagle       #+#    #+#             */
/*   Updated: 2023/02/28 17:25:50 by blind-eagle      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./Server.hpp"
#include "./Channel.hpp"
//! - Commands :

// & ------------- Command : + QUIT + ---------------------- 


void    Server::quit(User* user,std::string message){
    std::string quitMessage;
    std::vector<Channel>::iterator chanIt;

    if (message.empty())
        quitMessage = user->getNickName() + " is quitting the server now !";
    else
        quitMessage =  user->getNickName() + message;
    for (chanIt = _channels.begin(); chanIt != _channels.end(); ++chanIt){
        if (chanIt->checkMemberExistence(user->getNickName())){
            chanIt->deleteUserFromChannel(user->getNickName());
            buildResponseToSendToChanMembers(user, *chanIt, generatePrefix(user) + "QUIT :" + quitMessage);
             
            if (chanIt->beginMem() == chanIt->endMem()){ // channel becomes empty need to be deleted
                _channels.erase(chanIt);
                chanIt = _channels.begin();
                chanIt--;
            }
        }
    }
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

void    Server::buildResponseToSendToChanMembers(User const * senderUser, Channel  & channel, std::string data) const{
    std::vector<std::string>::const_iterator it;
    for (it = channel.beginMem(); it != channel.endMem() ; ++it){
        if (senderUser->getNickName() != *it)
            sendResponse(getUser(*it)->getPollFds(),data);
    }
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

void    Server::userNotLoginIn(User* user) const{
    buildResponseToSend(NULL, user, repliesMessage("451", user) + ":You have not registered");
}

// & ------------- Command : + JOIN + ---------------------- 

bool    Server::checkIfChannelNameValid(std::string channelName){
    if (channelName.size() < 2)
        return (false);
    if (channelName[0] != '#')
        return (false);
    return (true);
}

void    Server::listOfChannelMembers(Channel  & channel, User const * user) const{
    std::vector<std::string>::const_iterator it;
    bool               operatorMember;
    std::string        permissionCharacter;
    for (it = channel.beginMem() ; it != channel.endMem(); it++){
        operatorMember = channel.checkSuperUserPermission(*it);
        if (operatorMember)
            permissionCharacter = '@';
        else
            permissionCharacter = '+';
        buildResponseToSend(NULL, user, repliesMessage("353", user) +  " :" + permissionCharacter + *it);
        // buildResponseToSend(NULL, user, repliesMessage("353", user) + "Okay!!");

    //     // permissionCharacter.clear();
        std::cout << channel.getChannelName() << std::endl;
    }
    buildResponseToSend(NULL,user, repliesMessage("366", user) + channel.getChannelName() + " :End of NAMES list");
}

void    Server::join(User* user, std::vector<std::string> & argsVector){
    std::string     topicOfChannel;
    std::vector<Channel>::iterator chanIt;
    std::vector<std::string>::const_iterator argsIt;
    std::vector<std::string>::const_iterator userIt;

    
    if (argsVector.empty())
        buildResponseToSend(NULL, user, repliesMessage("461", user) + "JOIN :Not enough parameters");
    for (argsIt = argsVector.begin() ; argsIt != argsVector.end(); ++argsIt){ // iterate for all the arguments of argsVector
        for (chanIt = _channels.begin(); chanIt != _channels.end(); ++chanIt){ // iterate for all the channels to check if the new channel already exist
            if (chanIt->getChannelName() == *argsIt)
                break;
        }
        if (!checkIfChannelNameValid(*argsIt)){
            buildResponseToSend(NULL, user, repliesMessage("403", user) + *argsIt + " :No such channel");
            return ;
        }
        buildResponseToSend(user, user, "JOIN " + *argsIt);
        if (chanIt != _channels.end()){ // the cannel exist - No need to create a new one -
            for (userIt = chanIt->beginMem(); userIt != chanIt->endMem(); ++userIt){// check if the user is already in the channel
                if (*userIt == user->getNickName())
                    break;
            }
            if (userIt !=  chanIt->endMem()){ // the user already member of the channel 
                buildResponseToSend(NULL, user, repliesMessage("443", user) + user->getNickName()+ " " + chanIt->getChannelName() + " :is already on channel");
                return ; 
            }
            if (!(chanIt->addUserToChannel(user->getNickName()))){
                buildResponseToSend(NULL, user, repliesMessage("471", user) + chanIt->getChannelName() + " :Cannot join channel (+l)");
                return ;
            }
            buildResponseToSendToChanMembers(user, *chanIt, generatePrefix(user) + "JOIN " + chanIt->getChannelName());
            topicOfChannel = chanIt->getChannelTopic();

            if (topicOfChannel.empty())
                buildResponseToSend(NULL, user, repliesMessage("331", user) + chanIt->getChannelName() + " :No topic is set");
            else
                buildResponseToSend(NULL, user, repliesMessage("332", user) + chanIt->getChannelName() + " :" + topicOfChannel);
            listOfChannelMembers(*chanIt, user); // Send the list Of channel Members 
        }
        else { // the channel doesn't exist - need to create it -
            _channels.push_back(Channel(*argsIt));
            _channels.back().addUserToChannel(user->getNickName());
            _channels.back().addUserToChannelOperators(user->getNickName());
            buildResponseToSend(NULL, user, repliesMessage("331", user) + _channels.back().getChannelName() + " :No topic is set");
            listOfChannelMembers(_channels.back(), user);
        }   
    }
}