//
// Version 1.0.0
// Alternative Arduino IO manager, includes debounce class
// "Technician"
//

// ----------------------------------------------------------------------------
// Header Guard
#ifndef ALTERNATEIO_H
#define ALTERNATEIO_H

// ----------------------------------------------------------------------------
// Includes

#include "Arduino.h"

// ----------------------------------------------------------------------------



// ----------------------------------------------------------------------------
// Setup Defines

// #define ALTIO_INVERT_INPUT           // - invert inputs globally
// #define ALTIO_INVERT_OUTPUT          // - invert outputs globally
// #define ALTIO_RETAIN_OUTPUT_STATE    // - when writing output store the written state to be retrieved by state() and extending functions

#define ALTIO_DEFAULT_DEBOUNCE 100
#define ALTIO_DEFAULT_PIN_MODE INPUT

#ifndef ALTIO_INVERT_INPUT
#define _ALTIO_INVERT_INPUT
#else
#define _ALTIO_INVERT_INPUT !
#endif

#ifndef ALTIO_INVERT_OUTPUT
#define _ALTIO_INVERT_OUTPUT
#else
#define _ALTIO_INVERT_OUTPUT !
#endif

// ----------------------------------------------------------------------------



// ----------------------------------------------------------------------------
// Defines

// using ATMega328
//#define _mcu_ _atmega328_
#define _atmega328_ _mcu_ // either one of these works

#define NUMBER_OF_IO_PINS 20

#define FORCE_INLINE __attribute__((always_inline))

#define STATUS_BIT_PIN 0
#define STATUS_MSK_PIN 63
#define STATUS_BIT_STATE 6
#define STATUS_MSK_STATE 64
#define STATUS_BIT_LASTSTATE 7
#define STATUS_MSK_LASTSTATE 128

// ----------------------------------------------------------------------------



// ----------------------------------------------------------------------------
// MCU Class

class _mcu_
{
protected:
	static inline uint8_t _read_pin(uint8_t arduino_pin) FORCE_INLINE ;
	static inline void _write_pin_lo(uint8_t arduino_pin) FORCE_INLINE ;
	static inline void _write_pin_hi(uint8_t arduino_pin) FORCE_INLINE ;
public:
	static uint8_t read_pin(uint8_t arduino_pin) ;
	static void write_pin_lo(uint8_t arduino_pin) ;
	static void write_pin_hi(uint8_t arduino_pin) ;
};

// ----------------------------------------------------------------------------



// ----------------------------------------------------------------------------
// Main Class

class DigitalPin : public _mcu_
{
private:
	// bit0 ... bit5 - pin
	// bit6 - state
	// bit7 - last state
	uint8_t _status;

	inline void _set_state(uint8_t new_state);
	uint8_t _read();
	void _lo();
	void _hi();

public:
	uint8_t _state();
	uint8_t _last_state();

public:
	DigitalPin(uint8_t arduino_pin, uint8_t pin_mode = ALTIO_DEFAULT_PIN_MODE);

	void setup(uint8_t pin_mode);
	uint8_t pin();

	inline uint8_t read() FORCE_INLINE ;
	inline void lo() FORCE_INLINE ;
	inline void hi() FORCE_INLINE ;
	inline void write(uint8_t value) FORCE_INLINE ;

	// can not be used on io pins operating as outputs, with the execption of state() which
	// may be used on output pins when ALTIO_RETAIN_OUTPUT_STATE is defined
	inline uint8_t state() FORCE_INLINE ;
	inline uint8_t last_state() FORCE_INLINE ;
	bool change_state() FORCE_INLINE ;
	void update() FORCE_INLINE ;
};

// ----------------------------------------------------------------------------



// ----------------------------------------------------------------------------
// Debounce Class

class Debouncer
{
private:
	DigitalPin* _digital_io ;
	uint8_t _status ;
	uint16_t _t_debounce ;
	uint32_t* _t_start = 0 ;

	inline void _toggle_timer() ;
	uint8_t _state(uint8_t updates_elapsed) ;
	uint8_t _state() ;
	uint8_t _last_state() ;

public:
	Debouncer(DigitalPin& digital_io, uint16_t debounce_len = ALTIO_DEFAULT_DEBOUNCE);

	void setup(uint16_t debounce_len) ;
	inline uint8_t  state(uint8_t updates_elapsed)  FORCE_INLINE ;
	inline uint8_t  state() FORCE_INLINE ;
	inline uint8_t  last_state() FORCE_INLINE ;
	bool change_state() ;
	void update() ;
};

// ----------------------------------------------------------------------------



// ----------------------------------------------------------------------------
// Main Class Inlines

#ifndef ALTERNATEIO_CPP

inline uint8_t DigitalPin::read()
{
	return _ALTIO_INVERT_INPUT _read();
}

inline void DigitalPin::lo()
{
	#ifndef ALTIO_INVERT_OUTPUT
	_lo();
	#else
	_hi();
	#endif
	#ifdef ALTIO_RETAIN_OUTPUT_STATE
	_set_state(_ALTIO_INVERT_INPUT false);  // read functions only invert input
	#endif
}

inline void DigitalPin::hi()
{
	#ifndef ALTIO_INVERT_OUTPUT
	_hi();
	#else
	_lo();
	#endif
	#ifdef ALTIO_RETAIN_OUTPUT_STATE
	_set_state(_ALTIO_INVERT_INPUT true);
	#endif
}

inline void DigitalPin::write(uint8_t value)
{
	if(_ALTIO_INVERT_OUTPUT value) _hi();
	else _lo();
	#ifdef ALTIO_RETAIN_OUTPUT_STATE
	_set_state(_ALTIO_INVERT_INPUT value);
	#endif
}

inline uint8_t DigitalPin::state()
{
	return _ALTIO_INVERT_INPUT _state();
}

inline uint8_t DigitalPin::last_state()
{
	return _ALTIO_INVERT_INPUT _last_state();
}

#endif

// ----------------------------------------------------------------------------



// ----------------------------------------------------------------------------
// Debounce Class Inlines

#ifndef ALTERNATEIO_CPP

inline uint8_t Debouncer::state(uint8_t updates_elapsed)
{
	return _ALTIO_INVERT_INPUT _state(updates_elapsed);
}

inline uint8_t Debouncer::state()
{
	return _ALTIO_INVERT_INPUT _state();
}

inline uint8_t Debouncer::last_state()
{
	return _ALTIO_INVERT_INPUT _last_state();
}

#endif

// --------------------------------------------------------------------------*/
//#include "./AlternateIO.cpp"
#endif
