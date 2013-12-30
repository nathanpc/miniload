/**
 *  Firmware for the miniLoad (Rev B).
 *
 *  @author Nathan Campos <nathanpc@dreamintech.net>
 */

#include <LiquidCrystal.h>

// GPIO.
LiquidCrystal lcd(P2_0, P2_1, P2_3, P2_4, P2_5, P1_4);
unsigned int curr_set = P2_2;
unsigned int curr_sense = A6; // P1.6
unsigned int RE_A = P1_7;
unsigned int RE_B = P1_5;
unsigned int RE_BTN = P1_3;
unsigned int backlight = P1_2;

// Rotary encoder.
int RE_pos = 0;
int RE_last = LOW;

// Voltage readings.
const unsigned int nread = 100;
int readings[nread];
int readtotal = 0;
int avg = 0;
int rindex = 0;

// Backlight.
boolean bkl_flag = HIGH;

/**
 * Typical Arduino setup routine.
 */
void setup() {
  // Setup the pins.
  pinMode(RE_A, INPUT);
  pinMode(RE_B, INPUT);
  //pinMode(RE_BTN, INPUT);
  
  //pinMode(backlight, OUTPUT);

  pinMode(curr_sense, INPUT);
  pinMode(curr_set, OUTPUT);

  // Setup the LCD driver.
  lcd.begin(8, 2);
  
  for (int i = 0; i < nread; i++) {
    readings[i] = 0;
  }
  
  // Reset the backlight.
  //digitalWrite(backlight, bkl_flag);

  // Reset the current.
  set_current(0);
  
  // MSP430 doesn't support CHANGE.
  attachInterrupt(RE_B, update_encoder, digitalRead(RE_B)?FALLING:RISING);
  attachInterrupt(RE_A, update_encoder, digitalRead(RE_A)?FALLING:RISING); 
  //attachInterrupt(RE_BTN, toggle_backlight, FALLING);

  interrupts();
}

/**
 *  Toggles the LCD backlight.
 */
void toggle_backlight() {
  if (bkl_flag == HIGH) {
    bkl_flag = LOW;
  } else {
    bkl_flag = HIGH;
  }
  
  digitalWrite(backlight, bkl_flag);
}

/**
 *  Rotary encoder interrupt.
 */
void update_encoder() {
  unsigned int last_A_pos = digitalRead(RE_A);
  if ((RE_last == LOW) && (last_A_pos == HIGH)) {
    if (digitalRead(RE_B) == LOW) {
      RE_pos++;
    } else {
      if (RE_pos > 0) {
        RE_pos--;
      }
    }

    // Set the current.
    set_current(RE_pos);
  }

  RE_last = last_A_pos;
}

/**
 *  Sets the current.
 *
 *  @param current A "PWM value" to set the current.
 */
void set_current(int current) {
  // Clear the area in the display.
  lcd.setCursor(0, 1);
  lcd.print("        ");
  
  // Print the value.
  lcd.setCursor(0, 1);
  lcd.print((current * 3.315) / 0.255);
  lcd.print("mA");

  // Actually set the current.
  analogWrite(curr_set, current);
}

/**
 *  Get a good voltage reading.
 */
void read_voltage() {
  // Get multiple voltage readings.
  readtotal -= readings[rindex];
  readings[rindex] = analogRead(curr_sense);
  readtotal += readings[rindex];
  rindex++;

  if (rindex >= nread) {
    rindex = 0;
  }

  // Smooth the readings.  
  avg = readtotal / nread;
}

/**
 *  Main loop.
 */
void loop() {
  // Get some voltage readings.
  read_voltage();

  // Print the voltage that we've got.
  if (rindex == 0) {
    lcd.setCursor(0, 0);
    lcd.print("        ");
    lcd.setCursor(0, 0);
    lcd.print((avg * 3.31) / 1.024);
    //lcd.print((analogRead(curr_sense) * 3.31) / 1.024);
    lcd.print("mA");
  }

  delay(5);
}
