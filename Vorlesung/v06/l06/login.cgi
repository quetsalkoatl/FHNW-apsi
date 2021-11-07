#!/usr/bin/perl

# input POST params
@post_par = <>;
chomp @post_par;
print "Content-type: text/html\n";  
print "Set-Cookie: time=".time."\n";

# logged in by cookie?
$status_c = ($ENV{HTTP_COOKIE} =~ /status_cookie=logged_in/);

# Handle login and logout
if ((! $status_c)  and  grep(/^login=login$/, @post_par) ) { 
  # login via form button
  print "Set-Cookie: status_cookie=logged_in\n";
  $status_c = 1;
} elsif ( $status_c and grep(/^logout=logout$/, @post_par)) {
  # logout via form button
  print "Set-Cookie: status_cookie=logged_out\n";
  $status_c = 0;
}  

# end of header
print "\n";

# body
print "<html><head></head>\n<body>\n";
print "Cookies: ".$ENV{HTTP_COOKIE}."<br>\n";
print "POST Params:\n";
print "<ul>\n";
for $l (@post_par) {
  print "<li>$l</li>\n";
}
print "</ul>\n";  

# "login-form"
if (! $status_c) { # not logged in
  print "NOT LOGGED IN<br>\n";
  print "<form method=\"post\">\n";
  print "<input type=\"submit\" name=\"login\" value=\"login\">\n";
  print "<input type=\"submit\" name=\"nix\" value=\"nix\"><br>\n";
  print "</form>\n";
} else {
  print "LOGGED IN<br>\n"; 
  print "<form method=\"post\">\n";
  print "<input type=\"submit\" name=\"logout\" value=\"logout\">\n";
  print "<input type=\"submit\" name=\"nix\" value=\"nix\"><br>\n";
  print "</form>\n";
} 

print "</body></html>\n";
