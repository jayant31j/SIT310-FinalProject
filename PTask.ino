#include <Wire.h>
#include <ZumoShield.h>
#include <Pushbutton.h>

Pushbutton button(ZUMO_BUTTON);
ZumoReflectanceSensorArray reflectanceSensors;
ZumoMotors motors;
int lastError = 0;


const int MAX_SPEED = 250;
const int SENSOR_THRESHOLD = 1000;

void setup()
{

  reflectanceSensors.init();


  button.waitForButton();


  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

 
  delay(1000);
  int i;
  for (i = 0; i < 80; i++)
  {
    if ((i > 10 && i <= 30) || (i > 50 && i <= 70))
      motors.setSpeeds(-200, 200);
    else
      motors.setSpeeds(200, -200);
    reflectanceSensors.calibrate();


    delay(20);
  }
  motors.setSpeeds(0, 0);


  digitalWrite(13, LOW);


  button.waitForButton();


  while (button.isPressed())
    ;


  Serial.begin(9600);
  Serial.println("Sensor Readings | Motor Speeds");
}

void loop()
{
  unsigned int sensors[6];


  int position = reflectanceSensors.readLine(sensors);


  int error = position - 2500;


  int speedDifference = error / 4 + 6 * (error - lastError);

  lastError = error;


  int m1Speed = MAX_SPEED + speedDifference;
  int m2Speed = MAX_SPEED - speedDifference;


  if (m1Speed < 0)
    m1Speed = 0;
  if (m2Speed < 0)
    m2Speed = 0;
  if (m1Speed > MAX_SPEED)
    m1Speed = MAX_SPEED;
  if (m2Speed > MAX_SPEED)
    m2Speed = MAX_SPEED;

  motors.setSpeeds(m1Speed, m2Speed);


  if (sensors[0] > SENSOR_THRESHOLD && sensors[5] > SENSOR_THRESHOLD) {
    // Stop the robot
    motors.setSpeeds(0, 0);

    
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
