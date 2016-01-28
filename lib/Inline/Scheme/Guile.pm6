use NativeCall;

constant SENTINEL = 0;
constant TYPE_NIL = SENTINEL + 1;
constant TYPE_BOOLEAN = TYPE_NIL + 1;
constant TYPE_INTEGER = TYPE_BOOLEAN + 1;
constant TYPE_STRING = TYPE_INTEGER + 1;

class Inline::Scheme::Guile::AltType is repr('CUnion')
	{
	has int32 $.int_content;
	has Str   $.string_content;
	}

class Inline::Scheme::Guile::ConsCell is repr('CStruct')
	{
	has int32 $.type;
	HAS Inline::Scheme::Guile::AltType $.content;
	}

class Inline::Scheme::Guile
	{
	sub native(Sub $sub)
		{
		my Str $path = %?RESOURCES<libraries/guile-helper>.Str;
		die "unable to find libguile-helper library"
			unless $path;
		trait_mod:<is>($sub, :native($path));
		}

	sub run( Str $expression,
		 &marshal_guile (Pointer[Inline::Scheme::Guile::ConsCell]) )
		   { ... }
		native(&run);

	method run( Str $expression )
		{
		my @stuff;
		my $ref = sub ( Pointer[Inline::Scheme::Guile::ConsCell] $cell )
			{
			my $type = $cell.deref.type;
			given $type
				{
				when TYPE_INTEGER
					{
					my $content = $cell.deref.content;
					@stuff.push( $content.int_content );
					}
				when TYPE_STRING
					{
					my $content = $cell.deref.content;
					@stuff.push( $content.string_content );
					}
				}
			}
		run( $expression, $ref );
		return @stuff;
		}
	}
