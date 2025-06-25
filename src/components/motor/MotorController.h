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
      void RingForDuration(uint8_t motorOnDuration, uint16_t motorOffDuration, uint8_t times);
      void StartRinging();
      void StopRinging();

    private:
      static void Ring(TimerHandle_t xTimer);
      static void StopMotor(TimerHandle_t xTimer);
      TimerHandle_t shortVib;
      TimerHandle_t longVib;
      uint8_t ringCount = 0;
      uint8_t motorOnDuration = 0;
      uint8_t motorOffDuration = 0;
    };
  }
}
