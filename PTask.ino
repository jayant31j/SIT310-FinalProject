#include <Wire.h>
#include <ZumoShield.h>
#include <Pushbutton.h>

Pushbutton button(ZUMO_BUTTON);
ZumoReflectanceSensorArray reflectanceSensors;
ZumoMotors motors;
int lastError = 0;

// This is the maximum speed the motors will be allowed to turn.
// (400 lets the motors go at top speed; decrease to impose a speed limit)
const int MAX_SPEED = 250;
const int SENSOR_THRESHOLD = 1000;

void setup()
{
  // Initialize the reflectance sensors module
  reflectanceSensors.init();

  // Wait for the user button to be pressed and released
  button.waitForButton();

  // Turn on LED to indicate we are in calibration mode
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

  // Wait 1 second and then begin automatic sensor calibration
  // by rotating in place to sweep the sensors over the line
  delay(1000);
  int i;
  for (i = 0; i < 80; i++)
  {
    if ((i > 10 && i <= 30) || (i > 50 && i <= 70))
      motors.setSpeeds(-200, 200);
    else
      motors.setSpeeds(200, -200);
    reflectanceSensors.calibrate();

    // Since our counter runs to 80, the total delay will be
    // 80*20 = 1600 ms.
    delay(20);
  }
  motors.setSpeeds(0, 0);

  // Turn off LED to indicate we are through with calibration
  digitalWrite(13, LOW);

  // Wait for the user button to be pressed and released
  button.waitForButton();

  // Play music and wait for it to finish before we start driving.
  while (button.isPressed())
    ;

  // Print header for sensor readings and motor speeds
  Serial.begin(9600);
  Serial.println("Sensor Readings | Motor Speeds");
}

void loop()
{
  unsigned int sensors[6];

  // Get the position of the line.  Note that we *must* provide the "sensors"
  // argument to readLine() here, even though we are not interested in the
  // individual sensor readings
  int position = reflectanceSensors.readLine(sensors);

  // Our "error" is how far we are away from the center of the line, which
  // corresponds to position 2500.
  int error = position - 2500;

  // Get motor speed difference using proportional and derivative PID terms
  // (the integral term is generally not very useful for line following).
  // Here we are using a proportional constant of 1/4 and a derivative
  // constant of 6, which should work decently for many Zumo motor choices.
  // You probably want to use trial and error to tune these constants for
  // your particular Zumo and line course.
  int speedDifference = error / 4 + 6 * (error - lastError);

  lastError = error;

  // Get individual motor speeds.  The sign of speedDifference
  // determines if the robot turns left or right.
  int m1Speed = MAX_SPEED + speedDifference;
  int m2Speed = MAX_SPEED - speedDifference;

  // Here we constrain our motor speeds to be between 0 and MAX_SPEED.
  // Generally speaking, one motor will always be turning at MAX_SPEED
  // and the other will be at MAX_SPEED-|speedDifference| if that is positive,
  // else it will be stationary.  For some applications, you might want to
  // allow the motor speed to go negative so that it can spin in reverse.
  if (m1Speed < 0)
    m1Speed = 0;
  if (m2Speed < 0)
    m2Speed = 0;
  if (m1Speed > MAX_SPEED)
    m1Speed = MAX_SPEED;
  if (m2Speed > MAX_SPEED)
    m2Speed = MAX_SPEED;

  motors.setSpeeds(m1Speed, m2Speed);

  // Check if the robot has reached the end of the line
  if (sensors[0] > SENSOR_THRESHOLD && sensors[5] > SENSOR_THRESHOLD) {
    // Stop the robot
    motors.setSpeeds(0, 0);

    // Rotate left to follow the left-hand rule
    motors.setSpeeds(-200, 200);
    delay(250); // Adjust the delay as needed to make accurate turns

    // Move forward until the line is detected again
    while (sensors[0] < SENSOR_THRESHOLD || sensors[5] < SENSOR_THRESHOLD) {
      reflectanceSensors.readLine(sensors);
      motors.setSpeeds(MAX_SPEED, MAX_SPEED);
    }
  }

  // Print sensor readings and motor speeds to the Serial Monitor
  Serial.print(sensors[0]);
  Serial.print(" ");
  Serial.print(sensors[1]);
  Serial.print(" ");
  Serial.print(sensors[2]);
  Serial.print(" ");
  Serial.print(sensors[3]);
  Serial.print(" ");
  Serial.print(sensors[4]);
  Serial.print(" ");
  Serial.print(sensors[5]);
  Serial.print(" | ");
  Serial.print(m1Speed);
  Serial.print(" ");
  Serial.println(m2Speed);

  delay(100); // Delay for smoother output
}
