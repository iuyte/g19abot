#include "autonomous.h"
#include "util.h"

#define side_red 1
#define side_blue -1

void autonFlags(int side) {
  // set the intake to on
  intake.move(127);

  // drive forward and grab the ball
  drive::dpc.setTarget("F1");
  drive::dpc.waitUntilSettled();

  // ready the catapult
  catapult::ready();

  // back up
  drive::dpc.setTarget("F2", true);
  drive::dpc.waitUntilSettled();

  // turn towards the flags
  drive::dc.turnAngle(-93_deg * side);

  // drive forward a bit
  drive::dpc.setTarget("F3");
  drive::dpc.waitUntilSettled();

  waitUntil(catapult::atTarget(), 20);

  // wait a bit
  drive::moveVelocity(0, 0);
  delay(250);

  // fire the catapult
  catapult::fire();
  waitUntil(catapult::pot.get() > catapult::presets[3], 20);
  delay(250);

  // reverse intake for cap flipping
  intake.move(-127);

  // hit the bottom flag
  drive::dc.setMaxVelocity(125);
  drive::dc.turnAngle(-24_deg * side);
  drive::dc.moveDistance(20_in);
  drive::dc.turnAngle(24_deg * side);
  drive::dc.moveDistance(22_in);
  drive::dc.moveDistance(-10_in);
}

void flipCap(int side) {
  // back up from the wall
  drive::dpc.setTarget("FC1", true);
  drive::dpc.waitUntilSettled();

  // turn towards the cap
  drive::dc.turnAngle(100_deg * side);

  // flip the cap
  drive::dc.setMaxVelocity(85);
  drive::dc.moveDistance(24_in);

  // back away
  drive::dpc.setTarget("F3", true);
  drive::dpc.waitUntilSettled();

  // stop the intake
  intake.move(0);
}

void autonRedFlags() {
  autonFlags(side_red);
  flipCap(side_red);
}

void autonBlueFlags() {
  autonFlags(side_blue);
  flipCap(side_blue);
}
