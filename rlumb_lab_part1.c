/*	Author: Richard Tobing, rlumb001@ucr.edu
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #9  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding timer.h or example
 *	code, is my own original work.
	
	Demo Link: https://www.youtube.com/shorts/Za3kSmavv2w
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
///----------------------------------------timer.h----------------------------------------------////
volatile unsigned char TimerFlag = 0; 
unsigned long _avr_timer_M = 1; 
unsigned long _avr_timer_cntcurr = 0; 
void TimerOn() {
  TCCR1B = 0x0B;
  OCR1A = 125;
  TIMSK1 = 0x02;
  TCNT1 = 0;
  _avr_timer_cntcurr = _avr_timer_M;
  SREG |= 0x80;
}
void TimerOff() {TCCR1B = 0x00;}
void TimerISR() {TimerFlag = 1;}
ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--; 
	if(_avr_timer_cntcurr == 0) { 
		TimerISR(); 
		_avr_timer_cntcurr = _avr_timer_M;
	}
}
void TimerSet(unsigned long M) {
	_avr_timer_M = M;                               
	_avr_timer_cntcurr = _avr_timer_M;
}
////----------------------------------------/timer.h----------------------------------------------////


////----------------------------------------pulseWidthModulator----------------------------------------------////
void set_PWM(double frequency) {
    static double current_frequency;
    if (frequency != current_frequency) {
        if (!frequency) { TCCR3B &= 0x08; }
        else { TCCR3B |= 0x03; }
        
        if (frequency < 0.954) { OCR3A = 0xFFFF; }
        else if (frequency > 31250) { OCR3A = 0x0000; }
        else { OCR3A = (short) (8000000 / (128 * frequency)) - 1; }
        
        TCNT3 = 0;
        current_frequency = frequency;
    }
}
void PWM_on() {
    TCCR3A = (1 << COM3A0);
    TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
    set_PWM(0);
}

void PWM_off() {
    TCCR3A = 0x00;
    TCCR3B = 0x00;
}
////----------------------------------------/pulseWidthModulatorh----------------------------------------------////











enum states { SM_SMStart, wait,C, D, E } state;
unsigned char A,prev;
signed long cnt = 0;
signed long i = 0;

void Tick() {

    A = (~PINA) & 7; 
    PORTB = A;


    switch (state) {
        case wait:
            prev = 0;
	    if(A==1){state = C; }
	    else if(A==2){state =D; }
	    else if(A==4){state = E; }
	    else{state = wait; }
	    	break;
        case C:
            prev = 1;
	    if(A==1){state = C; }
	    else{state = wait; }
	    	break;
        case D:
            prev = 2;
	    if(A==2){state = D; }
	    else{state = wait; }
	    	break;
        case E:
	   prev = 3;
	   if(A==4){state = E; }
	   else{state = wait; }
		break;
    }
    
   switch(state){
			case wait: set_PWM(0);break;
	    	 	case C:set_PWM(261.63);break;
        		case D:set_PWM(293.66);break;
        		case E:set_PWM(329.63);break;
    }
}

int main(void) {
    	DDRA = 0x00; PORTA = 0xFF;   //when dealing with LCDs, ALL ports need to be used, or the output will look all funky
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	PWM_on();
	
    state = wait;
    
    while (1) {Tick(); }


    //return 1;
}

