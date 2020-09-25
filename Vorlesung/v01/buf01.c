#include <string.h>
#include <stdio.h>

/* Compile with stack-protection off:
 *     gcc -o buf01 -fno-stack-protector -g buf01.c
 *     buf01 <argument>
 */

int main (int argc, char *argv[]) {
  char s2[4] = "yes";    // set s2 to "yes"
  char s1[4] = "abc";    // set s1 to "abc"
  strcpy(s1, argv[1]);   // copy argv[1] into s1
  puts(s2);              // print s2
}
