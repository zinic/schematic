CC = gcc
CFLAGS = $(FLAGS) -std=gnu11 -Wall -I ./src/c/ -I ./src/c/interpreter -I ./src/tests/ -I includes/uthash/src/
BIN = schematic

C_FILES := $(shell find ./src -not -path './src/tests/specs/*' -name '*.c')
OBJECTS := $(patsubst %.c,%.o, $(C_FILES))


release: schematic
print-%: ; @echo $*=$($*)

schematic: CFLAGS += -O3
schematic: $(OBJECTS)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJECTS)

debug: CFLAGS += -g
debug: $(OBJECTS)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJECTS)

valgrind: debug
	valgrind --track-origins=yes --leak-check=yes ./$(BIN) ./src/schematic/hello_world.sch

callgrind: debug
	valgrind --dsymutil=yes --tool=callgrind ./$(BIN) ./src/schematic/hello_world.sch
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
	$(RM) ./src/tests/*.o
