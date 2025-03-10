#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "instruction_set.h"
#include "interpreter_context.h"
#include "get_line.inc"

static
void
handle_inter_ctx_error(inter_ctx *inter, instr_set *instr)
{

	const char *msg = interpreter_error_str[interpreter_error];
	printf("Interpreter error: %s\n", msg);

	const int line_index = find_line_of_index(instr, instr->index);
	printf("Error at character %ld, on line %d\n", instr->index, line_index);

   /* * * * * * * * * * * * * * * * * * * * * * *
	* Display the erronous character in context *
	* * * * * * * * * * * * * * * * * * * * * * */
	static const size_t ctx_offset = 4;

	// Print up to the character
	
	// Print the character
	
	// Print after the character

	for (int i = 0; i < ctx_offset; i++)
		putchar(' ');
	puts("^here");
}

#define PROMPT ">>"
#define LINE_SIZE_LIMIT 256
#define DELIM " \n"
int
main(int argc, char *argv[])
{
	inter_ctx *inter = create_inter_ctx(256);
	instr_set *instr = NULL;
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

			instr_set *old_instr = instr;
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
			INTER_CTX_STATE inter_state;
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

			fwrite(inter, sizeof(inter_ctx) + sizeof(TAPE_TYPE) * inter->tape_size, 1, fp);

			fclose(fp);
		}
		if (strcmp(command, "load") == 0)
		{
			char *filename = strtok(NULL, DELIM);
			if (filename == NULL) continue;

			FILE *fp = fopen(filename, "r");
			if (fp == NULL)
				perror("load");

			inter_ctx *old_inter = inter;
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
