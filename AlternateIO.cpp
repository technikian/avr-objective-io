//
// Alternative Arduino IO manager, includes debounce class
//

// ----------------------------------------------------------------------------
// Defines

#define ALTERNATEIO_CPP

// ----------------------------------------------------------------------------
// Includes

#include "AlternateIO.h"
#include "pins_arduino.h"
//#include "Time.h"

// ----------------------------------------------------------------------------



// ----------------------------------------------------------------------------
// Arduino Functions

inline uint8_t arduino_pin_to_port(uint8_t arduino_pin) FORCE_INLINE;
inline uint8_t arduino_pin_to_port(uint8_t arduino_pin)
{
	return pgm_read_byte_near(digital_pin_to_port_PGM + arduino_pin);
}

inline uint8_t arduino_pin_to_mask(uint8_t arduino_pin) FORCE_INLINE;
inline uint8_t arduino_pin_to_mask(uint8_t arduino_pin)
{
	return pgm_read_byte_near(digital_pin_to_bit_mask_PGM + arduino_pin);
}

inline uint8_t* port_register_input(uint8_t port) FORCE_INLINE;
inline uint8_t* port_register_input(uint8_t port)
{
	return pgm_read_byte_near(port_to_input_PGM + port);
}

inline uint8_t* port_register_output(uint8_t port) FORCE_INLINE;
inline uint8_t* port_register_output(uint8_t port)
{
	return pgm_read_byte_near(port_to_output_PGM + port);
}

// ----------------------------------------------------------------------------



// ----------------------------------------------------------------------------
// ATMega328 Functions - redefined as part of the _mcu_ class

//#ifdef _atmega328_

static inline uint8_t _atmega328_::_read_pin(uint8_t arduino_pin)
{
	uint8_t port = arduino_pin_to_port(arduino_pin);
	uint8_t mask = arduino_pin_to_mask(arduino_pin);
	if(*port_register_input(port) & mask) return HIGH;
	return LOW; // returning a bool isn't any faster
}

static inline void _atmega328_::_write_pin_lo(uint8_t arduino_pin)
{
	if(arduino_pin >= NUMBER_OF_IO_PINS) return;
	uint8_t port = arduino_pin_to_port(arduino_pin);
	uint8_t mask = arduino_pin_to_mask(arduino_pin);

	// no interupts while we do this
	uint8_t oldSREG = SREG;
	cli();

	*port_register_output(port) &= ~mask;

	// interupts back on if they were before
	SREG = oldSREG;
}

static inline void _atmega328_::_write_pin_hi(uint8_t arduino_pin)
{
	if(arduino_pin >= NUMBER_OF_IO_PINS) return;
	uint8_t port = arduino_pin_to_port(arduino_pin);
	uint8_t mask = arduino_pin_to_mask(arduino_pin);

	// no interupts while we do this
	uint8_t oldSREG = SREG;
	cli();

	*port_register_output(port) |= mask;

	// interupts back on if they were before
	SREG = oldSREG;
}

//#endif

// ----------------------------------------------------------------------------



// ----------------------------------------------------------------------------
// _mcu_ Class Functions

static uint8_t _mcu_::read_pin(uint8_t arduino_pin)
{
	return _read_pin(arduino_pin);
}

static void _mcu_::write_pin_lo(uint8_t arduino_pin)
{
	_write_pin_lo(arduino_pin);
}

static void _mcu_::write_pin_hi(uint8_t arduino_pin)
{
	_write_pin_hi(arduino_pin);
}

// ----------------------------------------------------------------------------



// ----------------------------------------------------------------------------
// Main Class

DigitalPin::DigitalPin(uint8_t arduino_pin, uint8_t pin_mode = ALTIO_DEFAULT_PIN_MODE)
{
	_status = arduino_pin;
	setup(pin_mode);
}

void DigitalPin::setup(uint8_t pin_mode)
{
	pinMode(_status & STATUS_MSK_PIN, pin_mode);
	if(pin_mode != OUTPUT)
	{ // input
		uint8_t pin_state = _read();
		if(pin_state) _status |= (STATUS_MSK_STATE | STATUS_MSK_LASTSTATE);
		else _status &= ~(STATUS_MSK_STATE | STATUS_MSK_LASTSTATE);
	}
	else
	{ // output
		_lo(); // start from a known state
		_set_state(0);
	}
}

uint8_t DigitalPin::pin()
{
	return (_status & STATUS_MSK_PIN);
}

// privates

inline void DigitalPin::_set_state(uint8_t new_state)
{
	// new (current) state
	if(new_state) _status |= STATUS_MSK_STATE;
	else _status &= ~STATUS_MSK_STATE;
}

uint8_t DigitalPin::_read()
{
	return _mcu_::_read_pin(_status & STATUS_MSK_PIN);
}

void DigitalPin::_lo()
{
	_mcu_::_write_pin_lo(_status & STATUS_MSK_PIN);
}

void DigitalPin::_hi()
{
	_mcu_::_write_pin_hi(_status & STATUS_MSK_PIN);
}

uint8_t DigitalPin::_state()
{
	if( _status & STATUS_MSK_STATE ) return HIGH;
	return LOW;
}

uint8_t DigitalPin::_last_state()
{
	if( _status & STATUS_MSK_LASTSTATE ) return HIGH;
	return LOW;
}

bool DigitalPin::change_state()
{
	return ( _state() ) ^ ( _last_state() );
}

void DigitalPin::update()
{
	// store last state
	if( _status & STATUS_MSK_STATE ) _status |= (_status & STATUS_MSK_STATE) << 1;
	else _status &= ~STATUS_MSK_LASTSTATE;
	// store current state
	if( _read() ) _status |= STATUS_MSK_STATE;
	else _status &= ~STATUS_MSK_STATE;
}

// ----------------------------------------------------------------------------



// ----------------------------------------------------------------------------
// Debounce Class

Debouncer::Debouncer(DigitalPin& digital_io, uint16_t debounce_len = ALTIO_DEFAULT_DEBOUNCE)
{
	_digital_io = &digital_io;
	setup(debounce_len);
}

inline void Debouncer::_toggle_timer()
{
	if(_t_start) // if there is a timer already running...
	{ // stop timer, failed to saturate debounce time
		delete _t_start;
		_t_start = 0;
	}
	else
	{ // otherwise start the debounce timer
		_t_start = new uint32_t;
		*_t_start = millis();
	}
}

void Debouncer::setup(uint16_t debounce_len)
{
	_t_debounce = debounce_len;
	if( _digital_io->_state() ) _status = ~0;
	else _status = 0;
}

uint8_t Debouncer::_state(uint8_t updates_elapsed)
{
	if( _status & (1 << updates_elapsed) ) return HIGH;
	return LOW;
}

uint8_t Debouncer::_state()
{
	if( _status & (1 << 0) ) return HIGH;
	return LOW;
}

uint8_t Debouncer::_last_state()
{
	if( _status & (1 << 1) ) return HIGH;
	return LOW;
}

bool Debouncer::change_state()
{
	return ( _state() ) ^ ( _last_state() );
}

void Debouncer::update()
{
	if(_digital_io->change_state()) _toggle_timer();
	_status = (_status << 1);
	if(_t_start)
	{ // if timer is running
		_status |= ! _digital_io->_state();
		if(millis() - *_t_start >= _t_debounce)
		{ // if running timer has saturated debounce length
			delete _t_start;
			_t_start = 0;
		}
	}
	else
	{ // if timer is not running
		_status |= _digital_io->_state();
	}
}

// --------------------------------------------------------------------------*/
