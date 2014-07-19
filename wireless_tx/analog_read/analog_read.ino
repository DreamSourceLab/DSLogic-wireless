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

void setup(){
  rf24l01Setup();
}

void loop(){
  sendDataToRF(analogRead(0) << 6);
  delay(2);
}

void sendDataToRF(int data)
{
  static int dataNum = 0;
  if (dataNum < TX_PLOAD_WIDTH) {
    rx_buf[dataNum++] = data;
    rx_buf[dataNum++] = (data >> 8);
  }
  while (dataNum >= TX_PLOAD_WIDTH) {
    unsigned char sstatus = SPI_Read(STATUS);                   // read register STATUS's value
    if(sstatus&TX_DS)                                           // if receive data ready (TX_DS) interrupt
    {
      SPI_RW_Reg(FLUSH_TX,0);                                  
      SPI_Write_Buf(WR_TX_PLOAD,rx_buf,TX_PLOAD_WIDTH);       // write playload to TX_FIFO
      dataNum = 0;
    }
    if(sstatus&MAX_RT)                                         // if receive data ready (MAX_RT) interrupt, this is retransmit than  SETUP_RETR                          
    {
      SPI_RW_Reg(FLUSH_TX,0);
      SPI_Write_Buf(WR_TX_PLOAD,rx_buf,TX_PLOAD_WIDTH);      // disable standy-mode
    }
    SPI_RW_Reg(WRITE_REG+STATUS,sstatus);                     // clear RX_DR or TX_DS or MAX_RT interrupt flag  
  }
}
