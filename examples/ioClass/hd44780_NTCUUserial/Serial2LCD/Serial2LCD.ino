// vi:ts=4
// ----------------------------------------------------------------------------
// Serial2LCD - simple demonstration printing characters from serial port
// Created by Bill Perry 2020-06-28
// bperrybap@opensource.billsworld.billandterrie.com
//
// This example code is unlicensed and is released into the public domain
// ----------------------------------------------------------------------------
//
// This sketch is for the Noritake CU-U series VFDs, in native serial mode
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

#include <SPI.h>                                // optional, include to use h/w spi
#include <hd44780.h>                            // main hd44780 header
#include <hd44780ioClass/hd44780_NTCUUserial.h> // Noritake CU-U serial i/o class header

// constructor parameters:
// lcd([cs], [clock, data])
// If no parameters, then library will use SS, SCK, and MOSI pins
// If cs parameter specified, then use it for chip select then SCK and MOSI
// If <SPI.h> is included and clock & data pins match h/w SPI pins SCK and MOSI,
//  h/w spi will be used
// If h/w spi is not possible, then the code will fall back to bit banging.
//
// NOTE:
//  - Leonardo h/w is "stupid" and does not bring out SS
//     (it only drives an LED)
//  - Leonardo does not bring SPI signals to female headers,
//     they are only on 6 pin ISP header.
//  - ESP8266 is does not use naked constants for digital pin numbers
//
//
// To work around these pin issues in this sketch,
// Leonardo will use uno digital pins for SPI h/w which means it will
// not use h/w spi. All the other boards will use the h/w SPI pins.
// Consult board pinout diagram to see where SS, SCK, and MOSI are available.
//

#if defined(ARDUINO_AVR_LEONARDO) || ( (USB_VID == 0x2341) && (USB_PID == 0x8036) )
const int cs=10, clk=13, data=11; // uno SPI pins (s/w bit banging will be used)
#else
const int cs=SS, clk=SCK, data=MOSI; // use h/w SPI pins on all other boards
#endif
hd44780_NTCUUserial lcd(cs, clk, data); // declare lcd object


// LCD geometry
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
