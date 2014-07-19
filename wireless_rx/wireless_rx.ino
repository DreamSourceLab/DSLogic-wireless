/*
 * This file is part of the DSLogic-wireless project.
 *
 * Copyright (C) 2014 DreamSourceLab <dreamsourcelab@dreamsourcelab.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
#include <SPI.h>
#include "API.h"
#include "nRF24L01.h"

unsigned char rx_buf[TX_PLOAD_WIDTH+3] = {0}; // initialize value
unsigned char tx_buf[TX_PLOAD_WIDTH] = {0};

void setup()
{
  rf24l01setup();
}

void loop() 
{
  unsigned int data16;
  for(;;)
  {
    unsigned char status;
    do
    {
      status = SPI_Read(STATUS);                         // read register STATUS's value
    } while(!(status&RX_DR));
    
    if(status&RX_DR)                                                 // if receive data ready (TX_DS) interrupt
    {
      SPI_Read_Buf(RD_RX_PLOAD, rx_buf, TX_PLOAD_WIDTH);             // read playload to rx_buf
      SPI_RW_Reg(FLUSH_RX,0);                                        // clear RX_FIFO
      if (digitalRead(DSEN)) {
        for(int j=0; j<25; j++) {
          for(int i=0; i<18; i++) {
              digitalWrite(DSCLK, i%2);
              PORTD = rx_buf[i];
              //digitalWrite(DSCLK, 1);
          }
          digitalWrite(DSCLK, 0);
        }
      }
    }
    SPI_RW_Reg(WRITE_REG+STATUS,status);                             // clear RX_DR or TX_DS or MAX_RT interrupt flag
  }
}
