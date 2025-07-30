//Code is derived from a examples given in zumoshield library and follows right hand rule 
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

char path[100] = "";
unsigned char path_length = 0;

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

  while (1)
  {
    button.waitForButton();
    MazeEnd();
  }
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
  if (found_right) // Gives priority to Right turns over left
    return 'R';
  else if (found_straight)
    return 'S';
  else if (found_left)
    return 'L';
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
      return;
    }
    else if (ABOVE_LINE(sensors[0]) || ABOVE_LINE(sensors[5]))
    {
      return;
    }
  }
}

void MazeSolver()
{
  while (1)
  {

    followSegment();

    unsigned char found_left = 0;
    unsigned char found_straight = 0;
    unsigned char found_right = 0;


    unsigned int sensors[6];
    reflectanceSensors.readLine(sensors);

    // Check for left and right exits.
    if (ABOVE_LINE(sensors[0]))
      found_left = 1;
    if (ABOVE_LINE(sensors[5]))
      found_right = 1;

    // have left or right segments.
    motors.setSpeeds(SPEED, SPEED);
    delay(OVERSHOOT(LINE_THICKNESS) / 2);

    reflectanceSensors.readLine(sensors);

    // Check for left and right exits.
    if (ABOVE_LINE(sensors[0]))
      found_left = 1;
    if (ABOVE_LINE(sensors[5]))
      found_right = 1;

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

    // If all four middle sensors are on dark black, we have
    // solved the maze.
    if (ABOVE_LINE(sensors[1]) && ABOVE_LINE(sensors[2]) && ABOVE_LINE(sensors[3]) && ABOVE_LINE(sensors[4]))
    {
      motors.setSpeeds(0, 0);
      break;
    }

    unsigned char dir = selectTurn(found_left, found_straight, found_right);

    turn(dir);

    path[path_length] = dir;
    path_length++;

    delay(1000);
  }
}

void MazeEnd()
{
  motors.setSpeeds(SPEED, SPEED);

  while (1)
  {
    unsigned int sensors[6];
    reflectanceSensors.readLine(sensors);

    if (!ABOVE_LINE(sensors[0]) && !ABOVE_LINE(sensors[1]) && !ABOVE_LINE(sensors[2]) && !ABOVE_LINE(sensors[3]) && !ABOVE_LINE(sensors[4]) && !ABOVE_LINE(sensors[5]))
    {
      motors.setSpeeds(0, 0);
      return;
    }
  }
}
