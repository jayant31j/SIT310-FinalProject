#include <Wire.h>
#include <ZumoShield.h>

#define SENSOR_THRESHOLD 300
#define ABOVE_LINE(sensor) ((sensor) > SENSOR_THRESHOLD)
#define TURN_SPEED 200
#define SPEED 250
#define LINE_THICKNESS 0.75
#define INCHES_TO_ZUNITS 17142.0
#define OVERSHOOT(line_thickness) (((INCHES_TO_ZUNITS * (line_thickness)) / SPEED))

ZumoReflectanceSensorArray reflectanceSensors;
ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON);

void setup()
{
  unsigned int sensors[6];
  unsigned short count = 0;
  unsigned short last_status = 0;
  int turn_direction = 1;

  reflectanceSensors.init();

  delay(500);
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

  button.waitForButton();

  for (int i = 0; i < 4; i++)
  {
    turn_direction *= -1;
    motors.setSpeeds(turn_direction * TURN_SPEED, -1 * turn_direction * TURN_SPEED);
    while (count < 2)
    {
      reflectanceSensors.calibrate();
      reflectanceSensors.readLine(sensors);
      if (turn_direction < 0)
      {
        count += ABOVE_LINE(sensors[5]) ^ last_status;
        last_status = ABOVE_LINE(sensors[5]);
      }
      else
      {
        count += ABOVE_LINE(sensors[0]) ^ last_status;
        last_status = ABOVE_LINE(sensors[0]);
      }
    }
    count = 0;
    last_status = 0;
  }

  turn('L');
  motors.setSpeeds(0, 0);
  digitalWrite(13, LOW);
}

void loop()
{
  MazeSolver();
}

void turn(char dir)
{
  unsigned short count = 0;
  unsigned short last_status = 0;
  unsigned int sensors[6];

  switch (dir)
  {
  case 'L':
  case 'B':
    motors.setSpeeds(-TURN_SPEED, TURN_SPEED);

    while (count < 2)
    {
      reflectanceSensors.readLine(sensors);
      count += ABOVE_LINE(sensors[1]) ^ last_status;
      last_status = ABOVE_LINE(sensors[1]);
    }

    break;

  case 'R':
    motors.setSpeeds(TURN_SPEED, -TURN_SPEED);

    while (count < 2)
    {
      reflectanceSensors.readLine(sensors);
      count += ABOVE_LINE(sensors[4]) ^ last_status;
      last_status = ABOVE_LINE(sensors[4]);
    }

    break;

  case 'S':
    break;
  }
}

char selectTurn(unsigned char found_left, unsigned char found_straight, unsigned char found_right) //Right Hand Rule Algorithm 
{
  if (found_left) // Gives priority to Right turns over left
    return 'L';
  else if (found_straight)
    return 'S';
  else if (found_right)
    return 'R';
  else
    return 'B';
}

void followSegment()
{
  unsigned int position;
  unsigned int sensors[6];
  int offset_from_center;
  int p_difference;

  while (1)
  {
    position = reflectanceSensors.readLine(sensors);
    offset_from_center = ((int)position) - 2500;

    p_difference = offset_from_center / 3;

    if (p_difference > SPEED)
      p_difference = SPEED;
    if (p_difference < -SPEED)
      p_difference = -SPEED;

    if (p_difference < 0)
      motors.setSpeeds(SPEED + p_difference, SPEED);
    else
      motors.setSpeeds(SPEED, SPEED - p_difference);

    if (!ABOVE_LINE(sensors[0]) && !ABOVE_LINE(sensors[1]) && !ABOVE_LINE(sensors[2]) && !ABOVE_LINE(sensors[3]) && !ABOVE_LINE(sensors[4]) && !ABOVE_LINE(sensors[5]))
    {
      stopRobot();
      return;
    }
    else if (ABOVE_LINE(sensors[0]) || ABOVE_LINE(sensors[5]))
    {
      return;
    }
  }
}

void stopRobot()
{
  motors.setSpeeds(0, 0);
  while (true)
    ;
}

void MazeSolver()
{
  while (1)
  {
    // Navigate current line segment
    followSegment();

    // These variables record whether the robot has seen a line to the
    // left, straight ahead, and right, while examining the current
    // intersection.
    unsigned char found_left = 0;
    unsigned char found_straight = 0;
    unsigned char found_right = 0;

    // Now read the sensors and check the intersection type.
    unsigned int sensors[6];
    reflectanceSensors.readLine(sensors);

    // Check for left and right exits.
    if (ABOVE_LINE(sensors[0]))
      found_left = 1;
    if (ABOVE_LINE(sensors[5]))
      found_right = 1;

    // Drive straight a bit more, until we are
    // approximately in the middle of intersection.
    // This should help us better detect if we
    // have left or right segments.
    motors.setSpeeds(SPEED, SPEED);
    delay(OVERSHOOT(LINE_THICKNESS) / 2);

    reflectanceSensors.readLine(sensors);

    // Check for left and right exits.
    if (ABOVE_LINE(sensors[0]))
      found_left = 1;
    if (ABOVE_LINE(sensors[5]))
      found_right = 1;

    // After driving a little further, we
    // should have passed the intersection
    // and can check to see if we've hit the
    // finish line or if there is a straight segment
    // ahead.
    delay(OVERSHOOT(LINE_THICKNESS) / 2);

    // Check for a straight exit.
    reflectanceSensors.readLine(sensors);

    // Check again to see if left or right segment has been found
    if (ABOVE_LINE(sensors[0]))
      found_left = 1;
    if (ABOVE_LINE(sensors[5]))
      found_right = 1;

    if (ABOVE_LINE(sensors[1]) || ABOVE_LINE(sensors[2]) || ABOVE_LINE(sensors[3]) || ABOVE_LINE(sensors[4]))
      found_straight = 1;

    // Intersection identification is complete.
    unsigned char dir = selectTurn(found_left, found_straight, found_right);

    // Make the turn indicated by the path.
    turn(dir);

    // Uncomment to delay between turns
    // delay(1000);
  }
}
