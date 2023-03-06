# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: bben-aou <bben-aou@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/02/08 15:11:24 by blind-eagle       #+#    #+#              #
#    Updated: 2023/03/06 17:14:19 by bben-aou         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


NAME = ircserv

CFLAGS = -Wall -Wextra -Werror -std=c++98

SRC = User.cpp \
	  Channel.cpp \
	  Commands.cpp \
	  bot.cpp \
	  Server.cpp \
	  utilsServer.cpp \
	  main.cpp \

HDRS = User.hpp \
	   Channel.hpp \
	   Commands.hpp \
	   Server.hpp \
	   main.hpp \

OBJ = $(SRC:.cpp=.o)

all : $(NAME)

$(NAME): $(OBJ)
	@c++ $(FLAGS) $(OBJ) -o $@
	@echo "\033[0;36m ⚙️  MakeFile ⚙️ : ✅ COMPILED SUCCESSFULY\033[0m"

%.o:%.cpp  HDRS
		@c++ $(CFLAGS) -o $@ -c $<
clean :
	@rm -f $(OBJ)
	@echo "\033[0;36m ⚙️  MakeFile ⚙️ : 🗑️ Objects removed!\033[0m"
fclean : clean
	@rm -f $(NAME)
	@echo "\033[0;36m ⚙️  MakeFile ⚙️ : 🗑️ CLEANED SUCCESSFULY\033[0m"

re: fclean all

.PHONY : all clean fclean re