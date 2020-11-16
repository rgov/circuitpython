/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Scott Shawcroft
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "shared-bindings/alarm/__init__.h"

//| """Light and deep sleep used to save power
//|
//| The `sleep` module provides sleep related functionality. There are two supported levels of
//| sleep, light and deep.
//|
//| Light sleep leaves the CPU and RAM powered so that CircuitPython can resume where it left off
//| after being woken up. Light sleep is automatically done by CircuitPython when `time.sleep()` is
//| called. To light sleep until a non-time alarm use `sleep.sleep_until_alarm()`. Any active
//| peripherals, such as I2C, are left on.
//|
//| Deep sleep shuts down power to nearly all of the chip including the CPU and RAM. This can save
//| a more significant amount of power, but CircuitPython must start code.py from the beginning when woken
//| up. CircuitPython will enter deep sleep automatically when the current program exits without error
//| or calls `sys.exit(0)`.
//| If an error causes CircuitPython to exit, error LED error flashes will be done periodically.
//| An error includes an uncaught exception, or sys.exit called with a non-zero argumetn.
//| To set alarms for deep sleep use `sleep.restart_on_alarm()` they will apply to next deep sleep only."""
//|

//| wake_alarm: Alarm
//| """The most recent alarm to wake us up from a sleep (light or deep.)"""
//|

//| reset_reason: ResetReason
//| """The reason the chip started up from reset state. This can may be power up or due to an alarm."""
//|

//| def sleep_until_alarm(alarm: Alarm, ...) -> Alarm:
//|     """Performs a light sleep until woken by one of the alarms. The alarm that woke us up is
//|        returned."""
//|     ...
//|

STATIC mp_obj_t sleep_sleep_until_alarm(size_t n_args, const mp_obj_t *args) {
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(sleep_sleep_until_alarm_obj, 1, MP_OBJ_FUN_ARGS_MAX, sleep_sleep_until_alarm);

//| def restart_on_alarm(alarm: Alarm, ...) -> None:
//|     """Set one or more alarms to wake up from a deep sleep. When awakened, ``code.py`` will restart
//| from the beginning. The last alarm to wake us up is available as `wake_alarm`. """
//|     ...
//|
STATIC mp_obj_t sleep_restart_on_alarm(size_t n_args, const mp_obj_t *args) {
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(sleep_restart_on_alarm_obj, 1, MP_OBJ_FUN_ARGS_MAX, sleep_restart_on_alarm);


mp_map_elem_t sleep_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_sleep) },

    { MP_ROM_QSTR(MP_QSTR_wake_alarm), mp_const_none },
    { MP_ROM_QSTR(MP_QSTR_reset_reason), mp_const_none },

    { MP_ROM_QSTR(MP_QSTR_sleep_until_alarm), sleep_sleep_until_alarm_obj },
    { MP_ROM_QSTR(MP_QSTR_restart_on_alarm), sleep_restart_on_alarm_obj },
};
STATIC MP_DEFINE_MUTABLE_DICT(sleep_module_globals, sleep_module_globals_table);

// These are called from common hal code to set the current wake alarm.
void common_hal_sleep_set_wake_alarm(mp_obj_t alarm) {
    // Equivalent of:
    // sleep.wake_alarm = alarm
    mp_map_elem_t *elem =
        mp_map_lookup(&sleep_module_globals_table, MP_ROM_QSTR(MP_QSTR_wake_alarm), MP_MAP_LOOKUP);
    if (elem) {
        elem->value = alarm;
    }
}

// These are called from common hal code to set the current wake alarm.
void common_hal_sleep_set_reset_reason(mp_obj_t reset_reason) {
    // Equivalent of:
    // sleep.reset_reason = reset_reason
    mp_map_elem_t *elem =
        mp_map_lookup(&sleep_module_globals_table, MP_ROM_QSTR(MP_QSTR_reset_reason), MP_MAP_LOOKUP);
    if (elem) {
        elem->value = reset_reason;
    }
}

const mp_obj_module_t sleep_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&sleep_module_globals,
};
