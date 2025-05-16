#include <lvgl/lvgl.h>
#include "displayapp/screens/WatchFaceTerminal.h"
#include "displayapp/screens/BatteryIcon.h"
#include "displayapp/screens/NotificationIcon.h"
#include "displayapp/screens/Symbols.h"
#include "components/battery/BatteryController.h"
#include "components/ble/BleController.h"
#include "components/ble/NotificationManager.h"
#include "components/heartrate/HeartRateController.h"
#include "components/motion/MotionController.h"
#include "components/settings/Settings.h"

using namespace Pinetime::Applications::Screens;

WatchFaceTerminal::WatchFaceTerminal(Controllers::DateTime& dateTimeController,
                                     const Controllers::Battery& batteryController,
                                     const Controllers::Ble& bleController,
                                     Controllers::NotificationManager& notificationManager,
                                     Controllers::Settings& settingsController,
                                     Controllers::HeartRateController& heartRateController,
                                     Controllers::MotionController& motionController)
  : currentDateTime {{}},
    dateTimeController {dateTimeController},
    batteryController {batteryController},
    bleController {bleController},
    notificationManager {notificationManager},
    settingsController {settingsController},
    heartRateController {heartRateController},
    motionController {motionController} {
  notificationIcon = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_align(notificationIcon, nullptr, LV_ALIGN_IN_LEFT_MID, 0, -100);

  labelPrompt1 = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_align(labelPrompt1, lv_scr_act(), LV_ALIGN_IN_LEFT_MID, 0, -80);
  lv_label_set_text_static(labelPrompt1, "timod@watch:~$ now");

  labelTime = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_recolor(labelTime, true);
  lv_obj_align(labelTime, lv_scr_act(), LV_ALIGN_IN_LEFT_MID, 0, -60);

  labelDate = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_recolor(labelDate, true);
  lv_obj_align(labelDate, lv_scr_act(), LV_ALIGN_IN_LEFT_MID, 0, -40);

  labelDay = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_recolor(labelDay, true);
  lv_obj_align(labelDay, lv_scr_act(), LV_ALIGN_IN_LEFT_MID, 0, -20);

  batteryValue = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_recolor(batteryValue, true);
  lv_obj_align(batteryValue, lv_scr_act(), LV_ALIGN_IN_LEFT_MID, 0, 0);

  stepValue = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_recolor(stepValue, true);
  lv_obj_align(stepValue, lv_scr_act(), LV_ALIGN_IN_LEFT_MID, 0, 20);

  heartbeatValue = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_recolor(heartbeatValue, true);
  lv_obj_align(heartbeatValue, lv_scr_act(), LV_ALIGN_IN_LEFT_MID, 0, 40);

  connectState = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_recolor(connectState, true);
  lv_obj_align(connectState, lv_scr_act(), LV_ALIGN_IN_LEFT_MID, 0, 60);

  labelPrompt2 = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_align(labelPrompt2, lv_scr_act(), LV_ALIGN_IN_LEFT_MID, 0, 80);
  lv_label_set_text_static(labelPrompt2, "timod@watch:~$");

  taskRefresh = lv_task_create(RefreshTaskCallback, LV_DISP_DEF_REFR_PERIOD, LV_TASK_PRIO_MID, this);
  Refresh();
}

WatchFaceTerminal::~WatchFaceTerminal() {
  lv_task_del(taskRefresh);
  lv_obj_clean(lv_scr_act());
}

void WatchFaceTerminal::Refresh() {
  powerPresent = batteryController.IsPowerPresent();
  batteryPercentRemaining = batteryController.PercentRemaining();
  if (batteryPercentRemaining.IsUpdated() || powerPresent.IsUpdated()) {
    lv_label_set_text_fmt(batteryValue, "[BATT]#387b54 %d%%", batteryPercentRemaining.Get());
    if (batteryController.IsPowerPresent()) {
      lv_label_ins_text(batteryValue, LV_LABEL_POS_LAST, " Charging");
    }
  }

  bleState = bleController.IsConnected();
  bleRadioEnabled = bleController.IsRadioEnabled();
  if (bleState.IsUpdated() || bleRadioEnabled.IsUpdated()) {
    if (!bleRadioEnabled.Get()) {
      lv_label_set_text_static(connectState, "[STAT]#0082fc Disabled#");
    } else {
      if (bleState.Get()) {
        lv_label_set_text_static(connectState, "[STAT]#0082fc Connected#");
      } else {
        lv_label_set_text_static(connectState, "[STAT]#0082fc Disconnected#");
      }
    }
  }

  notificationState = notificationManager.AreNewNotificationsAvailable();
  if (notificationState.IsUpdated()) {
    if (notificationState.Get()) {
      lv_label_set_text_static(notificationIcon, "new notifications");
    } else {
      lv_label_set_text_static(notificationIcon, "");
    }
  }

  currentDateTime = std::chrono::time_point_cast<std::chrono::seconds>(dateTimeController.CurrentDateTime());
  if (currentDateTime.IsUpdated()) {
    uint8_t hour = dateTimeController.Hours();
    uint8_t minute = dateTimeController.Minutes();
    uint8_t second = dateTimeController.Seconds();

    if (settingsController.GetClockType() == Controllers::Settings::ClockType::H12) {
      char ampmChar[3] = "AM";
      if (hour == 0) {
        hour = 12;
      } else if (hour == 12) {
        ampmChar[0] = 'P';
      } else if (hour > 12) {
        hour = hour - 12;
        ampmChar[0] = 'P';
      }
      lv_label_set_text_fmt(labelTime, "[TIME]#11cc55 %02d:%02d:%02d %s#", hour, minute, second, ampmChar);
    } else {
      lv_label_set_text_fmt(labelTime, "[TIME]#11cc55 %02d:%02d:%02d", hour, minute, second);
    }

    currentDate = std::chrono::time_point_cast<std::chrono::days>(currentDateTime.Get());
    if (currentDate.IsUpdated()) {
      uint16_t year = dateTimeController.Year();
      Controllers::DateTime::Months month = dateTimeController.Month();
      uint8_t day = dateTimeController.Day();

      lv_label_set_text_fmt(labelDate, "[DATE]#007fff %02d-%02d-%04d#", char(day), char(month), short(year));
      lv_label_set_text_fmt(labelDay, "[DAY ]#cb3e1f %s", dateTimeController.DayOfWeekLongToString());
    }
  }

  heartbeat = heartRateController.HeartRate();
  heartbeatRunning = heartRateController.State() != Controllers::HeartRateController::States::Stopped;
  if (heartbeat.IsUpdated() || heartbeatRunning.IsUpdated()) {
    if (heartbeatRunning.Get()) {
      lv_label_set_text_fmt(heartbeatValue, "[L_HR]#ee3311 %d bpm#", heartbeat.Get());
    } else {
      lv_label_set_text_static(heartbeatValue, "[L_HR]#ee3311 ---#");
    }
  }

  stepCount = motionController.NbSteps();
  if (stepCount.IsUpdated()) {
    lv_label_set_text_fmt(stepValue, "[STEP]#ee3377 %lu steps#", stepCount.Get());
  }
}
