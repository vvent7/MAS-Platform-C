PRJ=$(shell basename $(CURDIR))
INC=./inc
LIB=./lib
OBJ=./obj
SRC=./src

CC=gcc
CC_FLAGS1=-std=c11 -W -Wall -pthread
CC_FLAGS2=-std=c11 -W -Wall -pthread -c

sources=$(wildcard ./src/*.c)
objects=$(subst $(SRC), $(OBJ), $(patsubst %.c,%.o,$(sources)))

all: folders $(PRJ)
	@echo "Finished!"

folders:
	@mkdir -p obj src inc lib

$(PRJ): $(objects)
	$(CC) $(CC_FLAGS1)    -I $(INC) -L $(LIB) -o ./$(PRJ) $(objects)

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CC_FLAGS2) -I $(INC) -L $(LIB) -o $@ $<

.PHONY : all folders clean

clean:
	-@rm -f ./$(PRJ)  $(OBJ)/*.o  ./*~ $(INC)/*~ $(LIB)/*~ $(OBJ)/*~ $(SRC)/*~ 

