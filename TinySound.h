#define F_CPU 8000000  // This is used by delay.h library
#include <Arduino.h>

class TinySound{
	public:
		void init();
		void update();
		uint16_t clocks();
		bool analogRelevant(int id);
		uint8_t analogValues[4];
		
	private:
		uint16_t time;
		uint8_t analogChannelRead=1;
		uint8_t lastAnalogValues[4];
		uint8_t _clocks;
		uint8_t lastAnalogChannelRead;
		bool firstRead=false;
		const uint8_t analogToDigitalPinMapping[4]={PORTB5,PORTB2,PORTB4,PORTB3};
		uint8_t startupRead=0;
		uint8_t analogChannelSequence[6]={0,1,0,2,0,3};
		uint8_t analogChannelReadIndex;
		
		void setTimers(void);
};