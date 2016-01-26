use NativeCall;

constant SENTINEL = 0;
constant TYPE_NIL = 1;
constant TYPE_BOOLEAN = 2;
constant TYPE_INTEGER = 3;
constant TYPE_STRING = 4;

class Inline::Guile::AltType is repr('CUnion')
	{
	has int32 $.int_content;
	has Str   $.string_content;
	}

class Inline::Guile::ConsCell is repr('CStruct')
	{
	has int32 $.type;
	HAS Inline::Guile::AltType $.content;
	}

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

	method run_v( Str $expression )
		{
		guile_str_void( $expression );
		}

	sub do_guile( Str $expression ) returns int32 { ... }
		native(&do_guile);

	sub do_guile_cb( Str $expression,
			 &marshal_guile (Pointer[Inline::Guile::ConsCell]) ) returns int32 { ... }
		native(&do_guile_cb);

	method do_guile_cb( Str $expression ) returns int32
		{
		my $stuff;
		my $ref = sub ( Pointer[Inline::Guile::ConsCell] $cell )
			{
			my $type = $cell.deref.type;
			given $type
				{
				when TYPE_INTEGER
					{
					my $content = $cell.deref.content;
					$stuff = $content.int_content;
					}
				when TYPE_STRING
					{
					my $content = $cell.deref.content;
					$stuff = $content.string_content;
					}
				}
			}
		my $res = do_guile_cb( $expression, $ref );
		return $stuff;
		}
	}
