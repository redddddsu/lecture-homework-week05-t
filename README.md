
# Lecture Homework Week 05 - Thursday

For this lecture homework, you will explore and modify the UART device driver that uses memory mapping.

## Getting the Code

As with the previous lecture homework, this assignment is hosted on GitHub. You will create your own repository using the assignment repository as a template. To do this:

1. Click on **"Use this template."**
2. Select **"Create a new repository."**
3. Give your repository a descriptive name.
4. Click **"Create repository."**

Once created, clone the repository and open it in a GitHub Codespace to begin working.

## Calling main

As you have been taught, by default, every C and C++ program starts by calling a function called `main`. However, this name can easily be changed. In the assembly code provided with this assignment, we see the following:

```asm
        .global start, stack_top // stack_top defined in t.ld
start:
        ldr sp, =stack_top // set SVC mode stack pointer
        bl main // call main() in C
        b . // if main() returns, just loop
```

The line `bl main` calls the main function. For this part of the assignment, change the name of the `main` function to any name you want—for example, `cs122a_main`. Update both the `start.s` and `main.c` code to make this new name work. You will turn this code in as part of your submission.

## Working with Device Drivers

The UART device driver for the PL011 device supports four instances. If you look at the `uart_init` subroutine in `uart.c`, you will notice that it sets the base address for each memory-mapped UART instance, but it does not actually configure them. While this works in QEMU, it would fail on real hardware. 

Therefore, your assignment is to properly configure each UART device in the `init` function. Each should support an **8N1** configuration. We will set the baud rates for UART0 through UART3 to **9600**, **14400**, **19200**, and **38400** respectively.

### Setting the Configuration

In order to specify that each instance be **8N1**, we only need to change the value of one register: the `UARTLCR_H` register. This register controls the Line Control configuration. It is an 8-bit value where the bits represent the following:

**UARTLCR_H Register Bit Fields**

| Bits | Name | Description |
|:---|:---|:---|
| 7 | SPS | **Stick Parity Select.** When bits 1, 2, and 7 are set, parity is transmitted/checked as a '0'. When 1 and 7 are set but bit 2 is 0, parity is a '1'. |
| 6:5 | WLEN | **Word Length.** Number of data bits per frame: <ul><li>`11`: 8 bits</li><li>`10`: 7 bits</li><li>`01`: 6 bits</li><li>`00`: 5 bits</li></ul> |
| 4 | FEN | **Enable FIFOs.** <ul><li>`1`: Transmit and receive FIFOs enabled.</li><li>`0`: FIFOs disabled (1-byte holding register).</li></ul> |
| 3 | STP2 | **Two Stop Bits Select.** <ul><li>`1`: Two stop bits transmitted.</li><li>`0`: One stop bit transmitted.</li></ul> |
| 2 | EPS | **Even Parity Select.** <ul><li>`1`: Even parity.</li><li>`0`: Odd parity. (Only matters if PEN is 1).</li></ul> |
| 1 | PEN | **Parity Enable.** <ul><li>`1`: Parity enabled.</li><li>`0`: Parity disabled.</li></ul> |
| 0 | BRK | **Send Break.** <ul><li>`1`: Holds the TXD output low to signal a break condition.</li></ul> |

To set the configuration, you need to set the **WLEN** field to 8 bits. Use the table above and the offsets provided in `uart.h` to set the configuration for each device in `uart_init`.

### Setting the Baud Rate

You must also set the baud rate for each UART device. This is done by setting the `UARTIBRD` register to the proper value as specified in the following table:

| Integer Divisor | Baud Rate |
|:---|:---|
| `0x30` | 9600 |
| `0x20` | 14400 |
| `0x18` | 19200 |
| `0x0C` | 38400 |
| `0x04` | 115200 |

*Note: The course textbook has a few mistakes regarding baud rates; the table above provides the corrected values.*

In `uart.c`, ensure the four UART devices are configured as follows:

| UART Device | Target Baud Rate |
|:---|:---|
| UART0 | 9600 |
| UART1 | 14400 |
| UART2 | 19200 |
| UART3 | 38400 |

### Outputting this Configuration

Once you have updated `uart_init`, you will output the configuration to the QEMU console using the provided `uprintf` function. Loop through each UART device in the global array `uart` (defined in `uart.h` and `uart.c`). 

1. Read the `UARTIBRD` and `UARTLCR_H` registers into variables.
2. Convert the integer divisor back into a baud rate using the following formula:

$$\mathit{BaudRate} = \frac{\mathit{FUARTCLK}}{16 \times \mathit{BaudDivisor}}$$

Where $\mathit{FUARTCLK}$ is the **7.38 MHz** Clock mentioned in the book.

For the Line Control Register, output the entire byte as a hexadecimal integer using the `%x` format specifier. The output should look like this:

```text
UART[0] Baudrate: XXXX
UART[0] Line Control Register: 0xXX
UART[1] Baudrate: XXXX
UART[1] Line Control Register: 0xXX
UART[2] Baudrate: XXXX
UART[2] Line Control Register: 0xXX
UART[3] Baudrate: XXXX
UART[3] Line Control Register: 0xXX
```

**Note:** The calculated baud rates will not be exactly 9600, 14400, etc., but slightly higher. This is expected and handled by the autograder. Ensure the text format matches exactly so the autograder can parse it. It is recommended that you remove any code asking for user input to ensure the tests run smoothly.

To exit the QEMU console type CTRL-A x.

### Running Your Code

To run your code in QEMU, type the following command into your terminal:

```bash
qemu-system-arm -M versatilepb -m 128M -kernel build/uart.bin -nographic -serial mon:stdio
```

## What to Turn In

The files `src/start.s`, `src/main.c` and `uart.c` should contain your modified code. When your code is run in QEMU, it should produce the output specified above.

Once you are finished:
1. Commit and push your changes to your GitHub repository.
2. Submit the assignment via Gradescope.
3. Select your repository when prompted.