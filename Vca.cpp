#include <Vca.h>
#include <Arduino.h>

void Vca::init(){
	setAmplification(512);
	//setFrequency(512);
}

void Vca::setAmplification(int _amp){
  amp=_amp;
}

int Vca::render(int input, int other){
	TCNT1 = 0; 
	return constrain(((input>>2)*amp + (other>>4)*(1024-amp))>>8,0,512);
}