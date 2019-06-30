#include <TinySound.h>
#include <Vco.h>

TinySound ts;
Vco osc;

void setup() {
  osc.init();
  ts.init();
  osc.setMode(2);
  osc.setXYmode(true);
  osc.setFrequency(0);
  osc.setFrequency2(64);
  osc.setFrequency3(64);
}

void loop() {
  osc.update();
}

ISR(ADC_vect){ // interupt triggered ad completion of ADC counter
  ts.update();
  if(ts.analogRelevant(2)) osc.setFrequency(ts.analogValues[2]);
}

ISR(TIMER1_COMPA_vect){  //audiorate interrupt
  OCR0A = osc.sample;
  OCR0B = osc.sample2;
  osc.render();
}
