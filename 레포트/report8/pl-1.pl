#!/usr/bin/perl
use strict;
use warnings;


printf "---------�̸�---------\n";

my @set = ('A' ..'Z');
my $str = join '' => map $set[rand @set], 1 .. 3;
print "$str\n";

$a =(@set = ('A' ..'Z'));
$a =($str = join '' => map $set[rand @set], 1 .. 3);
print "$a\n";

$a =(@set = ('A' ..'Z'));
$a =($str = join '' => map $set[rand @set], 1 .. 3);
print "$a\n";


printf "---------����---------\n";

$a =(@set = ('0' ..'9'));
$a =($str = join '' => map $set[rand @set], 1 .. 2);
print "$a\n";

$a =(@set = ('0' ..'9'));
$a =($str = join '' => map $set[rand @set], 1 .. 2);
print "$a\n";


$a =(@set = ('0' ..'9'));
$a =($str = join '' => map $set[rand @set], 1 .. 2);
print "$a\n";
