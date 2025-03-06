#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

typedef struct
{
	size_t index;
	char text[];
} instr_set;
instr_set *
create_instr_set(const char *filename)
{
	FILE *fp = fopen(filename, "r");

	fseek(fp, 0, SEEK_END);
	size_t filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	instr_set *instr = malloc(sizeof(instr_set) + filesize);

	fclose(fp);
}
#define LOOP_LIMIT 256
typedef uint8_t TAPE_TYPE;
typedef struct
{
	size_t lstack[LOOP_LIMIT];
	size_t stack_i;

	size_t tape_size;
	size_t tape_i;
	TAPE_TYPE tape[];
} inter_ctx;

inter_ctx *
create_inter_ctx(size_t tape_size)
{
	inter_ctx *inter = malloc( sizeof(inter_ctx) + (sizeof(TAPE_TYPE) * tape_size));

	*inter = (inter_ctx)
	{
		.stack_i = 0,

		.tape_size = tape_size,
		.tape_i = 0
	};
	for(int i = 0; i < tape_size; i++)
		inter->tape[i] = 0;

	return inter;
}
void
destroy_inter_ctx(inter_ctx *inter)
{
	free(inter);
	inter = NULL;
}



size_t
get_line(char *line_buffer, size_t limit, FILE *stream)
{
	size_t bytes_read = 0;
	int ch;
	while ( (ch = getc(stream)) != '\n' && ch != EOF)
	{
		line_buffer[bytes_read] = ch;
		bytes_read++;
		if (bytes_read >= limit-1 || ch == '\n') // +1 for \0
			break;
	}
	line_buffer[bytes_read] = '\0';

	return bytes_read;
}

int
main(int argc, char *argv[])
{
	inter_ctx *inter = create_inter_ctx(256);

	char *line = malloc(256);
	size_t bytes_read;
	do
	{
		printf(">>");
		bytes_read =
			get_line(line, 256, stdin);
	} while (bytes_read > 0);

	free(line);
	destroy_inter_ctx(inter);

	exit(EXIT_SUCCESS);
}
