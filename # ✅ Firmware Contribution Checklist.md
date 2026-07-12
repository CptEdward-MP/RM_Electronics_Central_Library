# ✅ Firmware Contribution Checklist

## Architecture

* [ ] Follows layering: Application → Modules → Devices → Drivers → Platform
* [ ] No HAL usage outside platform layer
* [ ] No mixing of abstraction layers

---

## Code Rules

* [ ] Written in **standard C (C99+)** (except platform/drivers)
* [ ] No platform-specific headers outside platform
* [ ] No hardcoded values
* [ ] Proper error handling (no ignored return values)

---

## Drivers

* [ ] Platform independent
* [ ] Uses IO abstraction (function pointers)
* [ ] No direct HAL usage

---

## Devices (Sensors / Actuators)

* [ ] Clean API (`init`, `read`, `control`)
* [ ] No register-level exposure
* [ ] Returns usable/scaled data
* [ ] Uses drivers only

---

## Modules (PID, filters, etc.)

* [ ] Fully hardware independent
* [ ] No driver/HAL usage

---

## Documentation

* [ ] `README.md` added
* [ ] Usage explained
* [ ] Dependencies listed

---

## Testing (MANDATORY)

* [ ] Tested on hardware
* [ ] Test method documented
* [ ] Hardware used mentioned
* [ ] Limitations listed
* [ ] Failure cases mentioned

---

## Examples

* [ ] Example added in `examples/`

---

## Git / PR

* [ ] PR includes clear description
* [ ] Problem being solved explained
* [ ] Hardware tested (Yes/No) mentioned

---

## Final Check

* [ ] Code is reusable
* [ ] Code is platform-independent
* [ ] Code can be understood by next batch

