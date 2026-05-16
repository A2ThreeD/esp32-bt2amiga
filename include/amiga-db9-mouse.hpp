#pragma once

#include <stdint.h>
#include "driver/gpio.h"

class AmigaDB9Mouse
{
public:
  struct Pins
  {
    gpio_num_t xa;
    gpio_num_t xb;
    gpio_num_t ya;
    gpio_num_t yb;
    gpio_num_t left;
    gpio_num_t right;
    gpio_num_t middle;
  };

  explicit AmigaDB9Mouse(const Pins &pins);

  void begin();
  void move(int16_t dx, int16_t dy, int8_t wheel = 0);
  void set_buttons(uint8_t buttons);

private:
  static constexpr uint8_t kMouseStepDiv = 2;
  static constexpr uint32_t kQuadStepDelayUs = 300;
  static constexpr bool kSwapXY = false;
  static constexpr bool kInvertX = false;
  static constexpr bool kInvertY = false;

  static constexpr uint8_t kQuadSeq[4][2] = {
      {0, 0},
      {1, 0},
      {1, 1},
      {0, 1},
  };

  Pins pins_;
  uint8_t x_phase_;
  uint8_t y_phase_;

  void line_set(gpio_num_t pin, bool low_active) const;
  void set_axis(gpio_num_t a_pin, gpio_num_t b_pin, uint8_t phase) const;
  void set_button_line(gpio_num_t pin, bool pressed) const;
};
