
NAME = Sniffer

NAME_D = daemon

NAME_C = client

#---------------DIRECTORIES-----------------------------------------------------

D_SRC = src/

D_OBJ = obj/

D_INC = inc/

#----------DIRECTORIES----------------------------------------------------------

D_DAEMON = daemon/
D_CLIENT = client/

#--------------FILES------------------------------------------------------------

HEAD_FILES =	main.h \
				$(D_DAEMON)daemon.h \
				$(D_DAEMON)ip_vector.h \
				$(D_CLIENT)client.h

C_FILES =		$(D_DAEMON)daemon.c \
				$(D_DAEMON)ip_vector.c \
				errors.c
#DAEMON-------------------------------------------------------------------------

DAEMON_C = $(D_DAEMON)daemon_main.c $(C_FILES)

DAEMON_SRC = $(addprefix $(D_SRC), $(DAEMON_C))

DAEMON_OBJ = $(addprefix $(D_OBJ), $(DAEMON_C:.c=.o)) 

#CLIENT-------------------------------------------------------------------------

CLIENT_C = $(D_CLIENT)client_main.c $(C_FILES)

CLIENT_SRC = $(addprefix $(D_SRC), $(CLIENT_C))

CLIENT_OBJ = $(addprefix $(D_OBJ), $(CLIENT_C:.c=.o))

#FILES--------------------------------------------------------------------------

HEADERS =	$(addprefix $(D_INC), $(HEAD_FILES))

#--------------COMPILATION------------------------------------------------------

CC = gcc
CFLAGS = -g -Wall -Werror -Wextra

INC =	-I $(D_INC) \
		-I $(D_INC)$(D_DAEMON) \
		-I $(D_INC)$(D_CLIENT)

#--------------COLORS-----------------------------------------------------------

C_RED = \033[31m
C_GREEN = \033[32m
C_CYAN = \033[36m
C_NONE = \033[0m

#--------------RULES------------------------------------------------------------

all: daemon client

daemon: $(D_OBJ) $(DAEMON_OBJ)
	@$(CC) $(DAEMON_OBJ) -o $(NAME_D)
	@printf "$(C_CYAN)%-10s$(C_NONE)%-40s$(C_GREEN)[done]$(C_NONE)\n" $(NAME_D): $@

client: $(D_OBJ) $(CLIENT_OBJ)
	@$(CC) $(CLIENT_OBJ) -o $(NAME_C)
	@printf "$(C_CYAN)%-10s$(C_NONE)%-40s$(C_GREEN)[done]$(C_NONE)\n" $(NAME_C): $@

$(D_OBJ):
	@mkdir $(D_OBJ)
	@mkdir $(D_OBJ)$(D_DAEMON)
	@mkdir $(D_OBJ)$(D_CLIENT)
	@printf "$(C_CYAN)%-10s$(C_NONE)%-40s$(C_GREEN)[done]$(C_NONE)\n" $(NAME): $@

$(D_OBJ)%.o: $(D_SRC)%.c $(HEADERS)
	@$(CC) $(CFLAGS) -c $(INC) $< -o $@
	@printf "$(C_CYAN)%-10s$(C_NONE)%-40s$(C_GREEN)[done]$(C_NONE)\n" $(NAME): $@

clean_logs:
	@rm -rf .conn_layer_cl_dae
	@rm -rf .conn_layer_dae_cl
	@rm -rf .daemon_pid
	@rm -rf .*reboot
	@rm -rf errors.txt
	@rm -rf .vscode
	@printf "$(C_CYAN)$(NAME):$(C_GREEN)<TMP was cleaned>\n"

clean:
	@rm -rf $(D_OBJ)
	@printf "$(C_CYAN)%-10s$(C_NONE)%-40s$(C_RED)[done]$(C_NONE)\n" $(NAME): $@

fclean: clean
	@rm -rf $(NAME_D)
	@rm -rf $(NAME_C)
	@printf "$(C_CYAN)%-10s$(C_NONE)%-40s$(C_RED)[done]$(C_NONE)\n" $(NAME): $@

re: fclean all
