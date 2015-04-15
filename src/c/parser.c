#include "types.h"
#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum {
    S_START,
    S_LIST_ITEM,
    S_SYMBOL,
    S_STRING,
    S_CH_MINUS,
    S_NUMBER,
    S_DECIMAL
} State;

typedef struct Parser {
    List *root;
    List *cursor;
    List *history;

    double number;
    double number_mag;

    Buffer *buffer;
    State state;
    bool escaped;
} Parser;

Parser * Parser_new() {
    Parser *new_parser = (Parser *) malloc(sizeof(Parser));

    new_parser->root = List_new();
    new_parser->cursor = new_parser->root;
    new_parser->history= List_new();
    new_parser->number = 0;
    new_parser->number_mag = 0;
    new_parser->buffer = Buffer_new(4096);
    new_parser->state = S_START;
    new_parser->escaped = false;

    return new_parser;
}

void Parser_free(Parser *parser) {
    List_free(parser->history);
    Buffer_free(parser->buffer);
    free(parser);
}


void parser_push(Parser *parser) {
    List *new_list = List_new();

    // Add the new list to the cursor first
    List_append(parser->cursor, Box_wrap(TYPE_LIST, new_list));

    // Push the cursor onto the history stack
    List_append(parser->history, parser->cursor);

    // Write the new list as the new cursor
    parser->cursor = new_list;
}

void parser_pop(Parser *parser) {
    parser->cursor = (List *) List_pop(parser->history);
}

void state_start(Parser *parser, char next_ch) {
    if (!IS_WHITESPACE(next_ch)) {
        if (next_ch == CH_OPEN_BRACKET) {
            parser_push(parser);
            parser->state = S_LIST_ITEM;
        } else {
            printf("Bad token at start: %c\n", next_ch);
        }
    }
}

void state_list_item(Parser *parser, char next_ch) {
    if (!IS_WHITESPACE(next_ch)) {
        if (next_ch == CH_OPEN_BRACKET) {
            parser_push(parser);
        } else if (next_ch == CH_CLOSE_BRACKET) {
            parser_pop(parser);
        } else if (next_ch == CH_DOUBLE_QUOTE) {
            parser->state = S_STRING;
        } else if (next_ch == CH_MINUS) {
            parser->state = S_CH_MINUS;
        } else if (IS_NUMBER(next_ch)) {
            parser->number = next_ch - CH_0;
            parser->number_mag = 10;

            parser->state = S_NUMBER;
        } else {
            Buffer_append(parser->buffer, next_ch);
            parser->state = S_SYMBOL;
        }
    }
}

void state_minus(Parser *parser, char next_ch) {
    if (IS_NUMBER(next_ch)) {
        parser->number = -(next_ch - CH_0);
        parser->number_mag = 10;

        parser->state = S_NUMBER;
    }  else {
        Buffer_append(parser->buffer, CH_MINUS);

        if (IS_WHITESPACE(next_ch)) {
            List_append(parser->cursor, Box_wrap(TYPE_SYMBOL, Buffer_get(parser->buffer)));
            parser->state = S_LIST_ITEM;
        } else if (next_ch == CH_CLOSE_BRACKET) {
            List_append(parser->cursor, Box_wrap(TYPE_SYMBOL, Buffer_get(parser->buffer)));
            parser_pop(parser);

            parser->state = S_LIST_ITEM;
        }  else {
            Buffer_append(parser->buffer, next_ch);
            parser->state = S_SYMBOL;
        }
    }
}

void state_number(Parser *parser, char next_ch) {
    if (IS_WHITESPACE(next_ch)) {
        List_append(parser->cursor, Box_wrap_double(parser->number));
        parser->state = S_LIST_ITEM;

    } else if (next_ch == CH_CLOSE_BRACKET) {
        List_append(parser->cursor, Box_wrap_double(parser->number));
        parser_pop(parser);

        parser->state = S_LIST_ITEM;
    } else if (next_ch == CH_DOT) {
        parser->number_mag = parser->number < 0 ? -10 : 10;
        parser->state = S_DECIMAL;
    } else {
        parser->number *= parser->number_mag;
        parser->number = parser->number >= 0
            ? parser->number + (next_ch - CH_0)
            : parser->number - (next_ch - CH_0);
    }
}

void state_decimal(Parser *parser, char next_ch) {
     if (IS_WHITESPACE(next_ch)) {
        List_append(parser->cursor, Box_wrap_double(parser->number));
        parser->state = S_LIST_ITEM;

    } else if (next_ch == CH_CLOSE_BRACKET) {
        List_append(parser->cursor, Box_wrap_double(parser->number));
        parser_pop(parser);

        parser->state = S_LIST_ITEM;
    } else {
        parser->number += (next_ch - CH_0) / parser->number_mag;
        parser->number_mag *= 10;
    }
}

void state_symbol(Parser *parser, char next_ch) {
    if (IS_WHITESPACE(next_ch)) {
        if (Buffer_has_data(parser->buffer)) {
            List_append(parser->cursor, Box_wrap(TYPE_SYMBOL, Buffer_get(parser->buffer)));
        }

        parser->state = S_LIST_ITEM;
    } else if (next_ch == CH_OPEN_BRACKET) {
        printf("Bad token in symbol: %c\n", next_ch);

    } else if (next_ch == CH_CLOSE_BRACKET) {
        if (Buffer_has_data(parser->buffer)) {
            List_append(parser->cursor, Box_wrap(TYPE_SYMBOL, Buffer_get(parser->buffer)));
        }

        parser_pop(parser);
        parser->state = S_LIST_ITEM;
    } else {
        Buffer_append(parser->buffer, next_ch);
    }
}

void state_string(Parser *parser, char next_ch) {
    if (parser->escaped) {
        parser->escaped = false;

        if (next_ch == CH_N) {
            Buffer_append(parser->buffer, CH_NEWLINE);
        } else if (next_ch == CH_R) {
            Buffer_append(parser->buffer, CH_NEWLINE);
        } else if (next_ch == CH_T) {
            Buffer_append(parser->buffer, CH_NEWLINE);
        } else if (next_ch == CH_DOUBLE_QUOTE) {
            Buffer_append(parser->buffer, CH_NEWLINE);
        } else if (next_ch == CH_BACKSLASH) {
            Buffer_append(parser->buffer, CH_NEWLINE);
        } else {
            printf("Bad escape sequence: \\%c", next_ch);
        }
    } else {
        if (next_ch == CH_DOUBLE_QUOTE) {
            if (Buffer_has_data(parser->buffer)) {
                List_append(parser->cursor, Box_wrap(TYPE_STRING, Buffer_get(parser->buffer)));
            }

            parser->state = S_LIST_ITEM;
        } else if (next_ch == CH_BACKSLASH) {
            parser->escaped = true;
        } else {
            Buffer_append(parser->buffer, next_ch);
        }
    }
}

List * _parse(FILE *src_file) {
    const char read_buff[1];

    Parser *parser = Parser_new();
    List *root = parser->root;

    int read;

    while (true) {
        read = fread(&read_buff, 1, 1, src_file);

        if (read == 0) {
            break;
        }

        switch (parser->state) {
            case S_START:
                state_start(parser, read_buff[0]);
                break;

            case S_LIST_ITEM:
                state_list_item(parser, read_buff[0]);
                break;

            case S_CH_MINUS:
                state_minus(parser, read_buff[0]);
                break;

            case S_NUMBER:
                state_number(parser, read_buff[0]);
                break;

            case S_DECIMAL:
                state_decimal(parser, read_buff[0]);
                break;

            case S_SYMBOL:
                state_symbol(parser, read_buff[0]);
                break;

            case S_STRING:
                state_string(parser, read_buff[0]);
                break;
        }
    }

    Parser_free(parser);
    return root;
}

List * parse_file(char *filename) {
    FILE *file = fopen(filename, "r");
    List *root = _parse(file);
    fclose(file);

    return root;
}
