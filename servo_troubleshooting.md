# Troubleshooting Servo Motors with ESP32

## Common Reasons for Servo Not Working

1. **Insufficient Power Supply**:
   - Servos require more current than the ESP32 can provide through its 3.3V pins
   - Solution: Use an external 5V power supply that can provide at least 500mA per servo

2. **Incorrect Wiring**:
   - Double-check all connections:
     - Signal wires to GPIO26 and GPIO27
     - VCC to external power supply (5V recommended rather than 3.3V)
     - All GND connections properly joined

3. **Common Ground Issues**:
   - The ESP32 and your external power supply MUST share a common ground
   - Connect the GND of the ESP32 to the GND of your external power supply

4. **Servo Signal Requirements**:
   - Most servos expect a 5V logic level signal, but ESP32 outputs 3.3V
   - Some servos work fine with 3.3V signals, others might need a logic level converter

5. **Pulse Width Issues**:
   - Different servo models have different pulse width requirements
   - Try adjusting the min/max pulse width settings in the code:
     ```cpp
     // Try these values for servos that don't respond:
     servo1.attach(SERVO1_PIN, 500, 2500);  // Wider range
     // or
     servo1.attach(SERVO1_PIN, 700, 2300);  // Narrower range
     ```

## Testing with the Diagnostic Code

1. Upload the `ServoDiagnostic.cpp` code to your ESP32:
   ```
   pio run -e servo_diagnostic -t upload
   ```

2. Monitor the output to see diagnostic information:
   ```
   pio device monitor
   ```

3. The diagnostic code will:
   - Test the GPIO pins directly
   - Test each servo individually to reduce power requirements
   - Start with small movements to detect any response
   - Provide feedback on whether the servos were properly attached

## Additional Troubleshooting Tips

1. **Try One Servo First**:
   - Disconnect one servo and test only one at a time
   - This reduces power requirements and simplifies troubleshooting

2. **Check Physical Issues**:
   - Some servos might be physically stuck or damaged
   - Try gently turning the servo horn by hand to check for free movement

3. **Different Power Supply**:
   - Try a different power supply with higher current capability
   - Many servos need 5V to work properly, not 3.3V

4. **Different GPIO Pins**:
   - Try using different GPIO pins on the ESP32
   - Some pins might have limitations or be connected to special functions

5. **Hardware Test**:
   - Try connecting your servo directly to a battery or power supply to see if it works outside of the ESP32 control

If your servos still don't work after trying these steps, please share the diagnostic output for further assistance.
