CC = gcc
CFLAGS = -std=c11 -Wall -I ./src/c/ -I ./src/tests -I includes/uthash/src/
BIN = schematic

C_FILES := $(wildcard src/**/*.c)
OBJECTS := $(patsubst %.c,%.o, $(C_FILES))


release: schematic
print-%: ; @echo $*=$($*)

schematic: CFLAGS += -O3
schematic: $(OBJECTS)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJECTS)

debug: CFLAGS += -g
debug: $(OBJECTS)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJECTS)

valgrind: CFLAGS += -D CALLGRIND
valgrind: debug
	valgrind --track-origins=yes --leak-check=yes ./$(BIN) ./src/schematic/callgrind_test.sch

callgrind: CFLAGS += -D CALLGRIND
callgrind: debug
	valgrind --dsymutil=yes --tool=callgrind ./$(BIN) ./src/schematic/callgrind_test.sch
	callgrind_annotate callgrind.out.*

test: CFLAGS += -D TEST
test: debug
	./$(BIN)

gdb: debug
	gdb -ex r --args ./$(BIN) ./src/schematic/hello_world.sch

clean:
	$(RM) callgrind.out.*
	$(RM) schematic
	$(RM) ./src/c/*.o
