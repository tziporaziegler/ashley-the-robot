#include <NewPing.h>
#include <Servo.h>

#include <SoftwareSerial.h>
#include <SD.h>  // Needed by the EMIC2 library, though not utilized in this example
#include "EMIC2.h"

// Ultrasonic distance sensor - eyes
#define TRIGGER_PIN         5 
#define ECHO_PIN            6 
#define MAX_DISTANCE        100 // Max distance to ping for (in cm). Maximum distance is rated at 400-500cm.

// Servo - body
#define SERVO_PIN           9
#define SERVO_MIN_POSITION  0
#define SERVO_MAX_POSITION  180

// Emic 2
#define RX_PIN              10  // Connect to SOUT pin
#define TX_PIN              11  // Connect to SIN pin
#define EMIC_VOLUME         18
#define EMIC_VOICE          8   // 9 choices: 0-8

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
Servo myservo;
EMIC2 emic;

void setup() {
  Serial.begin(115200);
  myservo.attach(SERVO_PIN);
  
  // Initializes the EMIC2 instance.
  // The library sets up a SoftwareSerial port for the communication with the Emic 2 module.
  emic.begin(RX_PIN, TX_PIN);
  emic.setVolume(EMIC_VOLUME);
  emic.setVoice(EMIC_VOICE);  
  emic.speak("Hello there! My name is Ashley.");
}

const int SCAN_SKIP_SPEED = 5;

int startPosition = SERVO_MIN_POSITION;
int endPosition = SERVO_MAX_POSITION;
int closestDistance = MAX_DISTANCE;
int closestPosition = startPosition;
  
void loop() {
  Serial.print("Starting Distance: ");
  Serial.print(closestDistance);
  Serial.print(", Starting Postion: ");
  Serial.println(closestPosition);
  
  for (int servoPosition = startPosition; servoPosition <= endPosition; servoPosition += SCAN_SKIP_SPEED) {
    myservo.write(servoPosition);

    // Do multiple pings (default=5), discard out of range pings and return median in microseconds.
    int medianEchoTime = sonar.ping_median();

    // Converts microseconds to distance in centimeters.
    int currentDistance = sonar.convert_cm(medianEchoTime);

    Serial.print("Current Ping: ");
    Serial.println(currentDistance);
    
    if(currentDistance < closestDistance && currentDistance > 0) {
      closestDistance = currentDistance;
      closestPosition = servoPosition;

      Serial.print("Closest Distance: ");
      Serial.print(closestDistance);
      Serial.print(", Closest Postion: ");
      Serial.println(closestPosition);
    }

    // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
    delay(30);
  }

  moveServoToPosition(closestPosition);
  setStartAndEndPosition(closestDistance, closestPosition);

  delay(10000); 
}

void moveServoToPosition(int position) {
  Serial.print("Moving to position: ");
  Serial.println(position);  
  myservo.write(position);
  emic.speak("Hi there!");
}

void setStartAndEndPosition(int closestDistance, int currentPosition) {
  const int range = SCAN_SKIP_SPEED * 5;
  startPosition = max(SERVO_MIN_POSITION, currentPosition - range);
  endPosition = min(SERVO_MAX_POSITION, currentPosition + range);
    
  if(closestDistance >= MAX_DISTANCE || closestDistance <= 0 || abs(endPosition - startPosition) < range) {
    startPosition = SERVO_MIN_POSITION;
    endPosition = SERVO_MAX_POSITION; 
  }
}
