/************* t.c file of C2.5 **************/
int v[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};  // data array
int sum;

#include <string.h>  // contains strlen(), strcmp(), etc.

#include "uart.h"  // UART driver code file

int main() {
  int i;
  sum = 10;
  char string[64];
  UART* up;
  uart_init();  // initialize UARTs
  sum = 100;
  up = &uart[0];  // test UART0
  uprints(up, "Enter lines from serial terminal 0\n\r");
  sum = 1000;
  while (1) {
    ugets(up, string);
    uprints(up, " ");
    uprints(up, string);
    uprints(up, "\n\r");
    if (strcmp(string, "end") == 0) break;
  }

  uprints(up, "Compute sum of array:\n\r");
  sum = 0;
  for (i = 0; i < 10; i++) sum += v[i];
  uprints(up, "sum = ");
  uputc(up, (sum / 10) + '0');
  uputc(up, (sum % 10) + '0');
  uprints(up, "\n\rEND OF RUN\n\r");
}