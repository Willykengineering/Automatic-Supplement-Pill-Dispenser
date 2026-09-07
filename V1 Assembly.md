# V1 Assembly

## 1. Print and prepare parts

1. Print out all parts.
2. Install threaded inserts.
   - _(pictures)_
   - Note: not every single-hole location on the perimeter needs a threaded insert — 4 is enough.

## 2. Mechanical assembly

3. Mount the motor by driving the side screws into the threaded inserts.
   - _(picture)_
4. Attach the carousel.
   - Note: it should sit flush with the base, but may need a little force to seat fully.

## 3. Wiring

5. Connect the stepper motor to the driver.
6. Connect the driver pins to the ESP32 pins.

   ![ESP32 to ULN2003 wiring diagram](docs/wiring-diagram.svg)

   | ULN2003 pin | ESP32 pin |
   |-------------|-----------|
   | IN1         | 19        |
   | IN2         | 18        |
   | IN3         | 5         |
   | IN4         | 17        |
   | + (power)   | 5V        |
   | - (ground)  | GND       |

   Also connect the ULN2003's 4-pin motor cable to the 28BYJ-48 stepper motor. Note that in firmware the driver is wired as `IN1, IN3, IN2, IN4` (the order `AccelStepper` needs for half-step mode) — that's the physical wiring order above, it just isn't sequential IN1→IN4.

## 4. Firmware

7. Upload the code to the ESP32.
   - _(detailed upload instructions)_
   - _(how to enter your WiFi password)_
8. Verify the upload worked by confirming the web interface loads.
   - Set the correct date and time.

## 5. Verification and final assembly

9. Verify the wiring is correct by rotating the motor from the web interface.
10. Place all electronics in the base and screw on the bottom cover.
11. Plug the device in at its permanent location and add supplements to the dispenser.
