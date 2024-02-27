/*------------------
-- Company: TEIS AB
-- Engineer: Andrey Yunin
--
-- Create Date: 2022-05-08
-- History:
-- Design Name: bluetooth
-- Target Devices: ALTERA MAX 10
-- Tool versions:
-- Nios IIe Software Build Tools for Eclipse (Quartus Prime 18.1)
-- Description:
-- Test-program for bluetooth with HC-06.
--
------------------*/

#include <stdio.h>
#include <string.h>
#include <system.h>
#include <alt_types.h>
#include <io.h>
#include <altera_avalon_pio_regs.h>
#include <DE10_Lite_Arduino_Driver.h>

#define UART_SET_BAUD(b)   IOWR_32DIRECT(UART_0_BASE, 16, (50000000/b)-1)// Set transmission rate (Baud rate) using register Divisor
// Divisor-register is based on systemclock 50000000.

int bluetooth_main ()
{
	printf("Testing Bluetooth with module HC-06!\n");
	printf("Starting bluetooth-software\n");
	printf("Connect your bluetooth-device and start typing text.\n");
	printf("To terminate the program send an 'x'. \n\n");
	char* msg = "Detected the character 'x'. Terminating program.\n";
	char prompt = 0;
	char *send_buffer;
	FILE* fp;
	int led0=1;
	alt_u32 PIO_REG = 0x0;
	alt_32 run = 1, loop_index = 0;

	arduino_uart_enable(1);												// Brief Toggle Arduino pin D0 and D1 as UART.
	// Param value if LSB is 1, UART is enabled, otherwise disabled .
	UART_SET_BAUD(9600);												// Set baud rate

	fp = fopen ("/dev/uart_0", "r+"); 									// Open file for read and write
	if (!fp)
	{
		printf("Failed to open the UART file!");
		run = 0;
	}
	while(run)
	{
		while ((prompt != 'x') && (loop_index < 50000))					// Loop until we receive an 'x'.
		{
			prompt = getc(fp); 											// Get a character from the UART.

			PIO_REG = IORD_ALTERA_AVALON_PIO_DATA(PIO_LEDS_OUT_BASE);	// Check led status
			if (prompt == '0')											// Testing if a zero '0' was received
			{
				printf("received:0 led=%d\n", led0); 					// NiosII console
				if (led0 == 0)
				{
					send_buffer = "Ledr_0 OFF\n"; 						// Bluetooth console
					IOWR_ALTERA_AVALON_PIO_DATA(PIO_LEDS_OUT_BASE, PIO_REG & led0);
					led0 = 1;											// Turn on LED0
				}
				else
				{
					send_buffer = "Ledr_0 ON \n"; 						// Bluetooth console
					IOWR_ALTERA_AVALON_PIO_DATA(PIO_LEDS_OUT_BASE, PIO_REG | led0);
					led0 = 0;											// Turn off LED0
				}
				fwrite (send_buffer, strlen(send_buffer), 1, fp);		// Send message to bluetooth console
			}
			else
			{
				printf("%c \n",prompt);									// Print to console
			}
			++loop_index;
		}
		fwrite (msg, strlen (msg), 1, fp);								// Print a message if character is 'x'.
		printf("\nClosing the UART file.\n");
		fprintf(fp, "Closing the UART file.\n");
		fclose (fp);													// Close file
		run = 0;
	}
	printf("Finished testing Bluetooth with module HC-06!\n\n");

	return 0;
}
