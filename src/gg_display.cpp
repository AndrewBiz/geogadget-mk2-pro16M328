#include "gg_cfg.hpp"
#include "gg_display.hpp"
#include "gg_version.hpp"
#include "gg_format.hpp"

const uint8_t number_spin_steps = 8;
const char spin_step[number_spin_steps] PROGMEM = {'|','/','-','\\','|','/','-','\\'};


// Definition section
//-----------------------------------------------------------------------------
void GG_Display::init() {
  _display.begin();
  _display.setFont(u8x8_font_artossans8_r);
};

//-----------------------------------------------------------------------------
void GG_Display::show_1st_screen(void) {
  _display.setCursor(1, 3);
  _display.print(F(GG_NAME));
  _display.setCursor(4, 4);
  _display.print(F("v" GG_VERSION));
}

//-----------------------------------------------------------------------------
void GG_Display::clear() {
  _display.clear();
}

//-----------------------------------------------------------------------------
void GG_Display::sleep() {
  _display.setPowerSave(1); //activates power save on display
};

//-----------------------------------------------------------------------------
void GG_Display::wakeup() {
  _display.setPowerSave(0); //deactivates power save on display
};

//-----------------------------------------------------------------------------
uint8_t GG_Display::_satellites_ok(const NMEAGPS & gps) {
  uint8_t sats_ok = 0;
  for (uint8_t i=0; i < gps.sat_count; i++) {
    if ((gps.satellites[i].tracked) && (gps.satellites[i].snr > SAT_GOOD_SNR)) {
      sats_ok++;
    }
  }
  return sats_ok;
};

const char format_sta_sat[] PROGMEM = "ST %1d SAT: %02d(%02d)";
//-----------------------------------------------------------------------------
void GG_Display::_sta_sat(const NMEAGPS & gps, const gps_fix & fix) {
  char _buf[17];
  sprintf_P(_buf, format_sta_sat,
    fix.status,
    _satellites_ok(gps),
    fix.satellites
  );
  _display.print(_buf);
}

//-----------------------------------------------------------------------------
void GG_Display::_ymd(const NMEAGPS & gps, const gps_fix & fix) {
  char _buf[17];
  _display.print(F("YMD: "));
  _display.print(
    format_date( _buf, '-',
      fix.dateTime.full_year(fix.dateTime.year),
      fix.dateTime.month,
      fix.dateTime.date
    )
  );
  _display.print(F(" "));
}

//-----------------------------------------------------------------------------
void GG_Display::_hms(const NMEAGPS & gps, const gps_fix & fix) {
  char _buf[17];
  _display.print(F("HMS: "));
  _display.print(
    format_time( _buf, ':',
      fix.dateTime.hours,
      fix.dateTime.minutes,
      fix.dateTime.seconds
    )
  );
  _display.print(F("   "));
}

const char format_sat_found[] PROGMEM = "GOOD:%02d TOTAL:%02d";  // "GOOD:02 TOTAL:12"
//-----------------------------------------------------------------------------
void GG_Display::show_init_screen(const NMEAGPS & gps, const gps_fix & fix) {
  char _buf[17];
  static uint8_t spin_phase = 0;

  // _display.setFont(u8x8_font_artossans8_r);

  _display.setCursor(1, 0);
  _display.print(F(GG_NAME));
  _display.setCursor(4, 1);
  _display.print(F("v" GG_VERSION));

  _display.setCursor(3, 2);
  _display.print(F("Satellites:"));
  _display.setCursor(1, 2);
  // spin phases clockwise: "|/-\|/-\"
  _display.print((char)pgm_read_byte(&(spin_step[spin_phase++])));
  if (spin_phase >= number_spin_steps) spin_phase = 0;

  _display.setCursor(0, 3);
  sprintf_P(_buf, format_sat_found,
    _satellites_ok(gps),
    fix.satellites
  );
  _display.print(_buf);

  if ((!fix.valid.time) || (!fix.valid.date)) _display.setInverseFont(1);
  _display.setCursor(0, 4);
  _ymd(gps, fix);
  _display.setCursor(0, 5);
  _hms(gps, fix);
  _display.setInverseFont(0);

  _display.setCursor(0, 6);
  if (!fix.valid.location || fix.status < gps_fix::STATUS_STD)
    _display.setInverseFont(1);
  _display.print(F("LAT: "));
  _display.print(format_location(_buf, fix.latitudeL()));

  _display.setCursor(0, 7);
  _display.print(F("LON: "));
  _display.print(format_location(_buf, fix.longitudeL()));
  _display.setInverseFont(0);
}

//-----------------------------------------------------------------------------
void GG_Display::show_main_screen(const NMEAGPS & gps, const gps_fix & fix, char* gg_file_name) {
  char _buf[17];

  // _display.setFont(u8x8_font_artossans8_r);

  _display.setCursor(0, 0);
  _sta_sat(gps, fix);

  if ((!fix.valid.time) || (!fix.valid.date)) _display.setInverseFont(1);
  _display.setCursor(0, 1);
  _ymd(gps, fix);
  _display.setCursor(0, 2);
  _hms(gps, fix);
  _display.setInverseFont(0);

  _display.setCursor(0, 3);
  if (!fix.valid.location || fix.status < gps_fix::STATUS_STD)
    _display.setInverseFont(1);
  _display.print(F("LAT: "));
  _display.print(format_location(_buf, fix.latitudeL()));

  _display.setCursor(0, 4);
  _display.print(F("LON: "));
  _display.print(format_location(_buf, fix.longitudeL()));
  _display.setInverseFont(0);

  _display.setCursor(0, 5);
  if (!fix.valid.altitude) _display.setInverseFont(1);
  static const char fmt_dsp_alt[] PROGMEM = "ALT: %+6d m";
  sprintf_P(_buf,
    fmt_dsp_alt,
    int(fix.altitude())
  );
  _display.print(_buf);
  _display.setInverseFont(0);

  _display.setCursor(0, 6);
  static const char fmt_dsp_err[] PROGMEM = "ERR: %4d,%4d m";
  sprintf_P(_buf,
    fmt_dsp_err,
    int(fix.lat_err()),
    int(fix.alt_err())
  );
  _display.print(_buf);

  _display.setCursor(0, 7);
  _display.print(F("LOG: "));
  _display.print(gg_file_name);
}

//-----------------------------------------------------------------------------
void GG_Display::show_error_screen(const uint8_t error_type, PGM_P error_str) {
  char _buf[17];
  _display.clear();
  // _display.setFont(u8x8_font_artossans8_r);
  _display.setCursor(4, 2);
  _display.print(F("ERROR ("));
  _display.print(error_type);
  _display.print(F(")"));

  _display.setCursor(0, 4);
  strcpy_P(_buf, error_str);
  _display.print(_buf);
}
