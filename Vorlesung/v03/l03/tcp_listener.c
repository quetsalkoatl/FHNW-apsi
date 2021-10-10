// Binds to a port, accepts the first connection and 
// sends one input line onwards via out_file.
// Waits then for a response via in_file and,
// when that arrives echos that to the user and closes 
// the connection.

// Client: Use, e.g. telnet 127.0.0.1 8500
//              or   telnet localhost 8500
//         on the same machine.
//
// Note:   To exit a telnet-session give <crtl>-] 
//         and then the command "quit"
// Note:   You may have to adjust user and group in do_privilege_drop()
//         and the target directory in do_chroot()
// Note:   Ansolutely no warranty, this is for educational purposes only.
// Note:   1-2 minutes of waiting is required on an error-exit.
//         There is unfortunately no simple way around that.

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <string.h>

// The following parameters may need adjustment

// the directory to chroot to, must exist and must be writable and readable
// for the user we drop privileges to
#define TARGET_DIR "/tmp/apsi04/"
// the user to drop privileges to
#define TARGET_USER "wagner"


void on_error(int err, char * msg) {  
  // more convenient error output
  // Tests whether 'err' is -1 and prints error and msg if so
  if (err == -1) {                
    perror(msg);                  
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
  int srv_sock, inst_sock;
  struct sockaddr_in  srv_adr, inst_adr;
  int inst_adr_size;
  char s[100], t[100], u[1];
  int res;
  int outf, inf;
  ssize_t in_len, out_len, wr_len, rd_len;
  struct passwd * user_s;

  // prepare communication file names
  char * out_file = "input_data";
  char * out_file_tmp = "input_data.tmp";
  char * in_file = "result_data";
  char * in_file_tmp = "result_data.tmp";
  
  // prepare server address structure
  // allow only connections from the local host on port 8500
  srv_adr.sin_family = AF_INET;
  srv_adr.sin_port = htons(8500);    
  srv_adr.sin_addr.s_addr = inet_addr("127.0.0.1");                                   
  
  // create server listening socket 
  srv_sock = socket(AF_INET, SOCK_STREAM, 0);
  on_error(srv_sock, "Socket creation"); 

  // bind server listening socket to address 
  res = bind(srv_sock, (struct sockaddr *) &srv_adr, 
	     sizeof(srv_adr));
  on_error(res, "socket binding");
  
  // set listening socket queue length 
  // lengt = 1 means only one at a time can connect
  // and additional ones get an error
  res = listen(srv_sock, 1);
  on_error(res, "queue setting");

  // Now do chroot and privilege drop.
  // Note: This must always be done before the first external input.
  user_s = prep_privilege_drop();
  do_chroot();
  do_privilege_drop(user_s);

  // main loop
  while (1) {
    // accept requests, 
    inst_adr_size = sizeof(inst_adr);
    inst_sock = accept(srv_sock, (struct sockaddr *) &inst_adr,
                       &inst_adr_size);
    on_error(inst_sock, "socket instance creation");

    // now interact with the client on the instance socket.
    // we read exactly the first transfer 
    // with a telnet client, this will be the first line
    // We will read at most 100 chars.
    in_len = read(inst_sock, &s, 100); // waits for input and reads it into s
    on_error(in_len, "error accepting connection");
    if (in_len == 0) {                 // 0 char: client closed connection
      // clean up and wait for next connection
      close(inst_sock);
      continue; 
    }

    // Input validation: Reject input not exactly 5 chars long
    // Note: This is telnet, a cr+lf (0x0d 0x0a) gets attached
    //       as UNIX line ending, hence we are looking for 5 chars
    // We are nice and give the user an error message
    if (in_len != 5) {
      char * err = "error, input needs to be 3 chars\n\r";
      write(inst_sock, err, strlen(err));
      close(inst_sock);               
      continue;
    }  
    // Input validation: Make sure all 3 chars are ascii characters
    if (!islower(s[0]) || !islower(s[1]) || !islower(s[2])) {
      char * err = "error, input needs to be all lowercase chars\r\n";
      write(inst_sock, err, strlen(err));
      close(inst_sock);               
      continue;
    }
    // Input validation: Verify the cr+lf
    if (s[3] != '\r' || s[4] != '\n') {
      char * err = "error, input needs to be terminated by cr+lf\r\n";
      write(inst_sock, err, strlen(err));
      close(inst_sock);
      continue;
    }
    
    // Input normalization: We implicitely ignore additional 
    // lines with more characters by just not reading them.
    // This needs no code, we just do not read input anymore.
  

    // write data to out_file_tmp
    outf = open(out_file_tmp, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
    on_error(outf, "error creating output file");
    wr_len = write(outf, s, 3);
    on_error(wr_len, "error creating output file");
    if (wr_len != 3) {
      fprintf(stderr, "\nshort write to disk\n");
      exit(-1);
    }  
    res = close(outf);
    on_error(res, "error creating output file");
    // rename
    res = rename(out_file_tmp, out_file);  
    on_error(res, "error renaming file");    
    
    // wait for result
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
    rd_len = read(inf, t, 100);
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
    // remove file
    res = unlink(in_file);
    on_error(res, "error unlinking input file");

    // input validation: must be 3 chars and all uppercase
    if (rd_len != 3) {
      fprintf(stderr, "\nwrong size reading file, worker compromised\n");          
      exit(-1);
    }
    if (!isupper(t[0]) || !isupper(t[1]) || !isupper(t[2])) {
      fprintf(stderr, "\ninvalid data readinf file, worker compromised\n");
      exit(-1);
    }

    // attach \r \n
    t[3] = '\r';
    t[4] = '\n';
    rd_len = 5;
    
    // echo it to socket
    write(inst_sock, &t, rd_len);
    close(inst_sock);               // clean up
  }  
  close(srv_sock);                // clean up
}
  
