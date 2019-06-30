#include <Vco.h>
#include <TR_HH_AT.h>



void Vco::init(){
	XYmode=false;
	writeWave(0);
}

void Vco::render(){
  _phase += frequency;   

  _phase2 += frequency2;
  _phase4 += frequency4;
  _phase5 += frequency5;
  if(mode){ // other than FM, FM=0
    _phase3=_phase2>>5;
    _phase6 += frequency6;
  }
  else{
    _phs=(_phase+(WS_2_value*wavetable[_phase2 >> 8])) >> 6; 
    sample = (wavetable[_phs] );
  }
}

void Vco::writeWave(int wave) {
  switch (wave) {
  case 0:
    sineWave();
    break;
  case 1:
    triangleWave();
    break;
  case 2:
    squareWave();
    break;
  case 3:
    sawtoothWave();
    break;
  case 4:
    digitalWrite(0, LOW);
    zeroWave();
    break;
  }
}

//functions to populate the wavetable
void Vco::sineWave() {                                       //too costly to calculate on the fly, so it reads from the sine table. We use 128 values, then mirror them to get the whole cycle
  for (int i = 0; i < 128; ++i) {
    wavetable[i] = pgm_read_byte_near(sinetable + i);
  }
  wavetable[128] = 255;
  for (int i = 129; i < 256; ++i) {
    wavetable[i] = wavetable[256 - i] ;
  }
}
void Vco::sawtoothWave() {
  for (int i = 0; i < 256; ++i) {
    wavetable[i] = i; // sawtooth
  }
}
void Vco::triangleWave() {
  for (int i = 0; i < 128; ++i) {
    wavetable[i] = i * 2;
  }
  int value = 255;
  for (int i = 128; i < 256; ++i) {
    wavetable[i] = value;
    value -= 2;
  }
}
void Vco::squareWave() {
  for (int i = 0; i < 128; ++i) {
    wavetable[i] = 255;
  }
  for (int i = 128; i < 256; ++i) {
    wavetable[i] = 1;                  //0 gives problems (offset and different freq), related to sample  = ((wavetable[phase >> 8]*amplitude)>>8);
  }
}
void Vco::zeroWave() {
  for (int i = 0; i < 256; ++i) {
    wavetable[i] = 1;                  //0 gives problems
  }
}

void Vco::setFrequency(uint16_t input) {
  if(   mode==NOISE) frequency = ((input-200)<<2)+1;
  else frequency = (input<<2)+1;
}

void Vco::setFrequency2(uint16_t input) {
  if(   mode==NOISE) frequency2 = (((input-300)<<2)+1)/2;
  else if (   mode==TAH){
    uint8_t multiplierIndex=WS_2_value>>5;
    frequency2 = (input<<2)+1;
    frequency4=(frequency2+1)*multiplier[multiplierIndex];
    frequency5=(frequency2-3)*multiplier[multiplierIndex+8];
    frequency6=(frequency2+7)*multiplier[multiplierIndex+16];
  }
  else{
    frequency2 = (input<<2)+1;
    frequency4=frequency2;
    frequency5=frequency2;
  }
}

void Vco::setFrequency3(uint16_t input) {
	WS_2_value = input;
}

void Vco::update(){
  if(mode==FM){
    if(XYmode){
      _phs90=_phs+64;
      sample2 = (wavetable[_phs90] );
    }
    else{
      _lastSaw=_saw;
      _saw=(((255-(_phase>>8))*(WS_2_value))>>8);
      sample2 = ((_saw*wavetable[_phase4 >> 8] )>>8)+((wavetable[_phase5 >> 8]*(255-WS_2_value))>>8);
      if(_lastSaw<_saw) _phase4=64<<8; // hard sync for phase distortion
      uint8_t shft=abs(_saw-_lastSaw);
      if(shft>3) _phase5+=shft<<8; //soft sync for previous settings of waveshape
    }
  }

  if(mode==NOISE){
    if((_phase>>2)>=(WS_2_value-100)<<5){
      _phase=0;
    }
    _sample = (char)pgm_read_byte_near(sampleTable+(_phase>>2));
    _sample=(_sample*wavetable[_phase2>>8])>>8;
    sample=_sample;
    sample2 = (wavetable[_phase3+(_phase>>8)]);
  }

  if(mode==TAH){
    if((_phase2 >> 8)>WS_2_value)  _phs=_phase>>8, sample = (wavetable[_phs] );
    if(XYmode){
      _phs90=_phs+64;
      sample2 = (wavetable[_phs90] );
    }
    else{
      sample2 = (wavetable[_phase2 >>8]+ wavetable[_phase4 >>8] + wavetable[_phase5 >>8]+ wavetable[_phase6 >>8])>>2;
    }
  } 
}

void Vco::renderDecay(){
  if(decayTime!=0){
    if(0){
      decayCounter+=8;
      if(decayCounter>=decayTime)
      {
        decayCounter=0;
        if(decayVolume>0) decayVolume--;
        else decayVolume=255, _phase=0;
      }
    }
  } 
}

void Vco::setMode(uint8_t _mode){
	mode = _mode;
}

void Vco::setXYmode(bool _XYmode){
	XYmode = _XYmode;
}