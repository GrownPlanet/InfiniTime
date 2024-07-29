#pragma once

#include <FreeRTOS.h>
#include <timers.h>
#include <cstdint>

namespace Pinetime {
  namespace Controllers {

    class MotorController {
    public:
      MotorController() = default;

      void Init();
      void RunForDuration(uint8_t motorDuration);
      void RingPeriodically(uint8_t times);
      void StartRinging();
      void StopRinging();

    private:
      static void Ring(TimerHandle_t xTimer);
      static void StopMotor(TimerHandle_t xTimer);
      static void _RingPeriodically(TimerHandle_t xTimer);

      TimerHandle_t shortVib;
      TimerHandle_t longVib;
      TimerHandle_t ringPeriodically;

      uint8_t timesToRing;
      uint8_t timesRung;
    };
  }
}
