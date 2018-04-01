# Project logbook

## Power save modes
### MAX Power
device current: 77mA-100mA (~90mA)

### SLEEP_MODE_PWR_DOWN
wake_up takes 1000us

#### device current:
  external antenna eats 9mA
  good signal: 55mA - 77mA (with antenna)
  bad signal:  46mA - 63mA (without antenna)

## Memory use

|FLASH |RAM  |Comment                      |
|------|-----|-----------------------------|
|27620 |1504 | initial build (mod v1.00)
|27608 |1499 | re-factored, cleaned v2.03.02|
|27794 |1500 | re-factored, v2.07.04
|27810 |1499 | opt init screen, added 1st screen
|27862 |1500 | added 2hz flashing during GPS init 
