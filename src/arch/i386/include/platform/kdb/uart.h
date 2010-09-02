#ifndef _PLATFORM_KDB_UART_H_
#define _PLATFORM_KDB_UART_H_

#include <platform/kdb/i16550A.h>

/** primitive plaform indipendency */

#define kdb_platform_uart_init kdb_i16550A_init
#define kdb_platform_uart_getchar kdb_i16550A_getchar
#define kdb_platform_uart_putchar kdb_i16550A_putchar
#define kdb_platform_uart_tx_ien kdb_i16550A_tx_ien

#endif
