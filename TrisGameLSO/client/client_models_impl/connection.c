#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SERVER_NAME_LEN_MAX 256
#define MAX_RETRIES 3
#define RETRY_DELAY 2

int connect_to_server(const char *server_name, int server_port) {
  int socket_fd;
  struct hostent *server_host;
  struct sockaddr_in server_address;

  /* Get server host from server name. */
  server_host = gethostbyname(server_name);
  if (!server_host) {
    fprintf(stderr, "Could not resolve host\n");
    exit(1);
  }

  /* Inizializza server*/
  memset(&server_address, 0, sizeof server_address);
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(server_port);
  memcpy(&server_address.sin_addr.s_addr, server_host->h_addr,
         server_host->h_length);

  /* Crea TCP socket */
  if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    exit(1);
  }

  /* Connect to server */
  if (connect(socket_fd, (struct sockaddr *)&server_address,
              sizeof server_address) == -1) {
    perror("connect");
    close(socket_fd);
    exit(1);
  }

  return socket_fd;
}

int connect_with_retry(const char *server_name, int server_port) {
    int retries = 0;
    while(retries < MAX_RETRIES) {
        int socket_fd = connect_to_server(server_name, server_port);
        if(socket_fd != -1) {
            return socket_fd;
        }
        printf("Tentativo di connessione fallito. Nuovo tentativo in %d secondi...\n", RETRY_DELAY);
        sleep(RETRY_DELAY);
        retries++;
    }
    fprintf(stderr, "Impossibile connettersi al server dopo %d tentativi\n", MAX_RETRIES);
    exit(1);
}

void set_socket_timeout(int socket_fd, int seconds) {
    struct timeval timeout;
    timeout.tv_sec = seconds;
    timeout.tv_usec = 0;
    
    if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("setsockopt failed\n");
    }
}

void get_server_info(int argc, char *argv[], char *server_name,
                     int *server_port, int *num_players) {
  if (argc > 1) {
    strncpy(server_name, argv[1], SERVER_NAME_LEN_MAX);
  } else {
    strncpy(server_name, "localhost", SERVER_NAME_LEN_MAX);
  }

  if (argc > 2) {
    *server_port = atoi(argv[2]);
  } else {
    fprintf(stderr, "Uso: %s <nome_server> <nome_port> <num_clienti>\n",
            argv[0]);
    exit(1);
  }

  if (argc > 3) {
    *num_players = atoi(argv[3]);
  } else {
    fprintf(stderr, "Uso: %s <nome_server> <nome_port> <num_clienti>\n",
            argv[0]);
    exit(1);
  }
}