#ifndef parser_h
#define parser_h

#ifdef __cplusplus
extern "C" {
#endif

#define CH_N 'n'
#define CH_R 'r'
#define CH_T 't'

#define CH_0 '0'
#define CH_9 '9'
#define CH_DOT '.'
#define CH_MINUS '-'
#define CH_SPACE ' '
#define CH_TAB '\t'
#define CH_CARRIAGE_RETURN '\r'
#define CH_NEWLINE '\n'
#define CH_DOUBLE_QUOTE '"'
#define CH_BACKSLASH '\\'
#define CH_OPEN_BRACKET '['
#define CH_CLOSE_BRACKET ']'

#define IS_WHITESPACE(ch) (\
    ch == CH_SPACE ||\
    ch == CH_TAB ||\
    ch == CH_CARRIAGE_RETURN ||\
    ch == CH_NEWLINE)

#define IS_NUMBER(ch) (ch >= CH_0 && ch <= CH_9)

List * parse_file(char *filename);

#ifdef __cplusplus
}
#endif
#endif
