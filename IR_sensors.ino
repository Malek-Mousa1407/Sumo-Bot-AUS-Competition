#include <Wire.h>
#include <Zumo32U4.h>
#include <Zumo32U4IMU.h>

Zumo32U4OLED display;
Zumo32U4ProximitySensors proxSensors;
Zumo32U4Motors motors;
Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4Buzzer buzzer;
Zumo32U4LineSensors lineSensors;
Zumo32U4IMU IMU;

#define QTR_THRESHOLD 800
int initDelay = 250;
#define NUM_SENSORS 3
#define ZERO_Speed 0
int motorSpeed = 0;
unsigned int lineSensorValues[NUM_SENSORS];

void waitForButtonAndCountDown() {
  ledYellow(1);
  display.clear();
  display.print(F("Press A"));
  buttonA.waitForButton();
  ledYellow(0);
  display.clear();
 // delay(5000);

  for (int i = 0; i < 3; i++)
  {
    delay(1000);
    buzzer.playNote(NOTE_G(3), 200, 15);
  }
  delay(1000);
  buzzer.playNote(NOTE_G(4), 500, 15);
  delay(1000);

}

void batteryDisplay(){
  bool usbPower = usbPowerPresent();

  uint16_t batteryLevel = readBatteryMillivolts();

  // Print the results to the display.
  display.clear();
  display.print(F("B="));
  display.print(batteryLevel);
  display.print(F("mV   "));
  display.gotoXY(0, 1);
  display.print(F("USB="));
  display.print(usbPower ? 'Y' : 'N');

  // Print the results to the serial monitor.
  Serial.print(F("USB="));
  Serial.print(usbPower ? 'Y' : 'N');
  Serial.print(F(" B="));
  Serial.print(batteryLevel);
  Serial.println(F(" mV"));

  delay(200);
}

void setup() {
   if(buttonB.isPressed()){
    batteryDisplay();
    buttonB.waitForRelease();
    delay(2000);
  }

  waitForButtonAndCountDown();
  proxSensors.initThreeSensors();
  motors.setSpeeds(0, 0);
  lineSensors.initThreeSensors();
  Wire.begin();
  IMU.init();
  IMU.enableDefault();
  Serial.begin(9600);
  
  // Head on start
  motors.setSpeeds(350, -300);
  delay(initDelay);
  motors.setSpeeds(400, 400);
  delay(325);
  motors.setSpeeds(-400, 400);
  delay(initDelay);
  motors.setSpeeds(400, 400);
  delay(initDelay);
}

void loop() {  
  
  display.clear();
  display.gotoXY(0,0);
  display.print(F("YOU WILL"));
  display.gotoXY(0,1);
  display.print(F("FALL!"));
  
  motorSpeed = 400;
  motors.setSpeeds(motorSpeed, motorSpeed);
  // Read sensor values
  proxSensors.read();
  // If left sensor value is greater than set threshold value
  // Read sensor values
  lineSensors.read(lineSensorValues);
  // IMU sensor
  IMU.read();
  // If left sensor value is greater than set threshold value
  if (lineSensorValues[0] > QTR_THRESHOLD) {
     // reversing
    motorSpeed = 300;
    motors.setSpeeds(-motorSpeed, -motorSpeed);
    delay(50);

    motorSpeed = 300;
    motors.setSpeeds(motorSpeed, -motorSpeed);
    delay(375);
    motorSpeed = 400;
    motors.setSpeeds(motorSpeed, motorSpeed);
    delay(100);

  }
  // If right sensor value is greater than set threshold value
  // If right sensor value is greater than set threshold value
  else if (lineSensorValues[NUM_SENSORS - 1] > QTR_THRESHOLD) {
    // reversing
    motorSpeed = 300;
    motors.setSpeeds(-motorSpeed, -motorSpeed);
    delay(50);
    // rotating     
    motorSpeed = 300;
    motors.setSpeeds(-motorSpeed, motorSpeed);
    delay(375);
    motorSpeed = 400;
    motors.setSpeeds(motorSpeed, motorSpeed);
    delay(100);
  }
  else if(lineSensorValues[1] > QTR_THRESHOLD){
    display.clear();
    motorSpeed = 300;
    motors.setSpeeds(-motorSpeed, -motorSpeed);
    delay(50);
  }
  // If no line is detected
  else {
    if(IMU.a.x < -300 && !lineSensorValues[1] > QTR_THRESHOLD && !lineSensorValues[0] > QTR_THRESHOLD && !lineSensorValues[NUM_SENSORS - 1] > QTR_THRESHOLD){
        motorSpeed = 400;
        motors.setSpeeds(motorSpeed, motorSpeed);
        delay(150);
    }

    else if (proxSensors.countsFrontWithLeftLeds() >= 3) {
      display.clear();
      motorSpeed = 400;
      motors.setSpeeds(motorSpeed, motorSpeed);
    }
    // If left sensor value is greater than set threshold value
    else if (proxSensors.countsLeftWithLeftLeds() >= 4) {
      display.clear();
      motors.setSpeeds(-300, 400);
      delay(115);
    }
    // If rightsensor value is greater than set threshold value
    else if (proxSensors.countsRightWithRightLeds() >= 4) {
      display.clear();
      motors.setSpeeds(400, -300);
      delay(115);
    }
  }
}
