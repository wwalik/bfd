size_t
get_line(char *line_buffer, size_t limit, FILE *stream)
{
	size_t bytes_read = 0;
	int ch;
	while ( (ch = getc(stream)) != EOF)
	{
		line_buffer[bytes_read] = ch;
		bytes_read++;
		if (bytes_read >= limit-1 || ch == '\n') // +1 for \0
			break;
	}
	line_buffer[bytes_read] = '\0';

	return bytes_read;
}
