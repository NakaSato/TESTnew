# ESP32 Node32s Servo Wiring Guide

## Servo Motor Connections

### Servo 1
- **Signal (Orange/Yellow)** → Connect to **GPIO26** on the ESP32
- **VCC (Red)** → Connect to **3.3V External Power Supply**
- **GND (Brown/Black)** → Connect to **GND** of both the ESP32 and Power Supply

### Servo 2
- **Signal (Orange/Yellow)** → Connect to **GPIO27** on the ESP32
- **VCC (Red)** → Connect to **3.3V External Power Supply**
- **GND (Brown/Black)** → Connect to **GND** of both the ESP32 and Power Supply

## Important Notes

1. **Power Supply:**
   - It's important to note that most servos are designed to operate at 4.8-6V
   - Running servos at 3.3V might result in reduced torque/performance
   - If your servos need more power, consider using a 5V power supply instead
   - Ensure the power supply can provide sufficient current (typically 500mA-1A per servo)

2. **Ground Connection:**
   - **IMPORTANT:** The GND of the external power supply MUST be connected to the GND of the ESP32 to create a common ground reference

3. **Signal Voltage:**
   - The signal pins (GPIO26, GPIO27) operate at 3.3V, which is compatible with most servo signal inputs

## Wiring Diagram (Text-based)
```
ESP32 Node32s      External 3.3V Power     Servo 1            Servo 2
-------------      ------------------      -------            -------
GPIO26 ────────────────────────────────── Signal
                                          (Orange/Yellow)
                  
GPIO27 ─────────────────────────────────────────────────── Signal
                                                           (Orange/Yellow)
                  
GND ─────────────── GND ─────────────────── GND ─────────── GND
                                           (Brown/Black)    (Brown/Black)
                  
                    3.3V ────────────────── VCC ─────────── VCC
                                           (Red)            (Red)
```

## Running the Code

1. Make sure all connections are secure before powering on
2. Connect the ESP32 to your computer with a USB cable
3. In PlatformIO, select the `servo_example` environment
4. Upload the code to the ESP32
5. Open the serial monitor to see the servo movement status
6. The servos should move through various positions from 0° to 180° and back to center

## Troubleshooting

- **Servo not moving:**
  - Check power connections
  - Ensure signal wires are connected to the correct GPIO pins
  - Verify that the GND of the ESP32 and power supply are connected
  - Try adjusting the min/max pulse width in the code for your specific servos

- **Erratic movement:**
  - Power supply might be insufficient for both servos
  - Try connecting only one servo to test
  - Ensure your power supply can provide enough current

- **Servo buzzing or jittering:**
  - May indicate insufficient power
  - Could be a signal issue - try a different GPIO pin
  - Might require a capacitor (100-220μF) across the power supply to stabilize voltage
