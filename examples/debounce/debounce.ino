

#define ALTIO_INVERT_INPUT
//#define ALTIO_INVERT_OUTPUT
#define ARD_ALTIO_RETAIN_OUTPUT_STATE
#include "AlternateIO.h"


DigitalPin out(13);
DigitalPin in(2);
Debouncer debounce(in, 500);


uint32_t initial;
uint32_t final;


void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  Serial.println("Serial Begin...");

  in.setup(INPUT_PULLUP);
  debounce.setup(500); // this is optional if passed to constructor
  out.setup(OUTPUT);
}


void loop() {
  // put your main code here, to run repeatedly:

  in.update();
  debounce.update();

  // output and debounce
  out.write(debounce.state());

  // input change state (not debounced)
  if(in.change_state()) Serial.println("change state");

/*
  // output test
  delay(2000);
  out.write(HIGH);
  Serial.print("high ");
  Serial.println(out.state());
  delay(2000);
  out.write(LOW);
  Serial.print("low ");
  Serial.println(out.state());
*/

/*
  // speed test
  delay(4000);
  initial = micros();
  for(int i=0; i<50; i++) digitalRead(2);
  final = micros();
  Serial.println(final - initial);
  Serial.flush();

  delay(100);
  initial = micros();
  for(int i=0; i<50; i++) in.read();
  final = micros();
  Serial.println(final - initial);
  Serial.flush();
*/

}


