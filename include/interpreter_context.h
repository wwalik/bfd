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

inter_ctx *create_inter_ctx(size_t tape_size);
typedef enum 
{
	INTER_SUCCESS,
	INTER_BREAKPOINT,
	INTER_ERROR, // consider using a global variable to give more details about the error errno.h style?
	INTER_END_OF_INSTRUCTIONS = -1
} INTER_CTX_STATE;
INTER_CTX_STATE step_inter_ctx(inter_ctx *inter, instr_set *instr);
void show_inter_ctx_memory(inter_ctx *inter);
void destroy_inter_ctx(inter_ctx *inter);
void serialize_inter_ctx(inter_ctx *inter, FILE *fp);
inter_ctx *unserialize_inter_ctx(FILE *fp);
