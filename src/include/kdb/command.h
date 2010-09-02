#ifndef _KDB_COMMAND_H_
#define _KDB_COMMAND_H_

#include <algo/avl.h>
#include <kdb/gdb.h>

/** states in command state machine */
typedef enum {COMMAND=0, ARGUMENTS} kdb_commandState_t;

struct kdb_command;
typedef struct kdb_command kdb_command_t;

/** called when a command is completely parsed */
typedef void (*kdb_command_callback_t)(kdb_command_t * cmd);

/** defines a debugger command */
struct kdb_command {
  /** name of the command,
   * also the character used in the protocol.
   * so a search can be made using this field 
   **/
  char name;
  /** called when a command is completely parsed */
  kdb_command_callback_t callback;
  /** commands are stored in an avl tree for searching.
   * this attributes is the node of this search tree */
  avl_node_t node;
  /** stores the arguments of this command.
   * TODO: replace with parsed data not raw string!! */
  char arguments[128];
  /** keeps track of the current position in the arguments array
   * when filling. Used in the ARGUMENTS state. */
  int argpos;
};

/** avl search function */
avl_make_compare(kdb_command_t, node, name);
/** avl search function */
avl_make_match(kdb_command_t, name);

/** maintains the currentCommands in the parsing states of the state machine.
 * for example arguments are parsed and are attached to that specific command.
 *  in future the state machine will be extended using specific formats from 
 * the currentCommand, in order to parse command with different argument formats */
extern kdb_command_t *kdb_currentCommand;

/** init gdb command state machine */
void kdb_command_init();

/** dispatch point for command state machine */
void kdb_command_receive(char ch);
/** reset command state machine */
void kdb_command_reset();

/** process character in COMMAND state */
void kdb_command_process(char ch, kdb_state_t* nextState);
/** process character in ARGUMENT state */
void kdb_arguments_process(char ch, kdb_state_t* nextState);

/** respond to unknown command */
void kdb_command_unknown(kdb_command_t* cmd);
/** respond to get signal '?' command, returns the reasion the kernel is stopped */
void kdb_command_getSignal(kdb_command_t* cmd);
/** sets the current thread. (Currently ignored) */
void kdb_command_setThread(kdb_command_t* cmd);
/** continues execution */
void kdb_command_continue(kdb_command_t* cmd);
/** continues execution and detaches debugger */
void kdb_command_detach(kdb_command_t* cmd);
/** single steps execution */
void kdb_command_step(kdb_command_t* cmd);
/** responds the contents of memory at address requested 
 * by the debugger (in arguments array).
 * address is interpreted as virtual address.
 * returns an error to gdb if a memory violation occours. */
void kdb_command_readMemory(kdb_command_t* cmd);
/** writes to the kernel memory with address and contents given 
 * in the arguments array. Address is interpreted as virtual address. 
 * returns error to gdb if a memory violation occours */
void kdb_command_writeMemory(kdb_command_t* cmd);
/** returns to gdb the contents of all registers */
void kdb_command_readRegisters(kdb_command_t* cmd);
/** writes all registers */
void kdb_command_writeRegisters(kdb_command_t* cmd);
/** write a specific register */
void kdb_command_writeRegister(kdb_command_t* cmd);

#endif
