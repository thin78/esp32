#include <string.h>
#include "io.h"
#include "inc.h"
#include "mg.h"


static void ev_handler(struct mg_connection *nc, int ev, void *p) {

    switch (ev) {
        case MG_EV_WEBSOCKET_HANDSHAKE_DONE:
        {
            //LOG("connected", conn);
        }
        break;

        case MG_EV_WEBSOCKET_FRAME:
        {
            //mg_wsmsg_t *wm = (mg_wsmsg_t *)ev_data;

        }
        break;

        case MG_EV_TIMER:
        {
            
        }
        break;

        case MG_EV_CLOSE:
        break;
    }
}


void ws_task(void *args)
{
    struct mg_mgr mgr;
    struct mg_connection *nc;

    printf("Starting web-server on port %s\n", WS_PORT);
    mg_mgr_init(&mgr, NULL);

    nc = mg_bind(&mgr, WS_PORT, ev_handler);
    if (nc == NULL) {
        printf("Error setting up listener!\n");
        return;
    }
    mg_set_protocol_http_websocket(nc);

    /* Processing events */
    while (1) {
        mg_mgr_poll(&mgr, 1000);
    }
}
