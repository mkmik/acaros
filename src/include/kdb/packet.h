#ifndef _KDB_PACKET_H_
#define _KDB_PACKET_H_

/**
 * host: gdb program running on another machine
 * target: we, the kernel debugger
 */

/** returns the hexadecimal rapresentation of the nibble n */
#define hexdigit(n) (((n) < 10) ? '0' + (n) : 'a' + ((n) - 10))

/** begins sending a packet.
 * checksum counter is reset */
void kdb_beginPacket();
/** the packet is terminated and the so far collected checksum is sent
 */
void kdb_endPacket();
/** write a character to the debugging host, maintaining
 * the checksum count for the current packet.
 */
void kdb_putchar(char ch);
/** a convenience function that calls putchar for every
 * character in the str argument.
 */
void kdb_putstr(char* str);

/** a convenience function that sends an hex encoded
 * integer using host byte order covention
 */
void kdb_putint(u_int32_t val);

/** a convenience function that builds a packet from a string.
 * useful for quick responses, but usually data is builded
 * by repetitive calling of the putstr/putchar functions
 * because the debugger is very scarse in memory and sprintf 
 * and family are better avoided.
 */
void kdb_sendPacket(char *str);

/** convenience function
 * it sends a standard OK response to the host 
 */
void kdb_sendOk();

/** acknowledges a receiving packet.
 * the checksum was correct 
 */
void kdb_sendAck();

/** negatively acknowledges a receiving packet.
 * the checksum was not correct, but by not dropping
 * the packet the host won't delay the retransmission
 */
void kdb_sendNack();

/** sends an empty packet to the host.
 * it means that we don't support the command
 */
void kdb_sendNotSupported();


#endif
