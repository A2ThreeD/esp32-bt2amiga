#include "../include/amiga-db9-joystick.hpp"

AmigaDB9Joystick::AmigaDB9Joystick(const Pins &pins)
    : pins_(pins)
{
}

void AmigaDB9Joystick::line_set(gpio_num_t pin, bool low_active) const
{
  if (low_active)
  {
    gpio_set_level(pin, 0);
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
  }
  else
  {
    gpio_set_direction(pin, GPIO_MODE_INPUT);
  }
}

void AmigaDB9Joystick::begin()
{
  const gpio_num_t io_pins[] = {
      pins_.up, pins_.down, pins_.left, pins_.right,
      pins_.fire1, pins_.fire2, pins_.latch,
  };

  for (size_t i = 0; i < (sizeof(io_pins) / sizeof(io_pins[0])); i++)
  {
    gpio_reset_pin(io_pins[i]);
    line_set(io_pins[i], false);
  }
}

void AmigaDB9Joystick::set_state(const State &state)
{
  line_set(pins_.up, state.up);
  line_set(pins_.down, state.down);
  line_set(pins_.left, state.left);
  line_set(pins_.right, state.right);
  line_set(pins_.fire1, state.fire1);
  line_set(pins_.fire2, state.fire2);
  line_set(pins_.latch, state.latch);
}
