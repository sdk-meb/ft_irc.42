/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bben-aou <bben-aou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/12 03:46:06 by blind-eagle       #+#    #+#             */
/*   Updated: 2023/03/06 09:32:17 by bben-aou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef     CHANNEL_HPP
#define     CHANNEL_HPP

#include "./User.hpp"

#include <string>
#include <iostream>
#include <limits>
#include <vector>

class Channel{
    private :
    std::string                  _channelName;
    std::string                  _channelTopic;
    size_t                       _maxMembers;
    std::vector<std::string>     _channelMembers;
    std::vector<std::string>     _channelOperators;
    bool                         _privateChannel;
    bool                         _secretChannel;
    bool                         _inviteOnlyChannel;
    bool                         _allowOutMessages;
    bool                         _operatorsTopicControl;

    


    public  :
    Channel();
    Channel(std::string channelName);
    ~Channel();
    
    Channel(const Channel &obj);
    Channel&  operator = (const Channel &obj);
    
    //^ -Setters- :

    bool    setChannelName(std::string user, std::string channelName);
    bool    setChannelTopic(std::string user, std::string channelTopic);
    bool    setMaxMembers(std::string user, size_t max);
    bool    setOperatorsTopicControl(std::string user, bool status);
    bool    setPrivateChannel(std::string user, bool status);
    bool    setSecretChannel(std::string user, bool status);
    bool    setInviteOnlyChannel(std::string user, bool status);
    bool    setAllowOutMessage(std::string user, bool status);
    

    // ^-Getters- :

    std::string                getChannelName()  const;
    std::string                getChannelTopic() const;
    std::vector<std::string>   getChannelMembers() ;
    std::vector<std::string>   getChannelOperators();
    size_t                     getMaxMembers() const;
    bool                       getPrivateChannelStatus()  const;
    bool                       getSecretChannelStatus()   const;
    bool                       getInviteOnlyChannelStatus()   const;
    bool                       getAllowOutMessageStatus() const;
    bool                       getOperatorsTopicControlStatus() const;

    //^ -Checkers- :

    bool     checkSuperUserPermission(std::string user);
    bool     checkSetTopicPermission(std::string user);
    bool     checkMemberExistence(std::string memberName) const;
    bool     checkIfChannelIsFull();
    bool     checkIfUserAllowedToMessageChannel(std::string user) const;

    //^ -Manage Channel- :
    
    bool    addUserToChannel(std::string nickName);    
    bool    deleteUserFromChannel(std::string nickName);
    void    addUserToChannelOperators(std::string nickName);
    void    deleteUserFromChannelOperators(std::string nickName);
    bool    changeUserNickName(std::string currentNickName, std::string newNickName);


    std::vector<std::string>::const_iterator const beginMem() const;
    std::vector<std::string>::const_iterator const endMem() const;
};


#endif