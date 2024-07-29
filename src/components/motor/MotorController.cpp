#include "components/motor/MotorController.h"
#include <hal/nrf_gpio.h>
#include "systemtask/SystemTask.h"
#include "drivers/PinMap.h"

using namespace Pinetime::Controllers;

void MotorController::Init() {
  nrf_gpio_cfg_output(PinMap::Motor);
  nrf_gpio_pin_set(PinMap::Motor);

  shortVib = xTimerCreate("shortVib", 1, pdFALSE, nullptr, StopMotor);
  longVib = xTimerCreate("longVib", pdMS_TO_TICKS(1000), pdTRUE, this, Ring);

  ringPeriodically = xTimerCreate("ringPeriodically", pdMS_TO_TICKS(70), pdFALSE, this, _RingPeriodically);
}

void MotorController::Ring(TimerHandle_t xTimer) {
  auto* motorController = static_cast<MotorController*>(pvTimerGetTimerID(xTimer));
  motorController->RunForDuration(50);
}

void MotorController::RunForDuration(uint8_t motorDuration) {
  if (motorDuration > 0 && xTimerChangePeriod(shortVib, pdMS_TO_TICKS(motorDuration), 0) == pdPASS && xTimerStart(shortVib, 0) == pdPASS) {
    nrf_gpio_pin_clear(PinMap::Motor);
  }
}

void MotorController::StartRinging() {
  RunForDuration(50);
  xTimerStart(longVib, 0);
}

void MotorController::StopRinging() {
  xTimerStop(longVib, 0);
  nrf_gpio_pin_set(PinMap::Motor);
}

void MotorController::StopMotor(TimerHandle_t /*xTimer*/) {
  nrf_gpio_pin_set(PinMap::Motor);
}

void MotorController::RingPeriodically(uint8_t times) {
  timesToRing = times;
  timesRung = 0;

  if (xTimerChangePeriod(ringPeriodically, 0, 0) == pdPASS) {
    xTimerStart(ringPeriodically, 0);
  }
}

void MotorController::_RingPeriodically(TimerHandle_t xTimer) {
  auto* motorController = static_cast<MotorController*>(pvTimerGetTimerID(xTimer));

  motorController->RunForDuration(35);
  motorController->timesRung++;
  xTimerStop(motorController->ringPeriodically, 0);

  if (
    (motorController->timesRung < motorController->timesToRing) && 
    (xTimerChangePeriod(motorController->ringPeriodically, pdMS_TO_TICKS(450), 0) == pdPASS)
  ) {
    xTimerStart(motorController->ringPeriodically, 0);
  }
}
