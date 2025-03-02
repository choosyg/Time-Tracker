#include <Arduino.h>

#include <Arduino_LSM9DS1.h>

// put function declarations here:
int myFunction(int, int);

void setup() {
  Serial.begin(115200);    // initialize serial communication

  // begin initialization
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  float x, y, z, delta = 0.05;

  if (IMU.accelerationAvailable())
  {
    IMU.readAcceleration(x, y, z);
 
    if(y <= delta && y >= -delta)
          Serial.println("flat");
    else if(y > delta && y < 1 - delta)
          Serial.println("tilted to the left");
    else if(y >= 1 - delta)
          Serial.println("left");
    else if(y < -delta && y > delta - 1)
          Serial.println("tilted to the right");
    else
          Serial.println("right");
  } else {
    //Serial.println("ERROR: No Accelerometer");
  }
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}