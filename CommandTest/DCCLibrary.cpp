
#include "DCCLibrary.h"


// 23. November 2009
// works well with LMD18200 Booster !!!!!

/*Copyright (C) 2009 Michael Blank
 
 This program is free software; you can redistribute it and/or modify it under the terms of the
 GNU General Public License as published by the Free Software Foundation;
 either version 3 of the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU General Public License for more details.
*/

/* Jan 2014. This file has been modified from the original to seperate the DCC sending functions from the rest of the sketch.
 
 * SetupTimer2 converted to SetupDCC so that the DCC pinout could be sent from the sketch
 * struct message has been typedefed and put into DCCLibrary.h file.
       The array to hold the messages is now external to this file.
 
 * ISR() cleaned up a bit - flag now uses LONGPULSE and SHORTPULSE instead of 0 and 1.
 * Message swapping has been implemented as a function, GetNextMessage() which does nothing.
        As it stands msg[0] ( initially idle ) will always be sent
        The sketch should assign a proper function to GetNextMessage to send new messages.
*/


//Timer frequency is 2MHz for ( /8 prescale from 16MHz )
#define TIMER_SHORT 0x8D        // 58usec pulse length
#define TIMER_LONG  0x1B        // 116usec pulse length
#define LONGPULSE   0
#define SHORTPULSE  1

byte last_timer = TIMER_SHORT;  // store last timer value
byte flag = LONGPULSE;          // used for short or long pulse
byte every_second_isr = 0;      // pulse up or down

// definitions for state machine
#define PREAMBLE 0
#define SEPERATOR 1
#define SENDBYTE  2

byte state = PREAMBLE;
byte preamble_count = 16;
byte outbyte = 0;
byte cbit = 0x80;
int byteIndex = 0;
int outputPin;                  // Arduino pin for DCC out. This pin is connected to "DIRECTION" of LMD18200

extern Message msg[];
extern int currentIndex;

void DoNothing(void){}
void (*GetNextMessage)(void) = &DoNothing;                  // assign a proper function to send a new message


void SetupDCC(int DCC_OutPin){
    
    outputPin = DCC_OutPin;
      
    //Setup Timer2.
    //Configures the 8-Bit Timer2 to generate an interrupt at the specified frequency.
    //Timer2 Settings: Timer Prescaler /8, mode 0
    //Timmer clock = 16MHz/8 = 2MHz oder 0,5usec
    TCCR2A = 0;
    TCCR2B = 0<<CS22 | 1<<CS21 | 0<<CS20;
    
    //Timer2 Overflow Interrupt Enable
    TIMSK2 = 1<<TOIE2;
    
    //load the timer for its first cycle
    TCNT2 = TIMER_SHORT;
}


//Timer2 overflow interrupt vector handler
ISR(TIMER2_OVF_vect){
    //Capture the current timer value TCTN2. This is how much error we have
    //due to interrupt latency and the work in this function
    //Reload the timer and correct for latency.
    // for more info, see http://www.uchobby.com/index.php/2007/11/24/arduino-interrupts/
    byte latency;
    
    // for every second interupt just toggle signal
    if(every_second_isr){
        digitalWrite(outputPin,1);
        every_second_isr = 0;
        
        // set timer to last value
        latency = TCNT2;
        TCNT2 = latency + last_timer;
    }else{  // != every second interrupt, advance bit or state
        digitalWrite(outputPin,0);
        every_second_isr = 1;
        
        switch(state)  {
            case PREAMBLE:
                flag = SHORTPULSE;
                preamble_count--;
                if(preamble_count == 0){  // advance to next state
                    state = SEPERATOR;
                    // get next message
                    GetNextMessage();
                    byteIndex = 0; //start msg with byte 0
                }
                break;
            case SEPERATOR:
                flag = LONGPULSE;
                // then advance to next state
                state = SENDBYTE;
                // goto next byte ...
                cbit = 0x80;  // send this bit next time first
                outbyte = msg[currentIndex].data[byteIndex];
                break;
            case SENDBYTE:
                flag = (outbyte & cbit)? SHORTPULSE : LONGPULSE;
                cbit = cbit >> 1;
                if(cbit == 0){  // last bit sent, is there a next byte?
                    byteIndex++;
                    if(byteIndex >= msg[currentIndex].len){
                        // this was already the XOR byte then advance to preamble
                        state = PREAMBLE;
                        preamble_count = 16;
                    }else{
                        // send separator and advance to next byte
                        state = SEPERATOR ;
                    }
                }
                break;
        }
        
        if(flag == SHORTPULSE){
            latency = TCNT2;
            TCNT2 = latency + TIMER_SHORT;
            last_timer = TIMER_SHORT;
        }else{
            latency = TCNT2;
            TCNT2 = latency + TIMER_LONG;
            last_timer = TIMER_LONG;
        }
    }
}

