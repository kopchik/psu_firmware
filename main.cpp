#include <stdio.h>
#include <string.h>

#include "ch.h"
#include "hal.h"

#include "shell.h"
#include "chprintf.h"

#include "usbcfg.h"
#include "simple_font.h"

#define SHELL_WA_SIZE   THD_WORKING_AREA_SIZE(2048)

/*
 * TODO:
DB9 not connected
*/

typedef uint16_t u16;
typedef uint8_t u8;


IOBus busA = {GPIOA, 0xFF, 0};
IOBus busB = {GPIOB, 0xFF, 0};

#define CONTROL_PORT GPIOA
#define COMMAND_DATA 15 // DC / RS
#define WRITE_STROBE 8  // WR  CHECK
#define RD 15  // read data, active low CHECK
#define CS 10  // chip select, active low CHECK 
#define RESET 9  // CHECK

#define CHIP_ENABLE palClearPad(CONTROL_PORT, CS)
#define CHIP_DISABLE palSetPad(CONTROL_PORT, CS)
#define WRITE_BEGIN palClearPad(CONTROL_PORT, WRITE_STROBE);
#define WRITE_END palSetPad(CONTROL_PORT, WRITE_STROBE);
#define WRITE_STROBE2 {palClearPad(CONTROL_PORT, WRITE_STROBE); palSetPad(CONTROL_PORT, WRITE_STROBE);};
#define SEND_COMMAND palClearPad(CONTROL_PORT, COMMAND_DATA);
#define SEND_DATA palSetPad(CONTROL_PORT, COMMAND_DATA);

#define WHITE 0xFFFF
#define BLACK 0x0000

void delay(uint16_t msec) {
    chThdSleepMilliseconds(msec);
}


void bus_init(void) {
  palSetBusMode(&busA, PAL_MODE_OUTPUT_PUSHPULL);
  palSetBusMode(&busB, PAL_MODE_OUTPUT_PUSHPULL);

  palSetPadMode(CONTROL_PORT, COMMAND_DATA, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(CONTROL_PORT, WRITE_STROBE, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(CONTROL_PORT, RD,           PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(CONTROL_PORT, CS,           PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(CONTROL_PORT, RESET,        PAL_MODE_OUTPUT_PUSHPULL);

  palClearPad(CONTROL_PORT, COMMAND_DATA);
  palClearPad(CONTROL_PORT, WRITE_STROBE);
  palSetPad(CONTROL_PORT, RD);
  CHIP_ENABLE;
}


class Display {
public:
    uint16_t width = 0;
    uint16_t height = 0;
void reset(void) {
  palClearPad(CONTROL_PORT, RESET);
  delay(10);
  palSetPad(CONTROL_PORT, RESET);
  delay(200);
}

void write_bus(uint16_t data) {
  uint8_t byte_low, byte_high;
  byte_low = data & 0xFF;
  byte_high = (data >> 8) & 0xFF;

  palWriteBus(&busA, byte_low);
  palWriteBus(&busB, byte_high);
  WRITE_STROBE2;
}


void writecommand(uint8_t data) {
  SEND_COMMAND;
  write_bus(data);
}


void writedata(uint16_t data) {
  SEND_DATA;
  write_bus(data);
}

void init(void) {
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


void SetAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
  writecommand(0x2A);  // Column addr set
  writedata(x0 >> 8);
  writedata(x0 & 0xFF);  // XSTART
  writedata(x1 >> 8);
  writedata(x1 & 0xFF);  // XEND

  writecommand(0x2B);  // Row addr set                                                                                                             
  writedata(y0 >> 8);                                                                                                                          
  writedata(y0);  // YSTART                                                                                                                    
  writedata(y1 >> 8);                                                                                                                          
  writedata(y1);  // YEND                                                                                                                      

  writecommand(0x2C);  // write to RAM                                                                                                             
} 

void setXY(int x1, int y1, int x2, int y2) {                                                                                                   
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

#define HX8357_MADCTL  0x36
void set_orientation_landscape() {
    writecommand(HX8357_MADCTL);
    writedata(0b00100000);
    width = 480;
    height = 320;
}

void fill(uint16_t color) {
    uint32_t num_pixels = width * height;
    SetAddrWindow(0, 0, width-1, height-1);
    writedata(color);
    for(uint32_t i=1; i<num_pixels; i++) {
        WRITE_STROBE2;
    }}



  void draw_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color = WHITE) {
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
void printf(u16 x, u16 y, u16 size, u16 color, const char* fmt, ...) {
    char buf[100];
    va_list args;
    va_start(args, fmt);
//    vsnprintf(buf, sizeof(buf), fmt, args);
    chsnprintf(buf, sizeof(buf), fmt, args);
    print(buf, x, y, size, color);
    va_end(args);
  }

  void print(const char* str, u16 x, u16 y, u16 size = 4, u16 color = WHITE) {                                                                         
    int len = strlen(str);                                                                                                                    
    for (int pos = 0; pos < len; pos++) {                                                                                           
      draw_char(str[pos], x + 6 * pos * size, y, size, color);                                                                                   
    }        
  }
  
  void draw_char(unsigned char ascii,
                 uint16_t x,
                 uint16_t y,
                 uint16_t size = 4,
                 uint16_t color = WHITE) {
    char orientation = '3';
    SetAddrWindow(x, y, x + size, y + size);

    if ((ascii < 0x20) || (ascii > 0x7e))  // check for valid ASCII char
    {
      ascii = '?';  // char not supported
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
};

char old_string[100];
class StringWidget {
private:
    u16 x,y;
    char *buffer;
    size_t buffer_size;
    u8 fontsize;
    Display display;
    uint16_t bgcolor;
    uint16_t fgcolor;
    uint16_t maxlen;
public:
    StringWidget(
            u16 _x, 
            u16 _y,
            char *_buffer,
            size_t _buffer_size,
            u8 _fontsize, 
            Display _display, 
            uint16_t _bgcolor, 
            uint16_t _fgcolor):
    x(_x), 
    y(_y),
    buffer(_buffer), 
    buffer_size(_buffer_size),
    fontsize(_fontsize),
    display(_display),
    bgcolor(_bgcolor), 
    fgcolor(_fgcolor)  {
    }
    
    void print(const char *string) {
        // erase old string
        display.print(buffer, x, y, fontsize, bgcolor);
        // print new string
        display.print(string, x, y, fontsize, fgcolor);
//      // TODO: check for security / buffer overflow problems
        strcpy(buffer, string);
    }
    
};

void blink_led(void) {
//    while (true) {
      palWriteBus(&busB, 0xFF);
      chThdSleepMilliseconds(1);

      palWriteBus(&busB, 0x00);
      chThdSleepMilliseconds(200);
//    }
}

static adcsample_t samples[10];
static const ADCConversionGroup adcgrpcfg1 = {
  FALSE,  // circular
  1,  // num channels
  NULL,  //  read cb
  NULL,  // err cb
  ADC_SQR1_NUM_CH(1),  // CR1
  ADC_CR2_TSVREFE, // CR2
  ADC_SMPR1_SMP_VREF(ADC_SAMPLE_239P5) | ADC_SMPR1_SMP_SENSOR(ADC_SAMPLE_239P5),  // SMPR1
  0,      // SMPR2
  0,      // SQR1
  0,      // SQR2
  ADC_SQR3_SQ1_N(ADC_CHANNEL_SENSOR)      // SQR3
};


static THD_WORKING_AREA(waThread1, 256);
static __attribute__((noreturn)) THD_FUNCTION(Thread1, arg) {
  (void)arg;
  chRegSetThreadName("display");

  bus_init();
  Display display = Display();
  display.init();
  display.fill(BLACK);

  char buf[20];
  StringWidget widget = StringWidget(100, 100, buf, sizeof(buf), 4, display, BLACK, WHITE);
//  widget.print("TEST");
//  delay(1000);
//  widget.print("haBA aba");
//  delay(1000);
//  widget.print("xXx");
  char buf2[10];
  while (1) {
    adcConvert(&ADCD1, &adcgrpcfg1, samples, 1);
    chsnprintf(buf2, sizeof(buf2), "%u", samples[0]);
    widget.print(buf2);
    blink_led();
    delay(1000);
  }
//  display.print("test", 100, 100, 4, WHITE);
  while (1) {}
}


static void cmd_reboot(BaseSequentialStream *chp, int argc, char *argv[]) {
	(void) argc;
	(void) argv;
	chprintf(chp, "Rebooting...\r\n");
	chThdSleepMilliseconds(100);
	NVIC_SystemReset();
}


static const ShellCommand commands[] = {
    {"reboot", cmd_reboot},
    {NULL, NULL}
};


static const ShellConfig shell_cfg1 = {
  (BaseSequentialStream *)&SDU1,
  commands
};



int main(void) {
  halInit();
  chSysInit();
  sduObjectInit(&SDU1);
  sduStart(&SDU1, &serusbcfg);

  adcStart(&ADCD1, NULL);

  
  /*
   * Activates the USB driver and then the USB bus pull-up on D+.
   * Note, a delay is inserted in order to not have to disconnect the cable
   * after a reset.
   */
  usbDisconnectBus(serusbcfg.usbp);
  chThdSleepMilliseconds(1500);
  usbStart(serusbcfg.usbp, &usbcfg);
  usbConnectBus(serusbcfg.usbp);

  shellInit();

  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

  while (true) {
    if (SDU1.config->usbp->state == USB_ACTIVE) {
      thread_t *shelltp = chThdCreateFromHeap(NULL, SHELL_WA_SIZE,
                                              "shell", NORMALPRIO + 1,
                                              shellThread, (void *)&shell_cfg1);
      chThdWait(shelltp);
    }
    chThdSleepMilliseconds(1000);
  }
}
