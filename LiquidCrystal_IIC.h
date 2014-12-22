// ---------------------------------------------------------------------------
// Created/Adapted by Bill Perry 2013-04-01
// Copyright 2012 - Under creative commons license 3.0:
//        Attribution-ShareAlike CC BY-SA
// vi:ts=4
//
// This software is furnished "as is", without technical support, and with no 
// warranty, express or implied, as to its usefulness for any purpose.
//
// Thread Safe: No
// Extendable: Yes
//
// @file LiquidCrystal_IIC.h
// This file implements a basic liquid crystal library that comes as standard
// in the Arduino SDK by using the Wire library.
// 
// @brief 
// This class implements the all methods to command an LCD based
// on the Hitachi HD44780 and compatible chipsets using IIC backpacks
// that use a simple IIC i/o expander chip.
// Currently the PCF8574 or the MCP23008 are supported.
//
// The functionality provided by this class and its base class is identical
// to the original functionality of the Arduino LiquidCrystal library.
//
// LiquidCrystal_IIC constructor can specifiy expander output bit assignments
// or used canned entries
// examples:
// LiquidCrystal_IIC lcd(addr, chiptype,    e,rw,rs,d4,d5,d6,d7,bl, plpol);
// LiquidCrystal_IIC lcd(addr, IIC_BOARD_XXX);
//
// chiptype can also be IIC_UNKNOWN if you want the library to automatically
// detect the chip type.
//
// addr can also be IIC_ADDR_UNKNOWN if you want the library to autolocate
// the io/expander. But this only works if the devcice is the only device
// on the IIC bus.
//
// For backwards compabilty with the LiquidCrystal_I2C class,
// the following constructors are also supported
// LiquidCrystal_IIC lcd(addr, e,rw,rs,d4,d5,d6,d7,bl, plpol);
// LiquidCrystal_IIC lcd(addr, e,rw,rs,d4,d5,d6,d7);
//
// Note that when using the I2C backward compatibilty constuctors, that
// the library will autodetect the chip type.
//
//
// 2013.06.01  bperrybap - original creation
//
// @author Bill Perry - bperrybap@opensource.billsworld.billandterrie.com
// ---------------------------------------------------------------------------
#ifndef LiquidCrystal_IIC_h
#define LiquidCrystal_IIC_h
#include <inttypes.h>
#include <Print.h>

#include "LCD.h"

typedef enum
{
	IIC_UNKNOWN,
	IIC_PCF8574,
	IIC_MCP23008
} iicChipType;


// canned i2c board/backpack parameters
// allows using:
// LiquidCrystal_IIC lcd({i2c_address}, IIC_BOARD_XXX);
// instead of specifying all individual parameters.
// Note: some (the smarter boards) tie the LCD r/w line directly to ground
// Since the library has to drive all 8 output pins, the boards that have
// r/w tied to ground will be assigned an unused output pin for the r/w signal
// which will be set to LOW but ignored by the LCD on those boards.
//									i2cType, en,rw,rs,d4,d5,d6,d7,bl, blpol
#define IIC_BOARD_EXTRAIO		IIC_PCF8574, 6,5,4,0,1,2,3 // ElectroFun default wiring (no i2c backlight control)
#define IIC_BOARD_EXTRAIOnBL 	IIC_PCF8574, 6,5,4,0,1,2,3,7,NEGATIVE // Electrofun using NPN transistor for BL
#define IIC_BOARD_MJKDZ			IIC_PCF8574, 4,5,6,0,1,2,3,7,NEGATIVE // mjkdz backpack
#define IIC_BOARD_LCM1602		IIC_PCF8574, 2,1,0,4,5,6,7,3,NEGATIVE // Robot Arduino LCM1602 backpack
#define IIC_BOARD_YWROBOT		IIC_PCF8574, 2,1,0,4,5,6,7,3,POSITIVE // YwRobot/DFRobot/SainSmart backpack
#define IIC_BOARD_DFROBOT		IIC_PCF8574, 2,1,0,4,5,6,7,3,POSITIVE // YwRobot/DFRobot/SainSmart backpack
#define IIC_BOARD_SAINSMART		IIC_PCF8574, 2,1,0,4,5,6,7,3,POSITIVE // YwRobot/DFRobot/SainSmart backpack
#define IIC_BOARD_ADAFRUIT		IIC_MCP23008,2,0,1,3,4,5,6,7,POSITIVE // Adafruit #292 i2c/SPI backpack in i2c mode (lcd RW grounded)

#define IIC_ADDR_UNKNOWN 0xff // use to auto locate device (only works if only device on bus)


class LiquidCrystal_IIC : public LCD 
{
public:
   
	/*!
	 @method     
	 @abstract   Class constructor. 
	 @discussion Initializes class variables and defines the IIC address of the
	 LCD. The constructor does not initialize the LCD.
	 
	 @param      iic_addr[in] IIC address of the IO expansion module.
	 @param      iic_type[in] IIC chip type used on the i/o expansion module
	 @param      En[in] LCD En (Enable) pin connected to the IO extender module
	 @param      Rw[in] LCD Rw (Read/write) pin connected to the IO extender module
	 @param      Rs[in] LCD Rs (Reset) pin connected to the IO extender module
	 @param      d4[in] LCD data 0 pin map on IO extender module
	 @param      d5[in] LCD data 1 pin map on IO extender module
	 @param      d6[in] LCD data 2 pin map on IO extender module
	 @param      d7[in] LCD data 3 pin map on IO extender module
	 */
	LiquidCrystal_IIC(uint8_t  iic_addr, iicChipType iic_type, uint8_t En, uint8_t Rw,
	                  uint8_t Rs, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7 );
	// Constructor with backlight control
	LiquidCrystal_IIC(uint8_t  iic_addr, iicChipType iic_type, uint8_t En, uint8_t Rw,
	                  uint8_t Rs, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7,
	                  uint8_t backlightPin, __typeof__(POSITIVE) pol = POSITIVE);

	// Constructors that automatically determine chiptype
	// provides backward compatibility with LiquidCrystal_I2C constructors
	LiquidCrystal_IIC(uint8_t  iic_addr, uint8_t En, uint8_t Rw,
	                  uint8_t Rs, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7 );

	// uses __typeof__ as polarity enum as name changed and this allows it to work
	// regardless of the actual name
	LiquidCrystal_IIC(uint8_t  iic_addr, uint8_t En, uint8_t Rw,
	                  uint8_t Rs, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7,
	                  uint8_t backlightPin, __typeof__(POSITIVE) pol = POSITIVE);

	/*!
	 @function
	 @abstract   LCD initialization and associated HW.
	 @discussion Initializes the LCD to a given size (col, row). This methods
	 initializes the LCD, therefore, it MUST be called prior to using any other
	 method from this class or parent class.
	 
	 The begin method can be overloaded if necessary to initialize any HW that 
	 is implemented by a library and can't be done during construction, here
	 we use the Wire class.
	 
	 @param      cols[in] the number of columns that the display has
	 @param      rows[in] the number of rows that the display has
	 @param      charsize[in] size of the characters of the LCD: LCD_5x8DOTS or
	 LCD_5x10DOTS.
	 */
	virtual void begin(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS);   
	
	/*!
	 @function
	 @abstract   Send a particular value to the LCD.
	 @discussion Sends a particular value to the LCD for writing to the LCD or
	 as an LCD command.
	 
	 Users should never call this method.
	 
	 @param      value[in] Value to send to the LCD.
	 @param      mode[in] DATA - write to the LCD CGRAM, COMMAND - write a 
	 command to the LCD.
	 */
	virtual void send(uint8_t value, uint8_t mode);
	
	/*!
	 @function
	 @abstract   Sets the pin to control the backlight.
	 @discussion Sets the pin in the device to control the backlight. This device
	 doesn't support dimming backlight capability.
	 
	 @param      0: backlight off, 1..255: backlight on.
	 */
	void setBacklightPin ( uint8_t value, __typeof__(POSITIVE) pol = POSITIVE );
	
	/*!
	 @function
	 @abstract   Switch-on/off the LCD backlight.
	 @discussion Switch-on/off the LCD backlight.
	 In order for this to work, the full constructor that includes backlight
	 control must be used (recommended). Alternatively,
	 the setBacklightPin has to be called before setting the backlight for
	 this method to work. @see setBacklightPin.
	 
	 @param      value: backlight mode (BACKLIGHT_ON|BACKLIGHT_OFF)
	 */
	void setBacklight ( uint8_t value );
	
private:
	
	/*!
	 @method     
	 @abstract   Initializes the LCD class
	 @discussion Initializes the LCD class and IO expansion module.
	 */
	int  init();
	
	/*!
	 @function
	 @abstract   Initialises class private variables
	 @discussion This is the class single point for initialising private variables.
	 
	 @param      iic_addr[in] IIC address of the IO expansion module.
	 @param      iic_type[in] IIC IO expansion module type.
	 @param      En[in] LCD En (Enable) pin connected to the IO extender module
	 @param      Rw[in] LCD Rw (Read/write) pin connected to the IO extender module
	 @param      Rs[in] LCD Rs (Reset) pin connected to the IO extender module
	 @param      d4[in] LCD data 0 pin map on IO extender module
	 @param      d5[in] LCD data 1 pin map on IO extender module
	 @param      d6[in] LCD data 2 pin map on IO extender module
	 @param      d7[in] LCD data 3 pin map on IO extender module
	 */
	void config (uint8_t lcd_Addr, iicChipType iic_type, uint8_t En, uint8_t Rw, uint8_t Rs, 
	             uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7 );
	
	/*!
	 @function     
	 @abstract   Determines IIC i/o expander type
	 @discussion Probes chip at adddress to determine expander chip type
	 @param      iic_addr[in] IIC address of the IO expander chip
	 */
	iicChipType IdentifyIOexp(uint8_t iic_addr);

	/*!
	 @function     
	 @abstract   Locate address of an i2c device
	 @discussion Probes i2c bus to locate first i2c device
	 @param      (none)
	 */
	uint8_t LocateDevice(void);


	/*!
	 @method     
	 @abstract   Writes an 4 bit value to the LCD.
	 @discussion Writes 4 bits (the least significant) to the LCD control data lines.
	 @param      value[in] Value to write to the LCD
	 @param      mode[in]  Value to distinguish between command and data.
	 COMMAND == command, DATA == data.
	 */
	void write4bits(uint8_t value, uint8_t mode);
	
	/*!
	 @method     
	 @abstract   Pulse the LCD enable line (En).
	 @discussion Sends a pulse of 1 uS to the Enable pin to execute an command
	 or write operation.
	 */
	void pulseEnable(uint8_t);
	
	
	uint8_t _Addr;             // IIC Address of the IO expander
	uint8_t _iicType;          // IIC chip type used on the IO expander
	uint8_t _backlightPinMask; // Backlight IO pin mask
	uint8_t _backlightStsMask; // Backlight status mask
	uint8_t _En;               // LCD expander IO pin mask for enable pin
	uint8_t _Rw;               // LCD expander IO pin mask for R/W pin
	uint8_t _Rs;               // LCD expander IO pin mask for Register Select pin
	uint8_t _data_pins[4];     // LCD expander IO pin masks for data lines
	
};

#endif
