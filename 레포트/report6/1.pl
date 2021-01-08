#!/usr/bin/perl
use strict;
use warnings;

my $q = "main : my q";		
our $w = "main : our w";	
our $e = "main : our e";

print"< main >\n";
print "$q\n";	
print "$w\n";	 
print "$e\n\n";	

Perl_1();		

sub Perl_1 {
	local $w = "Perl_1 : local w";	
	my $e = "Perl_1 : my e";		
	
	print"< Perl_1 >\n";	
	print "$q\n";	
	print "$w\n";	 
	print "$e\n\n";	 

	Perl_2();
}

sub Perl_2 {
	print"< Perl_2 >\n";
	print "$q\n";	
	print "$w\n";	
	print "$e\n\n";			 	
}


