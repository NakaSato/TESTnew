#include <Arduino.h>
#include <ESP32Servo.h>

// Define servo pins - updated for ESP32-S3
const int SERVO1_PIN = 17;  // Changed to GPIO17 for ESP32-S3
const int SERVO2_PIN = 18;  // Changed to GPIO18 for ESP32-S3

// Create servo objects
Servo servo1;
Servo servo2;

// For diagnostics
void checkPin(int pin) {
  pinMode(pin, INPUT);
  int reading = digitalRead(pin);
  Serial.printf("Pin %d initial state: %d\n", pin, reading);
  
  // Test as output
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
  delay(500);
  digitalWrite(pin, LOW);
  delay(500);
  Serial.printf("Pin %d tested as output\n", pin);
  
  // Return to input mode for servo control
  pinMode(pin, INPUT);
}

void setup() {
  // Serial for debugging
  Serial.begin(115200);
  delay(1000); // Give serial time to connect
  Serial.println("\n\n--- ESP32 Servo Control Diagnostic ---");
  
  // Check GPIO pins
  Serial.println("Testing GPIO pins...");
  checkPin(SERVO1_PIN);
  checkPin(SERVO2_PIN);
  
  // Allow allocation of all timers for servo control
  Serial.println("Allocating timers...");
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  
  // Set servo parameters (50Hz is standard for servos)
  // Using lower frequency can sometimes help with weak power supplies
  servo1.setPeriodHertz(50);
  servo2.setPeriodHertz(50);
  
  Serial.println("Attaching servos...");
  
  // Try a wider pulse range to accommodate more servo types
  // Some servos have different min/max pulse widths
  servo1.attach(SERVO1_PIN, 500, 2500);
  if (servo1.attached()) {
    Serial.println("Servo 1 attached successfully");
  } else {
    Serial.println("ERROR: Failed to attach Servo 1");
  }
  
  servo2.attach(SERVO2_PIN, 500, 2500);
  if (servo2.attached()) {
    Serial.println("Servo 2 attached successfully");
  } else {
    Serial.println("ERROR: Failed to attach Servo 2");
  }
  
  // Start with small angle first to prevent servo stalling if power is limited
  Serial.println("Moving servos to 10° position...");
  servo1.write(10);
  servo2.write(10);
  delay(1000);
  
  // Then try center position
  Serial.println("Moving servos to center position (90°)...");
  servo1.write(90);
  servo2.write(90);
  
  Serial.println("Setup complete. Starting servo test sequence.");
  delay(2000);
}

void loop() {
  // Test one servo at a time to reduce power requirements
  Serial.println("\n--- Testing Servo 1 only ---");
  // Servo 1 to 0°
  Serial.println("Servo 1 to 0°");
  servo1.write(0);
  delay(1000);
  
  // Servo 1 to 90°
  Serial.println("Servo 1 to 90°");
  servo1.write(90);
  delay(1000);
  
  // Servo 1 to 180°
  Serial.println("Servo 1 to 180°");
  servo1.write(180);
  delay(1000);
  
  // Servo 1 back to center
  Serial.println("Servo 1, back to center");
  servo1.write(90);
  delay(1000);
  
  Serial.println("\n--- Testing Servo 2 only ---");
  // Servo 2 to 0°
  Serial.println("Servo 2 to 0°");
  servo2.write(0);
  delay(1000);
  
  // Servo 2 to 90°
  Serial.println("Servo 2 to 90°");
  servo2.write(90);
  delay(1000);
  
  // Servo 2 to 180°
  Serial.println("Servo 2 to 180°");
  servo2.write(180);
  delay(1000);
  
  // Servo 2 back to center
  Serial.println("Servo 2, back to center");
  servo2.write(90);
  delay(1000);
  
  // Try a gentle sweep with both servos
  Serial.println("\n--- Testing both servos with gentle sweep ---");
  for (int pos = 45; pos <= 135; pos += 5) {
    Serial.printf("Position: %d°\n", pos);
    servo1.write(pos);
    servo2.write(pos);
    delay(100);
  }
  
  for (int pos = 135; pos >= 45; pos -= 5) {
    Serial.printf("Position: %d°\n", pos);
    servo1.write(pos);
    servo2.write(pos);
    delay(100);
  }
  
  Serial.println("Test sequence completed. Pausing for 3 seconds.");
  delay(3000);
}
