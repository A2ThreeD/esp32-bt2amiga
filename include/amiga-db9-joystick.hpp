#pragma once

#include <stdint.h>
#include "driver/gpio.h"

class AmigaDB9Joystick
{
public:
  struct Pins
  {
    gpio_num_t up;
    gpio_num_t down;
    gpio_num_t left;
    gpio_num_t right;
    gpio_num_t fire1;
    gpio_num_t fire2;
    gpio_num_t latch;
  };

  struct State
  {
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
    bool fire1 = false;
    bool fire2 = false;
    bool latch = false;
  };

  explicit AmigaDB9Joystick(const Pins &pins);

  void begin();
  void set_state(const State &state);

private:
  Pins pins_;

  void line_set(gpio_num_t pin, bool low_active) const;
};
