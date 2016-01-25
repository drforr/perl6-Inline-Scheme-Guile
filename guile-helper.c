#include <libguile.h>
#include <stdio.h>

void* inner_with_guile(void* x)
	{
	SCM str = scm_from_latin1_string((char*) x);
	SCM foo = scm_eval_string(str);
	return foo;
	}

int guile_str_int( const char* str )
	{
	return scm_to_int
		(
		scm_with_guile( inner_with_guile, (void*)str )
		);
	}

char* guile_str_str( const char* str )
	{
	return scm_to_locale_string
		(
		scm_with_guile( inner_with_guile, (void*)str )
		);
	}
