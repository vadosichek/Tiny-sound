#include <TinySound.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/io.h>        // Adds useful constants
#include <util/delay.h>
#include <Utilit.h>

void TinySound::init(){
	digitalWrite(5,HIGH); //turn on pull up resistor for the reset pin
	pinMode(0, OUTPUT);
	pinMode(1, OUTPUT);

	setTimers(); //setup interrupts
	
    //setup ADC and run it in interrupt
	utilitInit();
	connectChannel(analogChannelRead);
	startConversion();
    _delay_us(100);
	while(startupRead<12){ loop(); }
}

void TinySound::setTimers(void){
	PLLCSR |= (1 << PLLE);               // Enable PLL (64 MHz)
	_delay_us(100);                      // Wait for a steady state
	while (!(PLLCSR & (1 << PLOCK)));    // Ensure PLL lock
	PLLCSR |= (1 << PCKE);               // Enable PLL as clock source for timer 1
	PLLCSR |= (1 << LSM); //low speed mode 32mhz
	cli();                               // Interrupts OFF (disable interrupts globally)


	TCCR0A = 2<<COM0A0 | 2<<COM0B0 | 3<<WGM00;
	TCCR0B = 0<<WGM02 | 1<<CS00;

	//  setup timer 0 to run fast for audiorate interrupt 

	TCCR1 = 0;                  //stop the timer
	TCNT1 = 0;                  //zero the timer
	GTCCR = _BV(PSR1);          //reset the prescaler
	OCR1A = 255;                //set the compare value
	OCR1C = 255;    
	TIMSK = _BV(OCIE1A);//interrupt on Compare Match A
	//start timer, ctc mode, prescaler clk/1    
	TCCR1 = _BV(CTC1) | _BV(CS12);
	sei();
}

uint16_t TinySound::clocks(){
  //return _clocks;
  return TCNT0|(_clocks<<8);
}

void TinySound::update(){
	startupRead++;
	if(!firstRead){ // discard first reading due to ADC multiplexer crosstalk
		//update values and remember last values
		lastAnalogValues[analogChannelRead]=analogValues[analogChannelRead];
		analogValues[analogChannelRead]= getConversionResult()>>2;
		//set ADC MULTIPLEXER to read the next channel
		lastAnalogChannelRead=analogChannelRead;
		analogChannelReadIndex++;
		if(analogChannelReadIndex>5) analogChannelReadIndex=0;
		analogChannelRead=analogChannelSequence[analogChannelReadIndex];
		connectChannel(analogChannelRead);
		firstRead=true;
		//start the ADC - at completion the interupt will be called again
		startConversion();
	}
	else{ 
		/* 
		at the first reading off the ADX (which will not used) 
		something else will happen the input pin will briefly turn to output to 
		discarge charge built up in passive mixing ciruit using zener diodes
		because zeners have some higher unpredictable capacitance, various voltages might get stuck on the pin
		*/
		if(analogValues[analogChannelRead]<200) bitWrite(DDRB,analogToDigitalPinMapping[analogChannelRead],1);
		bitWrite(DDRB,analogToDigitalPinMapping[analogChannelRead],0);
		bitWrite(PORTB,analogToDigitalPinMapping[analogChannelRead],0);
		firstRead=false;
		startConversion();
	}
}

bool TinySound::analogRelevant(int id){
	return lastAnalogChannelRead==id && lastAnalogValues[id]!=analogValues[id];
}