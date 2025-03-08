#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "instruction_set.h"
#include "interpreter_context.h"
#include "get_line.cc"

#define PROMPT ">>"
#define DELIM " \n"
int
main(int argc, char *argv[])
{
	inter_ctx *inter = create_inter_ctx(256);
	instr_set *instr = NULL;

	char *line = malloc(256);
	size_t bytes_read;
	do
	{
		printf(">>");
		bytes_read =
			get_line(line, 256, stdin);

		char *command = strtok(line, DELIM);
		if (command == NULL)
			continue;

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
			// Show where we are in the instructions
			if (instr == NULL)
				printf("The instruction set is empty\n");
			else 
				printf("%s", instr->text);
		}
		if (strcmp(command, "memory") == 0)
		{
			show_inter_ctx_memory(inter);
		}
		if (strcmp(command, "step") == 0)
		{
			if (instr == NULL)
			{
				printf("Please load an instruction set first\n");
				continue;
			}
			step_inter_ctx(inter, instr);
		}
		if (strcmp(command, "run") == 0)
		{
			if (instr == NULL)
			{
				printf("Please load an instruction set first\n");
				continue;
			}
			INTER_CTX_STATE inter_state;
			while ( (inter_state = step_inter_ctx(inter, instr)) == INTER_SUCCESS);
			switch (inter_state)
			{
				case INTER_BREAKPOINT:
					printf("Hit a breakpoint at character %ld\n", instr->index);
					instr->index++;
					break;
				case INTER_END_OF_INSTRUCTIONS:
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

	} while (bytes_read > 0);
	// CTRL+D i.e. EOF
	if (bytes_read == 0) putchar('\n');

	free(line);
	destroy_inter_ctx(inter);
	if (instr != NULL)
		destroy_instr_set(instr);

	exit(EXIT_SUCCESS);
}
