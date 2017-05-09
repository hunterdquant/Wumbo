/*
    Generates x86-64 assembly.

    ACTIVATION RECORD:
        h[arg, pbp, ra, bp, lv, sr, trs]l
        arg -> proc/func arguments
        pbp -> parent base pointer
        ra -> return address
        bp -> base pointer
        lv -> local variables
        sr -> saved registers
        trs -> temp register stack
*/

#ifndef __GENCODE__
#define __GENCODE__

#include "defs.h"
#include "exp_tree.h"
#include "stmt.h"

// GCC entry point generation.
void gen_code_begin(FILE *out, char *file_name);
void gen_code_end(FILE *out);
void gen_code_main(FILE *out, char *entry_name);

// Function entry generation.
void gen_code_func_begin(FILE *out, sym_node_t *func_ref);
void gen_code_func_end(FILE *out);
void gen_code_prelude(FILE *out, char *func_name);
void gen_code_prologue(FILE *out, char *func_name);

// Expression gencode.
void gen_code_exp(FILE *out, exp_tree_t *tree);
void gen_code_exp_op_tree(FILE *out, char * op, exp_tree_t *tree);
void gen_code_exp_op_regs(FILE *out, char *op, int r1, int r2);
void gen_code_exp_func(FILE *out, exp_tree_t *tree);
void gen_code_exp_list_push(FILE *out, exp_list_t *exp_list);
void gen_code_exp_list_pop(FILE *out, exp_list_t *exp_list);

// Statement gencode.
void gen_code_stmt(FILE *out, stmt_t *stmt);
void gen_code_if_stmt(FILE *out, stmt_t *stmt);
void gen_code_assign_stmt(FILE *out, stmt_t *stmt);
void gen_code_while_stmt(FILE *out, stmt_t *stmt);
void gen_code_for_stmt(FILE *out, stmt_t *stmt);
void gen_code_proc_stmt(FILE *out, stmt_t *stmt);

// Write and read handling called by proc if name is detected.
void gen_code_read(FILE *out, stmt_t *stmt);
void gen_code_write(FILE *out, stmt_t *stmt);

// Gen labels for write and read.
void gen_code_write_label(FILE *out);
void gen_code_read_label(FILE *out);

// Utility functions.
int is_leaf(exp_tree_t *tree);
void gen_code_label_tree(exp_tree_t *tree);
int gen_code_stack_push(int entry);
int gen_code_stack_pop();
int gen_code_stack_peek();
void gen_code_stack_swap();

// Generate register caching.
void gen_code_push_used_regs(FILE *out);
void gen_code_pop_used_regs(FILE *out);

// Generate non local access.
void gen_code_non_local_access(FILE *out, int depth, int offset, int read);

#endif