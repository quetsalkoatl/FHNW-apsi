#!/usr/bin/perl

# input POST params
@a = <>;
chomp @a;

# vars
print "Content-type: text/html\n";
print "Set-Cookie: test-cookie\n";
print "Set-Cookie: time=".time."\n";
print "\n";

# body
print "<html><head></head>\n<body>\n";

print "Environment Variables:<br>\n";
for $k (sort( keys( %ENV ))) {
  print "$k: ".$ENV{$k}."<br>\n";
}  

print "<br>\nGET Params:\n";
print "".$ENV{'QUERY_STRING'}."<br>\n";

print "POST Params:\n";
print "<ul>\n";
for $l (@a) {
  print "<li>$l</li>\n";
}
print "</ul>\n";  

print "</body></html>\n";
