#!/usr/bin/perl

sub trim {
	my $s = shift;
	$s =~ s/^\s+|\s+$//g;
	return $s
};

$login = 0;
$user = "";

%post = ();
foreach my $line ( <STDIN> ) {
    chomp( $line );
    @pst = split /&/, $line;
	foreach my $kv ( @pst ) {
	   ($key, $val) = split(/=/, $kv);
	   $key = trim($key);
	   $val = trim($val);
	   $post{$key} = $val;
	}
}

$rcvd_cookies = $ENV{'HTTP_COOKIE'};
@cks = split /;/, $rcvd_cookies;
%cookies = ();

foreach my $cookie ( @cks ) {
   ($key, $val) = split(/=/, $cookie);
   $key = trim($key);
   $val = trim($val);
   $cookies{$key} = $val;
}

$user = $cookies{'user'};
$login = $cookies{'login'} eq "true" ? 1 : 0;

if ($post{'action'} eq "login" && $post{'pass'} eq "Secr3t") {
	$login = 1;
	$user = $post{'user'};
	print "Set-Cookie: login=true;user=$post{'user'}\r\n";
} elsif ($post{'action'} eq "logout") {
	$login = 0;
	print "Set-Cookie: login=false;user=xxx\r\n";
}

print "Content-Type: text/html\r\n\r\n";

print "<!DOCTYPE html><html><head>";
print "<title>Test</title>";
print "</head>\r\n";

print "<body>";
if ($login) {
	print "<h1>Logged in as '$user'</h1>";
	print "<form action='cgi1.pl' method='post'>";
	print "<input type='submit' name='action' value='logout'>";
	print "</form>";
} else {
	print "<h1>Please login</h1>";
	print "<form action='cgi1.pl' method='post'>";
	print "User: <input type='text' name='user'><br>";
	print "Password: <input type='text' name='pass'><br>";
	print "<input type='submit' name='action' value='login'>";
	print "</form>";
}


print "</body>";
print "</html>";

1;