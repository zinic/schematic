//#include "types.h"
//#include "interpreter.h"
//
//#include <uthash.h>
//
//#include <stdio.h>
//#include <stdlib.h>
//#include <stdbool.h>
//
//
//// [print x "a" 421]
//Box * print_fn(Node *args, Scope *local) {
//    Node *cursor;
//    with_resolved_args(args, local, cursor, {
//        while (cursor != NULL) {
//            Box *box = ((Resolution *) Node_advance(&cursor))->box;
//
//            if (box->type == TYPE_STRING) {
//                printf("%s", UNBOX(String, box)->data);
//            } else if (box->type == TYPE_NUMBER) {
//                printf("%f", *UNBOX(double, box));
//            } else {
//                printf("Box(%s)\n", Type_name(box->type));
//            }
//        }
//    });
//    
//    return NULL;
//}
//
//// [def f [arg1 arg2] [do arg1 arg2]]
//Box * define_fn(Node *args, Scope *local) {
//    Node *cursor = args;
//    
//    // Function definitions
//    Box *name = (Box *) Node_advance(&cursor);
//    List *arg_defs = UNBOX(List, Node_advance(&cursor));
//    List *code = UNBOX(List, Node_advance(&cursor));
//
//    DynamicFunction *new_function_def = DynamicFunction_new(
//        name, arg_defs, code);
//
//    Scope_put(local, UNBOX(String, name), Box_wrap(TYPE_DYNAMIC_FUNC, new_function_def));
//
//    return NULL;
//}
//
//Box * accumulate_fn(Node *args, Scope *local) {
//    Node *cursor = NULL;
//    Box *value = NULL;
//    
//    with_resolved_args(args, local, cursor, {
//        Box *next = ((Resolution *) Node_advance(&cursor))->box;
//        
//        if (value != NULL) {
//            if (value->type == TYPE_STRING) {
//                String *old_str = UNBOX(String, value);
//                
//                if (next->type == TYPE_STRING) {
//                    value->data = String_join(old_str, UNBOX(String, next));
//                } else if (next->type == TYPE_NUMBER) {
//                    String *num_str = String_new(1024);
//                    String_resize(num_str, sprintf(num_str->data, "%f", *UNBOX(double, next)));
//                    value->data = String_join(old_str, num_str);
//                    
//                    String_free(num_str);
//                } else {
//                    // ??
//                }
//                
//                String_free(old_str);
//            } else if (value->type == TYPE_NUMBER) {
//                if (next->type == TYPE_STRING) {
//                    String *num_str = String_new(1024);
//                    String_resize(num_str, sprintf(num_str->data, "%f", *UNBOX(double, value)));
//                    value->data = String_join(num_str, UNBOX(String, next));
//                    
//                    String_free(num_str);
//                            
//                } else if (value->type == TYPE_NUMBER) {
//                    String *num_str_a = String_new(1024);
//                    String *num_str_b = String_new(1024);
//                    String_resize(num_str_a, sprintf(num_str->data, "%f", *UNBOX(double, value)));
//                    String_resize(num_str_b, sprintf(num_str->data, "%f", *UNBOX(double, value)));
//                    value->data = String_join(old_str, UNBOX(String, next));
//                    
//                    String_free(num_str);
//                } else {
//                    // ??
//                }
//            } else if (value->type == TYPE_LIST) {
//                List *vlist = UNBOX(List, value);
//                
//                if (next->type == TYPE_LIST) {
//                    Node *append_cursor = UNBOX(List, next->data)->head;
//                    
//                    while (append_cursor != NULL) {
//                        List_append(vlist, Node_advance(&append_cursor));
//                    }
//                } else {
//                    List_append(vlist, next);
//                }
//            } else {
//                printf("Values not addable!");
//                exit(1);
//            }
//        }
//    });
//    
//    return value;
//}
//
//Box * subtract_fn(Node *args, Scope *local) {
//    Node *cursor;
//    double value;
//    
//    with_resolved_args(args, local, cursor, {
//        double *a = RES_UNBOX(double, Node_advance(&cursor));
//        double *b = RES_UNBOX(double, Node_advance(&cursor));
//
//        value = *a - *b;
//    });
//    
//    return Box_wrap_double(value);
//}
//
//Box * product_fn(Node *args, Scope *local) {
//    Node *cursor;
//    double value;
//    
//    with_resolved_args(args, local, cursor, {
//        double *a = RES_UNBOX(double, Node_advance(&cursor));
//        double *b = RES_UNBOX(double, Node_advance(&cursor));
//
//        value = *a * *b;
//    });
//    
//    return Box_wrap_double(value);
//}
//
//Box * quotient_fn(Node *args, Scope *local) {
//    Node *cursor;
//    double value;
//    
//    with_resolved_args(args, local, cursor, {
//        double *a = RES_UNBOX(double, Node_advance(&cursor));
//        double *b = RES_UNBOX(double, Node_advance(&cursor));
//
//        value = *a / *b;
//    });
//    
//    return Box_wrap_double(value);
//}
//
//static String ACCUMULATE = {1, "+"};
//static String SUBTRACT = {1, "-"};
//static String PRODUCT = {1, "*"};
//static String QUOTIENT = {1, "/"};
//
//static String DEF = {3, "def"};
//static String PRINT = {5, "print"};
//
//void load_std_grammar(Frame *frame) {
//    Frame_put(frame, &ACCUMULATE, Box_wrap(TYPE_NATIVE_FUNC, &accumulate_fn));
//    Frame_put(frame, &SUBTRACT, Box_wrap(TYPE_NATIVE_FUNC, &subtract_fn));
//    Frame_put(frame, &PRODUCT, Box_wrap(TYPE_NATIVE_FUNC, &product_fn));
//    Frame_put(frame, &QUOTIENT, Box_wrap(TYPE_NATIVE_FUNC, &quotient_fn));
//    
//    Frame_put(frame, &DEF, Box_wrap(TYPE_NATIVE_FUNC, &define_fn));
//    Frame_put(frame, &PRINT, Box_wrap(TYPE_NATIVE_FUNC, &print_fn));
//}
