#include "uart.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/******** uart.c file of C2.5 : UART Driver Code ********/

UART uart[4];  // 4 UART structures

uint8_t uart_rate_divisors[] = {0x30, 0x20, 0x18, 0x0C};

// UART initialization function
int uart_init() {
  int i;
  UART* up;
  for (i = 0; i < 4; i++) {  // uart0 to uart2 are adjacent
    up = &uart[i];
    up->base = (char*)(0x101F1000 + i * 0x1000);
    up->n = i;
    *((unsigned int *)(up->base + 0x24)) = uart_rate_divisors[i];
    *((unsigned int *)(up->base + 0x2C)) = 0b01100000;
  }
  uart[3].base = (char*)(0x10009000);  // uart3 at 0x10009000
  *((unsigned int *)(uart[3].base + 0x2C)) = 0b01100000;
  *((unsigned int *)(uart[3].base + 0x24)) = uart_rate_divisors[3];
  
}

void test() {
  UART* up = &uart[0];

  for (int i = 0; i < 4; i++) {
    UART* up2 = &uart[i];
    char* base = up2->base;

    unsigned int ibrd = *(unsigned int *)(base + 0x24);
    unsigned int lcrh = *(unsigned int *)(base + 0x2C);
    unsigned int baud;

    baud = 7380000 / (16 * ibrd);

    uprintf(up, "UART[%d] Baudrate: %d\n", i, baud);
    uprintf(up, "UART[%d] Line Control Register: 0x%x\n", i, lcrh);
  }
}


// input a char from UART pointed by up
int ugetc(UART* up) {
  while (*(up->base + UARTFR) & RXFE);  // loop if UFR is RXFE
  return *(up->base + UARTDR);          // return a char in UDR
}

// output a char to UART pointed by up
int uputc(UART* up, char c) {
  while (*(up->base + UARTFR) & TXFF);  // loop if UFR is TXFF
  *(up->base + UARTDR) = c;             // write char to data register
}

// input a string of chars
int ugets(UART* up, char* s) {
  while ((*s = ugetc(up)) != '\r') {
    uputc(up, *s);
    s++;
  }
  *s = 0;
}

// output a string of chars
int uprints(UART* up, char* s) {
  while (*s) uputc(up, *s++);
}

#define MAX_STRING_LEN 255
int uprintu(UART* up, uint32_t val) {
  char output[MAX_STRING_LEN];
  uprints(up, itoa(val, output, 10));
}

int uprintd(UART* up, int val) {
  char output[MAX_STRING_LEN];
  uprints(up, itoa(val, output, 10));
}

int uprintx(UART* up, uint32_t val) {
  char output[MAX_STRING_LEN];
  uprints(up, itoa(val, output, 16));
}

int uprintf(UART* up, char* fmt, ...) {
  char* cp = fmt;            // cp points to the fmt string
  int* ip = (int*)&fmt + 1;  // ip points to first item in stack
  while (*cp) {              // scan the format string
    if (*cp != '%') {        // spit out ordinary chars
      uputc(up, *cp);
      if (*cp == '\n')    // for each ‘\n’
        uputc(up, '\r');  // print a ‘\r’
      cp++;
      continue;
    }
    cp++;           // cp points at a conversion symbol
    switch (*cp) {  // print item by %FORMAT symbol
      case 'c':
        uputc(up, (char)*ip);
        break;
      case 's':
        uprints(up, (char*)*ip);
        break;
      case 'u':
        uprintu(up, (uint32_t)*ip);
        break;
      case 'd':
        uprintd(up, (int)*ip);
        break;
      case 'x':
        uprintx(up, (uint32_t)*ip);
        break;
    }
    cp++;
    ip++;  // advance pointers
  }
}