#include "py/nlr.h"
#include "py/obj.h"
#include "py/objproperty.h"
#include "py/runtime.h"

#include "src/rp2_common/hardware_gpio/include/hardware/gpio.h"
#include "src/rp2_common/hardware_timer/include/hardware/timer.h"

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/time/__init__.h"

#include "bindings/pps/PPSPin.h"


#define PPS_GPIO_INVALID 0xFF
#define MAX_PPS_GPIOS MP_ARRAY_SIZE(pps_gpios)


uint pps_gpios[] = {
    PPS_GPIO_INVALID,
    PPS_GPIO_INVALID,
};

uint64_t pps_times[MAX_PPS_GPIOS] = { 0 };


STATIC void gpio_callback(uint gpio, uint32_t event_mask) {
    for (size_t i = 0; i < MAX_PPS_GPIOS; i++) {
        if (pps_gpios[i] == gpio) {
            pps_times[i] = common_hal_time_monotonic_ns();
            break;
        }
    }
}


typedef struct {
    mp_obj_base_t base;
    const mcu_pin_obj_t *pin;
} pps_ppspin_obj_t;


//| class PPSPin:
//|     def __init__(
//|         self,
//|         pin: microcontroller.Pin
//|     ) -> None:
//|         """Pulse-Per-Second timekeeping."""
//|
//|         :param ~microcontroller.Pin pin: the PPS pin
//|         """
STATIC mp_obj_t pps_ppspin_make_new(const mp_obj_type_t *type, size_t n_args,
                                    size_t n_kw, const mp_obj_t *all_args) {
    // Parse arguments
    enum { ARG_pin };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pin, MP_ARG_OBJ | MP_ARG_REQUIRED },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args,
        MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Create a new PPSPin object
    pps_ppspin_obj_t *self = mp_obj_malloc(pps_ppspin_obj_t, &pps_ppspin_type);

    // Claim the pin, ensuring that it is not already in use
    self->pin = validate_obj_is_free_pin(args[ARG_pin].u_obj, MP_QSTR_pin);
    claim_pin(self->pin);
    gpio_set_dir(self->pin->number, GPIO_IN);

    // Record the pin number for the interrupt handler
    size_t i;
    bool found = false;
    for (i = 0; i < MAX_PPS_GPIOS; i++) {
        if (pps_gpios[i] == PPS_GPIO_INVALID) {
            pps_gpios[i] = self->pin->number;
            found = true;
            break;
        }
    }

    if (!found) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_RuntimeError,
            MP_ERROR_TEXT("No more PPS GPIOs available")));
    }

    // For the first object, set up the interrupt handler
    if (i == 0) {
        gpio_set_irq_enabled_with_callback(
            self->pin->number, GPIO_IRQ_EDGE_RISE, true,
            &gpio_callback
        );
    } else {
        gpio_set_irq_enabled(self->pin->number, GPIO_IRQ_EDGE_RISE, true);
    }

    return MP_OBJ_FROM_PTR(self);
}


//|     gpio: microcontroller.Pin
//|     """The associated PPS pin."""
//|
STATIC mp_obj_t pps_ppspin_get_pin(mp_obj_t self_in) {
    pps_ppspin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_FROM_PTR(self->pin);
}
MP_DEFINE_CONST_FUN_OBJ_1(pps_ppspin_get_pin_obj, pps_ppspin_get_pin);
MP_PROPERTY_GETTER(pps_ppspin_pin_obj, (mp_obj_t)&pps_ppspin_get_pin_obj);


//|     timestamp: int
//|     """The timestamp of the last pulse."""
//|
STATIC mp_obj_t pps_ppspin_get_timestamp(mp_obj_t self_in) {
    pps_ppspin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    for (size_t i = 0; i < MAX_PPS_GPIOS; i++) {
        if (pps_gpios[i] == self->pin->number) {
            return mp_obj_new_int_from_ull(pps_times[i]);
        }
    }
    return mp_const_none;  // Should be unreachable
}
MP_DEFINE_CONST_FUN_OBJ_1(pps_ppspin_get_timestamp_obj,
                          pps_ppspin_get_timestamp);
MP_PROPERTY_GETTER(pps_ppspin_timestamp_obj,
                   (mp_obj_t)&pps_ppspin_get_timestamp_obj);


// Locals dictionary for the PPSPin class
STATIC const mp_rom_map_elem_t pps_ppspin_locals_dict_table[] = {
    // .pin property
    { MP_ROM_QSTR(MP_QSTR_pin), MP_ROM_PTR(&pps_ppspin_pin_obj) },

    // .timestamp property
    { MP_ROM_QSTR(MP_QSTR_timestamp), MP_ROM_PTR(&pps_ppspin_timestamp_obj) },
};
STATIC MP_DEFINE_CONST_DICT(
    pps_ppspin_locals_dict,
    pps_ppspin_locals_dict_table
);


MP_DEFINE_CONST_OBJ_TYPE(
    pps_ppspin_type,
    MP_QSTR_PPSPin,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, pps_ppspin_make_new,
    locals_dict, &pps_ppspin_locals_dict
);
