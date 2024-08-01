# SIT310-FinalProject

# Black Line Maze Runner

## Overview

The Black Line Maze Runner is a robot designed to navigate a maze by following black lines on a white surface. This project utilizes an Arduino microcontroller, infrared sensors, and DC motors to achieve autonomous navigation through the maze.

![Maze Runner](images/maze_runner.jpg)

## Features

- **Autonomous Navigation**: Follows black lines and makes decisions at intersections.
- **Obstacle Detection**: Detects and avoids obstacles within the maze.
- **Speed Control**: Adjustable speed settings for different maze complexities.

## Components

- **Microcontroller**: Arduino Uno
- **Sensors**: IR sensors for line detection
- **Motors**: DC motors with motor driver
- **Chassis**: 2-wheel drive chassis
- **Power Supply**: Battery pack

## Setup

1. **Hardware Setup**:
    - Connect IR sensors to the appropriate analog pins on the Arduino.
    - Connect DC motors to the motor driver and then to the Arduino.
    - Mount all components on the chassis.

2. **Software Setup**:
    - Install the Arduino IDE from [Arduino's official website](https://www.arduino.cc/en/software).
    - Clone this repository: 
      ```sh
      git clone https://github.com/yourusername/black-line-maze-runner.git
      ```
    - Open `maze_runner.ino` in the Arduino IDE.
    - Upload the code to the Arduino board.

## Code Explanation

- **maze_runner.ino**: Main file containing the setup and loop functions.
- **sensors.h**: Handles sensor data processing.
- **motors.h**: Controls motor actions and speed adjustments.

## How It Works

1. **Initialization**: The robot initializes sensors and motors in the setup function.
2. **Line Following**: In the loop function, the robot continuously reads sensor data to follow the black line.
3. **Decision Making**: At intersections, the robot makes decisions based on predefined logic to navigate through the maze.
4. **Obstacle Avoidance**: If an obstacle is detected, the robot stops and reroutes.

## Usage

1. Place the robot at the start of the maze on the black line.
2. Turn on the power supply.
3. The robot will start navigating the maze autonomously.

## Customization

- **Speed Adjustment**: Modify the `motorSpeed` variable in `motors.h` to change the speed.
- **Sensor Sensitivity**: Adjust the threshold values in `sensors.h` to fine-tune sensor sensitivity.

## Troubleshooting

- **Robot Not Moving**: Ensure all connections are secure and the power supply is sufficient.
- **Inaccurate Line Following**: Calibrate the IR sensors and adjust threshold values.

## Contributing

Contributions are welcome! Please fork the repository and submit a pull request with your changes.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Thanks to the open-source community for providing valuable resources and inspiration.
- Special thanks to Prof. Akan Coskun for project guidance and support.


