#include "devices.h"
#include "main.h"
#include "util.h"

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */

void autonomous() {
  left.setBrakeMode(okapi::AbstractMotor::brakeMode::brake);
  right.setBrakeMode(okapi::AbstractMotor::brakeMode::brake);

  auto dc = okapi::AsyncControllerFactory::motionProfile(1, 2, 10, drive);
  dc.generatePath({okapi::Point{0_ft, 0_ft, 0_deg}, okapi::Point{2_ft, 2_ft, 0_deg}}, "A");

  for (auto &&i : {1, 2, 3, 4}) {
    dc.setTarget("A");
    dc.waitUntilSettled();
    pros::delay(150);
    drive.turnAngle(90);
  }
}
