NAME	:=	ircserv

SRCS_DIR:=	./src
SRCS	:=	$(addsuffix .cpp,	main Client)

CC		:=	c++
CFLAGS	:=	-Wall -Wextra -Werror -std=c++98
COMPILE	:=	$(CC) $(CFLAGS)

RM		:= rm -rf
MKDIR	:= mkdir -p

OBJ_DIR	:=	./object
OBJS	:=	$(patsubst %,$(OBJ_DIR)/%,$(SRCS:%.cpp=%.o))

all: $(NAME)

$(OBJ_DIR)/%.o : $(SRCS_DIR)/%.cpp
	@$(MKDIR) $(OBJ_DIR)
	$(COMPILE) $(INCLUDES) -c $< -o $@

$(NAME): $(OBJS)
	$(COMPILE) $^ -o $@

clean:
	$(RM) $(OBJS)

fclean:
	$(RM) $(OBJS)
	$(RM) $(NAME)

re:
	@$(MAKE) fclean
	@$(MAKE) all

.PHONY: all clean fclean re
