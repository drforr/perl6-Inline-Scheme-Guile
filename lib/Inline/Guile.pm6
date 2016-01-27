use NativeCall;

constant SENTINEL = 0;
constant TYPE_NIL = SENTINEL + 1;
constant TYPE_BOOLEAN = TYPE_NIL + 1;
constant TYPE_INTEGER = TYPE_BOOLEAN + 1;
constant TYPE_STRING = TYPE_INTEGER + 1;

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

	sub run_v( Str $function ) { ... }
		native(&run_v);

	method run_v( Str $expression )
		{
		run_v( $expression );
		}

	sub run( Str $expression,
			 &marshal_guile (Pointer[Inline::Guile::ConsCell]) ) returns int32 { ... }
		native(&run);

	method run( Str $expression ) returns int32
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
		my $res = run( $expression, $ref );
		return $stuff;
		}
	}
