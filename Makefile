CC = gcc
CFLAGS = -Wall -g

OBJ = common.o list.o 
OUT = list

$(OUT): $(OBJ)
	$(CC) $(CFLAGS) -o $(OUT) $(OBJ)

clean:
	$(RM) $(OBJ) $(OUT)
