#include <errno.h>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/init.h>
#include <zephyr/pm/device.h>

#if DT_HAS_COMPAT_STATUS_OKAY(pixart_pmw3610)

#define TRACKBALL_NODE DT_COMPAT_GET_ANY_STATUS_OKAY(pixart_pmw3610)

static int charybdis_trackball_wake_init(void)
{
    const struct device *trackball = DEVICE_DT_GET(TRACKBALL_NODE);

    if (!device_is_ready(trackball)) {
        return -ENODEV;
    }

    /*
     * PMW3610をsystem wake sourceとして有効化。
     *
     * これによりZMKがdeep sleepへ入る際、
     * PMW3610をPM_DEVICE_ACTION_SUSPENDの対象から外す。
     *
     * センサーがSHUTDOWNされないため、
     * MOTピンからnRF52840をWakeできる。
     */
    if (!pm_device_wakeup_enable(trackball, true)) {
        return -EIO;
    }

    return 0;
}

SYS_INIT(
    charybdis_trackball_wake_init,
    APPLICATION,
    CONFIG_APPLICATION_INIT_PRIORITY
);

#endif
