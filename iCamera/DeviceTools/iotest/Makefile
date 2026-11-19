CC=arm-v01c02s020-linux-musleabi-gcc

INCLUDE = 

SRC = $(wildcard *.c)
BIN = iotest

all:
	$(CC) -g -o $(BIN) $(SRC) $(INCLUDE) $(CFLAG) $(X_CFLAGS) $(X_LIBS) $(GTK_LIB) $(GTK_INC)
	
clean:
	rm -f *.o
	rm -f $(BIN)

