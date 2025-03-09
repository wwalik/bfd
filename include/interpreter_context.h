#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include "instruction_set.h"


#define LOOP_LIMIT 16 
typedef uint8_t TAPE_TYPE;
typedef struct
{
	size_t lstack[LOOP_LIMIT];
	size_t lstack_i;

	size_t tape_size;
	size_t tape_i;
	TAPE_TYPE tape[];
} inter_ctx;

typedef enum 
{
	INTER_NO_INSTRUCTION_SET,
	INTER_LOOP_LIMIT_REACHED,
	INTER_MISSING_BRACKET // TODO: find a better name
} INTER_ERROR_TYPE;
extern INTER_ERROR_TYPE interpreter_error;
extern const char *interpreter_error_str[];

inter_ctx *create_inter_ctx(size_t tape_size);
typedef enum 
{
	INTER_STATE_SUCCESS,
	INTER_STATE_BREAKPOINT,
	INTER_STATE_ERROR,
	INTER_STATE_EOI = -1 // End of instructions
} INTER_CTX_STATE;
INTER_CTX_STATE step_inter_ctx(inter_ctx *inter, instr_set *instr);
void show_inter_ctx_memory(inter_ctx *inter);
void destroy_inter_ctx(inter_ctx *inter);
void serialize_inter_ctx(inter_ctx *inter, FILE *fp);
inter_ctx *unserialize_inter_ctx(FILE *fp);
