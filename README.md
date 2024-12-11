# AlternateIO
Arduino library for efficiently managing physical IO, with debounce.


# Usage
Simply clone or download and extract the zipped library into your Arduino/libraries folder.


**DigitalPin(uint8_t arduino_pin, uint8_t pin_mode = INPUT)**
DigitalPin constructor.

```C++
DigitalPin input(2);
DigitalPin output(13);
```


**void setup(uint8_t pin_mode)**
This should be called in your setup function. It initializes the input/output pin as input or output (default as input).

```C++
input.setup(INPUT_PULLUP);
output.setup(OUTPUT);
```


**uint8_t pin()**
Returns the arduino pin the DigitalPin object is attached to.

```C++
input.pin() == 2
output.pin() == 13
```


**bool read()**
Returns the state of the physical input pin.

```C++
bool inputState = input.read();
```


**void write(bool value)**
Sets the physical output pin either high or low.

```C++
output.write(true); // high
output.write(false); // low
```


**void lo()**
Sets the physical output pin low.

```C++
output.lo();
```


**void hi()**
Sets the physical output pin high.

```C++
output.hi();
```


**void update()**
This should be called in the loop function if physical pin is an input and the following functions are to be used.

```C++
input.update();
```


**bool state()**
Returns the state of the physical input pin at the moment when update() was last called.

```C++
bool inputState = input.state();
```


**bool last_state()**
Returns the state of the physical input pin at the moment when update() was last called (2 update's ago).

```C++
bool lastInputState = input.last_state();
```


**bool change_state()**
Returns true if the state of the physical input pin had changed when update() was last called.

```C++
bool inputChangeState = input.change_state();
```


# Wiring
Please refer to your microcontroller's pin diagram for hardware serial port pin information.
