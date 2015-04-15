#include "types.h"
#include "parser.h"
#include "interpreter.h"
#include "std_grammar.h"

#include <stdio.h>

void start(char *filename) {
	Scope *global;
	List *root;
        Node *cursor;

	// Setup global scope and add the default grammar
	global = Scope_new();
	load_std_grammar(Scope_descend(global));

#ifdef CALLGRIND
	for (int c = 0; c < 10; c++) {
		free_boxed_list(parse_file(filename));
	}
#else
	// Parse the source
	root = parse_file(filename);
        
        // Run all directives
        cursor = root->head;
        while (cursor != EMPTY_NODE) {
            evaluate(UNBOX(List, Node_advance(&cursor)), global);
        }
#endif

	free_boxed_list(root);
}

int main(int argc, char* argv[]) {
#ifdef TEST
#include "test.h"
	run_tests();
#else
	if (argc >= 2) {
		start(argv[1]);
	} else {
		printf("Usage: schematic <src_file>\n");
	}
#endif

	return 0;
}
