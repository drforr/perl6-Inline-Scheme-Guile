#include <libguile.h>
#include <stdio.h>

static void dump_scm( SCM scm )
	{
	printf("null               : %d\n", scm_is_null              ( scm ));
	printf("bool               : %d\n", scm_is_bool              ( scm ));
	printf("false              : %d\n", scm_is_false             ( scm ));
	printf("true               : %d\n", scm_is_true              ( scm ));
	printf("integer            : %d\n", scm_is_integer           ( scm ));
	printf("string             : %d\n", scm_is_string            ( scm ));
	printf("symbol             : %d\n", scm_is_symbol            ( scm ));
	printf("array              : %d\n", scm_is_array             ( scm ));
	printf("bitvector          : %d\n", scm_is_bitvector         ( scm ));
	printf("bytevector         : %d\n", scm_is_bytevector        ( scm ));
	printf("complex            : %d\n", scm_is_complex           ( scm ));
	printf("dynamic_state      : %d\n", scm_is_dynamic_state     ( scm ));
/*
	printf("exact              : %d\n", scm_is_exact             ( scm ));
*/
	printf("fluid              : %d\n", scm_is_fluid             ( scm ));
	printf("generalized_vector : %d\n", scm_is_generalized_vector( scm ));
/*
	printf("inexact            : %d\n", scm_is_inexact           ( scm ));
*/
	printf("keyword            : %d\n", scm_is_keyword           ( scm ));
	printf("number             : %d\n", scm_is_number            ( scm ));
	printf("rational           : %d\n", scm_is_rational          ( scm ));
	printf("real               : %d\n", scm_is_real              ( scm ));
	//printf("signed_integer     : %d\n", scm_is_signed_integer    ( scm ));
	printf("simple_vector      : %d\n", scm_is_simple_vector     ( scm ));
	//printf("typed_array        : %d\n", scm_is_typed_array       ( scm ));
	printf("uniform_vector     : %d\n", scm_is_uniform_vector    ( scm ));
	//printf("unsigned_integer   : %d\n", scm_is_unsigned_integer  ( scm ));
	printf("vector             : %d\n", scm_is_vector            ( scm ));
	printf("pair               : %d\n", scm_is_pair              ( scm ));
        printf("\n");
/*
#define scm_is_bool_and_not_nil(x)  \
#define scm_is_bool_or_nil(x)  \
#define scm_is_eq(x, y) (SCM_UNPACK (x) == SCM_UNPACK (y))
#define scm_is_false_and_not_nil(x)     (scm_is_eq ((x), SCM_BOOL_F))
#define scm_is_false_assume_not_nil(x)  (scm_is_eq ((x), SCM_BOOL_F))
#define scm_is_false_or_nil(x)    \
#define scm_is_lisp_false(x)  \
#define scm_is_null_and_not_nil(x)     (scm_is_eq ((x), SCM_EOL))
#define scm_is_null_assume_not_nil(x)  (scm_is_eq ((x), SCM_EOL))
#define scm_is_null_or_nil(x)  \
#define scm_is_true_and_not_nil(x) (!scm_is_false_or_nil (x))
#define scm_is_true_assume_not_nil(x)  (!scm_is_eq ((x), SCM_BOOL_F))
#define scm_is_true_or_nil(x)          (!scm_is_eq ((x), SCM_BOOL_F))
#define scm_i_symbol_is_interned(x) \
SCM_API SCM scm_char_is_both_p
SCM_INTERNAL int scm_i_is_narrow_string (SCM str);
SCM_INTERNAL int scm_i_is_narrow_symbol (SCM str);
*/
	}

typedef enum
	{
	UNKNOWN_TYPE = -2,
	VOID = -1,
	ZERO = 0,
	TYPE_NIL = 1, // Yes, redundant, but matching the Perl...
	TYPE_BOOL = 2,
	TYPE_INTEGER = 3,
	TYPE_STRING = 4,
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

	// '' is true and only 1 value, apparently.
	//
	if (	num_values == 1 &&
		!scm_is_bool( scm ) &&
		!scm_is_integer( scm ) &&
		scm_is_true( scm ) )
		{
//printf("Void\n");
		result[0].type = VOID;
		result[1].type = ZERO;
		return;
		}

	// '#nil' is null, bool, false and only 1 value.
	//
	if (	num_values == 1 &&
		scm_is_null( scm ) &&
		scm_is_bool( scm ) &&
		scm_is_false( scm ) )
		{
//printf("Nil\n");
		result[0].type = TYPE_NIL;
		result[1].type = ZERO;
		return;
		}

	// '#f' is not null, bool, false and only 1 value.
	//
	if (	num_values == 1 &&
		//scm_is_null( scm ) &&
		scm_is_bool( scm ) &&
		scm_is_false( scm ) )
		{
//printf("False\n");
		result[0].type = TYPE_BOOL;
		result[0].int_content = 0;
		result[1].type = ZERO;
		return;
		}

	// '#t' is not null, bool, not false, true and only 1 value.
	//
	if (	num_values == 1 &&
		//scm_is_null( scm ) &&
		scm_is_bool( scm ) &&
		scm_is_true( scm ) )
		{
//printf("True\n");
		result[0].type = TYPE_BOOL;
		result[0].int_content = 1;
		result[1].type = ZERO;
		return;
		}

	// '2' is an integer
	//
	if (	num_values == 1 &&
		scm_is_integer( scm ) )
		{
//printf("Integer\n");
		result[0].type = TYPE_INTEGER;
		result[0].int_content = scm_to_int( scm );
		result[1].type = ZERO;
		return;
		}

/*
	for ( i = 0; i < num_values; i++ )
		{
		SCM _scm = scm_c_value_ref( scm, i );
		result[i].type = UNKNOWN_TYPE;

//		else if ( scm_is_integer( _scm ) )
//			_populate_integer( &result[i], _scm );
//		else if ( scm_is_string( _scm ) )
//			_populate_string( &result[i], _scm );
		}
	result[i+1].type = ZERO;
*/
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

void* __dump( void* expression )
	{
	SCM str = scm_from_latin1_string( (char*) expression );
	SCM scm = scm_eval_string( str );

	printf("SCM object from '%s' returns %d cells\n",
		expression,
		scm_c_nvalues( scm ));
	dump_scm( scm );
	printf("SCM 0th cell\n");
	dump_scm( scm_c_value_ref( scm, 0 ) );

	return expression;
	}

void _dump( const char* expression )
	{
	(void)scm_with_guile( __dump, (void*)expression );
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
