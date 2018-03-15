#include <Arduino.h>
#include <ublox/ubxGPS.h>
#include <GPSport.h>
#include <PinButton.h>

#include "gg_cfg.hpp"
#include "gg_version.hpp"
#include "gg_int.hpp"
#include "gg_debug.hpp"
#include "gg_gps.hpp"
#include "gg_sd.hpp"
#include "gg_format.hpp"
#include "gg_display.hpp"
#include "gg_green.hpp"
#include "gg_error.hpp"

GG_Display display;

GG_Error error(&display);

D(DebugTools _debug(PD4, LOW);)    // for debug via logic analyzer - see port PD4 behaviour

const uint8_t PIN_GPS_PPS = PD3;    // Ublox pps connected to PD3 (INT1)

const uint8_t modeButtonPin = PD2;  // button connected to PD2 (INT0)
PinButton modeButton(modeButtonPin);
extern bool int_btn_event;
extern bool int_pps_event;

enum class Mode : uint8_t {LOGGING_DISPLAY, TO_LOGGING_NORMAL, LOGGING_NORMAL, TO_LOGGING_DISPLAY} mode;

static GPS gps(&gpsPort);
static gps_fix fix;

//--------------------------
void setup() {
  // DEBUG_PORT.begin(9600);

  display.init();

  // GPS device initializing
  pinMode(PIN_GPS_PPS, INPUT_PULLUP);

  gpsPort.begin(9600);

  // changing GPS_PORT baudrate
  gps.set_fast_baudrate();
  gpsPort.flush();
  gpsPort.end();
  delay(500);
  gpsPort.begin(GPS_UART_BAUDRATE_FAST);

  gps.start_running();

  uint32_t ts = 0;
  uint32_t ts_show = 0;
  do {
    if (gps.available(gpsPort)) {
      fix = gps.read();
    }
    ts = millis();
    if (ts - ts_show > 150) {
      display.show_init_screen(gps, fix);
      ts_show = ts;
    }
  } while (fix.status < gps_fix::STATUS_STD);

  // SD card initializing
  setup_sd(gps, fix, error);

  mode = Mode::TO_LOGGING_DISPLAY;
}

//--------------------------
void loop() {
  static uint32_t ts_now = 0;
  static uint32_t ts_logging_display_started = 0;

  ts_now = millis();
  modeButton.update();

  switch (mode) {

    case Mode::LOGGING_DISPLAY:
      if (gps.available(gpsPort)) {
        fix = gps.read();
        display.show_main_screen(gps, fix, gg_file_name);
        log_fix(gps, fix, error);
      }
      if (modeButton.isSingleClick() or ((ts_now - ts_logging_display_started) >= LOGGING_DISPLAY_INTERVAL)) {
        mode = Mode::TO_LOGGING_NORMAL;
      }
      break;

    case Mode::TO_LOGGING_NORMAL:
      gps.set_rate(NORMAL_RATE);
      gps.go_power_save();
      display.sleep();
      mode = Mode::LOGGING_NORMAL;
      break;

    case Mode::LOGGING_NORMAL:
      if (gps.available(gpsPort)) {
        fix = gps.read();
        log_fix(gps, fix, error);
        // D(_debug.toggle_test_pin();)
        CPU_sleepNow();
        // D(_debug.toggle_test_pin();)
      }
      if (modeButton.isSingleClick() || int_btn_event) {
        mode = Mode::TO_LOGGING_DISPLAY;
      }
      break;

    case Mode::TO_LOGGING_DISPLAY:
      gps.set_rate(FAST_RATE);  // 1Hz normally
      gps.go_power_max();
      display.init();
      int_btn_event = false;
      ts_logging_display_started = millis();
      mode = Mode::LOGGING_DISPLAY;
      break;

    default:
      ;
      break;
  }
}
