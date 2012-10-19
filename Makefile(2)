COMPILER = gcc
CCFLAGS = -g -Wall -pthread 
BINARY = mts
	SOURCE_FILES = main.c parser.c train_mngmnt.c queue.c 
mts: 
	$(COMPILER) $(CCFLAGS) $(SOURCE_FILES) -o $(BINARY)

clean:
	rm -f $(BINARY)
