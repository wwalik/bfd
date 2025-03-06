#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "instruction_set.h"
#include "interpreter_context.h"
#include "get_line.cc"

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
			// TODO: Show where the pointer is
			for (int i = 0; i < inter->tape_size; i++)
			{
				printf("%02x ", inter->tape[i]);
				if ((i+1) % 16 == 0)
					putchar('\n');
			}
			putchar('\n');
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
			while (step_inter_ctx(inter, instr) == 0);
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
