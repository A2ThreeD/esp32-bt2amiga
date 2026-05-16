#include "../include/amiga-db9-mouse.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

AmigaDB9Mouse::AmigaDB9Mouse(const Pins &pins)
    : pins_(pins), x_phase_(0), y_phase_(0)
{
}

void AmigaDB9Mouse::begin()
{
  const gpio_num_t io_pins[] = {
      pins_.xa, pins_.xb, pins_.ya, pins_.yb,
      pins_.left, pins_.right, pins_.middle,
  };

  for (size_t i = 0; i < (sizeof(io_pins) / sizeof(io_pins[0])); i++)
  {
    gpio_reset_pin(io_pins[i]);
    line_set(io_pins[i], false);
  }

  set_axis(pins_.xa, pins_.xb, x_phase_);
  set_axis(pins_.ya, pins_.yb, y_phase_);

  set_button_line(pins_.left, false);
  set_button_line(pins_.right, false);
  set_button_line(pins_.middle, false);
}

void AmigaDB9Mouse::line_set(gpio_num_t pin, bool low_active) const
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

void AmigaDB9Mouse::set_axis(gpio_num_t a_pin, gpio_num_t b_pin, uint8_t phase) const
{
  const uint8_t p = phase & 0x03;
  line_set(a_pin, kQuadSeq[p][0] == 0);
  line_set(b_pin, kQuadSeq[p][1] == 0);
}

void AmigaDB9Mouse::move(int16_t dx, int16_t dy, int8_t wheel)
{
  (void)wheel;
  int16_t x = dx;
  int16_t y = dy;

  if (kSwapXY)
  {
    const int16_t t = x;
    x = y;
    y = t;
  }
  if (kInvertX)
  {
    x = static_cast<int16_t>(-x);
  }
  if (kInvertY)
  {
    y = static_cast<int16_t>(-y);
  }

  while (x != 0 || y != 0)
  {
    const bool skip_x = ((x % kMouseStepDiv) != 0);
    const bool skip_y = ((y % kMouseStepDiv) != 0);

    if (x != 0 && !skip_x)
    {
      x_phase_ = (x < 0) ? static_cast<uint8_t>((x_phase_ + 3) & 0x03) : static_cast<uint8_t>((x_phase_ + 1) & 0x03);
      set_axis(pins_.xa, pins_.xb, x_phase_);
    }
    if (x < 0)
      x++;
    else if (x > 0)
      x--;

    if (y != 0 && !skip_y)
    {
      y_phase_ = (y < 0) ? static_cast<uint8_t>((y_phase_ + 3) & 0x03) : static_cast<uint8_t>((y_phase_ + 1) & 0x03);
      set_axis(pins_.ya, pins_.yb, y_phase_);
    }
    if (y < 0)
      y++;
    else if (y > 0)
      y--;

    ets_delay_us(kQuadStepDelayUs);
  }
}

void AmigaDB9Mouse::set_button_line(gpio_num_t pin, bool pressed) const
{
  line_set(pin, pressed);
}

void AmigaDB9Mouse::set_buttons(uint8_t buttons)
{
  set_button_line(pins_.left, (buttons & 0b001) != 0);
  set_button_line(pins_.right, (buttons & 0b010) != 0);
  set_button_line(pins_.middle, (buttons & 0b100) != 0);
}
