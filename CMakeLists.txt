zephyr_library()

# ZMK本体の内部ヘッダーを、この外部モジュールから参照する
zephyr_library_include_directories(${CMAKE_SOURCE_DIR}/include)

zephyr_library_sources(src/trackball_wake.c)

zephyr_library_sources_ifdef(
    CONFIG_ZMK_RGB_UNDERGLOW
    src/dongle_led_suspend.c
)

zephyr_library_sources_ifdef(
    CONFIG_ZMK_USB
    src/trackball_usb_suspend_filter.c
)
