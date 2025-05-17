# Updated Servo Wiring Guide

## New Servo Motor Connection Details

### Servo 1
- **Signal (Orange/Yellow)** → Connect to **GPIO13** on the ESP32
- **VCC (Red)** → Connect to **External 5V Power Supply** (changed from 3.3V)
- **GND (Brown/Black)** → Connect to **GND** of both the ESP32 and Power Supply

### Servo 2
- **Signal (Orange/Yellow)** → Connect to **GPIO12** on the ESP32
- **VCC (Red)** → Connect to **External 5V Power Supply** (changed from 3.3V)
- **GND (Brown/Black)** → Connect to **GND** of both the ESP32 and Power Supply

## Why These Changes?

1. **Different GPIO Pins**:
   - GPIO13 and GPIO12 are on different peripherals than the previous pins
   - These pins might have better PWM characteristics on your specific ESP32
   - They are not used for special boot functions like some other pins

2. **Increased Voltage**:
   - Changed from 3.3V to 5V for the servo power
   - Most standard servos are designed to operate at 4.8V-6V
   - 3.3V is often insufficient for proper servo operation

## Modified Wiring Diagram (Text-based)
```
ESP32 Node32s      External 5V Power      Servo 1            Servo 2
-------------      -----------------      -------            -------
GPIO13 ────────────────────────────────── Signal
                                         (Orange/Yellow)
                  
GPIO12 ─────────────────────────────────────────────────── Signal
                                                          (Orange/Yellow)
                  
GND ─────────────── GND ─────────────────── GND ─────────── GND
                                          (Brown/Black)    (Brown/Black)
                  
                    5V ─────────────────── VCC ─────────── VCC
                                          (Red)            (Red)
```

## Next Steps

1. Upload the servo_example code again:
   ```
   pio run -e servo_example -t upload
   ```

2. Monitor the output:
   ```
   pio device monitor
   ```

3. If the servos still don't work, try the servo_diagnostic program which has additional testing features:
   ```
   pio run -e servo_diagnostic -t upload
   ```
