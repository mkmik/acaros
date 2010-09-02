#ifndef _KDB_GDB_H_
#define _KDB_GDB_H_

/** main state machine states. handle packet deconding */
typedef enum {JUNK=0, DATA, CHECKSUM, CHECKSUM2} kdb_state_t;

/** maps a state with a function to execute for each character.
 * the function can change the state by setting the nextState argument
 * which is passed by reference */
struct kdb_stateMap {
  void (*process)(char ch, kdb_state_t* nextState);
  void (*change)();
};

typedef struct kdb_stateMap kdb_stateMap_t;

/** entry point for the command decoder.
 * as characters are received they are passed to this function 
 * which could call the kdb_suspend function and indefinitely
 * stall the execution of the current thread. so be prepared...
 **/
void kdb_receive(char ch);

/** convert a character from hexadecimal to integer */
#define fromhex(c) (((c) & 0x40) ? ((c) & 0x20 ? (c) - 'a' + 10: (c) - 'A' + 10) : (c) - '0')

// state machine

/** processes out-of-packet data, searching for a start-of-packet marker */
void kdb_junk_process(char ch, kdb_state_t* nextState);
/** process data inside of packet ending a packet when end-of-packet marker 
 * is found. Every data is appended to the checksum, and a second state machine
 * will parse command, using the kdb_command_receive entry point.
 */
void kdb_data_process(char ch, kdb_state_t* nextState);
/** process the first nibble of checksum */
void kdb_checksum_process(char ch, kdb_state_t* nextState);
/** process the second nibble of checksum, and if correct
 * executes the parsed command */
void kdb_checksum2_process(char ch, kdb_state_t* nextState);

#endif
