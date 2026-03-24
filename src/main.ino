#include <Servo.h>

const int hallSensorPin = A0;   // Analog pin connected to HW-477 sensor
const int brakePin = 9;         // PWM pin for servo motor (brake)
const int thresholdRPM = 2000;  // RPM threshold to apply brake

Servo brakeServo;

unsigned long lastTime = 0;
unsigned int pulseCount = 0;
float currentRPM = 0;

const int interval = 1000; // Measurement interval in milliseconds
bool brakeApplied = false;

void setup() {
  Serial.begin(9600);
  pinMode(hallSensorPin, INPUT);

  brakeServo.attach(brakePin);

  attachInterrupt(digitalPinToInterrupt(hallSensorPin), countPulse, CHANGE);

  brakeServo.write(0); // Assume 0 is brake released
}

void loop() {
  static bool lastSensorState = false;

  int sensorValue = analogRead(hallSensorPin);
  bool currentSensorState = sensorValue > 512; // Threshold for detection

  // Edge detection for pulse counting
  if (currentSensorState && !lastSensorState) {
    pulseCount++;
  }
  lastSensorState = currentSensorState;

  unsigned long currentTime = millis();

  if (currentTime - lastTime >= interval) {
    // Calculate RPM
    float revolutions = (float)pulseCount;
    currentRPM = (revolutions / (interval / 1000.0)) * 60.0;

    Serial.print("RPM: ");
    Serial.println(currentRPM);

    // Apply or release brake based on RPM threshold
    if (currentRPM >= thresholdRPM && !brakeApplied) {
      brakeServo.write(180); // Apply brake
      brakeApplied = true;
    } 
    else if (currentRPM < thresholdRPM && brakeApplied) {
      brakeServo.write(0); // Release brake
      brakeApplied = false;
    }

    // Reset pulse count for next interval
    pulseCount = 0;
    lastTime = currentTime;
  }
}

// Interrupt function
void countPulse() {
  pulseCount++;
}
