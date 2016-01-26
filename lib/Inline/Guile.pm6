use NativeCall;

class Inline::Guile
	{
	sub native(Sub $sub)
		{
		my Str $path = %?RESOURCES<libraries/guile-helper>.Str;
		die "unable to find libguile-helper library"
			unless $path;
		trait_mod:<is>($sub, :native($path));
		}

	sub guile_str_void( Str $function ) { ... }
		native(&guile_str_void);
	sub guile_str_int( Str $function ) returns int32 { ... }
		native(&guile_str_int);
	sub guile_str_str( Str $function ) returns Str { ... }
		native(&guile_str_str);

	method run_v( Str $expression )
		{
		guile_str_void( $expression );
		}

	method run_i( Str $expression ) returns int32
		{
		return guile_str_int( $expression );
		}

	method run_s( Str $expression ) returns Str
		{
		return guile_str_str( $expression );
		}

	sub do_guile( Str $expression ) returns int32 { ... }
		native(&do_guile);

	method run_malloc( Str $expression ) returns int32
		{
		return do_guile( $expression );
		}
	}
