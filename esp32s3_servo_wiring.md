# ESP32-S3 Servo Wiring Guide

## Important Update
We've discovered you're using an **ESP32-S3** board, not the original ESP32 Node32s. The ESP32-S3 has a different pinout and capabilities, which requires changes to both the code and wiring.

## Updated Servo Motor Connection Details

### Servo 1
- **Signal (Orange/Yellow)** → Connect to **GPIO17** on the ESP32-S3
- **VCC (Red)** → Connect to **External 5V Power Supply**
- **GND (Brown/Black)** → Connect to **GND** of both the ESP32-S3 and Power Supply

### Servo 2
- **Signal (Orange/Yellow)** → Connect to **GPIO18** on the ESP32-S3
- **VCC (Red)** → Connect to **External 5V Power Supply**
- **GND (Brown/Black)** → Connect to **GND** of both the ESP32-S3 and Power Supply

## ESP32-S3 Advantages for Servo Control

The ESP32-S3 is actually better for servo control than the original ESP32 for several reasons:

1. **Improved PWM Controllers**: The S3 has enhanced PWM capabilities
2. **More GPIO Pins**: Provides more flexibility for connecting multiple servos
3. **Better Power Management**: More stable operation for power-hungry components like servos

## Modified Wiring Diagram (Text-based)
```
ESP32-S3           External 5V Power      Servo 1            Servo 2
-------------      -----------------      -------            -------
GPIO17 ────────────────────────────────── Signal
                                         (Orange/Yellow)
                  
GPIO18 ─────────────────────────────────────────────────── Signal
                                                          (Orange/Yellow)
                  
GND ─────────────── GND ─────────────────── GND ─────────── GND
                                          (Brown/Black)    (Brown/Black)
                  
                    5V ─────────────────── VCC ─────────── VCC
                                          (Red)            (Red)
```

## Next Steps

1. Upload the updated servo_diagnostic program:
   ```
   pio run -e servo_diagnostic -t upload
   ```

2. Monitor the output:
   ```
   pio device monitor
   ```

3. If everything looks good with the diagnostic, run the servo_example:
   ```
   pio run -e servo_example -t upload
   ```
