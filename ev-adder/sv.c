#include <stdio.h>
#include <netinet/in.h>
#include <strings.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include "ev.h"


#define PORT_NO 8090
#define BUFFER_SIZE 1024


typedef struct {
    char buf[1024];
    size_t len;
    void *cli;
} parser_t;


typedef struct {
    parser_t parser;
    int total;
} cli_t;

#define MAXFD 10000

cli_t clients[MAXFD+1];

int total_clients = 0; // Total number of connected clients

void accept_cb(struct ev_loop *loop, struct ev_io *watcher, int revents);
void read_cb(struct ev_loop *loop, struct ev_io *watcher, int revents);

int main()
{
    struct ev_loop *loop = ev_default_loop(0);
    int sd;
    struct sockaddr_in addr;
    int addr_len = sizeof(addr);
    struct ev_io w_accept;

    if( (sd = socket(PF_INET, SOCK_STREAM, 0)) < 0 ){
        perror("socket error");
        return -1;
    }

    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT_NO);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sd, (struct sockaddr*) &addr, sizeof(addr)) != 0){
        perror("bind error");
        return -1;
    }

    if (listen(sd, 100) < 0){
        perror("listen error");
        return -1;
    }

    // Initialize and start a watcher to accepts client requests
    ev_io_init(&w_accept, accept_cb, sd, EV_READ);
    ev_io_start(loop, &w_accept);

    while (1){
        ev_loop(loop, 0);
    }

    return 0;
}

void accept_cb(struct ev_loop *loop, struct ev_io *watcher, int revents)
{
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int newfd;
    struct ev_io *w_client = (struct ev_io*) malloc (sizeof(struct ev_io));

    if(EV_ERROR & revents){
        perror("got invalid event");
        return;
    }

    newfd = accept(watcher->fd, (struct sockaddr *)&client_addr, &client_len);

    if (newfd < 0){
        perror("accept error");
        return;
    }
    assert( newfd <= MAXFD );
    clients[newfd].parser.len=0;
    clients[newfd].parser.cli= & clients[newfd];
    clients[newfd].total=0;

    total_clients ++; 
    fprintf(stderr,"Successfully connected with client.\n");
    fprintf(stderr,"%d client(s) connected.\n", total_clients);

    ev_io_init(w_client, read_cb, newfd, EV_READ);
    ev_io_start(loop, w_client);
}


void parser_add_data( parser_t *p, char *buf, ssize_t nr, int fd ){
    if( p->len+nr <sizeof(p->buf)-1 ){ // -1 for \0 termination
        memcpy(p->buf+p->len,buf,nr);
        p->len+=nr;
        p->buf[p->len]='\0';
    }

    // do we have newline?
    char line[1024];
    int i;
    line[0]='\0';
    for(i=0;i<p->len;i++){
        line[i]=p->buf[i];
        if(p->buf[i]=='\n'){
            line[i]='\0';
            break;
        }
    }
    if(line[0]){
        int v;
        sscanf(line,"%d",&v);
        p->len -= strlen(line)+1; // consume buffer!

        cli_t *cli = p->cli;
        cli->total += v;

        char retstr[100];
        snprintf( retstr, sizeof(retstr), "%d\n", cli->total );

        send( fd, retstr, strlen(retstr), 0 );

    }
}


void read_cb(struct ev_loop *loop, struct ev_io *watcher, int revents){
    char buffer[BUFFER_SIZE];
    ssize_t rn;

    if(EV_ERROR & revents){
        perror("got invalid event");
        return;
    }

    rn = recv(watcher->fd, buffer, BUFFER_SIZE, 0);
    if(rn < 0){
        return;
    }

    if(rn == 0){
        fprintf(stderr,"recv returns 0\n");
        ev_io_stop(loop,watcher);
        free(watcher);
        perror("peer might closing");
        total_clients --; 
        fprintf(stderr,"%d client(s) connected.\n", total_clients);
        return;
    } 

    cli_t *cli = &clients[ watcher->fd ];
    parser_add_data( &cli->parser, buffer, rn, watcher->fd );
    
    //    send(watcher->fd, buffer, rn, 0);
    
}

