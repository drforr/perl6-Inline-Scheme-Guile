#!/usr/bin/env perl6

use v6;
use Test;

use NativeCall;

plan 3;

use Inline::Scheme::Guile;

my $g = Inline::Scheme::Guile.new;

$g.run( q{(define (foo x) (+ x 1))} );

is $g.run( q{(foo 5)} ), 6,     q{User-defined function returns integer};
is $g.run( q{"foo"}   ), "foo", q{"foo" evaluates through callback};
is-deeply [ $g.run( q{(values 1 2)} ) ],
	  [ 1, 2 ],
	  q{(values 1 2) returns a list of values};
