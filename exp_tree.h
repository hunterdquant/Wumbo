/*
    Data structures for expressions.
*/

#ifndef __EXP_TREE__
#define __EXP_TREE__

#include "defs.h"
#include "sym_table.h"

struct exp_list_s;
typedef struct exp_list_s exp_list_t;

struct exp_tree_s;
typedef struct exp_tree_s exp_tree_t;

// Function call expression.
typedef struct func_exp_s {
    sym_node_t *sym_ref;
    exp_list_t *args;
} func_exp_t;

// Array lookup expression.
typedef struct array_exp_s {
    sym_node_t *sym_ref;
    exp_tree_t *arg;
} array_exp_t;

// Expression node.
// Can be float, int, variable, function, array, operator.
typedef struct exp_node_s {
    exp_type type;
    union {
        float fval;
        int ival;
        sym_node_t *sym_ref;
        func_exp_t *func_exp;
        array_exp_t *array_exp;
        op_type op;
    };
} exp_node_t;

// Expression tree.
typedef struct exp_tree_s {
    exp_node_t *node;
    int label;
    struct exp_tree_s *left;
    struct exp_tree_s *right;
} exp_tree_t;

// List of expressions.
typedef struct exp_list_s {
    exp_tree_t *exp;
    exp_list_t *next;
} exp_list_t;

void print_exp_tree(exp_tree_t *, int);
ret_type get_exp_type(exp_tree_t *);
exp_node_t *init_exp_node(exp_type, void *);
exp_tree_t *init_exp_tree(exp_node_t *);
exp_list_t *init_exp_list(exp_tree_t *);
func_exp_t *init_func_exp(sym_node_t *, exp_list_t *);
array_exp_t *init_array_exp(sym_node_t *, exp_tree_t *);

void destroy_func_exp(func_exp_t *);
void destroy_array_exp(array_exp_t *);
void destroy_exp_tree(exp_tree_t *);
void destroy_exp_node(exp_node_t *);
void destroy_exp_list(exp_list_t *);

#endif