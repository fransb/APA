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
#define SERVER_ID 0x81
#define CLIENT_ID 0x80


// Select Wireless device driver
#include <NRF24L01P.h>
NRF24L01P rf(NETWORK, SERVER_ID);

#include <VWI.h>
#include <../command_type.h>

typedef int16_t ping_t;

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

void get_analog_pin(int pin) {
  while (1) {
    message_t message;
    message.command = SEND_ANALOG_PIN;
    message.payload = pin;
    rf.send(CLIENT_ID, APA::PING_TYPE, &message, sizeof(message));
    
    size_t size = sizeof(message);
    uint8_t src;
    uint8_t port;
    int res = rf.recv(src, port, &message, size, APA::wait);
    if (res == (int) sizeof(message)) {
      trace << PSTR("A") << pin << message.payload << endl;
    }
  }
}

void loop()
{
  uint8_t port;
  uint8_t src;
  ping_t value;
  ping_t cmd = 1;
  message_t message;


  while (rf.recv(src, port, &message, sizeof(message)) != sizeof(message)) yield();

  if (message.command == AWAKE) {

    get_analog_pin(0);
    get_analog_pin(1);
    get_analog_pin(2);
    
  }
  message.command = SLEEP;
  message.payload = 10000;
  rf.send(src, port, &message, sizeof(message));
}


