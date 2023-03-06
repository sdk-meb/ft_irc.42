/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bben-aou <bben-aou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/06 10:55:29 by bben-aou          #+#    #+#             */
/*   Updated: 2023/03/06 18:13:33 by bben-aou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./Server.hpp"
#include "./Channel.hpp"

// struct time{
//     int     hours;
//     int     minutes;
// };

void    Server::buildBotResponse(std::string prefix, User const * recipient, std::string data) const{
    std::string  dataToSend;
    dataToSend = prefix + data;
    sendResponse(recipient->getPollFds(), dataToSend);
}

void    Server::saveLoginTime(User *user){
    ltime Mytime;
    std::time_t current_time = std::time(NULL);
    std::tm* time_info = std::localtime(&current_time);
    Mytime.hours =  time_info->tm_hour;
    Mytime.minutes =  time_info->tm_min;
    my_map.insert(std::make_pair(user->getNickName(), Mytime));
}

void    Server::help(User *user){
    buildBotResponse("bot@help", user , YEL "        Welcome to the IRC server help guide d(#_#)b "  RESET);
    buildBotResponse("", user, GRN "                     ======== Available Commands : ========" RESET);
    buildBotResponse("", user,  RED "        JOIN - PART - LIST -  NAMES - TOPIC - QUIT - TOPIC - PRIVMSG - KICK - MODE - INVITE " RESET);
    buildBotResponse("", user, CYN "  -   JOIN  - This command is used to join a specific channel         \033[0m            |\033[33m  Example: JOIN #channelname" RESET);
    buildBotResponse("", user, CYN "  -   PART  - This command is used to leave a specific channel      \033[0m              | \033[33m Example: PART #channelname : reason - optional - \033[0m");
    buildBotResponse("", user, CYN "  -   LIST  - This command is used to get a list channels            \033[0m             | \033[33m Example: LIST #channelname \033[0m");
    buildBotResponse("", user, CYN "  -   NAME  - This command is used to get a list of users on channels    \033[0m         | \033[33m Example: NAMES #channelname \033[0m");
    buildBotResponse("", user, CYN "  -   TOPIC - This command is used to get or set the topic of channels     \033[0m       | \033[33m Example: TOPIC #channelname : topic - optional - \033[0m");
    buildBotResponse("", user, CYN "  -   QUIT  - This command is used to disconnect from the IRC server      \033[0m        | \033[33m Example: QUIT  :  reason - optional - \033[0m");
    buildBotResponse("", user, CYN "  - PRIVMSG - This command is used to send a private message to a user or channel \033[0m| \033[33m Example: PRIVMSG #channelname :Hello everyone! \033[0m");
    buildBotResponse("", user, CYN "  -   KICK  - This command is used to remove a user from a channel         \033[0m       | \033[33m Example: KICK #channelname username : reason - optional - \033[0m");
    buildBotResponse("", user, CYN "  -   MODE  - This command is used to set the mode of a channel or user     \033[0m      | \033[33m Example: MODE #channelname +-mode username \033[0m");
    buildBotResponse("", user, CYN "  -  INVITE - This command is used to invite a user to a channel            \033[0m      | \033[33m Example: INVITE username #channelname \033[0m");

}

void    Server::generateTopic(User * user, std::string channelType){
    if (channelType.empty()){
        buildBotResponse("bot@error ", user, "Error syntax : please enter a specific channel type !");
        return;
    }
    std::cout << "|"<<channelType<<"|" <<std::endl;
    srand(time(0));
    std::string generatedTopic;
    
    if (channelType == "sport"){
        std::string  sport[5] = {
            "Sport fanatics",
            "Game talk",
            "Athlete's world",
            "Sport news",
            "Score update"
        };
        int randomIndex = rand() % 5;
        generatedTopic = sport[randomIndex];
        buildBotResponse("bot@generateTopic ", user, "Topic Is : \033[32m" +generatedTopic + RESET);
    }
    else if (channelType == "music"){
        std::string  music[3] = {
            "Soundcheck",
            "Music vibes",
            "Melodic musings"
        };
        int randomIndex = rand() % 3;
        generatedTopic = music[randomIndex];
        buildBotResponse("bot@generateTopic ", user, "Topic Is : \033[32m" + generatedTopic + RESET);
        return;
    }
    else if (channelType == "programming"){
        std::string  programming[3] = {
            "Hackerspace",
            "Dev squad",
            "Tech talk"
        };
        int randomIndex = rand() % 3;
        generatedTopic = programming[randomIndex];
        buildBotResponse("bot@generateTopic ", user,"Topic Is : \033[32m" + generatedTopic + RESET);
        return;
    }
    else{
        buildBotResponse("bot@error ", user,  "Error : unvailable service !");
        return;
    }
}

void    Server::bot(User * user, std::string option, std::string channelType){
    std::string service;
    if (option.empty()){
        buildBotResponse("bot@error ", user, "Error syntax : please enter a specific service !");
        // std::cout << " Error syntax : please enter a specific service !" << std::endl;
        
        return;
    }
    std::cout << option << " channelType : " << channelType << std::endl;
    service = option;
    std::map<std::string, ltime>::iterator it;

    
    ltime Mytime;
    std::time_t current_time = std::time(NULL);
    std::tm* time_info = std::localtime(&current_time);
    Mytime.hours =  time_info->tm_hour;
    Mytime.minutes =  time_info->tm_min;

    int loghours ;
    int logminutes;
        
    std::string    services[3] = {
                "logtime",
                "help",
                "generateTopic"
    };
    int i = 0;
    while (i < 3 && option != services[i])
        i++;
    switch(i){
        case 0 :{
            for (it = my_map.begin(); it != my_map.end(); ++it) {
                if (it->first == user->getNickName()){
                    loghours = Mytime.hours - it->second.hours;
                    logminutes = Mytime.minutes - it->second.minutes;
                    if (logminutes < 0){
                        loghours -= 1;
                        logminutes += 60;
                    }
                    std::string     message =  " -> User : \033[33m" + user->getNickName() + "\033[0m has been loggedin for : " + RED + std::to_string(loghours) + "h" + std::to_string(logminutes) + "min" + RESET;
                    buildBotResponse("bot@logtime", user, message);
                    return;
                }
            }
            if (it == my_map.end()){
                // std::cout << "user not on the network" << std::endl;
            buildBotResponse("bot@error ", user, "Error : user not found !");
                return;
            }
            break;
        }
        case 1 :{
            help(user);
            break;
        }
        case 2 :{
            generateTopic(user, channelType);
            break;
        }
        default :
            // std::cout << "invalid service !" << std::endl;
            buildBotResponse("bot@error ", user, "Error : unvailable service !");
    }
}