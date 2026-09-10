#include <stdbool.h>

#include <zmk/event_manager.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/rgb_underglow.h>
#include <zmk/usb.h>


/*
 * ============================================================
 * Dongle RGB LED USB Suspend control
 *
 * PC稼働中:
 *   PL9823 = 通常通り点灯
 *
 * PCスリープ / シャットダウン:
 *   USB_DC_SUSPEND を検出
 *   PL9823 = OFF
 *
 * PC復帰:
 *   Suspend前に点灯していた場合のみ復帰
 * ============================================================
 */


static bool restore_led_after_suspend = false;
static bool currently_suspended = false;


static int dongle_led_usb_listener(const zmk_event_t *eh)
{
    const struct zmk_usb_conn_state_changed *event =
        as_zmk_usb_conn_state_changed(eh);

    if (event == NULL) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    enum usb_dc_status_code usb_status = zmk_usb_get_status();


    /*
     * --------------------------------------------------------
     * PCがUSBをSuspend
     * --------------------------------------------------------
     */
    if (usb_status == USB_DC_SUSPEND) {

        if (!currently_suspended) {

            bool led_is_on = false;

            /*
             * Suspend前のLED状態を保存。
             *
             * 手動でLEDを消していた場合は、
             * PC復帰時にも勝手に点灯させない。
             */
            if (zmk_rgb_underglow_get_state(&led_is_on) == 0) {
                restore_led_after_suspend = led_is_on;
            } else {
                restore_led_after_suspend = false;
            }

            zmk_rgb_underglow_off();

            currently_suspended = true;
        }

        return ZMK_EV_EVENT_BUBBLE;
    }


    /*
     * --------------------------------------------------------
     * USBがSuspendから復帰
     * --------------------------------------------------------
     *
     * CONFIGURED / RESUME / SOF 等、
     * Suspend以外のUSBイベントが来たら復帰扱い。
     */
    if (currently_suspended) {

        currently_suspended = false;

        if (restore_led_after_suspend) {
            zmk_rgb_underglow_on();
        }

        restore_led_after_suspend = false;
    }


    return ZMK_EV_EVENT_BUBBLE;
}


ZMK_LISTENER(dongle_led_usb, dongle_led_usb_listener);
ZMK_SUBSCRIPTION(dongle_led_usb, zmk_usb_conn_state_changed);
