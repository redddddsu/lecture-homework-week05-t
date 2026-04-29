#ifndef __UART_H__
#define __UART_H__

#define UARTDR 0x00
#define UARTFR 0x18
#define UARTIBRD 0x24
#define UARTLCR 0x2c

#define TXFE 0x80
#define RXFF 0x40
#define TXFF 0x20
#define RXFE 0x10
#define BUSY 0x08

typedef volatile struct uart{
    char *base; // base address; as char *
    int n; // uart number 0-3
} UART;

extern UART uart[]; // 4 UART structures

extern int uart_init();
extern int ugetc(UART *up);
extern int uputc(UART *up, char c);
extern int ugets(UART *up, char *s);
extern int uprints(UART *up, char *s);
extern int uprintf(UART *up, char *s, ...);

#endif