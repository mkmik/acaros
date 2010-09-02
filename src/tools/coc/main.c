#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include "filter.h"
#include "dir.h"

const char* ext = ".co";
char* source=0;
char* dest=0;
int dolink = 1;
int onlyFilter = 0;
char buffer[4096];

int mysignals[] = {SIGINT, SIGSEGV, SIGILL, 0};

void transform_args(int argc, char** argv) {
  int i;

  argv[0] = getenv("CC") ? : "gcc";
  for(i=1; i<argc; i++) {
    if(strcmp(argv[i], "-c") == 0)
      dolink = 0;
    if(strcmp(argv[i], "-CO") == 0)
      onlyFilter = 1;
    
    else if(strlen(argv[i]) > strlen(ext)) {
      if(strncmp(argv[i]+strlen(argv[i])-strlen(ext), ext, strlen(ext)) == 0) {
	if(source) {
	  printf("cannot compile more than one file");
	  exit(1);
	}
	source = strdup(argv[i]);
	argv[i][strlen(argv[i])-1] = 0; // truncate "o"
	dest = argv[i];
      }
    }
  }
    
}

const char* parseIncludePaths(int argc, char** argv) {
  int i;

  buffer[0] = 0;
  for(i=1; i<argc; i++)
    if(strncmp(argv[i], "-I", 2) == 0) {
      strcat(buffer, argv[i]);
      strcat(buffer, " ");
    }
  strcat(buffer, " -I" CO_INCLUDE_DIR); // built-in
  return buffer;
}

void cleanup() {
  if(source) {
    sprintf(buffer, "rm %s -f", dest);
    system(buffer);
  }
}

void signal_cleanup(int sig) {
  printf("coc got signal %d, cleaning up\n", sig);
  cleanup();
  int* s = mysignals;
  while(*s)
    signal(*s++, SIG_DFL);

  kill(getpid(), sig);
}

void setup_cleanup() {
  atexit(cleanup);

  int* s = mysignals;
  while(*s)
    signal(*s++, signal_cleanup);
}

int main(int argc, char** argv) {
  int i;

  transform_args(argc, argv);

  if(source) {
    setup_cleanup();
    coc_filter(source, dest, parseIncludePaths(argc, argv));
  }

  if(onlyFilter && source) {
    sprintf(buffer, "cat %s", dest);
  } else {
    buffer[0] = 0;
    for(i=0; i<argc; i++) {
      strcat(buffer, argv[i]);
      strcat(buffer, " ");
    }
    
    if(dolink)
      strcat(buffer, " " CO_LIB);
    else
      strcat(buffer, " -I" CO_INCLUDE_DIR);

    if(source)
      strcat(buffer, " -D__CO");
  }

  int res = system(buffer);

  return WEXITSTATUS(res);
}
