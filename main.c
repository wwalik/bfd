#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* * * * * * * * * *
 * INSTRUCTION SET *
 * * * * * * * * * */
typedef struct
{
	size_t index;
	size_t filesize;
	char text[];
} instr_set;
instr_set *
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

	instr_set *instr = malloc(sizeof(instr_set) + filesize);
	if (instr == NULL)
	{
		perror("instr_set malloc()");
		return NULL;
	}
	*instr = (instr_set)
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
void
destroy_instr_set(instr_set *instr)
{
	free(instr);
	instr = NULL;
}

/* * * * * * * * * * * *
 * INTERPRETER CONTEXT *
 * * * * * * * * * * * */
#define LOOP_LIMIT 256
typedef uint8_t TAPE_TYPE;
typedef struct
{
	size_t lstack[LOOP_LIMIT];
	size_t lstack_i;

	size_t tape_size;
	size_t tape_i;
	TAPE_TYPE tape[];
} inter_ctx;
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
int
step_inter_ctx(inter_ctx *inter, instr_set *instr)
{
	char ch = instr->text[instr->index];
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
			;
			int ch = getchar();
			if (ch == EOF) // Exit if ctrl+d
				return -1;
			inter->tape[inter->tape_i] = ch;
			
			// get rid of trailing newline
			(void)getchar();
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
				return 0;
			}

			// Push the stack
			inter->lstack[inter->lstack_i++] = instr->index;
			if (inter->lstack_i > LOOP_LIMIT)
			{
				// TODO: handle this
				printf("too many loops\n");
				exit(EXIT_FAILURE);
			}
			break;
		case ']':
			if (inter->lstack_i == 0)
			{
				// TODO: handle this
				printf("you fucked up\n");
				exit(EXIT_FAILURE);
			}

			// Pop the stack
			instr->index = inter->lstack[--inter->lstack_i];
			return 0;
		default:
			// TODO: skip to next valid instruction character
			break;
	}

	instr->index++;
	if (instr->index >= instr->filesize)
		return -1; // TODO: handle end of instructions e.g: detect loop errors

	return 0;
}
void
destroy_inter_ctx(inter_ctx *inter)
{
	free(inter);
	inter = NULL;
}

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
