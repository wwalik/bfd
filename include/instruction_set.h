#pragma once
#include <stddef.h>

typedef struct
{
	size_t index;
	size_t filesize;
	char text[];
} instr_set;

instr_set *create_instr_set(const char *filename);
void show_instruction_set(instr_set *instr);
void destroy_instr_set(instr_set *instr);
