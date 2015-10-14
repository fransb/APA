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
/*#define PING_ID 0x80
#define PONG_ID 0x81
#define NETWORK 0xC05A
#define DEVICE PING_ID*/

#include <NRF24L01P.h>
#include <VWI.h>
#include <../command_type.h>
NRF24L01P rf(APA::NETWORK, APA::CLIENT_ID);

AnalogPin aPin0(Board::A0);
AnalogPin aPin1(Board::A1);
AnalogPin aPin2(Board::A2);

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
}

void loop()
{
  rf.powerup();
  APA::message_t message;
  message.command = APA::AWAKE;
  message.payload = 0;
  uint16_t value;
  rf.send(APA::SERVER_ID, APA::PING_TYPE, &message, sizeof(message));
  uint32_t sleep_time = 100L;
  while (1) {
    uint8_t src;
    uint8_t port;
    int res = rf.recv(src, port, &message, sizeof(message), APA::wait);
    if (res == (int) sizeof(message)) {
      trace << PSTR("client:cmd=") << message.command << endl;
      trace << PSTR("client:payload=") << message.payload << endl;
      if (message.command == APA::SEND_ANALOG_PIN){

	if (message.payload == 0) {
	  value = aPin0.sample();
	} else if (message.payload == 1) {
	  value = aPin1.sample();	
	} else {
	  value = aPin2.sample();
	}
	message.command = APA::ANALOG_PIN_VALUE;
	message.payload = value;
	rf.send(APA::SERVER_ID, APA::PING_TYPE, &message, sizeof(message));
	
      } else if (message.command == APA::SLEEP) {
	sleep_time = message.payload;
	break;
      }
    } else {
      break;
    }
  }
    
  rf.powerdown();
  delay(sleep_time); 
    
}
