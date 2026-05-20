#include "reader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

static int was_tab = 0;

static char** matches(Hash* table, const char* prefix, int preflen, int* count)
{
	const char* special_forms[] = { "define", "define-no-mem", "lambda", "lambda-no-mem",
							   "if", "quote", "set!", "and", "or", "map", NULL};
	*count = 0;

	char** match = (char**)malloc((table->size + 11) * sizeof(char*));
	if (match == NULL)
	{
		return NULL;
	}

	for (int i = 0; special_forms[i] != NULL; i++)
	{
		if (strncmp(special_forms[i], prefix, preflen) == 0)
		{
			match[*count] = strdup(special_forms[i]);
			(*count)++;
		}
	}

	for (int i = 0; i < table->capacity; i++)
	{
		Node* cur = table->arr[i];
		while (cur != NULL)
		{
			if (cur->name != NULL && strncmp(cur->name, prefix, preflen) == 0)
			{
				match[*count] = strdup(cur->name);
				(*count)++;
			}
			cur = cur->next;
		}
	}

	if (table->parent != NULL)
	{
		int par_count = 0;
		char** par_match = matches(table->parent, prefix, preflen, &par_count);
		if (par_match != NULL && par_count > 0)
		{
			int new_size = *count + par_count;
			char** tmp = realloc(match, new_size * sizeof(char*));
			if (tmp == NULL)
			{
				for (int i = 0; i < *count; i++)
				{
					free(match[i]);
				}
				free(match);
				for (int i = 0; i < par_count; i++)
				{
					free(par_match[i]);
				}
				free(par_match);
				return NULL;
			}
			match = tmp;

			for (int i = 0; i < par_count; i++)
			{
				match[*count] = par_match[i];
				(*count)++;
			}
			free(par_match);
		}
	}

	return match;
}

static void autotab(Hash* table, const char* prefix, int preflen, char* buf, int* pos)
{
	static int matches_idx = 0;
	static int added_len = 0;
	static int save = 0;
	static int prev_added_len = 0;

	if (was_tab == 0)
	{
		save = preflen;
		matches_idx = 0;
	}
	else
	{
		for (int i = 0; i < prev_added_len; i++)
		{
			(*pos)--;
			printf("\b \b");
		}
	}

	int count = 0;
	char** match = matches(table, prefix, save, &count);
	if (match == NULL || count == 0)
	{
		was_tab = 1;
		added_len = 0;
		return;
	}

	const char* word = match[matches_idx];
	int nowlen = strlen(word);
	prev_added_len = nowlen - save;
	added_len = prev_added_len;

	for (int i = save; i < nowlen; i++)
	{
		putchar(word[i]);
		buf[(*pos)++] = word[i];
	}
	
	for (int i = 0; i < count; i++)
	{
		free(match[i]);
	}
	free(match);

	matches_idx++;
	if (matches_idx >= count)
	{
		matches_idx = 0;
	}

	was_tab = 1;
}

char* read_input(Hash* table)
{
	printf(":> ");
	int c = _getch();
	int brackets = 0;
	int quotes = 0;
	int pos = 0;
	int size = 64;
	int line = 0;
	char* res = malloc(size);
	if (res == NULL)
	{
		fprintf(stderr, "ERROR: not enough memory to continue :(\n");
		return NULL;
	}

	while (1)
	{
		if (pos == size)
		{
			size *= 2;
			char* tmp = realloc(res, size);
			if (tmp == NULL)
			{
				free(res);
				fprintf(stderr, "ERROR: not enough memory to continue :(\n");
				return NULL;
			}
			res = tmp;
		}

		if (c != '\t')
		{
			was_tab = 0;
		}

		if (c == '(' && quotes == 0)
		{
			brackets++;
			res[pos++] = c;
			putchar(c);
		}
		else if (c == ')' && quotes == 0)
		{
			brackets--;
			res[pos++] = c;
			putchar(c);
		}
		else if (c == '"')
		{
			quotes ^= 1;
			res[pos++] = c;
			putchar(c);
		}
		else if (c == '\r' || c == '\n')
		{
			if (quotes == 0)
			{
				if (brackets == 0)
				{
					res[pos] = '\0';
					putchar('\n');
					return res;
				}
				else if (brackets < 0)
				{
					fprintf(stderr, "\nERROR: too many closing brackets :(\n");
					return NULL;
				}
				else
				{
					res[pos++] = '\n';
				}
				printf("\n:..> ");
				line = pos;
			}
			else
			{
				res[pos++] = '\n';
				printf("\n:..> ");
			}
		}
		else if (c == '\b')
		{
			if (pos > line)
			{	
				pos--;
				if (res[pos] == '(' && quotes == 0)
				{
					brackets--;
				}

				else if (res[pos] == ')' && quotes == 0)
				{
					brackets++;
				}
				else if (res[pos] == '"')
				{
					quotes ^= 1;
				}
				printf("\b \b");
			}
		}
		else if (c == '\t')
		{
			int start = pos - 1;
			while (start >= 0 && res[start] != '(' && res[start] != ' ' && res[start] != '\n' && res[start] != ')')
			{
				start--;
			}
			start++;
			const char* prefix = &res[start];
			int preflen = pos - start;

			if (preflen > 0)
			{
				autotab(table, prefix, preflen, res, &pos);
			}
		}
		else if (c == 3)
		{
			free(res);
			printf("\nBye-bye! :)\n");
			exit(0);
		}
		else
		{
			res[pos++] = c;
			putchar(c);
		}
		
		c = _getch();
	}
}