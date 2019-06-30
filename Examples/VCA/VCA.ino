#include <TinySound.h>
#include <Vca.h>

TinySound ts;
Vca vca;

void setup() {
  vca.init();
  ts.init();
}

void loop() {}

ISR(ADC_vect){ // interupt triggered ad completion of ADC counter
  ts.update();
  if(ts.analogRelevant(2)) vca.setAmplification(ts.analogValues[2]);
}

ISR(TIMER1_COMPA_vect){  //audiorate interrupt
  int value = vca.render(ts.analogValues[3], 0);
  OCR0A = value;
  OCR0B = value;
}
