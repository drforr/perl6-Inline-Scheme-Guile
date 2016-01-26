#!/usr/bin/env perl6

use v6;
use Test;

use NativeCall;

plan 2;

use Inline::Guile;

my $g = Inline::Guile.new;

$g.run_v('(define (foo x) (+ x 1))');
is $g.run_i('(foo 3)'), 4, q{Can call user-defined 'foo'};
is $g.run_s('"foo"'), 'foo', q{"foo" evaluates to 'foo'};

is $g.do_guile_cb('(foo 5)'), 6;
