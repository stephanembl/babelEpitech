NAME	=	babibel

GCC	=	g++

GCCFLAGS	=	-lportaudio -L. -lopus

SRC	=	AudioManager.cpp	\
		EncodeManager.cpp	\
		main.cpp

OBJ	=	$(SRC:.cpp=.o)


$(NAME)	:	$(OBJ)
		$(GCC) $(OBJ) -o $(NAME) $(GCCFLAGS)

all	:	$(NAME)

clean	:
		rm -f $(OBJ) *~

fclean	:	clean
		rm -f $(NAME)

re	:	fclean all