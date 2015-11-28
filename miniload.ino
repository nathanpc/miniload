/**
 * battery_capacity
 * Battery capacity tester.
 * 
 * @author Nathan Campos <nathanpc@dreamintech.net>
 */

#include <TimerOne.h>

#define VREF 2.929
#define BDIV 5.58526
#define MA_RANGE 0
#define A_RANGE  1
#define OFF_RANGE 2

const int batt_sense = A2;
const int usb_sense = A5;
const int ma_range = 12;
const int a_range = 11;
const int pwm_pin = 9;
const int led = 13;

float ratio = 1;
float cutoff = 0;

void setup() {
  analogReference(EXTERNAL);

  pinMode(ma_range, OUTPUT);
  pinMode(a_range, OUTPUT);
  pinMode(led, OUTPUT);

  Timer1.initialize(100);
  set_current(0, OFF_RANGE);

  Serial.begin(9600);
  Serial.setTimeout(20);
}

void loop() {
  String data;
  String command;
  int value;

  while (Serial.available() > 0) {
    data = Serial.readStringUntil('\n');
    command = data.substring(0, data.indexOf(' '));

    if (command == "SENSEV") {
      digitalWrite(led, HIGH);
      Serial.println(sense_voltage(), 3);
      digitalWrite(led, LOW);
    } else if (command == "USBV") {
      digitalWrite(led, HIGH);
      Serial.println(usb_voltage(), 3);
      digitalWrite(led, LOW);
    } else if (command == "ISET") {
      value = (data.substring(data.indexOf(' ') + 1)).toInt();
      set_current(value, MA_RANGE);
    } else if (command == "RATSET") {
      value = (data.substring(data.indexOf(' ') + 1)).toInt();
      ratio = value / 1000.0;

      Serial.print("OK. Ratio set: ");
      Serial.println(ratio, 3);
    } else if (command == "COSET") {
      value = (data.substring(data.indexOf(' ') + 1)).toInt();
      cutoff = value / 1000.0;

      Serial.print("OK. Cutoff set to ");
      Serial.print(cutoff, 3);
      Serial.println("V");
    } else {
      Serial.print("FAIL. Invalid command: ");
      Serial.println(data);
    }
  }
}

float sense_voltage() {
  float value = 0;

  for (int i = 0; i < 300; i++) {
    value += analogRead(batt_sense);
    delay(2);
  }

  value /= 300;
  value *= VREF / 1023.0;

  return value / ratio;
}

float usb_voltage() {
  float usbv = 0;

  for (int i = 0; i < 300; i++) {
    usbv += analogRead(usb_sense);
    delay(2);
  }

  usbv /= 300;
  usbv *= VREF / 1023.0;

  return usbv * 2;
}

void set_current(int current, uint8_t range) {
  if (range == MA_RANGE) {
    digitalWrite(a_range, LOW);
    digitalWrite(ma_range, HIGH);

    Timer1.pwm(pwm_pin, ((current / 1000.0) * 1024) /
      ((usb_voltage() * 2200) / (12000 + 2200)));
      
    Serial.print("OK. ");
    Serial.print(current);
    Serial.println("mA current set.");
  } else if (range == A_RANGE) {
    digitalWrite(a_range, HIGH);
    digitalWrite(ma_range, LOW);
    
    Timer1.pwm(pwm_pin, ((current / 1000.0) * 1024) /
      ((usb_voltage() * 18000) / (12000 + 18000)));

    Serial.print("OK. ");
    Serial.print(current);
    Serial.println("mA current set.");
  } else if (range == OFF_RANGE) {
    digitalWrite(a_range, LOW);
    digitalWrite(ma_range, HIGH);

    Timer1.pwm(pwm_pin, 0);
  } else {
    Timer1.pwm(pwm_pin, 0);
    
    Serial.print("Invalid current range selected! - ");
    Serial.println(range);
  }
}


