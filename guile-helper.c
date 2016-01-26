#include <libguile.h>
#include <stdio.h>

void* inner_with_guile(void* expression)
	{
	SCM str = scm_from_latin1_string((char*) expression);
	SCM foo = scm_eval_string(str);
	return foo;
	}

void guile_str_void( const char* expression )
	{
	scm_with_guile( inner_with_guile, (void*)expression );
	}

int guile_str_int( const char* expression )
	{
	return scm_to_int
		(
		scm_with_guile( inner_with_guile, (void*)expression )
		);
	}

char* guile_str_str( const char* expression )
	{
	return scm_to_locale_string
		(
		scm_with_guile( inner_with_guile, (void*)expression )
		);
	}

typedef enum
	{
	SENTINEL     = -1,
	TYPE_NIL     = 0,
	TYPE_INTEGER = 1,
	TYPE_STRING  = 2
	}
	cons_cell_type;

typedef struct
	{
	cons_cell_type type;
	union
		{
		long  int_content;
		char* str_content;
		};
	}
	cons_cell;

void* inner_do_guile(void* expression)
	{
	cons_cell* result = malloc(sizeof(cons_cell));
	SCM str = scm_from_latin1_string((char*) expression);
	SCM foo = scm_eval_string(str);
	//result[0].type = TYPE_INTEGER;
	result[0].type = TYPE_STRING;
	result[0].int_content = scm_to_int( foo );
	return result;
	}

int do_guile( const char* expression )
	{
	cons_cell* cells =
		scm_with_guile( inner_do_guile, (void*)expression );
	long res = cells[0].int_content;

	free(cells);
	return res;
	}

int do_guile_cb( const char* expression, void (*unmarshal(void*)) )
	{
	cons_cell* cells =
		scm_with_guile( inner_do_guile, (void*)expression );
	long res = cells[0].int_content;

	unmarshal(cells);

	free(cells);
	return res;
	}
