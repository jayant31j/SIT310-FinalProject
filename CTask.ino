#include <Wire.h>
#include <ZumoShield.h>
#include <Pushbutton.h>

Pushbutton button(ZUMO_BUTTON);
ZumoReflectanceSensorArray reflectanceSensors;
ZumoMotors motors;
int lastError = 0;
bool isCalibrated = false;


const int MAX_SPEED = 250;
const int SENSOR_THRESHOLD = 1000;

enum RobotState {
  LINE_FOLLOWING,
  LEFT_HAND_RULE
};

RobotState currentState = LINE_FOLLOWING;

void setup()
{

  reflectanceSensors.init();


  button.waitForButton();


  Serial.begin(9600);
  Serial.println("Sensor Readings | Motor Speeds");
}

void calibrateSensors()
{

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

  isCalibrated = true;
}

void loop()
{
  unsigned int sensors[6];

  int m1Speed, m2Speed; 

  if (!isCalibrated) {
    // Perform calibration
    calibrateSensors();
  }

  if (currentState == LINE_FOLLOWING) {

    int position = reflectanceSensors.readLine(sensors);

   
    int error = position - 2500;

    int speedDifference = error / 4 + 6 * (error - lastError);

    lastError = error;

 
    m1Speed = MAX_SPEED + speedDifference;
    m2Speed = MAX_SPEED - speedDifference;


    m1Speed = constrain(m1Speed, 0, MAX_SPEED);
    m2Speed = constrain(m2Speed, 0, MAX_SPEED);

    motors.setSpeeds(m1Speed, m2Speed);


    if (sensors[0] > SENSOR_THRESHOLD && sensors[5] > SENSOR_THRESHOLD) {

      motors.setSpeeds(0, 0);


      currentState = LEFT_HAND_RULE;
    }
  }
  else if (currentState == LEFT_HAND_RULE) {

    while (sensors[0] < SENSOR_THRESHOLD || sensors[5] < SENSOR_THRESHOLD) {
      reflectanceSensors.readLine(sensors);
      motors.setSpeeds(MAX_SPEED, MAX_SPEED);
    }


    motors.setSpeeds(0, 0);


    motors.setSpeeds(-200, 200);
    delay(250); 

    currentState = LINE_FOLLOWING;
  }


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

  delay(100);
}
