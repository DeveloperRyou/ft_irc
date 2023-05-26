NAME	:=	ircserv

INCLUDES:= ./include

SRCS_DIR:=	./src
SRCS	:=	$(addsuffix .cpp,	main Server Client Channel Parser ClientMode)

CC		:=	c++
CFLAGS	:=	-Wall -Wextra -Werror -std=c++98
COMPILE	:=	$(CC) $(CFLAGS)

RM		:= rm -rf
MKDIR	:= mkdir -p

OBJS_DIR:=	./object
OBJS	:=	$(patsubst %,$(OBJS_DIR)/%,$(SRCS:%.cpp=%.o))

all: $(NAME)

$(OBJS_DIR)/%.o : $(SRCS_DIR)/%.cpp
	@$(MKDIR) $(OBJS_DIR)
	$(COMPILE) -I$(INCLUDES) -c $< -o $@

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
