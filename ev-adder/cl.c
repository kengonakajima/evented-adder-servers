#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "ev.h"



#define PORT_NO 8090
#define BUFFER_SIZE 1024

#define NSEND 100000

struct ev_loop *loop;

#define MAXFD 1000

typedef struct {
    int cnt;
    int fd;
    struct ev_io *w_read;
} ses_t;

ses_t sessions[MAXFD+1];

int nFinished=0;
int nConnection;

void read_cb( struct ev_loop *loop, struct ev_io *watcher, int revents );


void try_send(ses_t *ses ){
    char msg[100];
    snprintf(msg, sizeof(msg),"%d\n", ses->cnt);

    int r = send( ses->fd, msg, strlen(msg),0);
    assert( r>=0);
    //    fprintf(stderr,"start read:%d\n", ses->fd);
}

void read_cb( struct ev_loop *loop, struct ev_io *watcher, int revents ){
    //    fprintf(stderr,"read_cb: fd:%d\n", watcher->fd);
    char line[100];

    int rret = recv(watcher->fd, line, sizeof(line)-1, 0);
    if(rret<=0){
        ev_io_stop(loop,watcher);
        free(watcher);
        fprintf(stderr, "socket end:%d\n", watcher->fd );
        return;
    }

    ses_t *ses = &sessions[watcher->fd];
    ses->cnt += 1;

    if( (ses->cnt%2000)==0){
        fprintf(stderr, "cnt: %d\n",ses->cnt);
    }
    if(ses->cnt<NSEND){
        try_send( ses );
    } else{
        fprintf(stderr, "finished:%d\n",ses->cnt);
        nFinished ++;
        if( nFinished == nConnection ){
            exit(0);
        }
    }

}



void connect_cb( struct ev_loop *loop, struct ev_io *watcher, int revents ){
    fprintf(stderr,"connect_cb, rev:%d\n", revents );
    ev_io_stop( loop, watcher );
    free(watcher);

    ses_t *ses = & sessions[watcher->fd];
    ses->cnt=0;
    ses->fd = watcher->fd;
    ses->w_read = (struct ev_io*)malloc( sizeof(struct ev_io));

    ev_io_init( ses->w_read, read_cb, ses->fd, EV_READ );
    ev_io_start( loop, ses->w_read );
    
    try_send(ses);
}


int main( int argc, char **argv )
{
    if( argc != 2 ){
        fprintf(stderr, "need arg(nConnection)\n");
        return 1;
    }
    
    nConnection = atoi(argv[1]);
        
    loop = ev_default_loop(0);
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT_NO);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int i;
    for(i=0;i<nConnection;i++){
        int fd= socket(PF_INET, SOCK_STREAM, 0 );
        assert(fd>0);
        int ret;
        ret = connect(fd, (struct sockaddr *)&addr, sizeof(addr));
        assert(ret==0);

        struct ev_io *w_connect = (struct ev_io*)malloc( sizeof(struct ev_io));
        ev_io_init( w_connect, connect_cb, fd, EV_WRITE );
        ev_io_start(loop, w_connect);
    }

    while(1){
        ev_loop(loop,0);
        //        recv(sd, buffer, BUFFER_SIZE, 0);
    }

    return 0;
}
