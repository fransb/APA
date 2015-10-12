/**
 * @file CosaWirelessPing.ino
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
 * Cosa Wireless interface demo; send-receive messages to and from
 * CosaWirelessPong. Send sequence number to pong and wait for reply
 * with incremented sequence number. Retransit after reply wait and
 * inter-message delay.
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
#include "Cosa/AnalogPin.hh"

// Configuration; network and device addresses.
#define PING_ID 0x80
#define PONG_ID 0x81
#define NETWORK 0xC05A
#define DEVICE PING_ID
#include <NRF24L01P.h>
NRF24L01P rf(NETWORK, DEVICE);

#include <VWI.h>
typedef int16_t ping_t;
static const uint8_t PING_TYPE = 0x80;

AnalogPin aPin(Board::A0);

void setup()
{
  uart.begin(9600);
  trace.begin(&uart, PSTR("Client: the sampler"));
  Watchdog::begin();
  RTC::begin();
  ASSERT(rf.begin());
#if defined(USE_LOW_POWER)
  rf.set_output_power_level(-18);
#endif
  trace << PSTR("nr=sequence number") << endl;
  trace << PSTR("rc=retransmission count") << endl;
  trace << PSTR("arc=accumulated retransmission count") << endl;
  trace << PSTR("dr%=drop rate procent (arc*100/(arc + nr))") << endl;
  trace << endl;
}

void loop()
{
  // Auto retransmission wait (ms) and acculated retransmission counter
  static const uint16_t ARW = 200;
  static uint16_t arc = 0;

  ping_t cmd = 0;
  ping_t value = aPin.sample();
  INFO("aPin = %d", value);

  // Receive port and source address
  uint8_t port;
  uint8_t src;

  // Send value, receive command.
  uint32_t now = RTC::millis();
  uint8_t rc = 0;
  trace << now << PSTR(":a0:value=") << value;
  while (1) {
    rf.send(PONG_ID, PING_TYPE, &value, sizeof(value));
    int res = rf.recv(src, port, &cmd, sizeof(cmd), ARW);
    if (res == (int) sizeof(cmd)) break;
    rc += 1;
  }
  arc += rc;
  trace << PSTR(",server:cmd=") << cmd
	<< PSTR(",rc=") << rc
	<< PSTR(",arc=") << arc
	<< endl;
  rf.powerdown();
  static const uint32_t PERIOD = 10000L;
  uint32_t ms = PERIOD - RTC::since(now);
  if (ms > PERIOD) ms = PERIOD;
  delay(ms);
}
