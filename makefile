CC=g++-11
CFLAGS=-Ofast -funroll-loops -fno-stack-protector -pthread

OUTPUT=output/gpuzzle

all:
	$(CC) $(CFLAGS) -o $(OUTPUT) gpuzzle.cpp

debug:
	$(CC) -pthread -o $(OUTPUT) -g gpuzzle.cpp
