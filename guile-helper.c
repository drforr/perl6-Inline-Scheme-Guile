#include <libguile.h>
#include <stdio.h>

typedef enum
	{
	ZERO,		// This way 0 is the usual array terminator.
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

static void _populate_zero( cons_cell* c_cell )
	{
	c_cell->type = ZERO;
	}

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

static void _populate_cell( cons_cell* cons, SCM scm )
	{
	if ( scm_is_integer( scm ) ) { _populate_integer( cons, scm ); return; }
	if ( scm_is_string( scm ) ) { _populate_string( cons, scm ); return; }
	}

void* _run(void* expression)
	{
	cons_cell* result = malloc( sizeof( cons_cell ) * 2);
	SCM str = scm_from_latin1_string( (char*) expression );
	SCM scm = scm_eval_string( str );
	size_t num_values = scm_c_nvalues( scm );
	int i;

	for ( i = 0; i < num_values; i++ )
		{
		SCM _scm = scm_c_value_ref( scm, i );
		_populate_cell( &result[i], _scm );
		}
	_populate_zero( &result[i+1] );
	return result;
	}

void run( const char* expression, void (*unmarshal(void*)) )
	{
	cons_cell* cells = scm_with_guile( _run, (void*)expression );
	cons_cell* head = cells;

	while( head->type != ZERO )
		{
		unmarshal(head++);
		}

	free(cells);
	}
