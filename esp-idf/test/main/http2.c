#include <string.h>
#include "inc.h"
#include "mg.h"


static struct mg_serve_http_opts s_opts;
static void mg_ev_handler(struct mg_connection *nc, int ev, void *p)
{
    struct http_message *msg = p;
    switch (ev) {

        case MG_EV_HTTP_REQUEST: {
        printf("__http req uri: %s", msg->uri.p);
        mg_serve_http(nc, (struct http_message *)p, s_opts);
        //nc->flags |= MG_F_SEND_AND_CLOSE;
        break;
        }
    }
}


void http_task(void *args)
{
    struct mg_mgr mgr;
    struct mg_connection *nc;

    printf("Starting web-server on port %s\n", HTTP_PORT);
    mg_mgr_init(&mgr, NULL);

    nc = mg_bind(&mgr, HTTP_PORT, mg_ev_handler);
    if (nc == NULL) {
        printf("Error setting up listener!\n");
        return;
    }
    mg_set_protocol_http_websocket(nc);
    s_opts.document_root = "/web";  // Serve current directory
    s_opts.enable_directory_listing = "yes";

    /* Processing events */
    while (1) {
        mg_mgr_poll(&mgr, 1000);
    }
}
