#include "display.h"


void Display::init(void)
{
    bus_init();
    reset();

    writecommand(0x11);
    delay(5);

    writecommand(0xD0);
    writedata(0x07);
    writedata(0x42);
    writedata(0x18);

    writecommand(0xD1);
    writedata(0x00);
    writedata(0x07);
    writedata(0x10);

    writecommand(0xD2);
    writedata(0x01);
    writedata(0x02);

    writecommand(0xC0);
    writedata(0x10);
    writedata(0x3B);
    writedata(0x00);
    writedata(0x02);
    writedata(0x11);

    writecommand(0xC5);
    writedata(0x08);

    writecommand(0xC8);
    writedata(0x00);
    writedata(0x32);
    writedata(0x36);
    writedata(0x45);
    writedata(0x06);
    writedata(0x16);
    writedata(0x37);
    writedata(0x75);
    writedata(0x77);
    writedata(0x54);
    writedata(0x0C);
    writedata(0x00);

    writecommand(0x36);
    writedata(0x0a);

    writecommand(0x3A);
    writedata(0x55);

    writecommand(0x2A);
    writedata(0x00);
    writedata(0x00);
    writedata(0x01);
    writedata(0x3F);

    writecommand(0x2B);
    writedata(0x00);
    writedata(0x00);
    writedata(0x01);
    writedata(0xDF);

    //    delay(120);
    writecommand(0x29);

    //    delay(25);
    set_orientation_landscape();
    fill(BLACK);
}


void Display::SetAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    writecommand(0x2A); // Column addr set
    writedata(x0 >> 8);
    writedata(x0 & 0xFF); // XSTART
    writedata(x1 >> 8);
    writedata(x1 & 0xFF); // XEND

    writecommand(0x2B); // Row addr set
    writedata(y0 >> 8);
    writedata(y0); // YSTART
    writedata(y1 >> 8);
    writedata(y1); // YEND

    writecommand(0x2C); // write to RAM
}


void Display::setXY(int x1, int y1, int x2, int y2) {
    writecommand(0x2A);
    writedata(x1 >> 8);
    writedata(x1);
    writedata(x2 >> 8);
    writedata(x2);

    writecommand(0x2B);
    writedata(y1 >> 8);
    writedata(y1);
    writedata(y2 >> 8);
    writedata(y2);

    writecommand(0x2C);
}


void Display::set_orientation_landscape() {
    writecommand(HX8357_MADCTL);
    writedata(0b00100000);
    width = 480;
    height = 320;
    max_x = width - 1;
    max_y = height - 1;
}


void Display::fill(uint16_t color) {
    uint32_t num_pixels = width * height;
    SetAddrWindow(0, 0, width - 1, height - 1);
    writedata(color);
    for (uint32_t i = 1; i < num_pixels; i++) {
        write_strobe();
    }
}

void Display::draw_rect(uint16_t x,
               uint16_t y,
               uint16_t w,
               uint16_t h,
               uint16_t color) {
    uint16_t _width = width;
    uint16_t _height = height;
    // rudimentary clipping (drawChar w/big text requires this)
    if ((x >= _width) || (y >= _height))
        return;
    if ((x + w - 1) >= _width)
        w = _width - x;
    if ((y + h - 1) >= _height)
        h = _height - y;

    SetAddrWindow(x, y, x + w - 1, y + h - 1);

    for (y = h; y > 0; y--) {
        for (x = w; x > 0; x--) {
            writedata(color);
        }
    }
}


void Display::hline(uint16_t x, uint16_t y, uint16_t w, uint16_t color) {
    setXY(x, y, x + w, y);
    for (int i = 0; i < w; i++) {
        writedata(color);
    }
}


void Display::vline(uint16_t x, uint16_t y, uint16_t h, uint16_t color) {
    setXY(x, y, x, y + h);
    for (int i = 0; i < h; i++) {
        writedata(color);
    }
}


void Display::border(uint16_t color) {
    hline(0, 0, max_x, color);
    hline(0, max_y, max_x, color);
    vline(0, 0, max_y, color);
    vline(max_x, 0, max_y, color);
}


void Display::print(const char* str, uint16_t x, uint16_t y, uint16_t size, uint16_t color) {
    int len = strlen(str);
    for (int pos = 0; pos < len; pos++) {
        draw_char(str[pos], x + 6 * pos * size, y, size, color);
    }
}


void Display::draw_char(unsigned char ascii,
               uint16_t x,
               uint16_t y,
               uint16_t size,
               uint16_t color) {
    char orientation = '3';
    SetAddrWindow(x, y, x + size, y + size);

    if ((ascii < 0x20) || (ascii > 0x7e)) // check for valid ASCII char
    {
        ascii = '?'; // char not supported
    }
    for (unsigned char i = 0; i < 8; i++) {
        unsigned char temp = simpleFont[ascii - 0x20][i];
        for (unsigned char f = 0; f < 8; f++) {
            if ((temp >> f) & 0x01) {
                switch (orientation) {
                    case '0':
                        draw_rect(x + f * size, y - i * size, size, size, color);
                        break;
                    case '1':
                        draw_rect(x - i * size, x - f * size, size, size, color);
                        break;
                    case '2':
                        draw_rect(x - f * size, y + i * size, size, size, color);
                        break;
                    case '3':
                    default:
                        draw_rect(x + i * size, y + f * size, size, size, color);
                }
            }
        }
    }
}
