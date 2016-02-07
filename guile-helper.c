#include <libguile.h>
#include <stdio.h>

static void dump_scm( SCM scm )
	{
	printf("null              X: %d\n", scm_is_null              ( scm ));
	printf("bool              X: %d\n", scm_is_bool              ( scm ));
	printf("false             X: %d\n", scm_is_false             ( scm ));
	printf("true              X: %d\n", scm_is_true              ( scm ));
	printf("integer           X: %d\n", scm_is_integer           ( scm ));
	printf("string            X: %d\n", scm_is_string            ( scm ));
	printf("symbol            X: %d\n", scm_is_symbol            ( scm ));
	printf("array              : %d\n", scm_is_array             ( scm ));
	printf("bitvector          : %d\n", scm_is_bitvector         ( scm ));
	printf("bytevector         : %d\n", scm_is_bytevector        ( scm ));
	printf("complex           X: %d\n", scm_is_complex           ( scm ));
	printf("dynamic_state      : %d\n", scm_is_dynamic_state     ( scm ));
/*
	printf("exact              : %d\n", scm_is_exact             ( scm ));
*/
	printf("generalized_vector : %d\n", scm_is_generalized_vector( scm ));
/*
	printf("inexact            : %d\n", scm_is_inexact           ( scm ));
*/
	printf("keyword           X: %d\n", scm_is_keyword           ( scm ));
	printf("number            X: %d\n", scm_is_number            ( scm ));
	printf("rational          X: %d\n", scm_is_rational          ( scm ));
	printf("real              X: %d\n", scm_is_real              ( scm ));
	//printf("signed_integer     : %d\n", scm_is_signed_integer    ( scm ));
	printf("simple_vector      : %d\n", scm_is_simple_vector     ( scm ));
	//printf("typed_array        : %d\n", scm_is_typed_array       ( scm ));
	printf("uniform_vector     : %d\n", scm_is_uniform_vector    ( scm ));
	//printf("unsigned_integer   : %d\n", scm_is_unsigned_integer  ( scm ));
	printf("vector             : %d\n", scm_is_vector            ( scm ));
	printf("pair               : %d\n", scm_is_pair              ( scm ));
        printf("\n");
	}

typedef enum
	{
	VECTOR_START  = -256,
	VECTOR_END    = -255,

	UNKNOWN_TYPE  = -2,
	VOID          = -1,

	TYPE_NIL      = 1, // Yes, redundant, but matching the Perl...
	TYPE_BOOL     = 2,
	TYPE_INTEGER  = 3,
	TYPE_STRING   = 4,
	TYPE_DOUBLE   = 5,
	TYPE_RATIONAL = 6,
	TYPE_COMPLEX  = 7,
	TYPE_SYMBOL   = 8,
	TYPE_KEYWORD  = 9,
	}
	cons_cell_type;

typedef struct
	{
	double real_part;
	double imag_part;
	}
	complex_value;

typedef struct
	{
	double numerator_part;
	double denominator_part;
	}
	rational_value;

typedef struct cons_cell cons_cell;
struct cons_cell
	{
	cons_cell_type type;
	union
		{
		long  int_content;
		char* string_content;
		double double_content;
		rational_value rational_content;
		complex_value complex_content;
		};
	cons_cell* next;
	cons_cell* previous;
	};

static cons_cell* _new_cons_cell()
	{
	cons_cell* cell = malloc( sizeof( cons_cell ) );
	cell->next = 0;
	cell->previous = 0;
	return cell;
	}

static cons_cell* _scm_to_cell( SCM scm )
	{
	cons_cell* head = _new_cons_cell();

	if ( scm_is_bool( scm ) )
		{
		if ( scm_is_false( scm ) )
			{
			// '#nil' is null, bool, false
			//
			if ( scm_is_null( scm ) )
				{
//printf("Nil\n");
				head->type = TYPE_NIL;
				}
			// '#f' is not null, bool, false
			//
			else
				{
//printf("False\n");
				head->type = TYPE_BOOL;
				head->int_content = 0;
				}
			}
		// '#t' is not null, bool, not false, true
		//
		else
			{
//printf("True\n");
			head->type = TYPE_BOOL;
			head->int_content = 1;
			}
		}

	// '2' is an integer
	//
	else if ( scm_is_integer( scm ) )
		{
//printf("Integer\n");
		head->type = TYPE_INTEGER;
		head->int_content = scm_to_int( scm );
		}

	// '""' is an string
	//
	else if ( scm_is_string( scm ) )
		{
//printf("String\n");
		head->type = TYPE_STRING;
		head->string_content = scm_to_locale_string( scm );
		}

	// "'a" is an symbol
	//
	else if ( scm_is_symbol( scm ) )
		{
//printf("Symbol\n");
		head->type = TYPE_SYMBOL;
		head->string_content =
			scm_to_locale_string( scm_symbol_to_string( scm ) );
		}

	// '#:a" is an keyword
	//
	else if ( scm_is_keyword( scm ) )
		{
//printf("keyword\n");
		head->type = TYPE_KEYWORD;
		head->string_content =
			scm_to_locale_string( scm_symbol_to_string( scm_keyword_to_symbol( scm ) ) );
		}

	// '-1.2' is a real
	//
	else if ( scm_is_real( scm ) )
		{
//printf("Real\n");
		head->type = TYPE_DOUBLE;
		head->double_content = scm_to_double( scm );
		}

	// '-1/2' is a rational (and complex, so test before complex)
	//
	else if ( scm_is_rational( scm ) )
		{
//printf("rational\n");
		head->type = TYPE_RATIONAL;
		head->rational_content.numerator_part =
			scm_to_double( scm_numerator( scm ) );
		head->rational_content.denominator_part =
			 scm_to_double( scm_denominator( scm ) );
		}

	// '-1i+2' is a complex
	//
	else if ( scm_is_complex( scm ) )
		{
//printf("complex\n");
		head->type = TYPE_COMPLEX;
		head->complex_content.real_part =
			scm_c_real_part( scm );
		head->complex_content.imag_part =
			 scm_c_imag_part( scm );
		}

	// '#(1 2 3)' is a vector, remember it can include other things.
	//
	else if ( scm_is_vector( scm ) )
		{
		int i;
		cons_cell* tail = head;
		head->type = VECTOR_START;

		for ( i = 0 ; i < scm_c_vector_length( scm ) ; i++ )
			{
			SCM _scm = scm_c_vector_ref( scm, i );
			cons_cell* next = _scm_to_cell( _scm );
// Walk to end of next's list
			next->previous = tail;
			tail->next = next;
			tail = next;
			}
		cons_cell* last = _new_cons_cell();
		last->type = VECTOR_END;
		tail->next = last;
		last->previous = tail;
		
//printf("vector\n");
		}

	// '' is true and only 1 value, apparently.
	//
	else if ( scm_is_true( scm ) )
		{
//printf("Void (fallback)\n");
		head->type = VOID;
		}
	return head;
	}

void* _run( void* expression )
	{
	SCM str = scm_from_utf8_string( (char*) expression );
	SCM response = scm_eval_string( str );
	cons_cell* head;

	// Sigh, special-case void lists.
	if ( scm_c_nvalues( response ) == 0 )
		{
		head = _new_cons_cell();
		head->type = VOID;
		}
	else
		{
		int i;
		SCM first = scm_c_value_ref( response, 0 );
		head = _scm_to_cell( first );
		cons_cell* tail = head;

		for ( i = 1 ; i < scm_c_nvalues( response ) ; i++ )
			{
			SCM scm = scm_c_value_ref( response, i );
			cons_cell* next = _scm_to_cell( scm );
			next->previous = tail;
			tail->next = next;
// Walk to end of next's list
while ( next->next )
	{
	next = next->next;
	}
			tail = next;
			}
		}

	return head;
	}

void* __dump( void* _expression )
	{
	char* expression = (char*) _expression;
	SCM str = scm_from_utf8_string( expression );
	SCM scm = scm_eval_string( str );

	printf("SCM object from '%s' returns %d cells\n",
		expression,
		(int)scm_c_nvalues( scm ));
	dump_scm( scm );
	printf("SCM 0th cell\n");
	dump_scm( scm_c_value_ref( scm, 0 ) );

	return expression;
	}

void _dump( const char* expression )
	{
	(void)scm_with_guile( __dump, (void*)expression );
	}

static void _display_list( cons_cell* head )
	{
	int depth = 0;
	while ( head )
		{
		switch( head->type )
			{
			case VECTOR_START:
				printf("%d VECTOR_START\n", depth);
				depth++;
				break;
			case VECTOR_END:
				depth--;
				printf("%d VECTOR_END\n", depth);
				break;
                                                                      
			case UNKNOWN_TYPE:
				printf("%d UNKNOWN_TYPE\n", depth);
				break;
			case VOID:
				printf("%d VOID\n", depth);
				break;

			case TYPE_NIL:
				printf("%d TYPE_NIL\n", depth);
				break;
			case TYPE_BOOL:
				printf("%d TYPE_BOOL (%ld)\n", depth, head->int_content);
				break;
			case TYPE_INTEGER:
				printf("%d TYPE_INTEGER (%ld)\n", depth, head->int_content);
				break;
			case TYPE_STRING:
				printf("%d TYPE_STRING (%s)\n", depth, head->string_content);
				break;
			case TYPE_DOUBLE:
				printf("%d TYPE_DOUBLE\n", depth);
				break;
			case TYPE_RATIONAL:
				printf("%d TYPE_RATIONAL\n", depth);
				break;
			case TYPE_COMPLEX:
				printf("%d TYPE_COMPLEX\n", depth);
				break;
			case TYPE_SYMBOL:
				printf("%d TYPE_SYMBOL\n", depth);
				break;
			case TYPE_KEYWORD:
				printf("%d TYPE_KEYWORD\n", depth);
				break;
			}
		head = head->next;
		}
	}

void run( const char* expression, void (*unmarshal(void*)) )
	{
	cons_cell* head = scm_with_guile( _run, (void*)expression );

_display_list(head);
	while( head )
		{
		unmarshal(head);
		head = head->next;
		}
	}
