#include "storage.h"
#include <stdio.h>
#include <string.h>

// FNV-1a for 32-bit
static int func_Hash(const char* name)
{
	unsigned int h = 2166136261u;
	while (*name)
	{
		h ^= *name;
		h *= 16777619;
		name++;
	}
	return h;
}

Hash* create_Hash(Hash* Parent)
{
	Hash* table = malloc(sizeof(Hash));
	if (table == NULL)
	{
		return NULL;
	}
	table->arr = calloc(8, sizeof(Node*));
	if (table->arr == NULL)
	{
		free(table);
		return NULL;
	}
	table->size = 0;
	table->capacity = 8;
	table->parent = Parent;
	return table;
}

int put_Hash(Hash* table, const char* name, lisp_object* obj)
{
	if (table->size == table->capacity)
	{
		int new_capacity = table->capacity * 2;
		Node** new_arr = calloc(new_capacity, sizeof(Node*));
		if (new_arr == NULL)
		{
			return 0;
		}

		for (int i = 0; i < table->capacity; i++)
		{
			Node* cur = table->arr[i];
			while (cur)
			{
				Node* next = cur->next;
				int new_h = func_Hash(cur->name) % new_capacity;
				cur->next = new_arr[new_h];
				new_arr[new_h] = cur;
				cur = next;
			}
		}

		free(table->arr);
		table->arr = new_arr;
		table->capacity = new_capacity;
	}
	int h = func_Hash(name) % table->capacity;
	Node* cur = table->arr[h];
	while (cur != NULL)
	{
		if (strcmp(cur->name, name) == 0)
		{
			del_point(cur->value);
			cur->value = obj;
			new_point(obj);
			return 1;
		}
		cur = cur->next;
	}

	Node* new = malloc(sizeof(Node));
	if (new == NULL)
	{
		return 0;
	}
	new->name = malloc(strlen(name) + 1);
	if (new->name == NULL)
	{
		free(new);
		return 0;
	}
	strcpy(new->name, name);
	new->value = obj;
	new_point(obj);
	new->next = table->arr[h];
	table->arr[h] = new;
	table->size++;
	return 1;
}

int set_Hash(Hash* table, const char* name, lisp_object* obj)
{
	int h = func_Hash(name) % table->capacity;
	Node* cur = table->arr[h];

	while (cur != NULL)
	{
		if (strcmp(cur->name, name) == 0)
		{
			del_point(cur->value);
			cur->value = obj;
			new_point(obj);
			return 1;
		}
		cur = cur->next;
	}

	if (table->parent != NULL)
	{
		return set_Hash(table->parent, name, obj);
	}

	return 0;
}

lisp_object* get_Hash(Hash* table, const char* name)
{
	int h = func_Hash(name) % table->capacity;
	Node* cur = table->arr[h];

	while (cur != NULL)
	{
		if (strcmp(cur->name, name) == 0)
		{
			return cur->value;
		}
		cur = cur->next;
	}

	if (table->parent != NULL)
	{
		return get_Hash(table->parent, name);
	}
	return NULL;
}

void del_Hash(Hash* table)
{
	for (int i = 0; i < table->capacity; i++)
	{
		Node* cur = table->arr[i];

		while (cur != NULL)
		{
			Node* next = cur->next;
			del_point(cur->value);
			free(cur->name);
			free(cur);
			cur = next; 
		}
	}
	free(table->arr);
	free(table);
}