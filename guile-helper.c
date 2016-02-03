#include <libguile.h>
#include <stdio.h>

typedef enum
	{
	UNKNOWN_TYPE = -2,
	VOID = -1,
	ZERO = 0,
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

static size_t _count_cells( SCM scm )
	{
	size_t num_cells = 1; // Start with the sentinel

	num_cells += scm_c_nvalues( scm );

	return num_cells;
	}

static void _walk_scm( SCM scm, cons_cell* result )
	{
	int num_values = scm_c_nvalues( scm );
	int i;

	for ( i = 0; i < num_values; i++ )
		{
		SCM _scm = scm_c_value_ref( scm, i );
//		result[i].type = UNKNOWN_TYPE;

		if ( scm_is_integer( _scm ) )
			_populate_integer( &result[i], _scm );
		if ( scm_is_string( _scm ) )
			_populate_string( &result[i], _scm );
		}
	result[i+1].type = ZERO;
	}

void* _run( void* expression )
	{
	SCM str = scm_from_latin1_string( (char*) expression );
	SCM scm = scm_eval_string( str );

	// Sigh, special-case void lists.
	if ( scm_c_nvalues( scm ) == 0 )
		{
		cons_cell* result = malloc( sizeof( cons_cell ) * 2 );
		result[0].type = VOID;
		result[1].type = ZERO;
		return result;
		}

	size_t num_values = _count_cells( scm );
	cons_cell* result = malloc( sizeof( cons_cell ) * num_values );

	_walk_scm( scm, result );

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
