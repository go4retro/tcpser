#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "debug.h"
#include "modem_core.h"
#include "phone_book.h"
#include "ip.h"
#include "bridge.h"


const int BACK_LOG = 5;

int ip_init(int port) {
    int sSocket = 0,
        on = 0,
        rc = 0;
    struct sockaddr_in serverName = { 0 };

    LOG_ENTER();

    LOG(LOG_DEBUG,"Creating server socket");

    sSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (-1 == sSocket) {
        ELOG(LOG_FATAL,"Server socket could not be created");
        exit(1);
    }

    /*
     * turn off bind address checking, and allow 
     * port numbers to be reused - otherwise
     * the TIME_WAIT phenomenon will prevent 
     * binding to these addreG.
     */

    on = 1;

    rc = setsockopt(sSocket, 
                    SOL_SOCKET, 
		                SO_REUSEADDR,
                    (const char *) &on, 
                    sizeof(on)
                   );
    if (-1 == rc) {
        ELOG(LOG_ERROR,"bind address checking could not be turned off");
    }

    serverName.sin_addr.s_addr=htonl(INADDR_ANY);
    serverName.sin_family = AF_INET;

    /* network-order */
    serverName.sin_port = htons(port);

    LOG(LOG_DEBUG,"Binding server socket to port %d",port);
    rc = bind(sSocket,  
	            (struct sockaddr *) &serverName,
              sizeof(serverName)
             );
    if (-1 == rc) {
        ELOG(LOG_FATAL,"Server socket could not be bound to port");
        exit(1);
    }
    LOG(LOG_INFO,"Server socket bound to port");

    rc = listen(sSocket, BACK_LOG);
    LOG(LOG_INFO,"Server socket listening for connections");
    if (-1 == rc) {
      ELOG(LOG_FATAL,"Server socket could not listen on port");
      exit(1);
    }

    return sSocket;
}



int ip_connect(char addy[]) {
	struct sockaddr_in pin;
  struct in_addr cin_addr;
	struct hostent *hp;
  int sd=0;
  int port=23;
  char* address=(char*)strtok(addy,":");
  char* tmp=(char*)strtok((char*)0,":");

  if(tmp != NULL && strlen(tmp) > 0) {
    port=atoi(tmp);
  }

  LOG_ENTER();
  LOG(LOG_DEBUG,"Calling %s",addy);
	memset(&pin, 0, sizeof(pin));

	/* go find out about the desired host machine */
	if ((hp = gethostbyname(address)) == 0) {
    // well, not a DNS entry... Treat as IP...
    if(1 != inet_aton(address,&cin_addr)) {
      ELOG(LOG_ERROR,"Host %s was invalid",addy);
      return -1;
    }
	} else {
    cin_addr=*((struct in_addr *)(hp->h_addr));
  }

	pin.sin_family = AF_INET;
	pin.sin_addr.s_addr = cin_addr.s_addr;
	pin.sin_port = htons(port);

	/* grab an Internet domain socket */
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		ELOG(LOG_ERROR,"could not create client socket");
    return -1;
	}

	/* connect to PORT on HOST */
	if (connect(sd,(struct sockaddr *)  &pin, sizeof(pin)) == -1) {
		ELOG(LOG_ERROR,"could not connect to address");
    return -1;
	}
  LOG(LOG_INFO, "Connection to %s established",addy);
  LOG_EXIT();
  return sd;
}

int ip_accept(int sSocket) {
  struct sockaddr_in clientName = { 0 };
  int clientLength = sizeof(clientName);
  int cSocket=-1;

  LOG_ENTER();

  (void) memset(&clientName, 0, sizeof(clientName));

  cSocket = accept(sSocket,
                   (struct sockaddr *) &clientName, 
                   &clientLength
                  );
  if (-1 == cSocket) {
    ELOG(LOG_ERROR,"Could not accept incoming connection");
    return -1;
  }

  if (-1 == getpeername(cSocket,
                        (struct sockaddr *) &clientName, 
                        &clientLength
                       )) {
    ELOG(LOG_WARN,"Could not obtain peer name");
  } else {
    LOG(LOG_INFO,"Connection accepted from %s",
           inet_ntoa(clientName.sin_addr)
          );
  }
  LOG_EXIT();
  return cSocket;
}

int ip_write(int fd,char* data,int len) {
  log_trace(TRACE_IP_OUT,data,len);
  return write(fd,data,len);
}


///  These are defined in modem_data.h....

int line_init_config(modem_config *cfg) {
  return 0;
}
 

int line_write(modem_config *cfg,char* data,int len) {
  return ip_write(cfg->line_data.fd,data,len);
}


int line_listen(modem_config *cfg) {
  return 0;
}

int line_off_hook(modem_config *cfg) {
  return 0;
}


int line_connect(modem_config *cfg) {
  int i=0;
  char* addy=cfg->dialno;
  LOG(LOG_INFO,"Connecting");
  addy = pb_search(addy);
  cfg->line_data.fd=ip_connect(addy);
  if(cfg->line_data.fd > -1) {
    cfg->line_data.valid_conn = TRUE;
    return 0;
  } else {
    cfg->line_data.valid_conn = FALSE;
    return -1;
  }
}


int line_disconnect(modem_config *cfg) {
  LOG(LOG_INFO,"Disconnecting");
  cfg->line_data.is_telnet=FALSE;
  cfg->line_data.first_char=TRUE;
  if(cfg->line_data.valid_conn == TRUE) {
    close(cfg->line_data.fd);
    cfg->line_data.valid_conn=FALSE;
  }
  return 0;
}

