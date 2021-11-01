#!/usr/bin/perl

print "Content-Type: text/html\r\n";
print "Set-Cookie: test=foobar\r\n\r\n";

print "<!DOCTYPE html><html><head>";
print "<title>Test</title>";
print "</head>\r\n";

print "<body>";

#$rcvd_cookies = $ENV{'HTTP_COOKIE'};
#@cookies = split /;/, $rcvd_cookies;

#foreach $cookie ( @cookies ) {
#   ($key, $val) = split(/=/, $cookie);
#   print "<p>'$key' = '$val'</p>"
#}	

print "</body>";
print "</html>";

1;