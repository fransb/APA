#ifndef CONFIG_H
#define CONGIG_H

#include "Cosa/Types.h"

namespace APA {

  enum command_t {
    AWAKE,
    SEND_ANALOG
  } __attribute__((packed));

  struct message_t {
    command_t command;
    uint16_t payload;
  };
}
#endif
