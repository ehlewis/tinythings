#pragma once
#include <Arduino.h>

struct MoonImage {
    const uint8_t* data;
    uint16_t width;
    uint16_t height;
    int16_t offsetX;
    int16_t offsetY;
};
