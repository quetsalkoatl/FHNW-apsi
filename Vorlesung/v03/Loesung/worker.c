// Looks for new input file, reads it deletes it
// Performs service on it: To upper case
// Note: The only valid input is 3 lowercase letters!
// Writes result to output file


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <string.h>


// the directory to chroot to, must exist and must be writable and readable
// for the user we drop privileges to
#define TARGET_DIR "/tmp/apsi04/"
// the user to drop privileges to
#define TARGET_USER "wagner"

void on_error(int err, char * msg) {  // more convenient error output
  if (err == -1) {                    // Tests whether 'err' is -1 and
    perror(msg);                      // prints error and msg if so.
    exit(-1);
  }
}


int do_chroot() {
  // Does the chroot operation.
  // For simplicity, we hardcode the targed dir here.
  // The target dir must exist and be readable and writable for
  // the user we do the privilege drop to.
  // Return value is 0 on success, does not return on error
  int res;

  // call chroot()
  res = chroot(TARGET_DIR);
  on_error(res, "chroot() failed");

  // Call chdir() so the current work directory is not outside
  // of the chroot target anymore if it was.
  res = chdir("/");
  on_error(res, "chdir() failed");

  return 0;
}


struct passwd * prep_privilege_drop() {
  // Prepares the privilege drop
  // As we cannot look up users anymore after the chroot(), we
  // need to do this before. On the other hand, the privilege
  // drop itself needs to be done after the chroot as chroot needs to
  // be done as root. Hence we split the task.
  // Does not return on error. Gives pointer to a passwd struct on success
  gid_t gid;
  struct passwd * user_s;
  int res;

  // make sure we are root
  if (getuid() != 0) {
    errno = EPERM;
    on_error(-1, "This needs to be called as root or as SUID root");
  }

  // look up gid and uid from names
  user_s = getpwnam(TARGET_USER);
  if (user_s == NULL) {
    on_error(-1, "getpwnam() failed");
  }
  gid = user_s->pw_gid;

  // Call initgroups() to remove any additional groups root may be in.
  // This prevents unauhorized accesses via those extra groups.
  res = initgroups(TARGET_USER, gid);
  on_error(res, "initgroups() failed");

  return user_s;
}


int do_privilege_drop(struct passwd * user_s) {
  // Does the privilege drop.
  // For simplicity, target user is hardcoded and group is its default group
  // Return value is 0 on success, does not return on error
  int res;

  // drop privilege, groups first
  res = setgid(user_s->pw_gid);
  on_error(res, "setgid() failed");
  res = setuid(user_s->pw_uid);
  on_error(res, "setuif() failed");

  // be paranoid: Verify we cannot become root anymore:
  if (setuid(0) != -1) {
    errno = EPERM;   // just something that sounds righ from POSIX.1
    on_error(-1, "Privilege drop verification failed");
  }
  return 0;
}


int main () {
  char s[100], u[1];
  int i;
  int res;
  int inf, outf;
  ssize_t in_len, out_len, wr_len, rd_len;
  struct passwd * user_s;

  
  // prepare communication file names
  char * in_file = "input_data";
  char * in_file_tmp = "input_data.tmp";
  char * out_file = "result_data";
  char * out_file_tmp = "result_data.tmp";
  
  // Now do chroot and privilege drop.
  // Note: This must always be done before the first external input.
  user_s = prep_privilege_drop();
  do_chroot();
  do_privilege_drop(user_s);

  // main loop
  while (1) {
    // wait for data
    while (1) {
      inf = open(in_file, O_RDONLY);
      if (inf == -1 && errno == ENOENT) {
        // file does not exist
        sleep(1);
        continue;
      }
      on_error(inf, "error opening input file");
      break;
    }
    // read result
    rd_len = read(inf, s, 100);
    on_error(rd_len, "error reading file");

    // input validation prep: make sure the whole file was read into t
    // Note: There is no Enf-Of-File check for normal files. We
    //       have to read and check for a zero lenght result.
    if (read(inf, u, 1) != 0) {
      fprintf(stderr, "\nerror reading whole file, something is broken\n");
      exit(-1);
    }

    res = close(inf);
    on_error(res, "error closing file after read");
    res = unlink(in_file);
    on_error(res, "error unlinking input file");

    // input validation: must be 3 chars and all lowercase
    if (rd_len != 3) {
      fprintf(stderr, "\nwrong size reading file, listener compromised\n");       
      exit(-1);
    }
    if (!islower(s[0]) || !islower(s[1]) || !islower(s[2])) {
      fprintf(stderr, "\ninvalid data reading file, listener compromised\n");
      exit(-1);
    }
    
    // convert to uppercase
    for (i = 0; i < 3; i++) {
      s[i] = s[i] - 32;
    }  
    
    // send result
    outf = open(out_file_tmp, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
    on_error(outf, "error creating output file");
    wr_len = write(outf, s, 3);
    on_error(wr_len, "error creating output file");
    if (wr_len != rd_len) {
      fprintf(stderr, "short write to disk\n");
      exit(-1);
    }
    res = close(outf);
    on_error(res, "error creating output file");
    // rename
    res = rename(out_file_tmp, out_file);
    on_error(res, "error renaming file");
  }
}  
  
  

