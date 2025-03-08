#include "interpreter_context.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

inter_ctx *
create_inter_ctx(size_t tape_size)
{
	inter_ctx *inter = malloc( sizeof(inter_ctx) + (sizeof(TAPE_TYPE) * tape_size));
	if (inter == NULL)
	{
		perror("inter malloc()");
		return NULL;
	}

	*inter = (inter_ctx)
	{
		.lstack_i = 0,

		.tape_size = tape_size,
		.tape_i = 0
	};
	for(int i = 0; i < tape_size; i++)
		inter->tape[i] = 0;

	return inter;
}
INTER_CTX_STATE
step_inter_ctx(inter_ctx *inter, instr_set *instr)
{
	if (instr == NULL)
	{
		printf("No instruction set is loaded\n");
		return INTER_ERROR;
	}

	int ch = instr->text[instr->index];
	switch (ch)
	{
		case '+':
			inter->tape[inter->tape_i]++;
			break;
		case '-':
			inter->tape[inter->tape_i]--;
			break;
		case '>':
			inter->tape_i++;
			break;
		case '<':
			inter->tape_i--;
			break;
		case '.':
			putchar(inter->tape[inter->tape_i]);
			break;
		case ',':
			printf("(%ld)>", inter->tape_i);
			int ch = getchar();
			if (ch == EOF) // Exit if ctrl+d
				exit(EXIT_SUCCESS); // TODO: fix this
			inter->tape[inter->tape_i] = ch;

			int garbage;
			while ( (garbage = getchar()) != '\n' && garbage != EOF); // flush stdin
			break;
		case '[':
			if (inter->tape[inter->tape_i] == 0)
			{
				int loop_depth = 0;
				do
				{
					char ch = instr->text[instr->index];

					if (ch == '[')
						loop_depth++;
					else if (ch == ']')
						loop_depth--;

					instr->index++;
				} while (loop_depth > 0);
				return INTER_SUCCESS;
			}

			// Push the stack
			inter->lstack[inter->lstack_i++] = instr->index;
			if (inter->lstack_i > LOOP_LIMIT)
			{
				// TODO: handle this
				printf("too many loops\n");
				exit(EXIT_FAILURE);
				return INTER_ERROR;
			}
			break;
		case ']':
			if (inter->lstack_i == 0)
			{
				// TODO: handle this
				printf("you fucked up\n");
				printf("Fix your brackets\n");
				exit(EXIT_FAILURE);
				return INTER_ERROR;
			}

			// Pop the stack
			instr->index = inter->lstack[--inter->lstack_i];
			return INTER_SUCCESS;
		case '#':
			return INTER_BREAKPOINT;
			break;
	}

	instr->index++;
	seek_valid_bf_character(instr);

	if (instr->index >= instr->filesize) // TODO: Test this
		return INTER_END_OF_INSTRUCTIONS; // TODO: handle end of instructions e.g: detect loop errors
	return INTER_SUCCESS;
}
void show_inter_ctx_memory(inter_ctx *inter)
{
	for (int i = 0; i < inter->tape_size; i++)
	{
		if (i == inter->tape_i)
			printf("\033[4m"); // Underline text
		printf("%02x", inter->tape[i]);
		printf("\033[0m "); // reset to normal text mode

		if ((i+1) % 16 == 0)
			putchar('\n');
	}
}
void
destroy_inter_ctx(inter_ctx *inter)
{
	free(inter);
	inter = NULL;
}
void
serialize_inter_ctx(inter_ctx *inter, FILE *fp)
{
	const size_t size = sizeof(inter_ctx) + sizeof(TAPE_TYPE) * inter->tape_size;
	const size_t bytes_read =
		fwrite(inter, size, 1, fp);
	if (bytes_read < size)
		perror("Serializing");
}
inter_ctx *
unserialize_inter_ctx(FILE *fp)
{
	// This is so crude and probably dangerous LMAO
	fseek(fp, 0, SEEK_END);
	const size_t filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	if (filesize <= 0)
		return NULL;

	inter_ctx *inter = malloc(filesize);
	const size_t bytes_read =
		fread(inter, 1, filesize, fp);
	if (bytes_read != filesize)
	{
		free(inter);
		return NULL;
	}

	return inter;
}
