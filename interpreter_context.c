#include "interpreter_context.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

inter_ctx_err_t interpreter_error; // TODO: This should not be a global
const char *
get_inter_ctx_err_str(inter_ctx_err_t err)
{
	switch (err)
	{
		case INTER_ERR_NO_ERROR:
			return "No interpreter error";
		case INTER_ERR_NO_INSTRUCTION_SET:
			return "No instruction set loaded";
		case INTER_ERR_LOOP_LIMIT_REACHED:
			return "The loop depth limit was reached";
		case INTER_ERR_MISSING_BRACKET:
			return "Missing bracket";
	}

	// We shouldnt ever get here but compiler is dumb
	return "How did we get here: error str";
}

inter_ctx_t *
create_inter_ctx(size_t tape_size)
{
	inter_ctx_t *inter = malloc( sizeof(inter_ctx_t) + (sizeof(tape_cell_t) * tape_size));
	if (inter == NULL)
	{
		perror("inter malloc()");
		return NULL;
	}

	*inter = (inter_ctx_t)
	{
		.lstack_i = 0,

		.tape_size = tape_size,
		.tape_i = 0
	};
	for(int i = 0; i < tape_size; i++)
		inter->tape[i] = 0;

	return inter;
}
inter_ctx_state_t
step_inter_ctx(inter_ctx_t *inter, instr_set_t *instr)
{
	if (instr == NULL)
	{
		interpreter_error = INTER_ERR_NO_INSTRUCTION_SET;
		return INTER_STATE_ERROR;
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
			inter->tape_i %= inter->tape_size; // This works because tape_cell_t is unsigned and should always be
			break;
		case '<':
			inter->tape_i--;
			inter->tape_i %= inter->tape_size; // This works because tape_cell_t is unsigned and should always be
			break;
		case '.':
			putchar(inter->tape[inter->tape_i]);
			break;
		case ',':
			printf("(%ld)>", inter->tape_i);
			int ch = getchar();
			// TODO: handle this in a smarter way
			if (ch == '\n')
				return INTER_STATE_EOI;

			inter->tape[inter->tape_i] = ch;

			// flush stdin
			int garbage;
			while ( (garbage = getchar()) != '\n' && garbage != EOF);
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
					if (instr->index >= LOOP_LIMIT)
					{
						interpreter_error = INTER_ERR_LOOP_LIMIT_REACHED;
						return INTER_STATE_ERROR;
					}
				} while (loop_depth > 0);
				return INTER_STATE_SUCCESS;
			}

			// Check if the stack is full
			if (inter->lstack_i >= LOOP_LIMIT)
			{
				interpreter_error = INTER_ERR_LOOP_LIMIT_REACHED;
				return INTER_STATE_ERROR;
			}
			// Push the stack
			inter->lstack[inter->lstack_i++] = instr->index;
			break;
		case ']':
			if (inter->lstack_i == 0)
			{
				interpreter_error = INTER_ERR_MISSING_BRACKET;
				return INTER_STATE_ERROR;
			}

			// Pop the stack
			instr->index = inter->lstack[--inter->lstack_i];
			return INTER_STATE_SUCCESS;
		case '#':
			return INTER_STATE_BREAKPOINT;
			break;
	}

	instr->index++;
	seek_valid_bf_character(instr);

	if (instr->index >= instr->filesize) // TODO: Test this
		return INTER_STATE_EOI; // TODO: handle end of instructions e.g: detect loop errors
	return INTER_STATE_SUCCESS;
}
void show_inter_ctx_memory(inter_ctx_t *inter)
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
destroy_inter_ctx(inter_ctx_t *inter)
{
	free(inter);
	inter = NULL;
}
void
serialize_inter_ctx(inter_ctx_t *inter, FILE *fp)
{
	const size_t size = sizeof(inter_ctx_t) + sizeof(tape_cell_t) * inter->tape_size;
	const size_t bytes_read =
		fwrite(inter, size, 1, fp);
	if (bytes_read < size)
		perror("Serializing");
}
inter_ctx_t *
unserialize_inter_ctx(FILE *fp)
{
	// This is so crude and probably dangerous LMAO
	fseek(fp, 0, SEEK_END);
	const size_t filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	if (filesize <= 0)
		return NULL;

	inter_ctx_t *inter = malloc(filesize);
	const size_t bytes_read =
		fread(inter, 1, filesize, fp);
	if (bytes_read != filesize)
	{
		free(inter);
		return NULL;
	}

	return inter;
}
