## TO-DO for GeoGadget
- [ ] check if ERR is wrong
- [ ] check if ALT is wrong
- [ ] green gg: make all unused pins as OUTPUT LOW
- [ ] green gg: to wake up GPS send dummy 0xff message to GPS and wait for ACK
- [ ] to understand how it works: ```
ublox::nav_timeutc_t::valid_t &v = *((ublox::nav_timeutc_t::valid_t *) &chr);```
- [ ] make program reset the device after 25 minutes of work - to avoid hangings
- [ ] green gg: log 1 fix per xx (5-10 sec) in all modes (in sd_log)
- [ ] long-click - prepare for shutdown (sd.sync, switch all off)
- [ ] make parsing UBX-NAV-PVT instead all other tags
