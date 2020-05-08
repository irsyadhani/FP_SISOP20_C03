/* Cara Kerja Program: find namaFile */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFSIZE 1000

int main(void) {
  int pfd[2], n;
  char str[BUFSIZE + 1];
  
/* Bila Pipe gagal */
  if (pipe(pfd) < 0) {
    printf("PIPE ERROR!\n");
    exit(-1);
  }

  n = fork();
/* Bila Fork gagal */
  if (n < 0) {
    printf("FORK ERROR!\n");
    exit(-2);
  } else if (n == 0) {
    close(pfd[0]);

    dup2(pfd[1], 1);
    close(pfd[1]);
    
/* Fungsi exec untuk find */
    execlp("find", "find", "filename", (char *) 0);
    printf("Oups, execlp failed.  Exiting\n"); /* Bagian ini akan dibaca oleh Parent */
    exit(-1); /* Mencegah execlp gagal, terutama pada loop nanti */
  } else {
    close(pfd[1]);

    while ((n = read(pfd[0], str, BUFSIZE)) > 0) {
      str[n] = '\0';
      printf("%s", str);
    }

    close(pfd[0]);
    wait(&n); /* Mencegah zombie process. */

    if (n != 0) {
       printf("FIND ERROR!\n");
    }
  }
}
