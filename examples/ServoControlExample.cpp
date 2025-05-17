#include <Arduino.h>
#include <ESP32Servo.h>

// Define servo pins - updated for ESP32-S3
const int SERVO1_PIN = 17;  // Changed to GPIO17 for ESP32-S3
const int SERVO2_PIN = 18;  // Changed to GPIO18 for ESP32-S3

// Create servo objects
Servo servo1;
Servo servo2;

void setup() {
  // Serial for debugging
  Serial.begin(115200);
  Serial.println("ESP32 Servo Control Example");
  
  // Allow allocation of all timers for servo control
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  
  // Set servo parameters (50Hz is standard for servos)
  servo1.setPeriodHertz(50);
  servo2.setPeriodHertz(50);
  
  // Attach servos to GPIO pins
  // The min/max pulse width parameters might need adjustment for your specific servos
  // Default is 500us-2400us; modify if your servo needs different values
  servo1.attach(SERVO1_PIN, 500, 2400);
  servo2.attach(SERVO2_PIN, 500, 2400);
  
  // Move servos to initial position (90° - center position)
  servo1.write(90);
  servo2.write(90);
  
  Serial.println("Servos initialized. Moving to center position (90°)");
  delay(1000);
}

void loop() {
  // Example: Move servos from 0° to 180° in steps
  
  // Move from center to minimum position (0°)
  Serial.println("Moving to 0°");
  servo1.write(0);
  servo2.write(0);
  delay(1000);
  
  // Move to 45°
  Serial.println("Moving to 45°");
  servo1.write(45);
  servo2.write(45);
  delay(1000);
  
  // Move to 90° (center)
  Serial.println("Moving to 90°");
  servo1.write(90);
  servo2.write(90);
  delay(1000);
  
  // Move to 135°
  Serial.println("Moving to 135°");
  servo1.write(135);
  servo2.write(135);
  delay(1000);
  
  // Move to maximum position (180°)
  Serial.println("Moving to 180°");
  servo1.write(180);
  servo2.write(180);
  delay(1000);
  
  // Move back to center
  Serial.println("Back to center (90°)");
  servo1.write(90);
  servo2.write(90);
  delay(2000);
}
