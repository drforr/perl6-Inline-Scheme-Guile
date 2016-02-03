#!/usr/bin/env perl6

use v6;
use Test;

use NativeCall;

plan 3;

use Inline::Scheme::Guile;

my $g = Inline::Scheme::Guile.new;

subtest sub
	{
	plan 3;

	is-deeply [ $g.run( q{} ) ], [ ], q{value (none)};
#is-deeply [ $g.run( q{#nil} ) ], [ Nil ], q{value Nil};
	is-deeply [ $g.run( q{1} ) ], [ 1 ], q{value 1};
	is-deeply [ $g.run( q{"foo"} ) ], [ "foo" ], q{value "foo"};
	},
	q{Single return value};

subtest sub
	{
	plan 3;

	is-deeply [ $g.run( q{(values)} ) ],
		  [ ],
		  q{(values) returns nothing};

	is-deeply [ $g.run( q{(values 1 2)} ) ],
		  [ 1, 2 ],
		  q{(values 1 2) returns a list of values};

	is-deeply [ $g.run( q{(values 1 2 3 4 5 6 7 8 9 10)} ) ],
		  [ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 ],
		  q{(values 1 2 3 4 5 6 7 8 9 10) returns a list of values};
	},
	q{Two return values (not a two-element list)};

subtest sub
	{
	plan 1;

	$g.run( q{(define (inc-it x) (+ x 1))} );
	is-deeply [ $g.run( q{(inc-it 2)} ) ], [ 3 ], q{(inc-it 2) is 3};
	},
	q{Environment persists};
