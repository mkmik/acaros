#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>

char buffer[4096];

#define INET_TYPE 0
#define UNIX_TYPE 1

#define GDBIDX 0
#define HOSTIDX 1
struct pollfd polli[2];

#define RED "\e[31m"
#define GREEN "\e[32m"
#define NORMAL "\e[00m"
#define COLOR(i) (i == 0 ? RED : GREEN)

struct sockaddr_in iaddr;
struct sockaddr_un uaddr;
struct sockaddr *caddr;
int caddr_len;
struct sockaddr_in saddr;

int makehostsocket(int type) {
  int hostsocket;
  bzero(&caddr, sizeof(caddr));
  
  if(type == INET_TYPE) {
    hostsocket = socket(PF_INET, SOCK_STREAM, 0);
  } else {
    hostsocket = socket(PF_UNIX, SOCK_STREAM, 0);
  }

  if(hostsocket == -1) {
    perror("socket");
    return 1;
  }
  
  if(type == 0) {
    iaddr.sin_family = AF_INET;
    iaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    iaddr.sin_port = htons(1234);
    
    caddr = (struct sockaddr*)&iaddr;
    caddr_len = sizeof(iaddr);
  } else {
    uaddr.sun_family = AF_UNIX;
    strcpy(uaddr.sun_path, getenv("HOME"));
    strcat(uaddr.sun_path, "/vmware/acaros/ttyS0");

    caddr = (struct sockaddr*)&uaddr;
    caddr_len = sizeof(uaddr.sun_family) + strlen(uaddr.sun_path);
  }

  return hostsocket;
}

int main(int argc, char **argv) {
  int hosttype = UNIX_TYPE;
  int single = 0;
  int quiet = 0;

  int i;
  for(i=1; i<argc; i++) {
    if(strcmp(argv[i], "-i") == 0)
      hosttype = INET_TYPE;
    else if(strcmp(argv[i], "-u") == 0)
      hosttype = UNIX_TYPE;
    else if(strcmp(argv[i], "-s") == 0)
      single = 1;
    else if(strcmp(argv[i], "-q") == 0)
      quiet = 1;
  }

  int server = socket(PF_INET, SOCK_STREAM, 0);
  if(server == -1) {
    perror("socket");
    return 1;
  }
  
  
  bzero(&saddr, sizeof(saddr));
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(1235);

  int yes = 1;
  setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

  if(bind(server, (struct sockaddr*) &saddr, sizeof(saddr)) < 0) {
    perror("bind");
    return 1;
  }

  int gdbsocket;

  if(listen(server, 5) < 0) {
    perror("listen");
    return 1;
  }

  do {
    gdbsocket = accept(server, 0, 0);
    
    if(!quiet)
      printf("connected\n");
    
    int hostsocket = makehostsocket(hosttype);
    
    if(connect(hostsocket, caddr, caddr_len) < 0) {
      if(!quiet)
	perror("connecting host");
      shutdown(gdbsocket, SHUT_RDWR);
      continue;
    }
    
    if(!quiet)
      printf("client connected\n");
    
    polli[GDBIDX].fd = gdbsocket;
    polli[GDBIDX].events = POLLIN | POLLERR;
    polli[HOSTIDX].fd = hostsocket;
    polli[HOSTIDX].events = POLLIN | POLLERR;
    
    
    int res;
    int i;
    while(1) {
      res = poll(polli, 2, -1);
      
      if(res == 0) {
	break;
      }
      
      if(res == -1) {
	perror("poll");
	return 1;
      }
      
      if((polli[0].revents & POLLERR) || 
	 (polli[1].revents & POLLERR)) {
	printf("socket error\n");
	break;
      }
      
      for(i=0; i<2; i++) {
	if(!(polli[i].revents & POLLIN))
	  continue;
	
	res = read(polli[i].fd, buffer, sizeof(buffer));
	if(res == 0) {
	  goto closed;
	}
	
	if(res < 0) {
	  perror("read");
	  return 1;
	}
	buffer[res] = 0;
	
	if(!quiet) {
	  printf("%s%s" NORMAL, COLOR(i), buffer);
	  char *buf = buffer;
	  while(*buf)
	  if(*buf++ == 3)
	    printf("%s<CTRLC>" NORMAL, COLOR(i));
	  
	  fflush(stdout);
	}
	
	write(polli[!i].fd, buffer, res);
      }
    }
    
  closed:
    shutdown(polli[0].fd, SHUT_RDWR);
    shutdown(polli[1].fd, SHUT_RDWR);
    
    if(!quiet)
      printf("disconnected\n");

  } while(!single);

  if(!quiet)
    printf("done\n");
  return 0;
}
