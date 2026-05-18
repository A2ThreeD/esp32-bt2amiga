/*
ESP32-BT2AMIGA
Bluepad32 migration runtime for Amiga DB9 output.
*/

#include "nvs_flash.h"
#include "esp_log.h"
#include <driver/gpio.h>
#include <cstring>

extern "C" {
#include <btstack_port_esp32.h>
#include <btstack_run_loop.h>
#include <btstack_stdio_esp32.h>
#include <uni.h>
}

#include "../include/amiga-db9-mouse.hpp"
#include "../include/amiga-db9-joystick.hpp"

#define BT2AMIGA_MOUSE_DEBUG 1
#define BT2AMIGA_JOYSTICK_DEBUG 1

static constexpr char const *TAG = "BT2AMIGA Main Module";
static constexpr uint16_t kCodMajorMask = 0x1f00;
static constexpr uint16_t kCodMinorMask = 0x00fc;
static constexpr uint16_t kCodMajorPeripheral = 0x0500;
static constexpr uint16_t kCodMinorKeyboard = 0x0040;

// Amiga DB9 output pins
static constexpr int MOUSE_XA_PIN = 25;
static constexpr int MOUSE_XB_PIN = 26;
static constexpr int MOUSE_YA_PIN = 27;
static constexpr int MOUSE_YB_PIN = 14;
static constexpr int MOUSE_LEFT_BTN_PIN = 33;
static constexpr int MOUSE_RIGHT_BTN_PIN = 32;
static constexpr int MOUSE_MIDDLE_BTN_PIN = 13;

static AmigaDB9Mouse mouse({
    static_cast<gpio_num_t>(MOUSE_XA_PIN),
    static_cast<gpio_num_t>(MOUSE_XB_PIN),
    static_cast<gpio_num_t>(MOUSE_YA_PIN),
    static_cast<gpio_num_t>(MOUSE_YB_PIN),
    static_cast<gpio_num_t>(MOUSE_LEFT_BTN_PIN),
    static_cast<gpio_num_t>(MOUSE_RIGHT_BTN_PIN),
    static_cast<gpio_num_t>(MOUSE_MIDDLE_BTN_PIN),
});

static AmigaDB9Joystick joystick({
    static_cast<gpio_num_t>(MOUSE_YB_PIN),
    static_cast<gpio_num_t>(MOUSE_XA_PIN),
    static_cast<gpio_num_t>(MOUSE_YA_PIN),
    static_cast<gpio_num_t>(MOUSE_XB_PIN),
    static_cast<gpio_num_t>(MOUSE_LEFT_BTN_PIN),
    static_cast<gpio_num_t>(MOUSE_RIGHT_BTN_PIN),
    static_cast<gpio_num_t>(MOUSE_MIDDLE_BTN_PIN),
});

static constexpr int kAnalogDeadzone = 140; // Bluepad32 axis range is roughly [-512, 511]

static void apply_gamepad_to_outputs(const uni_gamepad_t* gp) {
    bool up = (gp->dpad & DPAD_UP) != 0;
    bool down = (gp->dpad & DPAD_DOWN) != 0;
    bool left = (gp->dpad & DPAD_LEFT) != 0;
    bool right = (gp->dpad & DPAD_RIGHT) != 0;

    // Analog fallback when dpad is neutral
    if (!up && !down && !left && !right) {
        if (gp->axis_y < -kAnalogDeadzone)
            up = true;
        else if (gp->axis_y > kAnalogDeadzone)
            down = true;

        if (gp->axis_x < -kAnalogDeadzone)
            left = true;
        else if (gp->axis_x > kAnalogDeadzone)
            right = true;
    }

    const bool fire1 = (gp->buttons & BUTTON_A) != 0;
    const bool fire2 = (gp->buttons & BUTTON_B) != 0;
    const bool latch = (gp->buttons & BUTTON_X) != 0;

    AmigaDB9Joystick::State out;
    out.up = up;
    out.down = down;
    out.left = left;
    out.right = right;
    out.fire1 = fire1;
    out.fire2 = fire2;
    out.latch = latch;
    joystick.set_state(out);

    if (BT2AMIGA_JOYSTICK_DEBUG) {
        ESP_LOGI(TAG, "JOY U=%d D=%d L=%d R=%d F1=%d F2=%d LATCH=%d AX=%ld AY=%ld DPAD=0x%02x BTN=0x%04x",
                 up, down, left, right, fire1, fire2, latch,
                 (long)gp->axis_x, (long)gp->axis_y,
                 gp->dpad, gp->buttons);
    }

    // Optional mouse output from right stick + AB buttons
    int16_t mx = 0;
    int16_t my = 0;
    if (gp->axis_rx > kAnalogDeadzone)
        mx = 2;
    else if (gp->axis_rx < -kAnalogDeadzone)
        mx = -2;

    if (gp->axis_ry > kAnalogDeadzone)
        my = 2;
    else if (gp->axis_ry < -kAnalogDeadzone)
        my = -2;

    if (mx != 0 || my != 0)
        mouse.move(mx, my, 0);
    mouse.set_buttons((fire1 ? 1 : 0) | (fire2 ? 2 : 0));

    if (BT2AMIGA_MOUSE_DEBUG && (mx != 0 || my != 0 || fire1 || fire2)) {
        ESP_LOGI(TAG, "MOUSE B=%u X=%d Y=%d", (unsigned)((fire1 ? 1 : 0) | (fire2 ? 2 : 0)), mx, my);
    }
}

static void my_platform_init(int argc, const char** argv) {
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);
}

static void my_platform_on_init_complete(void) {
    uni_bt_start_scanning_and_autoconnect_unsafe();
    uni_bt_allow_incoming_connections(true);
}

static uni_error_t my_platform_on_device_discovered(bd_addr_t addr, const char* name, uint16_t cod, uint8_t rssi) {
    (void)addr;
    (void)rssi;

    // Keep this project mouse/gamepad only: reject Bluetooth keyboards.
    const bool is_peripheral = (cod & kCodMajorMask) == kCodMajorPeripheral;
    const bool has_keyboard_bit = (cod & kCodMinorMask & kCodMinorKeyboard) != 0;
    if (is_peripheral && has_keyboard_bit) {
        ESP_LOGI(TAG, "ignoring keyboard-class device: %s (cod=0x%04x)", name ? name : "<unknown>", cod);
        return UNI_ERROR_IGNORE_DEVICE;
    }

    return UNI_ERROR_SUCCESS;
}

static void my_platform_on_device_connected(uni_hid_device_t* d) {
    ESP_LOGI(TAG, "device connected: %p", d);
}

static void my_platform_on_device_disconnected(uni_hid_device_t* d) {
    ESP_LOGI(TAG, "device disconnected: %p", d);
}

static uni_error_t my_platform_on_device_ready(uni_hid_device_t* d) {
    ESP_LOGI(TAG, "device ready: %p", d);
    return UNI_ERROR_SUCCESS;
}

static void my_platform_on_controller_data(uni_hid_device_t* d, uni_controller_t* ctl) {
    ARG_UNUSED(d);
    if (ctl->klass != UNI_CONTROLLER_CLASS_GAMEPAD)
        return;

    apply_gamepad_to_outputs(&ctl->gamepad);
}

static const uni_property_t* my_platform_get_property(uni_property_idx_t idx) {
    ARG_UNUSED(idx);
    return nullptr;
}

static void my_platform_on_oob_event(uni_platform_oob_event_t event, void* data) {
    ARG_UNUSED(data);
    if (event == UNI_PLATFORM_OOB_BLUETOOTH_ENABLED) {
        ESP_LOGI(TAG, "Bluetooth enabled");
    }
}

extern "C" struct uni_platform* get_my_platform(void) {
    static struct uni_platform plat = {
        .name = "bt2amiga",
        .init = my_platform_init,
        .on_init_complete = my_platform_on_init_complete,
        .on_device_discovered = my_platform_on_device_discovered,
        .on_device_connected = my_platform_on_device_connected,
        .on_device_disconnected = my_platform_on_device_disconnected,
        .on_device_ready = my_platform_on_device_ready,
        .on_gamepad_data = nullptr,
        .on_controller_data = my_platform_on_controller_data,
        .get_property = my_platform_get_property,
        .on_oob_event = my_platform_on_oob_event,
        .device_dump = nullptr,
        .register_console_cmds = nullptr,
    };
    return &plat;
}

extern "C" void app_main(void) {
    gpio_reset_pin(GPIO_NUM_2);
    gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_2, 1);

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    mouse.begin();
    joystick.begin();

#ifdef CONFIG_ESP_CONSOLE_UART
#ifndef CONFIG_BLUEPAD32_USB_CONSOLE_ENABLE
    btstack_stdio_init();
#endif
#endif

    btstack_init();
    uni_platform_set_custom(get_my_platform());
    uni_init(0, NULL);
    btstack_run_loop_execute();
}
