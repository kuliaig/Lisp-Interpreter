#include "func.h"
#include "hash.h"
#include <stdio.h>
#include <limits.h>

static lisp_object* plus(lisp_object* args)
{
	printf("+\n");
	long long res = 0;
	lisp_object* cur = args;
	
	while (cur != NULL && cur->type == LISP_CONS)
	{
		lisp_object* value = cur->data.cons.car;
		if (value->type != LISP_NUM)
		{
			fprintf(stderr, "ERROR: + expects numbers :(\n");
			return NULL;
		}

		if (value->data.num_val > 0 && res > LLONG_MAX - value->data.num_val)
		{
			fprintf(stderr, "ERROR: sorry, interpreter can calculate numbers only less or equal than 9223372036854775807 :(\n");
			return NULL;
		}
		else if (value->data.num_val < 0 && res < LLONG_MIN - value->data.num_val)
		{
			fprintf(stderr, "ERROR: sorry, interpreter can calculate numbers only greater or equal than -9223372036854775808 :(\n");
			return NULL;
		}
		res += value->data.num_val;
		cur = cur->data.cons.cdr;
	}

	return create_num(res);
}

static lisp_object* minus(lisp_object* args)
{
	if (args == NULL || args->type == LISP_NIL)
	{
		fprintf(stderr, "ERROR: - expects at least 1 argument :(\n");
		return NULL;
	}

	lisp_object* cur = args;
	if (cur->data.cons.car->type != LISP_NUM)
	{
		fprintf(stderr, "ERROR: - expects numbers :(\n");
		return NULL;
	}

	long long res = cur->data.cons.car->data.num_val;
	cur = cur->data.cons.cdr;

	if (cur == NULL || cur->type == LISP_NIL)
	{
		if (res == LLONG_MIN)
		{
			fprintf(stderr, "ERROR: sorry, interpreter can calculate numbers only less or equal than 9223372036854775807 :(\n");
			return NULL;
		}
		return create_num(-res);
	}

	while (cur != NULL && cur->type == LISP_CONS)
	{
		lisp_object* value = cur->data.cons.car;
		if (value->type != LISP_NUM)
		{
			fprintf(stderr, "ERROR: - expects numbers :(\n");
			return NULL;
		}

		if (value->data.num_val < 0 && res > LLONG_MAX + value->data.num_val)
		{
			fprintf(stderr, "ERROR: sorry, interpreter can calculate numbers only less or equal than 9223372036854775807 :(\n");
			return NULL;
		}
		else if (value->data.num_val > 0 && res < LLONG_MIN + value->data.num_val)
		{
			fprintf(stderr, "ERROR: sorry, interpreter can calculate numbers only greater or equal than -9223372036854775808 :(\n");
			return NULL;
		}
		res -= value->data.num_val;
		cur = cur->data.cons.cdr;
	}

	return create_num(res);
}

static lisp_object* multy(lisp_object* args)
{
	long long res = 1;
	lisp_object* cur = args;

	while (cur != NULL && cur->type == LISP_CONS)
	{
		lisp_object* value = cur->data.cons.car;
		if (value->type != LISP_NUM)
		{
			fprintf(stderr, "ERROR: * expects numbers :(\n");
			return NULL;
		}
		if (res != 0 && value->data.num_val != 0)
		{
			if (res > 0 && value->data.num_val > 0 && value->data.num_val > LLONG_MAX / res ||
				res < 0 && value->data.num_val < 0 && value->data.num_val < LLONG_MAX / res)
			{
				fprintf(stderr, "ERROR: sorry, interpreter can calculate numbers only less or equal than 9223372036854775807 :(\n");
				return NULL;
			}
			else if (res > 0 && value->data.num_val < 0 && value->data.num_val < LLONG_MIN / res ||
				res < 0 && value->data.num_val > 0 && res < LLONG_MIN / value->data.num_val)
			{
				fprintf(stderr, "ERROR: sorry, interpreter can calculate numbers only greater or equal than -9223372036854775808 :(\n");
				return NULL;
			}
		}

		res *= value->data.num_val;
		cur = cur->data.cons.cdr;
	}

	return create_num(res);
}

static lisp_object* divis(lisp_object* args)
{
	if (args == NULL || args->type == LISP_NIL)
	{
		fprintf(stderr, "ERROR: / expects at least 1 argument :(\n");
		return NULL;
	}

	lisp_object* cur = args;
	if (cur->data.cons.car->type != LISP_NUM)
	{
		fprintf(stderr, "ERROR: / expects numbers :(\n");
		return NULL;
	}

	long long res = cur->data.cons.car->data.num_val;
	cur = cur->data.cons.cdr;

	if (cur == NULL || cur->type == LISP_NIL)
	{
		if (res == 0)
		{
			fprintf(stderr, "ERROR: division by zero :(\n");
			return NULL;
		}
		return create_num(1/res);
	}

	while (cur != NULL && cur->type == LISP_CONS)
	{
		lisp_object* value = cur->data.cons.car;
		if (value->type != LISP_NUM)
		{
			fprintf(stderr, "ERROR: / expects numbers :(\n");
			return NULL;
		}

		if (value->data.num_val == 0)
		{
			fprintf(stderr, "ERROR: division by zero :(\n");
			return NULL;
		}

		if (res == LLONG_MIN && value->data.num_val == -1)
		{
			fprintf(stderr, "ERROR: sorry, interpreter can calculate numbers only less or equal than 9223372036854775807 :(\n");
			return NULL;
		}

		res /= value->data.num_val;
		cur = cur->data.cons.cdr;
	}

	return create_num(res);
}

static lisp_object* equal(lisp_object* args)
{
	if (args == NULL || args->type == LISP_NIL)
	{
		fprintf(stderr, "ERROR: = expects at least 1 argument :(\n");
		return NULL;
	}

	lisp_object* cur = args;
	if (cur->data.cons.car->type != LISP_NUM)
	{
		fprintf(stderr, "ERROR: = expects numbers :(\n");
		return NULL;
	}

	int res = 1;
	long long prev = cur->data.cons.car->data.num_val;
	cur = cur->data.cons.cdr;

	if (cur == NULL || cur->type == LISP_NIL)
	{
		return create_bool(1);
	}

	while (cur != NULL && cur->type == LISP_CONS)
	{
		lisp_object* value = cur->data.cons.car;
		if (value->type != LISP_NUM)
		{
			fprintf(stderr, "ERROR: = expects numbers :(\n");
			return NULL;
		}

		if (value->data.num_val != prev)
		{
			res = 0;
		}

		cur = cur->data.cons.cdr;
	}

	return create_bool(res);
}

static lisp_object* great(lisp_object* args)
{
	if (args == NULL || args->type == LISP_NIL)
	{
		fprintf(stderr, "ERROR: > expects at least 1 argument :(\n");
		return NULL;
	}

	lisp_object* cur = args;
	if (cur->data.cons.car->type != LISP_NUM)
	{
		fprintf(stderr, "ERROR: > expects numbers :(\n");
		return NULL;
	}

	int res = 1;
	long long prev = cur->data.cons.car->data.num_val;
	cur = cur->data.cons.cdr;

	if (cur == NULL || cur->type == LISP_NIL)
	{
		return create_bool(1);
	}

	while (cur != NULL && cur->type == LISP_CONS)
	{
		lisp_object* value = cur->data.cons.car;
		if (value->type != LISP_NUM)
		{
			fprintf(stderr, "ERROR: > expects numbers :(\n");
			return NULL;
		}

		if (value->data.num_val >= prev)
		{
			res = 0;
		}

		prev = value->data.num_val;
		cur = cur->data.cons.cdr;
	}

	return create_bool(res);
}

static lisp_object* great_eq(lisp_object* args)
{
	if (args == NULL || args->type == LISP_NIL)
	{
		fprintf(stderr, "ERROR: >= expects at least 1 argument :(\n");
		return NULL;
	}

	lisp_object* cur = args;
	if (cur->data.cons.car->type != LISP_NUM)
	{
		fprintf(stderr, "ERROR: >= expects numbers :(\n");
		return NULL;
	}

	int res = 1;
	long long prev = cur->data.cons.car->data.num_val;
	cur = cur->data.cons.cdr;

	if (cur == NULL || cur->type == LISP_NIL)
	{
		return create_bool(1);
	}

	while (cur != NULL && cur->type == LISP_CONS)
	{
		lisp_object* value = cur->data.cons.car;
		if (value->type != LISP_NUM)
		{
			fprintf(stderr, "ERROR: >= expects numbers :(\n");
			return NULL;
		}

		if (value->data.num_val > prev)
		{
			res = 0;
		}

		prev = value->data.num_val;
		cur = cur->data.cons.cdr;
	}

	return create_bool(res);
}

static lisp_object* less(lisp_object* args)
{
	if (args == NULL || args->type == LISP_NIL)
	{
		fprintf(stderr, "ERROR: < expects at least 1 argument :(\n");
		return NULL;
	}

	lisp_object* cur = args;
	if (cur->data.cons.car->type != LISP_NUM)
	{
		fprintf(stderr, "ERROR: < expects numbers :(\n");
		return NULL;
	}

	int res = 1;
	long long prev = cur->data.cons.car->data.num_val;
	cur = cur->data.cons.cdr;

	if (cur == NULL || cur->type == LISP_NIL)
	{
		return create_bool(1);
	}

	while (cur != NULL && cur->type == LISP_CONS)
	{
		lisp_object* value = cur->data.cons.car;
		if (value->type != LISP_NUM)
		{
			fprintf(stderr, "ERROR: < expects numbers :(\n");
			return NULL;
		}

		if (value->data.num_val <= prev)
		{
			res = 0;
		}

		prev = value->data.num_val;
		cur = cur->data.cons.cdr;
	}

	return create_bool(res);
}

static lisp_object* less_eq(lisp_object* args)
{
	if (args == NULL || args->type == LISP_NIL)
	{
		fprintf(stderr, "ERROR: <= expects at least 1 argument :(\n");
		return NULL;
	}

	lisp_object* cur = args;
	if (cur->data.cons.car->type != LISP_NUM)
	{
		fprintf(stderr, "ERROR: <= expects numbers :(\n");
		return NULL;
	}

	int res = 1;
	long long prev = cur->data.cons.car->data.num_val;
	cur = cur->data.cons.cdr;

	if (cur == NULL || cur->type == LISP_NIL)
	{
		return create_bool(1);
	}

	while (cur != NULL && cur->type == LISP_CONS)
	{
		lisp_object* value = cur->data.cons.car;
		if (value->type != LISP_NUM)
		{
			fprintf(stderr, "ERROR: <= expects numbers :(\n");
			return NULL;
		}

		if (value->data.num_val < prev)
		{
			res = 0;
		}

		prev = value->data.num_val;
		cur = cur->data.cons.cdr;
	}

	return create_bool(res);
}

void module_math(Hash* table)
{
	put_Hash(table, "+", create_inside(plus, "+"));
	put_Hash(table, "-", create_inside(minus, "-"));
	put_Hash(table, "*", create_inside(multy, "*"));
	put_Hash(table, "/", create_inside(divis, "/"));
	put_Hash(table, "=", create_inside(equal, "="));
	put_Hash(table, ">", create_inside(great, ">"));
	put_Hash(table, ">=", create_inside(great_eq, ">="));
	put_Hash(table, "<", create_inside(less, "<"));
	put_Hash(table, "<=", create_inside(less_eq, "<="));
}

static lisp_object* car(lisp_object* args)
{
	if (args == NULL || args->type != LISP_CONS)
	{
		fprintf(stderr, "ERROR: wrong arguments in car :(\n");
		return NULL;
	}

	lisp_object* arg = args->data.cons.car;
	lisp_object* rest = args->data.cons.cdr;

	if (rest != NULL && rest->type != LISP_NIL)
	{
		fprintf(stderr, "ERROR: car expects 1 argument :(\n");
		return NULL;
	}

	if (arg->type != LISP_CONS)
	{
		fprintf(stderr, "ERROR: car expects a pair :(\n");
		return NULL;
	}

	return arg->data.cons.car;
}

static lisp_object* cdr(lisp_object* args)
{
	if (args == NULL || args->type != LISP_CONS)
	{
		fprintf(stderr, "ERROR: wrong arguments in cdr :(\n");
		return NULL;
	}

	lisp_object* arg = args->data.cons.car;
	lisp_object* rest = args->data.cons.cdr;
	if (rest != NULL && rest->type != LISP_NIL)
	{
		fprintf(stderr, "ERROR: cdr expects 1 argument :(\n");
		return NULL;
	}

	if (arg->type != LISP_CONS)
	{
		fprintf(stderr, "ERROR: cdr expects a pair :(\n");
		return NULL;
	}

	return arg->data.cons.cdr;
}

static lisp_object* cons(lisp_object* args)
{
	lisp_object* first = args->data.cons.car;
	lisp_object* second = args->data.cons.cdr->data.cons.car;
	lisp_object* third = second ? args->data.cons.cdr->data.cons.cdr->data.cons.car : NULL;

	if (first == NULL || second == NULL || third != NULL)
	{
		fprintf(stderr, "ERROR: cons expects 2 arguments :(\n");
		return NULL;
	}

	return create_cons(first, second);
}

static lisp_object* caar(lisp_object* args)
{
	if (args == NULL || args->type != LISP_CONS)
	{
		fprintf(stderr, "ERROR: wrong arguments in caar :(\n");
		return NULL;
	}

	lisp_object* arg = args->data.cons.car;
	lisp_object* rest = args->data.cons.cdr;

	if (rest != NULL && rest->type != LISP_NIL)
	{
		fprintf(stderr, "ERROR: caar expects 1 argument :(\n");
		return NULL;
	}

	if (arg->type != LISP_CONS)
	{
		fprintf(stderr, "ERROR: caar expects a pair :(\n");
		return NULL;
	}

	lisp_object* new = arg->data.cons.car;
	if (new == NULL || new->type != LISP_CONS)
	{
		fprintf(stderr, "ERROR: caar expects a pair :(\n");
		return NULL;
	}

	return new->data.cons.car;
}

static lisp_object* cadr(lisp_object* args)
{
	if (args == NULL || args->type != LISP_CONS)
	{
		fprintf(stderr, "ERROR: wrong arguments in cadr :(\n");
		return NULL;
	}

	lisp_object* arg = args->data.cons.car;
	lisp_object* rest = args->data.cons.cdr;

	if (rest != NULL && rest->type != LISP_NIL)
	{
		fprintf(stderr, "ERROR: cadr expects 1 argument :(\n");
		return NULL;
	}

	if (arg->type != LISP_CONS)
	{
		fprintf(stderr, "ERROR: cadr expects a pair :(\n");
		return NULL;
	}

	lisp_object* new = arg->data.cons.cdr;
	if (new == NULL || new->type != LISP_CONS)
	{
		fprintf(stderr, "ERROR: cadr expects a pair :(\n");
		return NULL;
	}

	return new->data.cons.car;
}

static lisp_object* cdar(lisp_object* args)
{
	if (args == NULL || args->type != LISP_CONS)
	{
		fprintf(stderr, "ERROR: wrong arguments in cdar :(\n");
		return NULL;
	}

	lisp_object* arg = args->data.cons.car;
	lisp_object* rest = args->data.cons.cdr;

	if (rest != NULL && rest->type != LISP_NIL)
	{
		fprintf(stderr, "ERROR: cdar expects 1 argument :(\n");
		return NULL;
	}

	if (arg->type != LISP_CONS)
	{
		fprintf(stderr, "ERROR: cdar expects a pair :(\n");
		return NULL;
	}

	lisp_object* new = arg->data.cons.car;
	if (new == NULL || new->type != LISP_CONS)
	{
		fprintf(stderr, "ERROR: cdar expects a pair :(\n");
		return NULL;
	}

	return new->data.cons.cdr;
}

static lisp_object* cddr(lisp_object* args)
{
	if (args == NULL || args->type != LISP_CONS)
	{
		fprintf(stderr, "ERROR: wrong arguments in cddr :(\n");
		return NULL;
	}

	lisp_object* arg = args->data.cons.car;
	lisp_object* rest = args->data.cons.cdr;

	if (rest != NULL && rest->type != LISP_NIL)
	{
		fprintf(stderr, "ERROR: cddr expects 1 argument :(\n");
		return NULL;
	}

	if (arg->type != LISP_CONS)
	{
		fprintf(stderr, "ERROR: cddr expects a pair :(\n");
		return NULL;
	}

	lisp_object* new = arg->data.cons.cdr;
	if (new == NULL || new->type != LISP_CONS)
	{
		fprintf(stderr, "ERROR: cddr expects a pair :(\n");
		return NULL;
	}

	return new->data.cons.cdr;
}

void module_cons(Hash* table)
{
	put_Hash(table, "cons", create_inside(cons, "cons"));
	put_Hash(table, "car", create_inside(car, "car"));
	put_Hash(table, "cdr", create_inside(cdr, "cdr"));
	put_Hash(table, "cadr", create_inside(cadr, "cadr"));
	put_Hash(table, "cdar", create_inside(cdar, "cdar"));
	put_Hash(table, "caar", create_inside(caar, "caar"));
	put_Hash(table, "cddr", create_inside(cddr, "cddr"));
}