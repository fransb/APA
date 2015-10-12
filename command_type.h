#ifndef COMMAND_TYPE_H
#define COMMAND_TYPE_H

#include "Cosa/Types.h"

namespace APA {

  enum command_t {
    AWAKE,
    SEND_ANALOG_PIN,
    ANALOG_PIN_VALUE,
    SLEEP
  } __attribute__((packed));

  struct message_t {
    command_t command;
    uint16_t payload;
  } __attribute__((packed));
  
  static const uint32_t wait = 400;
  
  static const uint8_t CLIENT_ID = 0x80;
  static const uint8_t SERVER_ID = 0x81;
  static const uint8_t PING_TYPE = 0x80;
}
#endif
