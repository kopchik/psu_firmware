//#ifndef PSU_FIRMWARE_DISPLAY_H
//#define PSU_FIRMWARE_DISPLAY_H
#pragma once

#include <stdint.h>
#include <cstring>
#include <cstdarg>

#include "simple_font.h"

#define HX8357_MADCTL 0x36

#define WHITE 0xFFFF
#define BLACK 0x0000
#define RED 0xF800
#define GREEN 0x07E0
#define DARKGREEN 0x03E0
#define BLUE 0x001F
#define ORANGE 0xFD20
#define MAROON 0x7800


class Display {
public:
    uint16_t width = 0;
    uint16_t height = 0;
    uint16_t max_x = 0;
    uint16_t max_y = 0;

    void init(void);
    void SetAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
    void setXY(int x1, int y1, int x2, int y2);
    void set_orientation_landscape();
    void fill(uint16_t color);
    void draw_rect(uint16_t x,
                   uint16_t y,
                   uint16_t w,
                   uint16_t h,
                   uint16_t color = WHITE);
    void hline(uint16_t x, uint16_t y, uint16_t w, uint16_t color = WHITE);
    void vline(uint16_t x, uint16_t y, uint16_t h, uint16_t color = WHITE);
    void border(uint16_t color = WHITE);
    void print(const char* str, uint16_t x, uint16_t y, uint16_t size = 4, uint16_t color = WHITE);
    void draw_char(unsigned char ascii,
                   uint16_t x,
                   uint16_t y,
                   uint16_t size = 4,
                   uint16_t color = WHITE);

    virtual void reset(void) = 0;
    virtual void bus_init(void) = 0;
    virtual void write_strobe() = 0;
    virtual void write_bus(uint16_t data) = 0;
    virtual void writecommand(uint8_t data) = 0;
    virtual void writedata(uint16_t data) = 0;
    virtual void delay(uint16_t msec) = 0;
};
