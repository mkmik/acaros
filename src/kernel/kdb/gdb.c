#include <kdb/gdb.h>
#include <kdb/kdb.h>
#include <platform/kdb/kdb.h>
#include <platform/kdb/uart.h>
#include <platform/mal/interrupt.h>
#include <kdb/packet.h>
#include <kdb/command.h>
#include <log/log.h>

/** implemented as a two level state machine.
 * the DATA state in the top level machine runs a 
 * sub-state-machine to parse the commands;
 * this way checksums for incoming packets can
 * be checked in the DATA state keeping thinks simpler */

static kdb_stateMap_t kdb_mainStates[] = {
  {kdb_junk_process, 0},
  {kdb_data_process, 0},
  {kdb_checksum_process, 0},
  {kdb_checksum2_process, 0}
};

static kdb_state_t currentState = JUNK;

static u_int8_t kdb_checksum = 0;

void kdb_receive(char ch) {
  kdb_mainStates[currentState].process(ch, &currentState);
}

void kdb_junk_process(char ch, kdb_state_t* nextState) {
  if(ch == 3) { // CTRL-C
    kdb_currentContext = kdb_uart_savedContext;
    kdb_prepareRegisters();
    kdb_service();
  } else if(ch == '$') {
    kdb_checksum = 0;
    *nextState = DATA;
  } 
}

void kdb_data_process(char ch, kdb_state_t* nextState) {
  if(ch == '#') {
    *nextState = CHECKSUM;
    return;
  }

  kdb_checksum += (u_int8_t)ch;

  kdb_command_receive(ch);
}

void kdb_checksum_process(char ch, kdb_state_t* nextState) {
  if(fromhex(ch) != (kdb_checksum >> 4)) {
    kdb_sendNack();
    *nextState = JUNK;
  } else
    *nextState = CHECKSUM2;
}

void kdb_checksum2_process(char ch, kdb_state_t* nextState) {
  if(fromhex(ch) == (kdb_checksum & 0xF)) {
    kdb_sendAck();    

    int wasAttached = kdb_attached;

    kdb_currentCommand->callback(kdb_currentCommand);
    kdb_command_reset();

    if(!wasAttached) {
      kdb_attached = 1;
      *nextState = JUNK;

      if(!kdb_suspended) {
	kdb_currentContext = kdb_uart_savedContext;
	kdb_prepareRegisters();
	kdb_suspend();
      }
      return;
    }

  } else
    kdb_sendNack();
  *nextState = JUNK;
}
