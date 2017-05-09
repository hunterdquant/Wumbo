#include <stdio.h>

#include "gencode.h"
#include "exp_tree.h"
#include "stmt.h"
#include "defs.h"

extern sym_stack_t *sym_table;

long int label = 0;

int sp = 0;
int ssize = 9;
int rstack[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
char *regs[] = {"%rcx", "%r8", "%r9", "%r10", "%r11", "%r12", "%r13", "%r14", "%r15"};

void gen_code_begin(FILE *out, char *file_name) {
    wfprintf(out, "\t.file\t\"%s\"\n", file_name);
    wfprintf(out, "\t.text\n");
}

void gen_code_end(FILE *out) {
    wfprintf(out, "\t.ident\t\"GCC: (Ubuntu 4.8.5-2ubuntu1-14.04.1) 4.8.5\"\n");
    wfprintf(out, "\t.section\t.note.GNU-stack,\"\",@progbits\n");
}

void gen_code_main(FILE *out, char *entry_name) {
    wfprintf(out, "main:\n");
    wfprintf(out, "\tpushq\t%%rbp\n");
    wfprintf(out, "\tpushq\t%%rbp\n");
    wfprintf(out, "\tmovq\t%%rsp,\t%%rbp\n");
    wfprintf(out, "\tmovl\t$0,\t%%eax\n");
    wfprintf(out, "\tcall\t%s\n", entry_name);
    wfprintf(out, "\tpopq\t%%rbp\n");
    wfprintf(out, "\tpopq\t%%rbp\n");
    wfprintf(out, "\tret\n");
}

void gen_code_func_begin(FILE *out, sym_node_t *func_ref) {
    wfprintf(out, "%s:\n", func_ref->sym);
    wfprintf(out, "\tpushq\t%%rbp\n");
    wfprintf(out, "\tmovq\t%%rsp,\t%%rbp\n");
    wfprintf(out, "\taddq\t$%d,\t%%rsp\n", func_ref->offset);
    gen_code_push_used_regs(out);
}

void gen_code_func_end(FILE *out) {
    gen_code_pop_used_regs(out);
    wfprintf(out, "\tmovq\t%%rbp,\t%%rsp\n");
    wfprintf(out, "\tpopq\t%%rbp\n");
    wfprintf(out, "\tret\n");
}

void gen_code_prelude(FILE *out, char *func_name) {
    wfprintf(out, "\t.globl\t%s\n", func_name);
    wfprintf(out, "\t.type\t%s,\t@function\n", func_name);
}

void gen_code_prologue(FILE *out, char *func_name) {
    wfprintf(out, "\t.size\t%s,\t.-%s\n", func_name, func_name);
}


void gen_code_exp(FILE *out, exp_tree_t *tree) {
    if (!tree) {
        return;
    }
    wprintf("\n\na%d\n\n", tree->label);
    if (is_leaf(tree)) { // Case 0 - leaf node
        if (tree->node->type == INTEGER_EXP) {
            wfprintf(out, "\tmovq\t$%d,\t%s\n", tree->node->ival, regs[gen_code_stack_peek()]);
        } else if (tree->node->type == REAL_EXP) {
            panic("\nGencode doesn't support reals.\n");
        } else if (tree->node->type == FUNC_EXP) {
            gen_code_exp_func(out, tree);
            wfprintf(out, "\tmovq\t%%rax,\t%s\n", regs[gen_code_stack_peek()]);
        } else if (tree->node->type == VAR_EXP) {
            if (sym_table->depth != tree->node->sym_ref->depth) {
                gen_code_non_local_access(out, sym_table->depth - tree->node->sym_ref->depth, tree->node->sym_ref->offset, 1);
                wfprintf(out, "\tmovq\t%%rbx,\t%s\n", regs[gen_code_stack_peek()]);
            } else {
                wfprintf(out, "\tmovq\t%d(%%rbp),\t%s\n", tree->node->sym_ref->offset, regs[gen_code_stack_peek()]);
            }
        } else if (tree->node->type == ARRAY_EXP) {
            panic("\nGencode doesn't support arrays.\n");
        } else {
            panic("\nUnxepected leaf node for expression in gencode.\n");
        }
    } else if ((tree->right && tree->left) && is_leaf(tree->right)) {
        gen_code_exp(out, tree->left);
        if (tree->node->type != OP_EXP) {
            panic("\nParent node for expression in gencode must be an operator.\n");
        }
        switch (tree->node->op) {
            case ADD_OP:
                gen_code_exp_op_tree(out, "addq", tree->right);
                break;
            case SUB_OP:
                gen_code_exp_op_tree(out, "subq", tree->right);
                break;
            case MUL_OP:
                gen_code_exp_op_tree(out, "imul", tree->right);
                break;
            case DIV_OP:
                if (tree->right->node->type == INTEGER_EXP) {
                    wfprintf(out, "\tmovq\t%s,\t%%rax\n", regs[gen_code_stack_peek()]);
                    wfprintf(out, "\tmovq\t$%d,\t%%rbx\n", tree->right->node->ival);
                    wfprintf(out, "\tmovq\t$0,\t%%rdx\n");
                    wfprintf(out, "\tidivq\t%%rbx\n");
                    wfprintf(out, "\tmovq\t%%rax,\t%s\n", regs[gen_code_stack_peek()]);
                } else if (tree->right->node->type == REAL_EXP) {
                    panic("\nGencode doesn't support reals.\n");
                } else if (tree->right->node->type == FUNC_EXP) {
                    gen_code_exp_func(out, tree->right);
                    wfprintf(out, "\tmovq\t%%rax,\t%%rbx\n");
                    wfprintf(out, "\tmovq\t%s,\t%%rax\n", regs[gen_code_stack_peek()]);
                    wfprintf(out, "\tmovq\t$0,\t%%rdx\n");
                    wfprintf(out, "\tidivq\t%%rbx\n");
                    wfprintf(out, "\tmovq\t%%rax,\t%s\n", regs[gen_code_stack_peek()]);
                } else if (tree->right->node->type == VAR_EXP) {
                    wfprintf(out, "\tmovq\t%s,\t%%rax\n", regs[gen_code_stack_peek()]);
                    wfprintf(out, "\tmovq\t$0,\t%%rdx\n");
                    if (sym_table->depth != tree->right->node->sym_ref->depth) {
                        gen_code_non_local_access(out, sym_table->depth - tree->right->node->sym_ref->depth, tree->right->node->sym_ref->offset, 1);
                        wfprintf(out, "\tidivq\t%%rbx\n");
                    } else {
                        wfprintf(out, "\tidivq\t%d(%%rbp)\n", tree->right->node->sym_ref->offset);
                    }
                    wfprintf(out, "\tmovq\t%%rax,\t%s\n", regs[gen_code_stack_peek()]);
                } else if (tree->right->node->type == ARRAY_EXP) {
                    panic("\nGencode doesn't support arrays.\n");
                } else {
                    panic("\nUnxepected leaf node for expression in gencode.\n");
                }
                break;
            case EQ_OP:
            case NEQ_OP:
            case L_OP:
            case G_OP:
            case LE_OP:
            case GE_OP:
                if (tree->right->node->type == INTEGER_EXP) {
                    wfprintf(out, "\tmovq\t$%d,%%rax\n", tree->right->node->ival);
                    wfprintf(out, "\tcmp\t%%rax,\t%s\n", regs[gen_code_stack_peek()]);
                } else if (tree->right->node->type == REAL_EXP) {
                    panic("\nGencode doesn't support reals.\n");
                } else if (tree->right->node->type == FUNC_EXP) {
                    gen_code_exp_func(out, tree->right);
                    wfprintf(out, "\tcmp\t%%rax,\t%s\n", regs[gen_code_stack_peek()]);
                } else if (tree->right->node->type == VAR_EXP) {
                    wfprintf(out, "\tcmp\t%d(%%rbp),\t%s\n", tree->right->node->sym_ref->offset, regs[gen_code_stack_peek()]);
                } else if (tree->right->node->type == ARRAY_EXP) {
                    panic("\nGencode doesn't support arrays.\n");
                } else {
                    panic("\nUnxepected leaf node for expression in gencode.\n");
                }
                break;
            default:
                panic("\nUnxepected operator for expression in gencode.\n");
        }
    } else if (1 <= tree->left->label < tree->right->label && tree->left->label < ssize - sp) {
        gen_code_stack_swap();
        gen_code_exp(out, tree->right);
        int r = gen_code_stack_pop();
        gen_code_exp(out, tree->left);
        if (tree->node->type != OP_EXP) {
            panic("\nParent node for expression in gencode must be an operator.\n");
        }
        switch (tree->node->op) {
            case ADD_OP:
                gen_code_exp_op_regs(out, "addq", r, gen_code_stack_peek());
                break;
            case SUB_OP:
                gen_code_exp_op_regs(out, "subq", r, gen_code_stack_peek());
                break;
            case MUL_OP:
                gen_code_exp_op_regs(out, "imul", r, gen_code_stack_peek());
                break;
            case DIV_OP:
                wfprintf(out, "\tmovq\t%s,\t%%rax\n", regs[gen_code_stack_peek()]);
                wfprintf(out, "\tmovq\t$0,\t%%rdx\n");
                wfprintf(out, "\tidivq\t%s\n", regs[r]);
                wfprintf(out, "\tmovq\t%%rax,\t%s\n", regs[gen_code_stack_peek()]);
                break;
            case EQ_OP:
            case NEQ_OP:
            case L_OP:
            case G_OP:
            case LE_OP:
            case GE_OP:
                wfprintf(out, "\tcmp\t%s,\t%s\n", regs[r], regs[gen_code_stack_peek()]);
                break;
            default:
                panic("\nUnxepected operator for expression in gencode.\n");
        }
        gen_code_stack_push(r);
        gen_code_stack_swap();

    } else if (tree->right->label <= tree->left->label && tree->right->label < ssize - sp) {
        gen_code_exp(out, tree->left);
        int r = gen_code_stack_pop();
        gen_code_exp(out, tree->right);
        if (tree->node->type != OP_EXP) {
            panic("\nParent node for expression in gencode must be an operator.\n");
        }
        switch (tree->node->op) {
            case ADD_OP:
                gen_code_exp_op_regs(out, "addq", gen_code_stack_peek(), r);
                break;
            case SUB_OP:
                gen_code_exp_op_regs(out, "subq", gen_code_stack_peek(), r);
                break;
            case MUL_OP:
                gen_code_exp_op_regs(out, "imul", gen_code_stack_peek(), r);
                break;
            case DIV_OP:
                wfprintf(out, "\tmovq\t%s,\t%%rax\n", regs[r]);
                wfprintf(out, "\tmovq\t$0,\t%%rdx\n");
                wfprintf(out, "\tidivq\t%s\n", regs[gen_code_stack_peek()]);
                wfprintf(out, "\tmovq\t%%rax,\t%s\n", regs[r]);
                break;
            case EQ_OP:
            case NEQ_OP:
            case L_OP:
            case G_OP:
            case LE_OP:
            case GE_OP:
                wfprintf(out, "\tcmp\t%s,\t%s\n", regs[gen_code_stack_peek()], regs[r]);
                break;
            default:
                panic("\nUnxepected operator for expression in gencode.\n");
        }
        gen_code_stack_push(r);
    } else {
       // gen_code_exp(tree->right);

    }
}

void gen_code_exp_op_tree(FILE *out, char * op, exp_tree_t *tree) {
    if (tree->node->type == INTEGER_EXP) {
        wfprintf(out, "\t%s\t$%d,\t%s\n", op, tree->node->ival, regs[gen_code_stack_peek()]);
    } else if (tree->node->type == REAL_EXP) {
        panic("\nGencode doesn't support reals.\n");
    } else if (tree->node->type == FUNC_EXP) {
        int r = gen_code_stack_pop();
        gen_code_exp_func(out, tree);
        gen_code_stack_push(r);
        wfprintf(out, "\t%s\t%%rax,\t%s\n", op, regs[gen_code_stack_peek()]);
    } else if (tree->node->type == VAR_EXP) {
        wfprintf(out, "\t%s\t%d(%%rbp),\t%s\n", op, tree->node->sym_ref->offset, regs[gen_code_stack_peek()]);
    } else if (tree->node->type == ARRAY_EXP) {
        panic("\nGencode doesn't support arrays.\n");
    } else {
        panic("\nUnxepected leaf node for expression in gencode.\n");
    }
}

void gen_code_exp_func(FILE *out, exp_tree_t *tree) {
    exp_list_t *exp_list = tree->node->func_exp->args;
    gen_code_exp_list_push(out, exp_list);
    if (sym_table->depth + 1 == tree->node->func_exp->sym_ref->depth) {
        wfprintf(out, "\tpushq\t%%rbp\n");
    } else {
        int i, diff = sym_table->depth - tree->node->func_exp->sym_ref->depth;
        if (diff < 0) {
            diff = -diff; 
        }
        wfprintf(out, "\tmovq\t16(%%rbp),\t%%rbx\n");
        for (i = 0; i < diff; i++) {
            wfprintf(out, "\tmovq\t16(%%rbx),\t%%rbx\n");
        }
        wfprintf(out, "\tpushq\t%%rbx\n");
    }
    wfprintf(out, "\tcall\t%s\n", tree->node->func_exp->sym_ref->sym);
    wfprintf(out, "\tpopq\t%%rbx\n");
    gen_code_exp_list_pop(out, exp_list);
}

void gen_code_exp_list_push(FILE *out, exp_list_t *exp_list) {
    if (!exp_list) {
        return;
    }
    gen_code_exp_list_push(out, exp_list->next);
    gen_code_label_tree(exp_list->exp);
    gen_code_exp(out, exp_list->exp);
    wfprintf(out, "\tpushq\t%s\n", regs[gen_code_stack_peek()]);
}

void gen_code_exp_list_pop(FILE *out, exp_list_t *exp_list) {
    if (!exp_list) {
        return;
    }
    gen_code_exp_list_pop(out, exp_list->next);
    wfprintf(out, "\tpopq\t%%rbx\n");
}

void gen_code_exp_op_regs(FILE *out, char *op,  int r1, int r2) {
    wfprintf(out, "\t%s\t%s,\t%s\n", op, regs[r1], regs[r2]);
}

void gen_code_stmt(FILE *out, stmt_t *stmt) {
    if (!stmt) {
        return;
    }
    switch (stmt->type) {
        case COMPOUND_STMT: {
            stmt_list_t *stmt_list = stmt->stmt.compound_stmt.body_stmt;
            while (stmt_list) {
                gen_code_stmt(out, stmt_list->stmt);
                stmt_list = stmt_list->next;
            }
            break;
        }
        case ASSIGNMENT_STMT:
            gen_code_assign_stmt(out, stmt);
            break;
        case PROCEDURE_STMT: {
            gen_code_proc_stmt(out, stmt);
            break;
        }
        case IF_STMT:
            gen_code_if_stmt(out, stmt);
            break;
        case WHILE_STMT:
            gen_code_while_stmt(out, stmt);
            break;
        case FOR_STMT:
            gen_code_for_stmt(out, stmt);
            break;
        default:
            wprintf("Unrecognized stmt type\n");
    }
}

void gen_code_if_stmt(FILE *out, stmt_t *stmt) {
    if (stmt->type != IF_STMT) {
        panic("\nError in if gencode, expected proc.\n");
    }
    exp_tree_t *tree = stmt->stmt.if_stmt.exp;
    gen_code_label_tree(tree);
    gen_code_exp(out, tree);
    if (tree->node->type != OP_EXP) {
        panic("Invalid operator in gencode, if stmt can't use not or arithmetic ops at root.");
    }
    // Avoids label conflict with recursive calls.
    int loc_label = label;
    label+=2;
    switch (tree->node->op) {
        case EQ_OP:
            wfprintf(out, "\tjne\t.L%d\n", loc_label);
            break;
        case NEQ_OP:
            wfprintf(out, "\tje\t.L%d\n", loc_label);
            break;
        case L_OP:
            wfprintf(out, "\tjge\t.L%d\n", loc_label);
            break;
        case G_OP:
            wfprintf(out, "\tjle\t.L%d\n", loc_label);
            break;
        case LE_OP:
            wfprintf(out, "\tjg\t.L%d\n", loc_label);
            break;
        case GE_OP:
            wfprintf(out, "\tjl\t.L%d\n", loc_label);
            break;
        default:
            panic("Invalid operator in gencode, if stmt can't use not or arithmetic ops at root.");
    }
    gen_code_stmt(out, stmt->stmt.if_stmt.true_stmt);
    wfprintf(out, "\tjmp\t.L%d\n", loc_label+1);
    wfprintf(out, ".L%d:\n", loc_label);
    loc_label++;
    if (stmt->stmt.if_stmt.false_stmt) {
        gen_code_stmt(out, stmt->stmt.if_stmt.false_stmt);
    }
    wfprintf(out, ".L%d:\n", loc_label);
}

void gen_code_assign_stmt(FILE *out, stmt_t *stmt) {
    sym_node_t *node = stmt->stmt.assn_stmt.sym_ref;
    exp_tree_t *tree = stmt->stmt.assn_stmt.exp;
    gen_code_label_tree(tree);
    gen_code_exp(out, tree);
    if (node->ntype == FUNC_NODE) {
        wfprintf(out, "\tmovq\t%s,\t%%rax\n", regs[gen_code_stack_peek()]);
    } else {
        if (sym_table->depth != node->depth) {
            gen_code_non_local_access(out, sym_table->depth - node->depth, node->offset, 0);
            wfprintf(out, "\tmovq\t%s,\t(%%rbx)\n", regs[gen_code_stack_peek()]);
        } else {
            wfprintf(out, "\tmovq\t%s,\t%d(%%rbp)\n", regs[gen_code_stack_peek()], node->offset);
        }
    }
}

void gen_code_while_stmt(FILE *out, stmt_t *stmt) {
    if (stmt->type != WHILE_STMT) {
        panic("\nError in while gencode, expected proc.\n");
    }
    exp_tree_t *tree = stmt->stmt.while_stmt.exp;
    if (tree->node->type != OP_EXP) {
        panic("Invalid operator in gencode, if stmt can't use not or arithmetic ops at root.");
    }
    int loc_label = label;
    label+=2;
    wfprintf(out, ".L%d:\n", loc_label);
    gen_code_label_tree(tree);
    gen_code_exp(out, tree);
    // Avoids label conflict with recursive calls.
    switch (tree->node->op) {
        case EQ_OP:
            wfprintf(out, "\tjne\t.L%d\n", loc_label+1);
            break;
        case NEQ_OP:
            wfprintf(out, "\tje\t.L%d\n", loc_label+1);
            break;
        case L_OP:
            wfprintf(out, "\tjge\t.L%d\n", loc_label+1);
            break;
        case G_OP:
            wfprintf(out, "\tjle\t.L%d\n", loc_label+1);
            break;
        case LE_OP:
            wfprintf(out, "\tjg\t.L%d\n", loc_label+1);
            break;
        case GE_OP:
            wfprintf(out, "\tjl\t.L%d\n", loc_label+1);
            break;
        default:
            panic("Invalid operator in gencode, if stmt can't use not or arithmetic ops at root.");
    }
    gen_code_stmt(out, stmt->stmt.while_stmt.body_stmt);
    wfprintf(out, "\tjmp\t.L%d\n", loc_label);
    wfprintf(out, ".L%d:\n", loc_label+1);
}

void gen_code_for_stmt(FILE *out, stmt_t *stmt) {
    if (stmt->type != FOR_STMT) {
        panic("\nError in for gencode, expected proc.\n");
    }
    gen_code_assign_stmt(out, stmt->stmt.for_stmt.assign_stmt);
    exp_tree_t *tree = stmt->stmt.for_stmt.exp_bound;
    int loc_label = label;
    label+=2;
    wfprintf(out, ".L%d:\n", loc_label);
    gen_code_label_tree(tree);
    gen_code_exp(out, tree);
    // Avoids label conflict with recursive calls.
    wfprintf(out, "\tcmp\t\t%d(%%rbp),%s\n", stmt->stmt.for_stmt.assign_stmt->stmt.assn_stmt.sym_ref->offset, regs[gen_code_stack_peek()]);
    wfprintf(out, "\tjle\t.L%d\n", loc_label+1);
    gen_code_stmt(out, stmt->stmt.for_stmt.body_stmt);
    wfprintf(out, "\taddq\t$1,\t%d(%%rbp)\n", stmt->stmt.for_stmt.assign_stmt->stmt.assn_stmt.sym_ref->offset);
    wfprintf(out, "\tjmp\t.L%d\n", loc_label);
    wfprintf(out, ".L%d:\n", loc_label+1);
}

void gen_code_proc_stmt(FILE *out, stmt_t *stmt) {
    if (stmt->type != PROCEDURE_STMT) {
        panic("\nError in procedure gencode, expected proc.\n");
    }
    if (strcmp(stmt->stmt.proc_stmt.sym_ref->sym, "write") == 0) {
        gen_code_write(out, stmt);
        return;
    } else if (strcmp(stmt->stmt.proc_stmt.sym_ref->sym, "read") == 0) {
        gen_code_read(out, stmt);
        return;
    }
    exp_list_t *exp_list = stmt->stmt.proc_stmt.exp_list;
    gen_code_exp_list_push(out, exp_list);
    if (sym_table->depth + 1 == stmt->stmt.proc_stmt.sym_ref->depth) {
        wfprintf(out, "\tpushq\t%%rbp\n");
    } else {
        int i, diff = sym_table->depth - stmt->stmt.proc_stmt.sym_ref->depth;
        if (diff < 0) {
            diff = -diff; 
        }
        wfprintf(out, "\tmovq\t16(%%rbp),\t%%rbx\n");
        for (i = 0; i < diff; i++) {
            wfprintf(out, "\tmovq\t16(%%rbx),\t%%rbx\n");
        }
        wfprintf(out, "\tpushq\t%%rbx\n");
    }
    wfprintf(out, "\tcall\t%s\n", stmt->stmt.proc_stmt.sym_ref->sym);
    wfprintf(out, "\tpopq\t%%rbx\n");
    gen_code_exp_list_pop(out, exp_list);

}

void gen_code_read(FILE *out, stmt_t *stmt) {
    sym_node_t *sym = stmt->stmt.proc_stmt.exp_list->exp->node->sym_ref;
    wfprintf(out, "\tleaq\t%d(%%rbp),\t%%rax\n", sym->offset);
    wfprintf(out, "\tmovq\t%%rax,\t%%rsi\n");
    wfprintf(out, "\tmovq\t$.LC0,\t%%rdi\n");
    wfprintf(out, "\tmovq\t$0,\t%%rax\n");
    wfprintf(out, "\tcall\t__isoc99_scanf\n");
}

void gen_code_write(FILE *out, stmt_t *stmt) {
    exp_tree_t *tree = stmt->stmt.proc_stmt.exp_list->exp;
    gen_code_label_tree(tree);
    gen_code_exp(out, tree);
    if (tree->node->type == FUNC_EXP) {
        wfprintf(out, "\tmovq\t%%rax,\t%%rsi\n");
    } else {
        wfprintf(out, "\tmovq\t%s,\t%%rsi\n", regs[gen_code_stack_peek()]);
    }
    wfprintf(out, "\tmovq\t$.LC1,\t%%rdi\n");
    wfprintf(out, "\tmovq\t$0,\t%%rax\n");
    wfprintf(out, "\tcall\tprintf\n");
}

void gen_code_read_label(FILE *out) {
    wfprintf(out, ".LC0:\n");
    wfprintf(out, "\t.string\t\"%%ld\"\n");
}

void gen_code_write_label(FILE *out) {
    wfprintf(out, ".LC1:\n");
    wfprintf(out, "\t.string\t\"%%ld\\n\"\n");
}

int is_leaf(exp_tree_t *tree) {
    if (!tree) {
        return 0;
    }
    if (!tree->left && !tree->right) {
        return 1;
    }
    return 0;
}

void gen_code_label_tree(exp_tree_t *tree) {
    if (!tree || is_leaf(tree)) {
        return;
    }

    gen_code_label_tree(tree->left);
    gen_code_label_tree(tree->right);
    if (is_leaf(tree->left)) {
        tree->left->label = 1;
    } else if (is_leaf(tree->right)) {
        tree->right->label = 0;
    }
    if (tree->left && tree->right) {
        if (tree->left->label == tree->right->label) {
            tree->label = tree->left->label + 1;
        } else {
            tree->label = (tree->left->label >= tree->right->label) ? tree->left->label : tree->right->label;
        }
    } else if (tree->left) {
        tree->label = tree->left->label;
    } else if (tree->right) {
        tree->label = tree->right->label;
    }
}


int gen_code_stack_push(int entry) {
    if (sp == 0) {
        panic("\nInvalid stack push in gencode\n");
    }
    rstack[sp-1] = entry;
    sp -= 1;
    return rstack[sp];
}
int gen_code_stack_pop() {
    if (sp == ssize) {
        panic("\nInvalid stack pop in gencode\n");
    }
    int tmp = rstack[sp];
    sp += 1;
    return tmp;
}
int gen_code_stack_peek() {
    return rstack[sp];
}

void gen_code_stack_swap() {
    int tmp1 = gen_code_stack_pop();
    int tmp2 = gen_code_stack_pop();
    gen_code_stack_push(tmp1);
    gen_code_stack_push(tmp2);
}

void gen_code_push_used_regs(FILE *out) {
    int i;
    for (i = 0; i < ssize; i++) {
        wfprintf(out, "\tpushq\t%s\n", regs[i]);
    }
}

void gen_code_pop_used_regs(FILE *out) {
    int i;
    for (i = ssize-1; i >= 0; i--) {
        wfprintf(out, "\tpopq\t%s\n", regs[i]);
    }
}

void gen_code_non_local_access(FILE *out, int depth, int offset, int read) {
    wfprintf(out, "\tmovq\t16(%%rbp),\t%%rbx\n");
    int i;
    for (i = 1; i < depth; i++) {
        wfprintf(out, "\tmovq\t16(%%rbx),\t%%rbx\n");
    }
    if (read) {
        if (offset < 0) {
            wfprintf(out, "\tmovq\t%d(%%rbx),\t%%rbx\n", offset);
        } else {
            wfprintf(out, "\tmovq\t%d(%%rbx),\t%%rbx\n", offset);
        }
    } else {
        if (offset < 0) {
            wfprintf(out, "\tleaq\t%d(%%rbx),\t%%rbx\n", offset);
        } else {
            wfprintf(out, "\tleaq\t%d(%%rbx),\t%%rbx\n", offset);
        }
    }
}