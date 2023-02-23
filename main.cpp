/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: blind-eagle <blind-eagle@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/08 15:01:33 by blind-eagle       #+#    #+#             */
/*   Updated: 2023/02/08 15:08:35 by blind-eagle      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./User.hpp"
#include "./Server.hpp"


int main(int argc, char **argv){
    if (argc != 3){
        std::cerr << "Error: bad arguments" << std::endl;
        exit(EXIT_FAILURE);
    }
    int i = 0;
    while (argv[1][i]){
        if (argv[1][i] >= '0' && argv[1][i] <= '9'){
            i++;
        }
        else{
            std::cerr << "Error : port should be number !" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    Server  server("server1", atoi(argv[1]), argv[2]);
    server.establishListening();
    server.handlingEvents();
}