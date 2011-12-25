#include "uv.h"
#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>


#define NSEND 100000


int nFinished=0;
int nConnection;

uv_loop_t *loop;

typedef struct {
    uv_tcp_t handle;
    int cnt;    
} ses_t;

uv_buf_t on_allocate(uv_handle_t* handle, size_t size){
    char *buf = malloc(size);
    return uv_buf_init( buf, size );
}
void after_close(uv_handle_t* handle) {
    fprintf(stderr, "closed\n");
}

void after_read( uv_stream_t* tcp, ssize_t nread, uv_buf_t buf) ;

void after_write( uv_write_t* req, int status) {
    int ret;
    uv_stream_t* tcp = req->handle;

    ret = uv_read_start((uv_stream_t*)tcp, on_allocate, after_read );
    if(ret){
        fprintf(stderr, "uv_tcp_read_start error: %s\n", uv_strerror(uv_last_error(loop)));                
    }else{
        //        fprintf(stderr, "start reading\n");
    }
    free(req);
}


void try_send(uv_stream_t *tcp, ses_t *ses ) {
    char msg[100];
    snprintf(msg, sizeof(msg),"%d\n", ses->cnt);
    uv_buf_t buf = uv_buf_init( msg, strlen(msg));
    uv_write_t *wr = malloc(sizeof(uv_write_t));
    int ret = uv_write(wr, tcp, &buf, 1, after_write );
    if(ret){
        fprintf(stderr, "uv_write error: %s\n", uv_strerror(uv_last_error(loop)));                
    } else {
        //        fprintf(stderr, "writing\n");
    }
}

void after_read( uv_stream_t* tcp, ssize_t nread, uv_buf_t buf) {
    if (nread < 0) {
        if (buf.base) {free(buf.base);}
        uv_close((uv_handle_t*) tcp, after_close );
        return;
    }
    char line[100];
    if( nread <sizeof(line)-1){
        memcpy( line, buf.base, nread);
        line[nread]='\0';
        //        fprintf(stderr, "line: '%s'", line);
        ses_t *ses = tcp->data;
        ses->cnt += 1;
        if( (ses->cnt%2000)==0){
            fprintf(stderr, "cnt: %d\n",ses->cnt);
        }
        if(ses->cnt<NSEND){
            try_send( tcp, ses );
        } else{
            fprintf(stderr, "finished:%d\n",ses->cnt);
            nFinished ++;
            if( nFinished == nConnection){
                exit(0);
            }
        }
    }

    free(buf.base);
}





void after_connect( uv_connect_t *req,  int status ) {
    fprintf(stderr,"after_connect:%d\n",status);
    int ret;
    
    uv_stream_t* tcp = req->handle;
    ses_t *ses = tcp->data;

    try_send(tcp,ses);
    free(req);
}




ses_t *startSession() {
    int ret;
    
    ses_t *ses = malloc( sizeof(ses_t) );
    assert(ses);
    ses->cnt=0;
    ses->handle.data = ses;
    
    struct sockaddr_in server_addr;

    server_addr = uv_ip4_addr("127.0.0.1", 8090);
    ret = uv_tcp_init(loop, & ses->handle);
    if(ret){
        fprintf(stderr, "uv_tcp_init error: %s\n", uv_strerror(uv_last_error(loop)));        
    }
    uv_connect_t *cr = malloc( sizeof(uv_connect_t));
    ret = uv_tcp_connect(cr, & ses->handle, server_addr, after_connect );
    if(ret){
        fprintf(stderr, "uv_tcp_connect error: %s\n", uv_strerror(uv_last_error(loop)));        
    } else {
        fprintf(stderr,"connecting\n");
    }
}


int main( int argc, char **argv )
{
    if( argc != 2 ){
        fprintf(stderr,"need arg(nConnection)\n");
        return 1;
    }
    nConnection = atoi(argv[1]);
    
    loop = uv_default_loop();
    int i;
    for(i=0;i<nConnection;i++){
        startSession();
    }

    uv_run(loop);
    return 0;
}


