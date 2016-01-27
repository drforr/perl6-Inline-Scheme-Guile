#include <libguile.h>
#include <stdio.h>

void* inner_with_guile(void* expression)
	{
	SCM str = scm_from_latin1_string((char*) expression);
	SCM foo = scm_eval_string(str);
	return foo;
	}

void run_v( const char* expression )
	{
	scm_with_guile( inner_with_guile, (void*)expression );
	}

typedef enum
	{
	SENTINEL,	// This way 0 is the usual array terminator.
	TYPE_NIL,
	TYPE_BOOLEAN,
	TYPE_INTEGER,
	TYPE_STRING,
	}
	cons_cell_type;

typedef struct
	{
	cons_cell_type type;
	union
		{
		long  int_content;
		char* string_content;
		};
	}
	cons_cell;

static void _populate_integer( cons_cell* c_cell, SCM scm_cell )
	{
	c_cell->type = TYPE_INTEGER;
	c_cell->int_content = scm_to_int( scm_cell );
	}

static void _populate_string( cons_cell* c_cell, SCM scm_cell )
	{
	c_cell->type = TYPE_STRING;
	c_cell->string_content = scm_to_locale_string( scm_cell );
	}

void* inner_do_guile(void* expression)
	{
	cons_cell* result = malloc( sizeof( cons_cell ));
	SCM str = scm_from_latin1_string( (char*) expression );
	SCM scm_obj = scm_eval_string( str );
	if ( scm_is_integer( scm_obj ) )
		_populate_integer( &result[0], scm_obj );
	else if ( scm_is_string( scm_obj ) )
		_populate_string( &result[0], scm_obj );
	return result;
	}

int run( const char* expression, void (*unmarshal(void*)) )
	{
	cons_cell* cells = scm_with_guile( inner_do_guile, (void*)expression );
	long res         = cells[0].int_content;

	unmarshal(cells);

	free(cells);
	return res;
	}
