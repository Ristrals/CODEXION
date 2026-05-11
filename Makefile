CC = cc
CFLAGS = -Wall -Wextra -Werror
VAL = valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all

NAME = codexion
F_SRC = src
F_OBJ = obj
F_INC = include

ARGS = 4 1900 400 200 600 4 100
FIFO = fifo
EDF = edf

SRCS =	$(F_SRC)/main.c \
		$(F_SRC)/initializer.c \
		$(F_SRC)/parser.c \
		$(F_SRC)/monitor.c \
		$(F_SRC)/coder.c \
		$(F_SRC)/coder_tools.c \
		$(F_SRC)/utils.c \

OBJS = $(SRCS:$(F_SRC)/%.c=$(F_OBJ)/%.o)

all: $(NAME)

val: $(NAME)
	$(VAL) ./$(NAME) $(ARGS) $(EDF)

fifo:
	./$(NAME) $(ARGS) $(FIFO)

edf:
	./$(NAME) $(ARGS) $(EDF)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

$(F_OBJ)/%.o: $(F_SRC)/%.c
	@mkdir -p $(F_OBJ)
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm	-r $(F_OBJ)

fclean: clean
	@rm -f	$(NAME)

re: fclean all

lint:
	norminette .

.PHONY: all clean fclean re
	