#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include "instruction_set.h"


#define LOOP_LIMIT 16 
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
	INTER_NO_INSTRUCTION_SET,
	INTER_LOOP_LIMIT_REACHED,
	INTER_MISSING_BRACKET // TODO: find a better name
} inter_ctx_err_t;
extern inter_ctx_err_t interpreter_error;
extern const char *interpreter_error_str[];

inter_ctx_t *create_inter_ctx(size_t tape_size);
typedef enum 
{
	INTER_STATE_SUCCESS,
	INTER_STATE_BREAKPOINT,
	INTER_STATE_ERROR,
	INTER_STATE_EOI = -1 // End of instructions
} inter_ctx_state_t;
inter_ctx_state_t step_inter_ctx(inter_ctx_t *inter, instr_set_t *instr);
void show_inter_ctx_memory(inter_ctx_t *inter);
void destroy_inter_ctx(inter_ctx_t *inter);
void serialize_inter_ctx(inter_ctx_t *inter, FILE *fp);
inter_ctx_t *unserialize_inter_ctx(FILE *fp);
