#ifndef __IO_H__
#define __IO_H__

///////////////////////////////////////
typedef void (*uart_isr)(char *data, int len);

int uart_init(int n,int speed);

int uart_setisr(int n, uart_isr isr);

int uart_write(int n, char *data, int len);
//////////////////////////////////////
typedef void (*gpio_isr)(char *data, int len);

int gpio2_init(int n, int dir, int hl);

int gpio_dir(int io);

int gpio_get(int n);

int gpio_set(int n, int hl);

int gpio_setisr(int n, gpio_isr isr);
/////////////////////////////////////






#endif
