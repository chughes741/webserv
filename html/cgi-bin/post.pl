#!/usr/bin/perl

use strict;
use warnings;
use CGI;

# Create a new CGI object
my $cgi = CGI->new();

# Get the value of 'text_input' parameter
my $text_input = $cgi->param('text_input');

# Start the HTTP header and HTML output
print "Content-Type: text/html\n\n";
print $cgi->start_html('Hello from Perl CGI!');
print $cgi->h1('Hello from Perl CGI!');

# Check if 'text_input' parameter exists and print its value
if (defined $text_input) {
    print $cgi->p("User entered this text: " . $cgi->escapeHTML($text_input));
} else {
    print $cgi->p("User did not take the opportunity to express itself. Very sad");
}

# End the HTML
print $cgi->end_html();
