#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "instruction_set.h"
#include "interpreter_context.h"
#include "get_line.inc"

// God forbid me from writing text formatting code ever again
static
void
print_instr_set_index_in_context(instr_set_t *instr)
{
	static const int max_n_lines = 2;
	static const int max_n_chars = 30;
	static const int tab_width = 4;


	// seek back until we've went over more lines or characters than the maximum specified
	size_t pos = instr->index;
	for (int n_lines = 0,
			n_chars = 0;
			pos >= 0 &&
			n_chars < max_n_chars &&
			n_lines < max_n_lines;
			pos--)
	{
		if (instr->text[pos] == '\n')
			n_lines++;
		n_chars++;
	}

	// Print up to and including the character
	int chars_on_line = 0; 
	while (pos < instr->index || instr->text[pos] != '\n')
	{
		/* If the character is \t we'll handle it ourselves 
		to make sure its width is what we expect */
		if (instr->text[pos] == '\t')
		{

			chars_on_line += tab_width;
			for (int i = 0; i < tab_width; i++)
				putchar(' ');
			pos++;
			continue;
		}

		// Count the number of characters on the current line
		if (instr->text[pos] == '\n')
			chars_on_line = 0;
		else
			chars_on_line++;

		putchar(instr->text[pos]);
		pos++;
	}
	putchar('\n');

	// Point to the exact character
	while (--chars_on_line)
	{
		putchar(' ');
	}
	puts("^here");
}
static
void
handle_inter_ctx_error(inter_ctx_t *inter, instr_set_t *instr)
{

	const char *msg = get_inter_ctx_err_str(interpreter_error);
	printf("Interpreter error: %s\n", msg);

	const int line_index = find_line_of_index(instr, instr->index);
	printf("Error at character %ld, which is on line %d\n", instr->index, line_index);

	print_instr_set_index_in_context(instr);
}

#define PROMPT ">>"
#define LINE_SIZE_LIMIT 256
#define DELIM " \n"
int
main(int argc, char *argv[])
{
	inter_ctx_t *inter = create_inter_ctx(256);
	instr_set_t *instr = NULL;
	if (argc == 2)
	{
		instr = create_instr_set(argv[1]);
	} else if (argc > 2)
	{
		printf("%s: Too many arguments\n", argv[0]);
	}

	char *line = malloc(LINE_SIZE_LIMIT);
	size_t bytes_read;
	do
	{
		printf(PROMPT);
		bytes_read =
			get_line(line, LINE_SIZE_LIMIT, stdin);

		char *command = strtok(line, DELIM);
		if (command == NULL)
			continue;

		// I need to fix this if statement chaos asap
		if (strcmp(command, "file") == 0)
		{
			char *filename = strtok(NULL, DELIM);
			if (filename == NULL) continue;

			instr_set_t *old_instr = instr;
			instr = create_instr_set(filename);
			if (instr == NULL)
				instr = old_instr;
		}
		if (strcmp(command, "instructions") == 0)
		{
			show_instruction_set(instr);
		}
		if (strcmp(command, "memory") == 0)
		{
			show_inter_ctx_memory(inter);
		}
		if (strcmp(command, "step") == 0)
		{
			step_inter_ctx(inter, instr);
		}
		if (strcmp(command, "run") == 0)
		{
			inter_ctx_state_t inter_state;
			while ( (inter_state = step_inter_ctx(inter, instr)) == INTER_STATE_SUCCESS);
			switch (inter_state)
			{
				case INTER_STATE_BREAKPOINT:
					printf("Hit a breakpoint at character %ld\n", instr->index);
					instr->index++;
					seek_valid_bf_character(instr);
					break;
				case INTER_STATE_ERROR:
					handle_inter_ctx_error(inter, instr);
					break;
				case INTER_STATE_EOI:
					break;
				default:
					printf("how did this happen?\n unhandled interpreter state in run loop\n");
			}
		}
		if (strcmp(command, "reset") == 0)
		{
			instr->index = 0;
			destroy_inter_ctx(inter);
			inter = create_inter_ctx(256);
		}
		if (strcmp(command, "save") == 0)
		{
			char *filename = strtok(NULL, DELIM);
			if (filename == NULL) continue;

			FILE *fp = fopen(filename, "w+");
			if (fp == NULL)
				perror("save");

			fwrite(inter, sizeof(inter_ctx_t) + sizeof(tape_cell_t) * inter->tape_size, 1, fp);

			fclose(fp);
		}
		if (strcmp(command, "load") == 0)
		{
			char *filename = strtok(NULL, DELIM);
			if (filename == NULL) continue;

			FILE *fp = fopen(filename, "r");
			if (fp == NULL)
				perror("load");

			inter_ctx_t *old_inter = inter;
			inter = unserialize_inter_ctx(fp);
			if (inter == NULL)
			{
				printf("Could not load file\n");
				inter = old_inter;
			} else
				destroy_inter_ctx(old_inter);

			fclose(fp);
		}

	} while (bytes_read > 0);
	
	// CTRL+D i.e. EOF
	if (bytes_read == 0)
		putchar('\n');

	free(line);
	destroy_inter_ctx(inter);
	if (instr != NULL)
		destroy_instr_set(instr);

	exit(EXIT_SUCCESS);
}
