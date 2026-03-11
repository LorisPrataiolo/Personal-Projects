#include <stdio.h>
#include <stdlib.h>
#include <sys/errno.h>



void fail_errno(const char * const msg ) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void fail(const char * const msg) {
    fprintf(stderr, "%s\n",msg);
    exit(EXIT_FAILURE);
}

