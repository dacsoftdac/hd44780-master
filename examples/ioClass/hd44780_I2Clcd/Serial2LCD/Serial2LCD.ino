// vi:ts=4
// ----------------------------------------------------------------------------
// Serial2LCD - simple demonstration printing characters from serial port
// Created by Bill Perry 2020-06-28
// bperrybap@opensource.billsworld.billandterrie.com
//
// This example code is unlicensed and is released into the public domain
// ----------------------------------------------------------------------------
//
// This sketch is for LCD modules that have a native I2C interface such as
// PCF2119x, PCF2116, or certain RayStar LCDs rather than those LCD modules that
// use an i/o expander chip based based backpack.
//
// Sketch demonstrates hd44780 how to read a message of characters from 
// serial port and display it on the LCD.
// It takes advantage of the hd44780 library automatic line
// wrapping capability.
// See the LineWrap sketch for details about line wrapping.
// Characters are written to the LCD as they come in.
// The LCD is cleared when a new message starts
// <CR> or <LF> are used to mark the end of a message.
//
// NOTE:
//	These devices usually need external pullups as they typically are not on
//	the module.
// WARNING:
//	Use caution when using 3v only processors like arm and ESP8266 processors
//	when interfacing with 5v modules as not doing proper level shifting or
//	incorrectly hooking things up can damage the processor.
// 
// Configure LCD_COLS, LCD_ROWS and BAUDRATE if desired/needed
// Expected behavior of the sketch:
// - characters received from serial port are displayed on LCD
// - CR and LF mark end of message (will clear lcd when next message starts)
//
// If initialization of the LCD fails and the arduino supports a built in LED,
// the sketch will simply blink the built in LED with the initalization error
// code.
//
// Some 16x1 displays are actually a 8x2 display that have both lines on
// a single line on the display.
// If you have one of these displays, simply set the geometry to 8x2 instead
// of 16x1. 

// include the needed headers.
#include <Wire.h>
#include <hd44780.h>						// main hd44780 header
#include <hd44780ioClass/hd44780_I2Clcd.h>	// i2c LCD i/o class header

// Note, i2c address can be specified or automatically located
// If you wish to use a specific address comment out this constructor
// and use the constructor below that specifies the address

// declare the lcd object for auto i2c address location
hd44780_I2Clcd lcd;

//
// enter address of LCD.
// Addresses seen so far include:
// - 0x3a, 0x3b (PCF2119x)
// - 0x3c (unknwon chip)
// - 0x3d (unknwon chip)
// - 0x3e (unknwon chip)
// - 0x3f (unknwon chip)

// declare i2c address and constructor for specified i2c address
//const int i2c_addr = 0x3e;
//hd44780_I2Clcd lcd(i2c_addr); // use device at this address

// LCD geometry
// while 16x2 will work on most displays even if the geometry is different,
// for actual wrap testing of a particular LCD it is best to use the correct
// geometry.
const int LCD_COLS = 16;
const int LCD_ROWS = 2;

const uint32_t BAUDRATE = 9600;

void setup()
{
int status;

	// initalize Serial port
	Serial.begin(BAUDRATE);

	// initialize LCD with number of columns and rows: 
	// hd44780 returns a status from begin() that can be used
	// to determine if initalization failed.
	// the actual status codes are defined in <hd44780.h>
	status = lcd.begin(LCD_COLS, LCD_ROWS);
	if(status) // non zero status means it was unsuccesful
	{
		// begin() failed

		Serial.print("LCD initalization failed: ");
		Serial.println(status);

		// blink error code using the onboard LED if possible
		hd44780::fatalError(status); // does not return
	}

	// turn on automatic line wrapping
	// which automatically wraps lines to the next lower line and wraps back
	// to the top when at the bottom line
	// NOTE: 
	// noLineWrap() can be used to disable automatic line wrapping.
	// _write() can be called instead of write() to send data bytes
	// to the display bypassing any special character or line wrap processing.
	lcd.lineWrap();

	// auto format the display depending on geometry
	if(LCD_ROWS < 2 || LCD_COLS < 16) // for 8x1, 8x2, 16x1
	{
		lcd.print("Ser2LCD "); 
		if(LCD_COLS * LCD_ROWS > 8)	// see if more than 8 printable characters
			lcd.print(BAUDRATE); // 16x1 prints on top line, 8x2 wraps to 2nd line
	}
	else // for multi line displays with 16+ chars/line
	{
		lcd.print("Serial2LCD "); 
		lcd.print(LCD_COLS);
		lcd.print('x');
		lcd.print(LCD_ROWS);
		lcd.setCursor(0,1);
		lcd.print(BAUDRATE);
	}
}

void loop()
{
static uint8_t needClear = 1; 

	// check to see if characters available
	// indicating a message is coming in
	if (Serial.available())
	{
	char c;
		c = Serial.read();
		if(c == '\r' || c == '\n') // CR and LF characters mark end of message
		{
			needClear = 1; // clear screen before printing next character
		}
		else
		{
    		if(needClear) // clear screen if starting a new message
			{
				lcd.clear();
				needClear = 0;
			}
			lcd.write(c);
		}
	}
}
