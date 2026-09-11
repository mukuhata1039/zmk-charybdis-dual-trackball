/*
 * Prevent only the left trackball X/Y movement from waking
 * the USB host while the PC is suspended.
 *
 * Keyboard keys and encoder scrolling are NOT blocked.
 */

#define DT_DRV_COMPAT zmk_input_processor_usb_suspend_blocker

#include <zephyr/device.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/dt-bindings/input/input-event-codes.h>

#include <drivers/input_processor.h>

#include <zmk/usb.h>


static int usb_suspend_blocker_handle_event(
    const struct device *dev,
    struct input_event *event,
    uint32_t param1,
    uint32_t param2,
    struct zmk_input_processor_state *state)
{
    ARG_UNUSED(dev);
    ARG_UNUSED(param1);
    ARG_UNUSED(param2);
    ARG_UNUSED(state);

    if (zmk_usb_get_status() != USB_DC_SUSPEND) {
        return ZMK_INPUT_PROC_CONTINUE;
    }

    if (event->type == INPUT_EV_REL &&
        (event->code == INPUT_REL_X ||
         event->code == INPUT_REL_Y)) {

        return ZMK_INPUT_PROC_STOP;
    }

    return ZMK_INPUT_PROC_CONTINUE;
}


static struct zmk_input_processor_driver_api
    usb_suspend_blocker_driver_api = {

    .handle_event = usb_suspend_blocker_handle_event,
};


#define USB_SUSPEND_BLOCKER_INST(n)                    \
    DEVICE_DT_INST_DEFINE(                             \
        n,                                             \
        NULL,                                          \
        NULL,                                          \
        NULL,                                          \
        NULL,                                          \
        POST_KERNEL,                                   \
        CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,           \
        &usb_suspend_blocker_driver_api                 \
    );


DT_INST_FOREACH_STATUS_OKAY(
    USB_SUSPEND_BLOCKER_INST
)
