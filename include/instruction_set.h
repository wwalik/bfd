#pragma once
#include <stddef.h>

typedef struct
{
	size_t index;
	size_t filesize;
	char text[];
} instr_set_t;

instr_set_t *create_instr_set(const char *filename);
void seek_valid_bf_character(instr_set_t *instr);
int find_line_of_index(instr_set_t *instr, size_t index);
void show_instruction_set(instr_set_t *instr);
void destroy_instr_set(instr_set_t *instr);
