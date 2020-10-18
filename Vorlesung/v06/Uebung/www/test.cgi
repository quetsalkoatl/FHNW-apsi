#!/usr/bin/perl

print "Content-type: text/html\n";
print "Set-Cookie: test-cookie\n";
print "Set-Cookie: time=".time."\n";
print "\n";

# body
print "<html><head></head>\n<body>\n";
print "<h1>CGI and Perl is running properly on this machine\n</body>\n</html>\n";
