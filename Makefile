# Exercício zero (desafio MAKE)
# Autor: André Cavalcante
# Data: 10 de maio de 2020

PRJ=$(shell basename $(CURDIR))
INC=./inc
LIB=./lib
OBJ=./obj
SRC=./src

CC=gcc
CC_FLAGS1=-std=c11 -W -Wall
CC_FLAGS2=-std=c11 -W -Wall -c

sources=$(wildcard ./src/*.c)
objects=$(subst $(SRC), $(OBJ), $(patsubst %.c,%.o,$(sources)))

all: $(PRJ)
	@echo "Finished!"

$(PRJ): $(objects)
	$(CC) $(CC_FLAGS1)    -I $(INC) -L $(LIB) -o ./$(PRJ) $(objects)

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CC_FLAGS2) -I $(INC) -L $(LIB) -o $@ $<

.PHONY : clean

clean:
	-@rm -f ./$(PRJ)  $(OBJ)/*.o  ./*~ $(INC)/*~ $(LIB)/*~ $(OBJ)/*~ $(SRC)/*~ 

