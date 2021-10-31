/*	Author: Richard Tobing, rlumb001@ucr.edu
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #9  Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding timer.h or example
 *	code, is my own original work.
	
	Demo Link: https://www.youtube.com/watch?v=vJLKy7iGsoM
 */
#include <avr/io.h>
#include <avr/interrupt.h>
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











enum states{wait,cMin,cPlus,clear}state;
unsigned char A,prev;
signed long cnt = 0;
unsigned char sndOn = 1;


void Tick(){
	 A = (~PINA) & 7;
	   
	    
	    switch(state){
	    	case wait:
				prev = 0;
				if((A==4)){state = clear; }
				else if(A==1){state = cPlus; }
				else if(A==2){state = cMin; }
				else{state = wait; }
				break;
		case cPlus:
				prev = 1;
				if((A==4)){state = clear; }
				else if(A==1){state = cPlus; }
				else if(A==2){state = cMin; }
				else{state = wait; }
				break;
		case cMin:
				prev = 2;
				if((A==4)){state = clear; }
				else if(A==1){state = cPlus; }
				else if(A==2){state = cMin; }
				else{state = wait; }
				break;
		case clear:
				prev = 4;
				if(A!=3){state = wait; sndOn = !sndOn; }
				else{state = clear; }
				break;
	    }

	    //-----------------------------------------------------------

	    switch(state){
		case wait:
			break;

		case cPlus:
			if((prev!=1)&&(cnt<8)){cnt++; }
			if(prev == 1){cnt++; }
			break;

		case cMin:
			if((prev!=2)&&(cnt>0)){
				if(cnt ==0){cnt = 0; }
				else{cnt = cnt -1; }
			}
			if(prev == 2){cnt = cnt-1; }
			break;

		case clear:
			//cnt = 0;
			break;
	    }

	    if(cnt > 8){cnt = 8; }
	    else if(cnt < 0){cnt = 0; }
	    PORTB = cnt;

	if(sndOn){
		if (cnt == 0) {set_PWM(0);  }
		else if (cnt == 1) {set_PWM(261.63);  }
		else if (cnt == 2) {set_PWM(293.63);  }
		else if (cnt == 3) {set_PWM(329.63);  }
		else if (cnt == 4) {set_PWM(349.23);   }
		else if (cnt == 5) {set_PWM(392);   }
		else if (cnt == 6) {set_PWM(440);  }
		else if (cnt == 7) {set_PWM(493.88);  }
		else if (cnt == 8) {set_PWM(523.25);  }

	}
	else{set_PWM(0);  }
	

	    
}


int main(void) {
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	//DDRC = 0xFF; PORTC = 0x00;
	TimerSet(200);
	TimerOn();
	PWM_on();
	
	state = wait;
	

    while (1) {
	 
	Tick();
	
	while (!TimerFlag);	
	TimerFlag = 0;

		    
    }
    //return 0;
}


