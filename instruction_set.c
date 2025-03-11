#include "instruction_set.h"

#include <stdio.h>
#include <stdlib.h>

instr_set_t *
create_instr_set(const char *filename)
{
	FILE *fp = fopen(filename, "r");
	if (fp == NULL)
	{
		perror("Could not open file");
		return NULL;
	}

	fseek(fp, 0, SEEK_END);
	size_t filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	instr_set_t *instr = malloc(sizeof(instr_set_t) + filesize);
	if (instr == NULL)
	{
		perror("instr_set malloc()");
		return NULL;
	}
	*instr = (instr_set_t)
	{
		.index = 0,
		.filesize = filesize
	};

	size_t bytes_read =
		fread(instr->text, 1, filesize, fp);
	if (bytes_read < filesize)
	{
		perror("Could not read file");
		return NULL;
	}

	fclose(fp);
	return instr;
}
// enum
// {
// 	LEFT_ARROW,
// 	RIGHT_ARROW,
// 	PLUS,
// 	MINUS,
// 	DOT,
// 	COMMA,
// 	OPEN_BRACKET,
// 	CLOSED_BRACKET,
// 	HASHTAG,
// 	N_VALID_BRAINFUCK_CHARACTERS
// };
static const char valid_brainfuck_characters[] =
{
'<',
'>',
'+',
'-',
'.',
',',
'[',
']',
'#',
};
void
seek_valid_bf_character(instr_set_t *instr)
{
	while (instr->index < instr->filesize)
	{
		char ch = instr->text[instr->index];
		for (int i = 0; i < sizeof(valid_brainfuck_characters)/sizeof(valid_brainfuck_characters[0]); i++)
			if (ch == valid_brainfuck_characters[i])
				return;

		instr->index++;
	}
}
int
find_line_of_index(instr_set_t *instr, size_t index)
{
	if (index >= instr->filesize)
		return -1; // TODO: ummmmm how

	int n_lines = 1;
	for (int i = 0; i < index; i++)
		if (instr->text[i] == '\n')
			n_lines++;

	return n_lines;
}
void
show_instruction_set(instr_set_t *instr)
{
	if (instr == NULL)
	{
		printf("No instruction set is loaded\n");
		return;
	}

	for (int i = 0; i < instr->filesize; i++)
	{
		if (i == instr->index)
			printf("\033[6m");

		putchar(instr->text[i]);
		printf("\033[0m");
	}
}
void
destroy_instr_set(instr_set_t *instr)
{
	free(instr);
	instr = NULL;
}
