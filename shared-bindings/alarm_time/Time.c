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

#include "py/obj.h"
#include "shared-bindings/alarm_time/__init__.h"

//| """alarm_time module
//|
//| The `alarm_time` module implements deep sleep."""

STATIC mp_obj_t alarm_time_duration(mp_obj_t seconds_o) {
    #if MICROPY_PY_BUILTINS_FLOAT
    mp_float_t seconds = mp_obj_get_float(seconds_o);
    mp_float_t msecs = 1000.0f * seconds + 0.5f;
    #else
    mp_int_t seconds = mp_obj_get_int(seconds_o);
    mp_int_t msecs = 1000 * seconds;
    #endif

    if (seconds < 0) {
        mp_raise_ValueError(translate("sleep length must be non-negative"));
    }
    common_hal_alarm_time_duration(msecs);

    alarm_time_obj_t *self = m_new_obj(alarm_time_obj_t);
    self->base.type = &alarm_time_type;

    return self;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(alarm_time_duration_obj, alarm_time_duration);

STATIC mp_obj_t alarm_time_disable(void) {
    common_hal_alarm_time_disable();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(alarm_time_disable_obj, alarm_time_disable);

STATIC const mp_rom_map_elem_t alarm_time_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_alarm_time) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Duration), MP_ROM_PTR(&alarm_time_duration_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Disable), MP_ROM_PTR(&alarm_time_disable_obj) },
};
STATIC MP_DEFINE_CONST_DICT(alarm_time_module_globals, alarm_time_module_globals_table);

const mp_obj_module_t alarm_time_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&alarm_time_module_globals,
};

const mp_obj_type_t alarm_time_type = {
    { &mp_type_type },
    .name = MP_QSTR_timeAlarm,
};
