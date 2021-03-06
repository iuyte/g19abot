/**
 * @author Ryan Benasutti, WPI
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "okapi/api/chassis/controller/chassisController.hpp"
#include "okapi/api/control/iterative/iterativePosPidController.hpp"
#include "okapi/api/util/abstractRate.hpp"
#include "okapi/api/util/logging.hpp"
#include "okapi/api/util/timeUtil.hpp"
#include <atomic>
#include <memory>
#include <tuple>

namespace okapi {
class ChassisControllerPID : public virtual ChassisController {
  public:
  /**
   * ChassisController using PID control. Puts the motors into encoder degree units. Throws a
   * std::invalid_argument exception if the gear ratio is zero.
   *
   * @param imodelArgs ChassisModelArgs
   * @param idistanceController distance PID controller
   * @param iangleController angle PID controller (keeps the robot straight)
   * @param igearset motor internal gearset and gear ratio
   * @param iscales see ChassisScales docs
   * @param ilogger The logger this instance will log to.
   */
  ChassisControllerPID(const TimeUtil &itimeUtil,
                       const std::shared_ptr<ChassisModel> &imodel,
                       std::unique_ptr<IterativePosPIDController> idistanceController,
                       std::unique_ptr<IterativePosPIDController> iturnController,
                       std::unique_ptr<IterativePosPIDController> iangleController,
                       AbstractMotor::GearsetRatioPair igearset = AbstractMotor::gearset::green,
                       const ChassisScales &iscales = ChassisScales({1, 1}, imev5GreenTPR),
                       const std::shared_ptr<Logger> &ilogger = std::make_shared<Logger>());

  ChassisControllerPID(ChassisControllerPID &&other) = delete;

  ChassisControllerPID &operator=(ChassisControllerPID &&other) = delete;

  ~ChassisControllerPID() override;

  /**
   * Drives the robot straight for a distance (using closed-loop control).
   *
   * @param itarget distance to travel
   */
  void moveDistance(QLength itarget) override;

  /**
   * Drives the robot straight for a distance (using closed-loop control).
   *
   * @param itarget distance to travel in motor degrees
   */
  void moveDistance(double itarget) override;

  /**
   * Sets the target distance for the robot to drive straight (using closed-loop control).
   *
   * @param itarget distance to travel
   */
  void moveDistanceAsync(QLength itarget) override;

  /**
   * Sets the target distance for the robot to drive straight (using closed-loop control).
   *
   * @param itarget distance to travel in motor degrees
   */
  void moveDistanceAsync(double itarget) override;

  /**
   * Turns the robot clockwise in place (using closed-loop control).
   *
   * @param idegTarget angle to turn for
   */
  void turnAngle(QAngle idegTarget) override;

  /**
   * Turns the robot clockwise in place (using closed-loop control).
   *
   * @param idegTarget angle to turn for in motor degrees
   */
  void turnAngle(double idegTarget) override;

  /**
   * Sets the target angle for the robot to turn clockwise in place (using closed-loop control).
   *
   * @param idegTarget angle to turn for
   */
  void turnAngleAsync(QAngle idegTarget) override;

  /**
   * Sets the target angle for the robot to turn clockwise in place (using closed-loop control).
   *
   * @param idegTarget angle to turn for in motor degrees
   */
  void turnAngleAsync(double idegTarget) override;

  /**
   * Delays until the currently executing movement completes.
   */
  void waitUntilSettled() override;

  /**
   * Stops the robot (set all the motors to 0 and stops the PID controllers).
   */
  void stop() override;

  /**
   * Gets the ChassisScales.
   */
  ChassisScales getChassisScales() const override;

  /**
   * Gets the GearsetRatioPair.
   */
  AbstractMotor::GearsetRatioPair getGearsetRatioPair() const override;

  /**
   * Sets the velocity mode flag. When the controller is in velocity mode, the control loop will
   * set motor velocities. When the controller is in voltage mode (ivelocityMode = false), the
   * control loop will set motor voltages. Additionally, when the controller is in voltage mode,
   * it will not obey maximum velocity limits.
   *
   * @param ivelocityMode Whether the controller should be in velocity or voltage mode.
   */
  void setVelocityMode(bool ivelocityMode);

  /**
   * Sets the gains for all controllers.
   *
   * @param idistanceGains The distance controller gains.
   * @param iturnGains The turn controller gains.
   * @param iangleGains The angle controller gains.
   */
  void setGains(const IterativePosPIDController::Gains &idistanceGains,
                const IterativePosPIDController::Gains &iturnGains,
                const IterativePosPIDController::Gains &iangleGains);

  /**
   * Gets the current controller gains.
   *
   * @return The current controller gains in the order: distance, turn, angle.
   */
  std::tuple<IterativePosPIDController::Gains,
             IterativePosPIDController::Gains,
             IterativePosPIDController::Gains>
  getGains() const;

  /**
   * Starts the internal thread. This should not be called by normal users. This method is called
   * by the ChassisControllerFactory when making a new instance of this class.
   */
  void startThread();

  /**
   * Returns the underlying thread handle.
   *
   * @return The underlying thread handle.
   */
  CrossplatformThread *getThread() const;

  protected:
  std::shared_ptr<Logger> logger;
  TimeUtil timeUtil;
  std::unique_ptr<IterativePosPIDController> distancePid;
  std::unique_ptr<IterativePosPIDController> turnPid;
  std::unique_ptr<IterativePosPIDController> anglePid;
  ChassisScales scales;
  AbstractMotor::GearsetRatioPair gearsetRatioPair;
  bool velocityMode{true};
  std::atomic_bool doneLooping{true};
  std::atomic_bool doneLoopingSeen{true};
  std::atomic_bool newMovement{false};
  std::atomic_bool dtorCalled{false};
  QTime threadSleepTime{10_ms};

  static void trampoline(void *context);
  void loop();

  bool waitForDistanceSettled();
  bool waitForAngleSettled();
  void stopAfterSettled();

  typedef enum { distance, angle, none } modeType;
  modeType mode{none};

  CrossplatformThread *task{nullptr};
};
} // namespace okapi
