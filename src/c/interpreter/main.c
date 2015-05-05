#include "types.h"
#include "parser.h"
#include "interpreter.h"
#include "std_grammar.h"

#include <time.h>
#include <stdio.h>

void start(char *filename) {
#ifdef CALLGRIND
	for (int c = 0; c < 10; c++) {
		free_boxed_list(parse_file(filename));
	}
#else
	Scope *global;
	List *root;
        Node *cursor;
        Frame *std_grammer_frame;
        
	// Setup global scope and add the default grammar
	global = Scope_new();
        std_grammer_frame = Scope_descend(global);
        
//	load_std_grammar(std_grammer_frame);

	// Parse the source
	root = parse_file(filename);
        
        // Run all directives
        cursor = root->head;
        while (cursor != NULL) {
            evaluate(UNBOX(List, Node_advance(&cursor)), global);
        }
        
	rfree(TYPE_LIST, root);
        Scope_free(global);
#endif
}
int amain(int argc, char* argv[]) {
#ifdef TEST
#include "test.h"
	run_tests();
#else
	if (argc >= 2) {
            start(argv[1]);
	}
#endif

	return 0;
}

