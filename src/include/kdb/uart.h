#ifndef _KDB_UART_H_
#define _KDB_UART_H_

/** initializes kdb_uart */
void kdb_uart_init();

/** handle receive interrupt.
 * called from platform kdb uart driver
 **/
void kdb_uart_rx_isr(int port, char ch);

/** handle ready to transmit interrupt.
 * called from platform kdb uart driver
 **/
void kdb_uart_tx_isr(int port);

/** write a character to the default serial port
 * the operation blocks until the operation succeeds (polling)
 **/
void kdb_uart_putchar(char ch);
/**
 * read a character from the default serial port
 * the operation returns -1 when there is no byte in the buffer
 *
 */
int kdb_uart_getchar();

/**
 * acknowledges the uart interrupt.
 * useful when we want to continue comunicating
 * with the host but effectively blocking the interrupted thread
 *  see kdb_suspend
 */
void kdb_uart_ack();

#endif
