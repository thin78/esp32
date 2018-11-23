#ifndef __INC_H__
#define __INC_H__


#define WS_PORT     "8191"
#define HTTP_PORT   "80"

void ws_task(void *args);
void http_task(void *args);

#endif
