#!/usr/bin/env perl6

use v6;
use Test;

use NativeCall;

plan 2;

use Inline::Guile;

my $g = Inline::Guile.new;

is $g.run_i('(+ 1 2)'), 3, q{(+ 1 2) evaluates to 3};
is $g.run_s('"foo"'), 'foo', q{"foo" evaluates to 'foo'};
