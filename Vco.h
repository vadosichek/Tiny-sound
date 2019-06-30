#include <Arduino.h>

//defines for synth types
//all are dual oscillator setups - 
#define NOISE 1 // phase distortion -
#define FM 0 //aka phase modulation
#define TAH 2 //aka track & hold modulation (downsampling with T&H)

//defines for synth parameters
#define PITCH 2
#define WS_1 3
#define WS_2 1

class Vco{
	public:
		byte sample;
		byte sample2;
		
		void init();
		void render();
		void update();
		void writeWave(int wave);
		void setFrequency(uint16_t input);
		void setFrequency2(uint16_t input);
		void setFrequency3(uint16_t input);
		void setMode(uint8_t _mode);
		void setXYmode(bool _XYmode);
	private:
		uint8_t mode;
		unsigned int _phase;
		unsigned int frequency;
		unsigned int _phase2, _phase4, _phase5,_phase6;
		unsigned int frequency2, frequency4, frequency5, frequency6;
		uint8_t _phs,_phs90;
		uint8_t _phase3;
		uint16_t WS_2_value;
		bool XYmode;
		unsigned char wavetable[256];
		uint16_t sampleEnd;
		uint8_t decayVolume;
		uint8_t decayTime;
		uint8_t _sample;
		uint8_t _saw, _lastSaw;
		uint8_t decayCounter=0;
		
		void sineWave();
		void sawtoothWave();
		void triangleWave();
		void squareWave();
		void zeroWave();
		void renderDecay();
};