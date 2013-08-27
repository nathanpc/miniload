/**
 *  PCD8544.h
 *  PCD8544 (bit bang) driver for the MSP430G2553 based on RobG's code.
 *
 *  @author Nathan Campos <nathanpc@dreamintech.net>
 */

#ifndef PCD8544_H_
#define PCD8544_H_

// Function Set bits (PD, V, H).
#define PCD8544_POWERDOWN       0b00000100
#define PCD8544_ENTRYMODE_V     0b00000010
#define PCD8544_EXTINSTRUCTIONS 0b00000001

// Display Control bits (D, E).
#define PCD8544_DISPLAYBLANK    0b00000000
#define PCD8544_DISPLAYNORMAL   0b00000100
#define PCD8544_DISPLAYALLON    0b00000001
#define PCD8544_DISPLAYINVERTED 0b00000101

// Command bits (H = 0).
#define PCD8544_FUNCTIONSET    0b00100000
#define PCD8544_DISPLAYCONTROL 0b00001000
#define PCD8544_SETYADDR       0b01000000
#define PCD8544_SETXADDR       0b10000000

// Command bits (H = 1).
#define PCD8544_SETTEMP 0b00000100
#define PCD8544_SETBIAS 0b00010000
#define PCD8544_SETVOP  0b10000000


// Function Prototypes.
void lcd_setup();
void lcd_init();

void lcd_command(const char command, const char data);

void lcd_putc(const char c);
void lcd_print(const char *string);

void lcd_clear();
void lcd_clear_row(unsigned int row);

void lcd_set_pos(unsigned int x, unsigned int y);

#endif
