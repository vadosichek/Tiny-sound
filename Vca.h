#include <Arduino.h>

class Vca{
	public:
		void init();
		int render(int input, int other);
		void setAmplification(int _amp);
	private:
		uint8_t amp=0;
};