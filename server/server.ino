/**
 * @file CosaWirelessPong.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, Mikael Patel
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * @section Description
 * Cosa Wireless interface demo; receive-send messages from and to
 * CosaWirelessPing. Check for correct port(PING_TYPE) and message
 * size. Increment ping number and send as reply. Note: asymmetric
 * naming, pong does not know the ping device address.
 *
 * @section Circuit
 * See Wireless drivers for circuit connections.
 *
 * This file is part of the Arduino Che Cosa project.
 */

#include "Cosa/Trace.hh"
#include "Cosa/IOStream/Driver/UART.hh"
#include "Cosa/Watchdog.hh"
#include "Cosa/RTC.hh"

// Configuration; network and device addresses.
#define NETWORK 0xC05A
#define DEVICE 0x81

// Select Wireless device driver
#include <NRF24L01P.h>
NRF24L01P rf(NETWORK, DEVICE);

#include <VWI.h>

typedef int16_t ping_t;
static const uint8_t PING_TYPE = 0x80;

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("Server: the commander"));
  Watchdog::begin();
  RTC::begin();
  ASSERT(rf.begin());
#if defined(USE_LOW_POWER)
  rf.set_output_power_level(-18);
#endif
}

void loop()
{
  uint8_t port;
  uint8_t src;
  ping_t value;
  ping_t cmd = 1;

  while (rf.recv(src, port, &value, sizeof(value)) != sizeof(value)) yield();
  if (port != PING_TYPE) return;
  trace << RTC::millis() << PSTR(":samper:value=") << value << endl;
  rf.send(src, port, &cmd, sizeof(cmd));
}
