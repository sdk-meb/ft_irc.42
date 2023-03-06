/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bben-aou <bben-aou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/12 02:53:28 by blind-eagle       #+#    #+#             */
/*   Updated: 2023/03/06 15:02:17 by bben-aou         ###   ########.fr       */
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
        if (user->Authenticate(_password)){
            saveLoginTime(user);
            welcome(user);
        }
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
        if (chanIt != _channels.end()){ // the cannel exist - No need to create a new one -
            for (userIt = chanIt->beginMem(); userIt != chanIt->endMem(); ++userIt){// check if the user is already in the channel
                if (*userIt == user->getNickName())
                    break;
            }
            if (userIt !=  chanIt->endMem()){ // the user already member of the channel 
                buildResponseToSend(NULL, user, repliesMessage("443", user) + user->getNickName()+ " " + chanIt->getChannelName() + " :is already on channel");
                return ; 
            }
            if (chanIt->getInviteOnlyChannelStatus()){
                buildResponseToSend(NULL, user, repliesMessage("473", user) + chanIt->getChannelName() + " :Cannot join channel (+i)");
                return;
            }
            if (!(chanIt->addUserToChannel(user->getNickName()))){
                buildResponseToSend(NULL, user, repliesMessage("471", user) + chanIt->getChannelName() + " :Cannot join channel (+l)");
                return ;
            }
             buildResponseToSend(user, user, "JOIN " + *argsIt);
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
            buildResponseToSend(user, user, "JOIN " + *argsIt);
            listOfChannelMembers(_channels.back(), user);
        }   
    }
}

// & ------------- Command : + PONG + ---------------------- 

void    Server::pong(User* user, std::string response) const{
    buildResponseToSend(NULL, user, "PONG :" + response);
}

// & ------------- Command : + PART + ---------------------- 

void    Server::part(User *user, std::vector<std::string> & chanArgs, std::string partReasonMessage){
    std::string reasonMessage;
    std::vector<User>::const_iterator   userIt;
    std::vector<Channel>::iterator      chanIt;
    std::vector<std::string>::iterator  chanArgsIt;

    if (partReasonMessage.empty())
        reasonMessage = "someone just left the channel";
    else
        reasonMessage = partReasonMessage;
    if (chanArgs.empty()){
        buildResponseToSend(NULL, user, repliesMessage("461", user) + "PART :Not enough parameters");
        return ;
    }

    for (chanArgsIt = chanArgs.begin(); chanArgsIt != chanArgs.end(); ++chanArgsIt){
        for(chanIt = _channels.begin(); chanIt != _channels.end(); ++chanIt){
            if (chanIt->getChannelName() == *chanArgsIt)
                break;
        }
        if (chanIt != _channels.end()){
            if (!(chanIt->deleteUserFromChannel(user->getNickName()))){
                buildResponseToSend(NULL, user, repliesMessage("442", user) + *chanArgsIt + " :You're not on that channel");
                return;
            }
            buildResponseToSend(user, user, "PART " + chanIt->getChannelName());
            buildResponseToSendToChanMembers(user, *chanIt, generatePrefix(user) + "PART " + chanIt->getChannelName() + " :" + reasonMessage);
            if (chanIt->beginMem() == chanIt->endMem()){
                _channels.erase(chanIt);
                chanIt = _channels.begin();
                chanIt--;
            }
        }
        else{
            buildResponseToSend(NULL, user, repliesMessage("403", user) + *chanArgsIt + " :No such channel");
            return;
        }
    }   
}

// & ------------- Command : + PRIVMSG + ---------------------- 

void    Server::privmsg(User *user, std::vector<std::string> & targets, std::string message) const{
    std::vector<std::string>::const_iterator  targetIt;
    std::vector<User>::const_iterator         userIt;
    std::vector<Channel>::const_iterator      chanIt;
    
    if (targets.empty()){
        buildResponseToSend(NULL, user, repliesMessage("411", user) + ":No recipient given (<PRIVMSG>)");
        return;
    }
    if (message.empty()){
        buildResponseToSend(NULL, user, repliesMessage("412", user) + ":No text to send");
        return;
    }
    for (targetIt = targets.begin(); targetIt != targets.end(); ++targetIt){
        if ((*targetIt)[0] == '#'){
            for (chanIt = _channels.begin(); chanIt != _channels.end(); ++chanIt){
                if (chanIt->getChannelName() == *targetIt)
                    break;
            }
            if (chanIt == _channels.end())
                buildResponseToSend(NULL, user, repliesMessage("401", user) + *targetIt + " :No such nick/channel");
            else if (!(chanIt->checkIfUserAllowedToMessageChannel(user->getNickName())))
                buildResponseToSend(NULL, user, repliesMessage("404", user) + *targetIt + " :Cannot send to channel");
            else
                buildResponseToSendToChanMembers(user, *chanIt, generatePrefix(user) + "PRIVMSG " + *targetIt + " :"+ message);
        }
        else{
            for (userIt = _users.begin(); userIt != _users.end(); ++userIt){
                if (userIt->getNickName() == *targetIt)
                    break;
            }
            if (userIt == _users.end()){
                buildResponseToSend(NULL, user, repliesMessage("401", user) + *targetIt + " :No such nick/channel");
                return;
            }
            buildResponseToSend(user, &*userIt, "PRIVMSG " + *targetIt + " :" +message);
        }
    }
}

// & ------------- Command : + NOTICE + ---------------------- 

void    Server::notice(User * user, std::vector<std::string> & targets, std::string noticeMessage) const{
    std::vector<std::string>::const_iterator   targetIt;
    std::vector<User>::const_iterator          userIt;
    std::vector<Channel>::const_iterator       chanIt;

    if (targets.empty() || noticeMessage.empty())
        return;
    for (targetIt = targets.begin(); targetIt != targets.end(); ++targetIt){
        if ((*targetIt)[0] == '#'){
            for (chanIt = _channels.begin(); chanIt != _channels.end(); ++chanIt){
                if (chanIt->getChannelName() == *targetIt)
                    break;
            }
            buildResponseToSendToChanMembers(user, *chanIt, generatePrefix(user) + "NOTICE " + *targetIt + " :" + noticeMessage);
        }
        else{
            for (userIt = _users.begin(); userIt != _users.end(); ++userIt){
                if (userIt->getNickName() == *targetIt){
                    buildResponseToSend(user, &*userIt, "NOTICE " + *targetIt + " :" + noticeMessage);
                    break;
                }
            }
        }
    }
}

// & ------------- Command : + LIST + ---------------------- 

void    Server::list(User* user, std::vector<std::string>  & channels) const{
    std::vector<std::string>::const_iterator  chanArgsIt;
    std::vector<Channel>::const_iterator      chanIt;
    std::string       channelName;
    std::string       topic;
    
    buildResponseToSend(NULL, user, repliesMessage("321", user) + "Channel :Users Name");
    if (channels.empty()){
        for (chanIt = _channels.begin(); chanIt != _channels.end(); ++chanIt){
            if (!(chanIt->getSecretChannelStatus()) || (chanIt->checkMemberExistence(user->getNickName()))){
                channelName = "priv";
                topic = "";
                if (!(chanIt->getPrivateChannelStatus()) || chanIt->checkMemberExistence(user->getNickName())){
                    channelName = chanIt->getChannelName();
                    topic  = chanIt->getChannelTopic();
                }
                buildResponseToSend(NULL, user, repliesMessage("322", user) + channelName + " :" + topic);
            }
        }
    }
    else{
        for (chanArgsIt = channels.begin(); chanArgsIt != channels.end(); ++chanArgsIt){            
            for (chanIt = _channels.begin(); chanIt != _channels.end(); ++chanIt){
                if (chanIt->getChannelName() == *chanArgsIt){
                    if (!(chanIt->getSecretChannelStatus()) || (chanIt->checkMemberExistence(user->getNickName()))){
                    channelName = "priv";
                    topic = "";
                    if (!(chanIt->getPrivateChannelStatus()) || chanIt->checkMemberExistence(user->getNickName())){
                        channelName = chanIt->getChannelName();
                        topic  = chanIt->getChannelTopic();
                    }
                    buildResponseToSend(NULL, user, repliesMessage("322", user) + channelName + " :" + topic);
                    }
                }
            }
        }
    }
    buildResponseToSend(NULL, user, repliesMessage("323", user) + ":End of /LIST");
}

// & ------------- Command : + MODE + ---------------------- 

void    Server::showChannelModes(User * user, Channel * channel){
    std::string mode = " +";

    if (!(channel->getAllowOutMessageStatus()))
        mode = mode + "n";
    if (channel->getPrivateChannelStatus())
        mode = mode + "p";
    if (channel->getSecretChannelStatus())
        mode = mode + "s";
    if (channel->getOperatorsTopicControlStatus())
        mode = mode + "t";
    if (channel->getInviteOnlyChannelStatus())
        mode = mode + "i";
    if (channel->getMaxMembers() != 0)
        mode = mode + "l" + std::to_string(channel->getMaxMembers());
    if (mode == " +")
        mode.clear();
    buildResponseToSend(NULL, user, repliesMessage("324", user) + channel->getChannelName() + mode); 
}

void    Server::userModes(User* user, User* target, std::vector<std::string> modes){
    (void)target;
    if (modes.empty())
        buildResponseToSend(NULL, user, repliesMessage("221", user));
    else
        buildResponseToSend(NULL, user, repliesMessage("501", user) + "MODE :Uknown MODE flag");
}


void    Server::channelModes(User * user, Channel *channel, std::vector<std::string> modes){
    std::vector<std::string>::const_iterator    argsIt;
    std::string      modeLine;
    User * target;
    size_t      i;
    std::string modeStatus;
    int     limitNumber;
    bool        addMode;
    
    if (modes.empty()){
        showChannelModes(user, channel);
        return;
    }
    argsIt = modes.begin();
    modeLine = *argsIt;
    argsIt += 1;
    
    if (modeLine[0] == '+')
        addMode = true;
    else if (modeLine[0] == '-')
        addMode = false;
    else{
        buildResponseToSend(NULL, user, repliesMessage("472", user) + modeLine[0] + " :is unknown mode char to me for " + channel->getChannelName());
        return ;
    }
    for (i = 1; i < modeLine.size(); ++i){
        if (modeLine[i] == 'o'){
            if (argsIt->empty()){
                buildResponseToSend(NULL, user, repliesMessage("461", user) + "MODE :Not enough parameters");
                return;
            }
            target = getUser(*(argsIt++));
            if (!target){
                buildResponseToSend(NULL, user, repliesMessage("441", user) + *(argsIt - 1) + " " + channel->getChannelName() + " :They aren't on that channel");
                return ;
            }
            if (channel->checkSuperUserPermission(user->getNickName())){
                if (addMode){
                    modeStatus = "+";
                    channel->addUserToChannelOperators(target->getNickName());
                }
                else if (!addMode){
                    modeStatus = "-";
                    channel->deleteUserFromChannelOperators(target->getNickName());
                }
                buildResponseToSendToChanMembers(user, *channel, generatePrefix(user) + "MODE " + modeStatus + "o " + target->getNickName());
                buildResponseToSend(user, user, "MODE " + channel->getChannelName() + " " + modeStatus + "o " + target->getNickName());
            }
            else{
                buildResponseToSend(NULL, user, repliesMessage("482", user) + channel->getChannelName() + " :You're not channel operator");
                return;
            }
        }
        else if (!channel->checkSuperUserPermission(user->getNickName())){
            buildResponseToSend(NULL, user, repliesMessage("482", user) + channel->getChannelName() + " :You're not channel operator");
                return;
        }
        else if (modeLine[i] == 'n')
            channel->setAllowOutMessage(user->getNickName(), !addMode);
        else if (modeLine[i] == 'p'){
            if (channel->getSecretChannelStatus())
                channel->setSecretChannel(user->getNickName(), false);
            channel->setPrivateChannel(user->getNickName(), addMode);
        }
        else if (modeLine[i] == 's'){
            if (channel->getPrivateChannelStatus())
                channel->setPrivateChannel(user->getNickName(), false);
            channel->setSecretChannel(user->getNickName(), addMode);
        }
        else if (modeLine[i] == 'i')
            channel->setInviteOnlyChannel(user->getNickName(), addMode);
        else if (modeLine[i] == 't')
            channel->setOperatorsTopicControl(user->getNickName(), addMode);
        else if (modeLine[i] == 'l'){
            if (addMode){
                if (argsIt->empty()){
                    buildResponseToSend(NULL, user, repliesMessage("461", user) + "MODE :Not enough parameters");
                    return;
                }
                std::stringstream tmp;
                tmp << *(argsIt++);
                if (!tmp.fail()){
                    tmp >> limitNumber;
                    if (limitNumber > 0)
                        channel->setMaxMembers(user->getNickName(), limitNumber);
                }
            }
            else
                channel->setMaxMembers(user->getNickName(), 0);
        }
        else
        {
            buildResponseToSend(NULL, user, repliesMessage("472", user) + modeLine[0] + " :is unknown mode char to me for " + channel->getChannelName());
            return ;
        }
    }
    showChannelModes(user, channel);
}

void    Server::mode(User *user, std::string target, std::vector<std::string> modes){
    std::vector<User>::iterator     userIt;
    std::vector<Channel>::iterator  chanIt;

    if (target.empty()){
        buildResponseToSend(NULL, user, repliesMessage("461", user) + "MODE :Not enough parameters");
        return;
    }
    for (userIt = _users.begin(); userIt != _users.end(); ++userIt){
        if (userIt->getNickName() == target)
            break;
    }
    if (userIt != _users.end()){
        userModes(user, &*userIt, modes);
        return;
    }
    for (chanIt = _channels.begin(); chanIt != _channels.end(); ++chanIt){
        if (chanIt->getChannelName() == target)
            break;
    }
    if (chanIt != _channels.end()){
        channelModes(user, &*chanIt, modes);
        return ;
    }
    else{
        buildResponseToSend(NULL, user, repliesMessage("401", user) + target + " :No such nick/channel");
        return;
    }
}

// & ------------- Command : + TOPIC + ---------------------- 

void    Server::topic(User * user, std::string channel, std::string topic){
    std::vector<Channel>::iterator it;
    std::vector<std::string>::const_iterator chanMemberIt;
    if (channel.empty()){
        buildResponseToSend(NULL, user, repliesMessage("461", user) + "TOPIC :Not enough parameters");
        return;
    }
    for (it = _channels.begin(); it != _channels.end(); ++it){
        if (it->getChannelName() == channel)
            break;
    }
    if (!(it->checkMemberExistence(user->getNickName())) || it == _channels.end()){
        buildResponseToSend(NULL, user, repliesMessage("442", user) + channel + " :You're not on that channel");
        return;
    }
    if (topic.empty()){
        if (it->getChannelTopic().empty())
            buildResponseToSend(NULL, user, repliesMessage("331", user) + channel + " :No topic is set");
        else
            buildResponseToSend(NULL, user, repliesMessage("332", user) + channel + " :" + it->getChannelTopic());
    }
    else if (it->setChannelTopic(user->getNickName(), topic)){
        for (chanMemberIt = it->beginMem(); chanMemberIt != it->endMem(); ++chanMemberIt)
            buildResponseToSend(NULL, getUser(*chanMemberIt), repliesMessage("332", getUser(*chanMemberIt)) + channel + " :" + it->getChannelTopic());
    }
    else
    {
        buildResponseToSend(NULL, user, repliesMessage("482", user) + channel + " :You're not channel operator");
        return;    
    }
}

// & ------------- Command : + KICK + ---------------------- 

void    Server::kick(User * user, std::string channel, std::string target, std::string reason){
    std::vector<Channel>::iterator      chanIt;
    std::vector<User>::const_iterator  userIt;
    std::string msg;

    if (channel.empty() || target.empty()){
        buildResponseToSend(NULL, user, repliesMessage("461", user) + "KICK :Not enough parameters");
        return;
    }
    for (chanIt = _channels.begin(); chanIt != _channels.end(); ++chanIt){
        if (chanIt->getChannelName() == channel)
            break;
    }
    if (chanIt == _channels.end()){
        buildResponseToSend(NULL, user, repliesMessage("403", user) + channel + " :No such channel");
        return;
    }
    if (!(chanIt->checkSuperUserPermission(user->getNickName())))
        buildResponseToSend(NULL, user, repliesMessage("482", user) + channel + " ::You're not channel operator");
    else if (!(chanIt->deleteUserFromChannel(target)))
        buildResponseToSend(NULL, user, repliesMessage("441", user) + target + " " + channel + " :They aren't on that channel");
    else{
        if (reason.empty())
            msg = "KICK " + channel + " " + target ;
        else 
            msg = "KICK " + channel + " " + target + " " + reason;
        for (userIt = _users.begin(); userIt != _users.end() ; ++userIt){
                if (userIt->getNickName() == target)
                    break;
        }
        buildResponseToSend(user, &*userIt, msg); // to notify the user that kicked out of the channel
        buildResponseToSend(user, user, msg); // print msg to the operator who kicked out the user
        buildResponseToSendToChanMembers(user, *chanIt, msg); // notify all the channel members that user -target- kicked out 
    }
}

// & ------------- Command : + NAMES + ---------------------- 

// void    Server::names(User * user, std::string channel){
//     std::vector<Channel>::iterator  chanIt;
//     std::vector<User>::iterator     userIt;
//     std::string     channelPrefix;
    
//     for (chanIt = _channels.begin(); chanIt != _channels.end(); ++chanIt){
//         if (chanIt->getChannelName() == channel)
//             break;
//     }
//     if (chanIt == _channels.end()){
//         buildResponseToSend(NULL, user, repliesMessage("366", user) + channel + " :End of /NAMES list");
//         return;
//     }
//     else if (!(chanIt->checkMemberExistence(user->getNickName())) && (chanIt->getPrivateChannelStatus() || chanIt->getSecretChannelStatus())){
//         if (chanIt->getPrivateChannelStatus()){
//             buildResponseToSend(NULL, user, repliesMessage("366", user) + channel + " *" + " :End of /NAMES list");
//             return;
//         }
//         else if (chanIt->getSecretChannelStatus()){
//             buildResponseToSend(NULL, user, repliesMessage("366", user) + channel + " @" + " :End of NAMES list");
//             return;
//         }
//     }
//     if (!channel.empty()){
//         if (chanIt->getPrivateChannelStatus())
//             channelPrefix = "* ";
//         else if (chanIt->getSecretChannelStatus())
//             channelPrefix = "@ ";
//         else
//             channelPrefix = "= ";
//         for (userIt = _users.begin(); userIt != _users.end(); ++userIt){
//             if (chanIt->checkMemberExistence(userIt->getNickName()))
//                 buildResponseToSend(NULL, user, repliesMessage("353", user) + channelPrefix + channel + " : " + userIt->getNickName());
//         }
//         buildResponseToSend(NULL, user, repliesMessage("366", user) +  channel + " :End of NAMES list");
//     }
//     else{
//         for (chanIt = _channels.begin(); chanIt != _channels.end(); ++chanIt){
//             if (!(chanIt->checkMemberExistence(user->getNickName())) && (chanIt->getPrivateChannelStatus() || chanIt->getSecretChannelStatus())){
//                 if (chanIt->getPrivateChannelStatus()){
//                     buildResponseToSend(NULL, user, repliesMessage("366", user) + "*" + " :End of /NAMES list");
//                 return;
//                 }
//                 else if (chanIt->getSecretChannelStatus()){
//                     buildResponseToSend(NULL, user, repliesMessage("366", user)  + "@" + " :End of NAMES list");
//                     return;
//                 }
//             }
//             // else if (){
                
//             // }
            
//         }
//         buildResponseToSend(NULL, user, repliesMessage("461", user) + "NAMES :Not enough parameters");
//         return;
//     }   
// }


// & ------------- Command : + NAMES V2+ ---------------------- 

void    Server::names(User * user, std::vector<std::string> & channels){
    std::vector<Channel>::iterator      chanIt;
    std::vector<User>::iterator         userIt;
    std::vector<std::string>::iterator  strIt;
    std::string                         chanPrefix;
    if (!channels.empty()){
        for (strIt = channels.begin(); strIt != channels.end(); ++strIt){
            for (chanIt = _channels.begin(); chanIt != _channels.end(); ++chanIt){
                if (chanIt->getChannelName() == *strIt)
                    break;
            }
            if (chanIt == _channels.end()){
                buildResponseToSend(NULL, user, repliesMessage("366", user) + *strIt + " :End of /NAMES list");
                return;
            }
            for (userIt = _users.begin(); userIt != _users.end(); ++userIt){
                if (chanIt->getPrivateChannelStatus())
                    chanPrefix = "* ";
                else if (chanIt->getSecretChannelStatus())
                    chanPrefix = "@ ";
                else
                    chanPrefix = "= ";
                if (chanIt->checkMemberExistence(user->getUserName()) && (chanIt->getPrivateChannelStatus() || chanIt->getSecretChannelStatus())){
                    if(chanIt->checkMemberExistence(userIt->getNickName()))
                        buildResponseToSend(NULL, user, repliesMessage("353", user) + chanPrefix + *strIt + " : " + userIt->getNickName());
                }
                else if (!chanIt->getPrivateChannelStatus() && !chanIt->getSecretChannelStatus()){
                    if(chanIt->checkMemberExistence(userIt->getNickName()))
                        buildResponseToSend(NULL, user, repliesMessage("353", user) + chanPrefix + *strIt + " : " + userIt->getNickName());
                }
            }
            buildResponseToSend(NULL, user, repliesMessage("366", user) +  *strIt + " :End of NAMES list");
        }
    }
    else{
        if (_channels.empty()){
            // buildResponseToSend(NULL, user, repliesMessage("366", user) + "" + " :End of NAMES list");
            std::cout << "there is no channel created yet !" << std::endl;
            return;
        }
        for (chanIt = _channels.begin(); chanIt != _channels.end(); ++chanIt){
            if (chanIt->getPrivateChannelStatus())
                chanPrefix = "* ";
            else if (chanIt->getSecretChannelStatus())
                chanPrefix = "@ ";
            else
                chanPrefix = "= ";
            if (!chanIt->checkMemberExistence(user->getNickName()) &&  (chanIt->getPrivateChannelStatus() || chanIt->getSecretChannelStatus())){
                buildResponseToSend(NULL, user, repliesMessage("366", user) + "*" + " :End of /NAMES list");
                continue;
            }
            else if ((chanIt->checkMemberExistence(user->getNickName())) || (!chanIt->getPrivateChannelStatus() && !chanIt->getSecretChannelStatus())){
                for (userIt = _users.begin(); userIt != _users.end(); ++userIt){
                    if (chanIt->checkMemberExistence(userIt->getNickName()))
                        buildResponseToSend(NULL, user, repliesMessage("353", user) + chanPrefix + chanIt->getChannelName() + " : " + userIt->getNickName());
                }
            }
            buildResponseToSend(NULL, user, repliesMessage("366", user) +  chanIt->getChannelName() + " :End of NAMES list");
        }
    }
}

// & ------------- Command : + INVITE + ---------------------- 

void    Server::invite(User * user, std::string invitedUser, std::string channel){
    if (invitedUser.empty() || channel.empty()){
        buildResponseToSend(NULL, user, repliesMessage("461", user) + "INVITE :Not enough parameters");
        return;
    }
    std::vector<Channel>::iterator  chanIt;
    std::vector<User>::iterator     userIt;
    
    for (chanIt = _channels.begin(); chanIt != _channels.end(); ++chanIt){
        if (chanIt->getChannelName() == channel)
            break;
    }
    if (chanIt == _channels.end()){
        buildResponseToSend(NULL, user, repliesMessage("401", user) + channel + " :No such nick/channel");
        return;
    }
    for (userIt = _users.begin(); userIt != _users.end(); ++userIt){
        if (userIt->getNickName() == invitedUser)
            break;
    }
    if (userIt == _users.end()){
        buildResponseToSend(NULL, user, repliesMessage("401", user) + invitedUser + " :No such nick/channel");
        return;
    }
    if (chanIt->checkMemberExistence(invitedUser)){
        buildResponseToSend(NULL, user, repliesMessage("443", user) + invitedUser + channel + " :is already on channel");
        return;
    }
    if (chanIt->checkMemberExistence(user->getNickName())){
        if (!chanIt->checkSuperUserPermission(user->getNickName())){
            buildResponseToSend(NULL, user, repliesMessage("482", user) + channel + " :You're not channel operator");
            return;
        }
        chanIt->addUserToChannel(invitedUser);
        buildResponseToSend(NULL, user, channel + " " + invitedUser);
    }
    else{
        buildResponseToSend(NULL, user, repliesMessage("442", user) + channel + " :You're not on that channel");
        return;
    }
}
//comment for test
