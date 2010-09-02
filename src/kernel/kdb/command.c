#include <kdb/command.h>
#include <kdb/kdb.h>
#include <platform/kdb/kdb.h>
#include <kdb/gdb.h>
#include <kdb/packet.h>
#include <kdb/uart.h>
#include <log/log.h>

kdb_command_t kdb_commands[] = {
  {0, kdb_command_unknown}, 
  {'?', kdb_command_getSignal}, 
  {'H', kdb_command_setThread}, 
  {'c', kdb_command_continue},
  {'D', kdb_command_detach}, 
  {'s', kdb_command_step}, 
  {'m', kdb_command_readMemory},
  {'M', kdb_command_writeMemory}, 
  {'g', kdb_command_readRegisters}, 
  {'G', kdb_command_writeRegisters},
  {'P', kdb_command_writeRegister}
};

avl_node_t *kdb_commandRoot = 0;

kdb_stateMap_t kdb_commandStates[] = {
  {kdb_command_process, 0},
  {kdb_arguments_process, 0}
};

static kdb_state_t currentState = COMMAND;
kdb_command_t *kdb_currentCommand = 0;

void kdb_command_init() {
  int i;
  for(i=0; i<sizeof(kdb_commands)/sizeof(kdb_command_t); i++) {
    kdb_commands[i].node = null_avl_node;
    avl_add(&kdb_commands[i].node, &kdb_commandRoot, kdb_command_t);
  }
  
}

void kdb_command_receive(char ch) {
  kdb_commandStates[currentState].process(ch, &currentState);
}

void kdb_command_reset() {
  currentState = COMMAND;
  kdb_currentCommand = &kdb_commands[0]; // robustness
}

void kdb_command_process(char ch, kdb_state_t* nextState) {
  *nextState = ARGUMENTS;
  kdb_command_t *cmd = avl_find(ch, kdb_commandRoot, kdb_command_t, node);
  if(!cmd) {
    return;
  }
  
  kdb_currentCommand = cmd;
  cmd->argpos = 0;
}

void kdb_arguments_process(char ch, kdb_state_t* nextState) {
  assert(kdb_currentCommand);

  if(kdb_currentCommand->argpos < sizeof(kdb_currentCommand->arguments)) {
    kdb_currentCommand->arguments[kdb_currentCommand->argpos++] = ch;
    kdb_currentCommand->arguments[kdb_currentCommand->argpos] = 0;
  }
}

void kdb_command_unknown(kdb_command_t* cmd) {
  //  klogf(LOG_DEBUG, "unknown cmd callback\n");
  kdb_sendNotSupported();
}

void kdb_command_getSignal(kdb_command_t* cmd) {
  //  klogf(LOG_DEBUG, "getSignal cmd callback\n");
  kdb_sendPacket("S05"); // SIGTRAP
}

void kdb_command_setThread(kdb_command_t* cmd) {
  //  klogf(LOG_DEBUG, "set thread cmd callback\n");
  kdb_sendOk();
}

void kdb_command_continue(kdb_command_t* cmd) {
  //  klogf(LOG_DEBUG, "continue cmd callback\n");
  kdb_resume();
}

void kdb_command_detach(kdb_command_t* cmd) {
  kdb_attached = 0;
  kdb_sendOk();
  kdb_resume();
}

void kdb_command_step(kdb_command_t* cmd) {
  kdb_enableStep();
  kdb_resume();
}

void kdb_command_readMemory(kdb_command_t* cmd) {
  // parse arguments
  char *args = cmd->arguments;

  pointer_t addr = 0;
  while((*args != 0) && (*args != ',')) {
    addr <<= 4;
    addr |= fromhex(*args);
    args++;
  }

  u_int32_t len = 0;
  if(*args)
    args++;
  while(*args) {
    len <<=4;
    len |= fromhex(*args);
    args++;
  }
    
  kdb_beginPacket();
  int i;
  for(i=0; i<len; i++) {
    u_int8_t byte = kdb_readByte(addr + i);
    kdb_putchar(hexdigit(byte >> 4));
    kdb_putchar(hexdigit(byte & 0xF));
  }
   
  kdb_endPacket();
}

void kdb_command_writeMemory(kdb_command_t* cmd) {
  //  klogf(LOG_DEBUG, "writeMemory cmd callback\n");
  
  // parse arguments
  char *args = cmd->arguments;

  pointer_t addr = 0;
  while((*args != 0) && (*args != ',')) {
    addr <<= 4;
    addr |= fromhex(*args);
    args++;
  }

  u_int32_t len = 0;
  if(*args)
    args++;
  while((*args != 0) && (*args != ':')) {
    len <<=4;
    len |= fromhex(*args);
    args++;
  }

  if(len > 4) {
    kdb_sendNotSupported();
    return;
  }

  u_int32_t data = 0;
  if(*args)
    args++;
  while(*args) {
    data <<=4;
    data |= fromhex(*args);
    args++;
  }

  int i;
  for(i=0; i<len; i++)
    kdb_writeByte(addr+i, ((u_int8_t*)&data)[i]);

  kdb_sendOk();
}

void kdb_command_readRegisters(kdb_command_t* cmd) {
  kdb_beginPacket();
  int i;

  u_int8_t *reg = (u_int8_t*)&kdb_registers;
  for(i=0; i<sizeof(kdb_registers_t); i++) {
    kdb_putchar(hexdigit(reg[i] >> 4));
    kdb_putchar(hexdigit(reg[i] & 0xF));
  }
  kdb_endPacket();
}

void kdb_command_writeRegisters(kdb_command_t* cmd) {
  kdb_sendNotSupported();
}

void kdb_command_writeRegister(kdb_command_t* cmd) {
  // parse arguments
  char *args = cmd->arguments;

  u_int32_t reg = 0;
  while((*args != 0) && (*args != '=')) {
    reg <<= 4;
    reg |= fromhex(*args);
    args++;
  }

  u_int32_t val = 0;
  if(*args)
    args++;
  while(*args) {
    val <<= 4;
    val |= fromhex(*args);
    args++;
  }  

  klogf(LOG_DEBUG, "writing register %d val %x\n", reg, val);

  kdb_registers.reg[reg] = val;

  kdb_sendOk();
}
