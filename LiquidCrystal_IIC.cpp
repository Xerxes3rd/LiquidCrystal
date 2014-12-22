// ---------------------------------------------------------------------------
// Created/Adapted by Bill Perry 2013-04-01
// Copyright 2013 - Under creative commons license 3.0:
//        Attribution-ShareAlike CC BY-SA
// vi:ts=4
//
// This software is furnished "as is", without technical support, and with no 
// warranty, express or implied, as to its usefulness for any purpose.
//
// Thread Safe: No
// Extendable: Yes
//
// @file LiquidCrystal_IIC.c
// This file implements a basic liquid crystal library that comes as standard
// in the Arduino SDK but using an IIC IO extension board.
// 
// @brief 
// This class implements the all methods to command an LCD based
// on the Hitachi HD44780 and compatible chipsets using IIC extension
// backpacks that use a simple IIC i/o expander chip.
//
// The functionality provided by this class and its base class is identical
// to the original functionality of the Arduino LiquidCrystal library.
//
//
// 2013.06.01  bperrybap - original creation
//
// @author Bill Perry - bperrybap@opensource.billsworld.billandterrie.com
// ---------------------------------------------------------------------------
#if (ARDUINO <  100)
#include <WProgram.h>
#else
#include <Arduino.h>
#endif

#include <inttypes.h>

#include "LiquidCrystal_IIC.h"

// include the Wire.h header
// The reference is relative to the "core" directory which is always on
// the include path to ensure that the header can always be found 
// regardless of whether this library is installed in either
// the system libary area (on top of the original) or in the users
// sketchbook/libraries directory
#include <../../../../libraries/Wire/Wire.h>

// hide ugly 1.x Wire API change nonsense
#if ARDUINO < 100
#define write(_data) send(_data)
#define read() receive()
#endif


// CONSTRUCTORS
// ---------------------------------------------------------------------------
/*
 * Explicit io expander type constructors
 */
LiquidCrystal_IIC::LiquidCrystal_IIC(uint8_t iic_addr, iicChipType iic_type, uint8_t En, uint8_t Rw,
                                     uint8_t Rs, uint8_t d4, uint8_t d5,
                                     uint8_t d6, uint8_t d7 )
{
   config(iic_addr, iic_type, En, Rw, Rs, d4, d5, d6, d7);
}

LiquidCrystal_IIC::LiquidCrystal_IIC(uint8_t iic_addr, iicChipType iic_type, uint8_t En, uint8_t Rw,
                                     uint8_t Rs, uint8_t d4, uint8_t d5,
                                     uint8_t d6, uint8_t d7,
                                     uint8_t backlightPin, __typeof__(POSITIVE) pol)
{
   config(iic_addr, iic_type, En, Rw, Rs, d4, d5, d6, d7);
   setBacklightPin(backlightPin, pol);
}

/*
 * "Automatic"/"Auto-Detect" i/o expander type constructors
 * You can't use the i2c interface in the constructor because something
 * isn't initalized yet. (maybe interrupts??)
 * These are backward compatible with the LiquidCrystal_I2C constructors
 */
LiquidCrystal_IIC::LiquidCrystal_IIC(uint8_t iic_addr, uint8_t En, uint8_t Rw,
                     uint8_t Rs, uint8_t d4, uint8_t d5,
                     uint8_t d6, uint8_t d7 )
{
   config(iic_addr, IIC_UNKNOWN, En, Rw, Rs, d4, d5, d6, d7);
}

LiquidCrystal_IIC::LiquidCrystal_IIC(uint8_t iic_addr, uint8_t En, uint8_t Rw,
                      uint8_t Rs, uint8_t d4, uint8_t d5,
                      uint8_t d6, uint8_t d7,
                      uint8_t backlightPin, __typeof__(POSITIVE) pol)
{
   config(iic_addr, IIC_UNKNOWN, En, Rw, Rs, d4, d5, d6, d7);
   setBacklightPin(backlightPin, pol);
}



// PUBLIC METHODS
// ---------------------------------------------------------------------------

//
// begin
void LiquidCrystal_IIC::begin(uint8_t cols, uint8_t lines, uint8_t dotsize) 
{
   
	init();     // Initialise the IIC expander interface
	LCD::begin ( cols, lines, dotsize );   
}


// User commands - users can expand this section
//----------------------------------------------------------------------------
// Turn the (optional) backlight off/on

//
// setBacklightPin
void LiquidCrystal_IIC::setBacklightPin ( uint8_t value, __typeof__(POSITIVE) pol)
{
	_backlightPinMask = ( 1 << value );
	_polarity = pol;

// I'm thinking setBacklightPin() should only setup the pin information but
// not actually talk to hardware. The main reason being that it is called from
// the constructors which are called before begin() which means that the i2c interface
// is not initialized. While it seems to not create an issue right now,
// I'm concerned that this might cause the hardware to hang becuase not only is the
// i2c not initiliazed at this point but some parts of Arduino are not fully up either
// since constructors are called so early.
//
// LCD:begin() will be calling setBacklight() to set the default backlight state 
// anyway so touching the hardware in
// setBacklightPin() should not be necessary.
// --- bap
}

//
// setBacklight
void LiquidCrystal_IIC::setBacklight( uint8_t value ) 
{
	// Check if backlight is available
	// ----------------------------------------------------
	if ( _backlightPinMask != 0x0 )
	{
		// Check for polarity to configure status mask accordingly
		// ----------------------------------------------------------
		if  (((_polarity == POSITIVE) && (value > 0)) || 
				((_polarity == NEGATIVE ) && ( value == 0 )))
		{
			_backlightStsMask = _backlightPinMask;
		}
		else 
		{
			_backlightStsMask = 0;
		}
		Wire.beginTransmission(_Addr);
		if(_iicType == IIC_MCP23008)
		{
			Wire.write( 0x0A); // point to OLAT
		}
		Wire.write( _backlightStsMask );
		Wire.endTransmission();
	}
}


// PRIVATE METHODS
// ---------------------------------------------------------------------------

//
// init
// Returns non zero if initialization failed.
int LiquidCrystal_IIC::init()
{
int status = 0;

	/*
	 * First, initialize the i2c (Wire) library.
	 * This really shouldn't be here 
	 * because Wire.begin() should only be called once, but
	 * unfortunately, there is no way to know if anybody
	 * else has called this.
	 * I believe that it is unreasonable to require the the user
	 * sketch code to do it, because all that should change between
	 * interfaces should be the constructor
	 * So we go ahead and call it here.
	 */
	Wire.begin();

	if(_Addr == IIC_ADDR_UNKNOWN) // go locate device
		_Addr = LocateDevice();

	if(_Addr == IIC_ADDR_UNKNOWN) // if we couldn't locate it, return error
		return(-1);

	if(_iicType == IIC_UNKNOWN) // figure out which chip if we weren't told
		_iicType = IdentifyIOexp(_Addr);

	if(_iicType == IIC_UNKNOWN) // if we coudn't figure it out, return error
		return(-1);
  
	// initialize the backpack IO expander
	// and display functions.
	// ------------------------------------------------------------------------
	Wire.begin(_Addr);
	Wire.beginTransmission(_Addr);

	if(_iicType == IIC_MCP23008)
	{
		/*
		 * First make sure to put chip into BYTE mode
		 * BYTE mode is used to make a MCP23008 work more like PCF8574
		 * In BYTE mode the address register does not increment so that
		 * once you point it to OLAT you can write to it over and over again
		 * within the same i2c connection by simply sending more bytes.
		 * This is necessary as the code uses back to back writes to perform
		 * the nibble updates as well as the toggling the enable signal.
		 * This methodology offers significant performance gains.
		 */
		Wire.write(5);	// point to IOCON
		Wire.write(0x20);// disable sequential mode (enables BYTE mode)
		Wire.endTransmission();

		/*
		 * Now set up output port
		 */
		Wire.beginTransmission(_Addr);
		Wire.write(0); // point to IODIR
		Wire.write(0); // all pins output
		Wire.endTransmission();
	
		/*
		 * point chip to OLAT
		 */
		Wire.beginTransmission(_Addr);
		Wire.write(0x0A); // point to OLAT
		
	}
	Wire.write(0);  // Set the entire output port to LOW
	status = Wire.endTransmission();
	_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
   return ( status );
}

//
// config
void LiquidCrystal_IIC::config ( uint8_t iic_Addr, iicChipType iic_type, uint8_t En, uint8_t Rw, uint8_t Rs, 
                                uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7 )
{
	// Save away config data into lcd object
	_iicType = iic_type;
	_Addr = iic_Addr;
   
	_backlightPinMask = 0;
	_backlightStsMask = 0;
	_polarity = POSITIVE;
   
	_En = ( 1 << En );
	_Rw = ( 1 << Rw );
	_Rs = ( 1 << Rs );
   
	// Initialise pin mapping
	_data_pins[0] = ( 1 << d4 );
	_data_pins[1] = ( 1 << d5 );
	_data_pins[2] = ( 1 << d6 );
	_data_pins[3] = ( 1 << d7 );   
}

/*
 * Locate I2C device
 * NOTE: While this does attempt to verify
 * that the device found is a known type,
 * it safest to only search the bus when
 * the device is the only device on the bus.
 */


uint8_t LiquidCrystal_IIC::LocateDevice(void)
{
uint8_t error, address;
uint8_t rval = IIC_ADDR_UNKNOWN;
 
	for(address = 0; address <= 127; address++ )
	{
		Wire.beginTransmission(address);
		error = Wire.endTransmission();
		if (error == 0) // if no error we found something
		{
			if(IdentifyIOexp(address) == IIC_UNKNOWN) // if we can't identify it, keep looking
				continue;
			rval = address;
			break;
		}
	}
	return(rval);
}

/*
 * Identify I2C device type.
 * Currently PCF8574 or MCP23008
 */

iicChipType LiquidCrystal_IIC::IdentifyIOexp(uint8_t address)
{
uint8_t data;
iicChipType chiptype;

	/*
	 * Identify PCF8574 vs MCP23008
	 * It appears that on a PCF8574 that 1 bits turn on pullups and make the pin an input.
	 * and 0 bits set the output pin to 0.
	 * And a read always reads the port pins.
	 *
	 * Strategy:
	 *	- Try to Write 0xff to MCP23008 IODIR register (location 0)
	 *  - Point MCP23008 to IODIR register (location 0)
	 *	- Read 1 byte
	 *
	 * On a MCP23008 the read will return 0xff because it will read the IODIR we just wrote
	 * On a PCF8574 we should read a 0 since we last wrote zeros to all the PORT bits
	 */

	/*
	 * First try to write 0xff to MCP23008 IODIR
	 * On a PCF8574 this will end up writing 0 and then ff to output port
	 */
	Wire.beginTransmission(address);
	Wire.write((uint8_t) 0);	// try to point to MCP23008 IODR
	Wire.write((uint8_t) 0xff);	// try to write to MCP23008 IODR
	Wire.endTransmission();

	/*
	 * Now try to point MCP23008 to IODIR for read
	 * On a PCF8574 this will end up writing a 0 to the output port
	 */

	Wire.beginTransmission(address);
	Wire.write((uint8_t) 0);	// try to point to MCP23008 IODR
	Wire.endTransmission();

	/*
	 * Now read a byte
	 * On a MCP23008 we should read the 0xff we wrote to IODIR
	 * On a PCF8574 we should read 0 since the output port was set to 0
	 */
	Wire.requestFrom((int)address, 1);
	data = Wire.read();

	if(data == 0xff)
	{
		chiptype = IIC_MCP23008;
	}
	else if (data == 0x00)
	{
		chiptype = IIC_PCF8574;
	}
	else
	{
		chiptype = IIC_UNKNOWN;
	}
	return(chiptype);
}


// low level data pushing commands
//----------------------------------------------------------------------------

//
// send - write either command or data
void LiquidCrystal_IIC::send(uint8_t value, uint8_t mode) 
{
	if(_Addr == IIC_ADDR_UNKNOWN)
		return;

	// No need to use the delay routines since the time taken to write takes
	// longer that what is needed both for toggling and enable pin an to execute
	// the command.
   
	if ( mode == FOUR_BITS )
	{
		Wire.beginTransmission(_Addr);
		if(_iicType == IIC_MCP23008)
		{
			/*
			 * point chip to OLAT
			 */
			Wire.write(0x0A); // point to OLAT
		}
		write4bits( (value & 0x0F), COMMAND);
		Wire.endTransmission();
	}
	else 
	{
		if(mode == DATA)
		{
			/*
			 * toss carriage returns and linefeeds so niave users that
			 * use lcd.println() don't get garbage characters.
			 */
			if(value == '\r' || value == '\n')
				return; // toss cariage returns and linefeeds
		}
		// grab i2c bus
		Wire.beginTransmission(_Addr);
		if(_iicType == IIC_MCP23008)
		{
			/*
			 * point chip to OLAT
			 */
			Wire.write(0x0A); // point to OLAT
		}
		// send both nibbles in same i2c connection
		write4bits( (value >> 4), mode );
		write4bits( (value & 0x0F), mode);
		Wire.endTransmission();
	}
}

//
// write4bits
void LiquidCrystal_IIC::write4bits ( uint8_t value, uint8_t mode ) 
{
uint8_t pinMapValue = 0;
   
	// Map the value to LCD pin mapping
	// --------------------------------
	for ( uint8_t i = 0; i < 4; i++ )
	{
		if ( ( value & 0x1 ) == 1 )
		{
			pinMapValue |= _data_pins[i];
		}
		value = ( value >> 1 );
	}
   
	// Is it a command or data
	// -----------------------
	if ( mode == DATA )
	{
		mode = _Rs;
	}
   
	pinMapValue |= mode | _backlightStsMask;
	pulseEnable ( pinMapValue );
}

//
// pulseEnable
void LiquidCrystal_IIC::pulseEnable (uint8_t data)
{
	Wire.write(data |_En);   // En HIGH
	Wire.write(data & ~_En); // En LOW
}
