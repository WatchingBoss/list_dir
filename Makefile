CC = gcc
CFLAGS = -Wall -g

OBJ = list.o common.o
OUT = list

$(OUT): $(OBJ)
	$(CC) $(CFLAGS) -o $(OUT) $(OBJ)

clean:
	$(RM) $(OBJ) $(OUT)
