#!/usr/bin/env perl6

use v6;
use Test;

use NativeCall;

plan 2;

use Inline::Guile;

my $g = Inline::Guile.new;

$g.run_v( '(define (foo x) (+ x 1))' );

is $g.run( q{(foo 5)} ), 6, q{User-defined function returns integer};
is $g.run( q{"foo"} ), "foo", q{"foo" evaluates through callback};
