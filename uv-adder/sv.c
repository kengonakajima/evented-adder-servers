#include "uv.h"
#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>


typedef struct {
    char buf[1024];
    size_t len;
    void *cli;
} parser_t;


typedef struct {
    uv_tcp_t handle; // pseudo class (handle>stream>tcp>cli)
    parser_t parser;
    int total;
} cli_t;



uv_loop_t* loop;
uv_tcp_t tcpAdderServer;
uv_handle_t* adderServer;





void adder_on_server_close(uv_handle_t* handle) {
    assert(handle == adderServer);
}

void adder_on_close(uv_handle_t* peer) {
    free(peer);
}

void adder_after_shutdown(uv_shutdown_t* req, int status) {
    uv_close((uv_handle_t*)req->handle, adder_on_close);
    free(req);
}


void adder_after_write(uv_write_t* req, int status) {
    if (status) {
        uv_err_t err = uv_last_error(loop);
        fprintf(stderr, "uv_write error: %s\n", uv_strerror(err));
        assert(0);
    }

    free(req);
}

void parser_init( parser_t *p ){
    memset(p->buf, 0, sizeof(p->buf));
    p->len=0;
}
void parser_add_data( parser_t *p, char *buf, ssize_t nr ){
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

        uv_write_t *wr = malloc(sizeof(uv_write_t));
        uv_buf_t buf = uv_buf_init( retstr,strlen(retstr));
        if( uv_write( wr, (uv_stream_t*) &cli->handle, &buf, 1, adder_after_write ) ){
            assert(!"uv_write fail");
        }
    }
}

void dump(char*p,size_t n){
    int i;    
    for(i=0;i<n;i++){
        fprintf(stderr, "%d ", p[i]);
    }    
}

void adder_after_read(uv_stream_t* handle, ssize_t nread, uv_buf_t buf) {
    cli_t *cli = handle->data;
    int i;

    uv_shutdown_t* req;
    if (nread < 0) {
        /* Error or EOF */
        assert (uv_last_error(loop).code == UV_EOF);
        if (buf.base) {
            free(buf.base);
        }
        req = (uv_shutdown_t*) malloc(sizeof *req);
        uv_shutdown(req, handle, adder_after_shutdown);

        return;
    }

    if (nread == 0) {
        /* Everything OK, but nothing read. */
        free(buf.base);
        return;
    }
    parser_add_data( & cli->parser, buf.base, nread );
    free(buf.base);
}

uv_buf_t adder_alloc(uv_handle_t* handle, size_t suggested_size) {
    //    fprintf(stderr, "suggested_size: %d\n", (int)suggested_size );
    return uv_buf_init(malloc(suggested_size), suggested_size);
}


void adder_on_connection(uv_stream_t* server, int status) {
    cli_t* cli;
    int r;
    fprintf(stderr, "adder ");

    if (status != 0) {
        fprintf(stderr, "Connect error %d\n",uv_last_error(loop).code);
    }
    assert(status == 0);

    cli = malloc(sizeof(cli_t));
    assert(cli);
    cli->total=0;
    
    r = uv_tcp_init(loop, (uv_tcp_t*) & cli->handle );
    assert(r == 0);

    /* associate server with stream */
    cli->handle.data = cli;
    cli->parser.cli = cli;

    r = uv_accept(server, (uv_stream_t*) & cli->handle );
    assert(r == 0);

    r = uv_read_start( (uv_stream_t*) & cli->handle, adder_alloc, adder_after_read);
    assert(r == 0);
}


void startAdderServer(uv_loop_t*loop)
{
    struct sockaddr_in addr = uv_ip4_addr("0.0.0.0", 8080);
    int r;

    adderServer = (uv_handle_t*)&tcpAdderServer;

    r = uv_tcp_init(loop, &tcpAdderServer);
    if (r) {
        fprintf(stderr, "Socket creation error\n");
        return;
    }

    r = uv_tcp_bind(&tcpAdderServer, addr);
    if (r) {
        fprintf(stderr, "Bind error\n");
        return;
    }

    r = uv_listen((uv_stream_t*)&tcpAdderServer, SOMAXCONN, adder_on_connection);
    if (r) {
        fprintf(stderr, "Listen error %s\n", uv_err_name(uv_last_error(loop)));
        return;
    }
}

int main(int argc, char **argv )
{
    loop = uv_default_loop();
    startAdderServer(loop);
    uv_run(loop);
    return 0;

}

