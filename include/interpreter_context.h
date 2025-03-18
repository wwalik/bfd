#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include "instruction_set.h"


#define LOOP_LIMIT 32
typedef uint8_t tape_cell_t;
typedef struct
{
	size_t lstack[LOOP_LIMIT];
	size_t lstack_i;

	size_t tape_size;
	size_t tape_i;
	tape_cell_t tape[];
} inter_ctx_t;

typedef enum 
{
	INTER_ERR_NO_ERROR,
	INTER_ERR_NO_INSTRUCTION_SET,
	INTER_ERR_LOOP_LIMIT_REACHED,
	INTER_ERR_MISSING_BRACKET
} inter_ctx_err_t;
extern inter_ctx_err_t interpreter_error; // TODO: This shouldnt be a global what if multiple interpreters?
const char *get_inter_ctx_err_str(inter_ctx_err_t err);

inter_ctx_t *create_inter_ctx(size_t tape_size); // Allocate and Initialize the memory used by an inter_ctx_t object
void destroy_inter_ctx(inter_ctx_t *inter); // Free the memory used by an inter_ctx_t object
typedef enum // Collection of all the states an interpreter might be in
{
	INTER_STATE_SUCCESS,
	INTER_STATE_BREAKPOINT,
	INTER_STATE_ERROR,
	INTER_STATE_EOI = -1 // End of instructions
} inter_ctx_state_t;
inter_ctx_state_t step_inter_ctx(inter_ctx_t *inter, instr_set_t *instr); // Step through an instruction set using an interpreter one valid brainfuck character at a time
void show_inter_ctx_memory(inter_ctx_t *inter); // Display a nice render of the interpreter's memory to standard output
void save_inter_ctx(inter_ctx_t *inter, const char *filename);
void serialize_inter_ctx(inter_ctx_t *inter, FILE *fp); 
inter_ctx_t *load_inter_ctx(const char *filename); 
inter_ctx_t *unserialize_inter_ctx(FILE *fp); 
