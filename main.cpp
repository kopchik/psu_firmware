#include <stdio.h>
#include <string.h>

#include "ch.h"
#include "hal.h"

#include "chprintf.h"
#include "shell.h"


#include "usbcfg.h"

#include "display/display.h"

#include "config.h"
#include "macros.h"


/*
 * TODO:
DB9 not connected
 */


IOBus busA = { GPIOA, 0xFF, 0 };
IOBus busB = { GPIOB, 0xFF, 0 };

#define SEND_COMMAND palClearPad(CONTROL_PORT, COMMAND_DATA);
#define SEND_DATA palSetPad(CONTROL_PORT, COMMAND_DATA);

void delay(uint16_t msec) {
  chThdSleepMilliseconds(msec);
}


class MyDisplay: public Display {
    void reset(void) {
      palClearPad(CONTROL_PORT, RESET_PAD);
      delay(10);
      palSetPad(CONTROL_PORT, RESET_PAD);
      delay(200);
    }

    void chip_enable() {
      palClearPad(CONTROL_PORT, CHIP_SELECT_PIN);
    }

    void bus_init() {
      palSetBusMode(&busA, PAL_MODE_OUTPUT_PUSHPULL);
      palSetBusMode(&busB, PAL_MODE_OUTPUT_PUSHPULL);

      palSetPadMode(CONTROL_PORT, COMMAND_DATA, PAL_MODE_OUTPUT_PUSHPULL);
      palSetPadMode(CONTROL_PORT, WRITE_PIN, PAL_MODE_OUTPUT_PUSHPULL);
      palSetPadMode(CONTROL_PORT, CHIP_SELECT_PIN, PAL_MODE_OUTPUT_PUSHPULL);
      palSetPadMode(CONTROL_PORT, RESET_PAD, PAL_MODE_OUTPUT_PUSHPULL);

      palClearPad(CONTROL_PORT, COMMAND_DATA);
      palClearPad(CONTROL_PORT, WRITE_PIN);
      palSetPad(CONTROL_PORT, COMMAND_DATA);
      chip_enable();
    }

    void write_bus(uint16_t data) {
      uint8_t byte_low, byte_high;
      byte_low = data & 0xFF;
      byte_high = (data >> 8) & 0xFF;

      palWriteBus(&busA, byte_low);
      palWriteBus(&busB, byte_high);
      write_strobe();
    }

    void writecommand(uint8_t data)  {
      SEND_COMMAND;
      write_bus(data);
    }

    void writedata(uint16_t data) {
      SEND_DATA;
      write_bus(data);
    }

    inline void write_strobe() {
        palClearPad(CONTROL_PORT, WRITE_PIN);
        palSetPad(CONTROL_PORT, WRITE_PIN);
    }

    void delay(uint16_t msec) {
      chThdSleepMilliseconds(msec);
    }

    void printf(uint16_t x, uint16_t y, uint16_t size, uint16_t color, const char* fmt, ...) {
      char buf[100];
      va_list args;
      va_start(args, fmt);
      //    vsnprintf(buf, sizeof(buf), fmt, args);
      chsnprintf(buf, sizeof(buf), fmt, args);
      print(buf, x, y, size, color);
      va_end(args);
    }

};


template<int SIZE = 10, int FONTSIZE = 3>
class StringWidget
{
private:
  uint16_t x, y;
  char buffer[SIZE];
  size_t buffer_size = SIZE;
  uint8_t fontsize = FONTSIZE;
  Display* display;
  uint16_t bgcolor = BLACK;
  uint16_t fgcolor = WHITE;

public:
  inline StringWidget& position(uint16_t _x, uint16_t _y)
  {
    x = _x;
    y = _y;
    return *this;
  }

  inline StringWidget& disp(Display* _display)
  {
    display = _display;
    return *this;
  }

  inline StringWidget& color(uint16_t fg, uint16_t bg)
  {
    fgcolor = fg;
    bgcolor = bg;
    return *this;
  }

  inline StringWidget& size(uint8_t _fontsize)
  {
    fontsize = _fontsize;
    return *this;
  }

  void print(const char* string)
  {
    // TODO: print only changed chars
    // erase old string
    display->print(buffer, x, y, fontsize, bgcolor);
    // print new string
    display->print(string, x, y, fontsize, fgcolor);
    // TODO: check for security / buffer overflow problems
    strcpy(buffer, string);
  }
};

class Pad {
public:
  ioportid_t port;
  uint32_t pad;

  Pad(ioportid_t _port, uint32_t _pad)
    : port(_port)
    , pad(_pad) {
    palSetPadMode(port, pad, PAL_MODE_OUTPUT_PUSHPULL);
  }

  void toggle(uint32_t msec=0) {
    palTogglePad(port, pad);
    delay(msec);
  }

  void on() { palSetPad(port, pad); }

  void off() { palClearPad(port, pad); }

  void blink() {
    while (1) {
      on();
      delay(1);
      off();
      delay(200);
    }
  }
};



Pad led = Pad(LED_PORT, LED_PAD);

STATIC_MAILBOX(input_mbox, INPUT_QUEUE_SIZE);
//extern mailbox_t input_mbox;
STATIC_MAILBOX(readback, READBACK_QUEUE_SIZE);

/*
ADC
*/
STATIC_MAILBOX(readback_mbox, 4);

static adcsample_t samples[10];
static const ADCConversionGroup adcgrpcfg1 = {
  FALSE,              // circular
  1,                  // num channels
  NULL,               //  read cb
  NULL,               // err cb
  ADC_SQR1_NUM_CH(1), // CR1
  ADC_CR2_TSVREFE,    // CR2
  ADC_SMPR1_SMP_VREF(ADC_SAMPLE_239P5) |
    ADC_SMPR1_SMP_SENSOR(ADC_SAMPLE_239P5), // SMPR1
  0,                                        // SMPR2
  0,                                        // SQR1
  0,                                        // SQR2
  ADC_SQR3_SQ1_N(ADC_CHANNEL_SENSOR)        // SQR3
};

//static THD_WORKING_AREA(waReadbackThread, 128);
static __attribute__((noreturn)) THD_FUNCTION(ReadbackThread, arg) {
  (void) arg;
  chRegSetThreadName("readback");

  adcStart(&ADCD1, NULL);

  while (1) {
    adcConvert(&ADCD1, &adcgrpcfg1, samples, 1);
    msg_t value = samples[0];
    chMBPostI(&readback_mbox, value);
    delay(100);
  }
}

#define offset(idx, size) ((size * 8 + 4) * idx)

class Channel {
public:
  StringWidget<6, 3> vread;
  StringWidget<6, 3> iread;

  StringWidget<6, 3> vset;
  StringWidget<6, 3> iset;

  Channel(Display* display, uint16_t horiz_offset, uint8_t size = 3) {
    int vert_offset = 6;
    int gap = 8;
    int vert_step = size * 8 + gap;

    vread.position(horiz_offset, vert_offset)
      .size(size)
      .color(GREEN, BLACK)
      .disp(display);

    vert_offset += vert_step;
    iread.position(horiz_offset, vert_offset)
      .size(size)
      .color(RED, BLACK)
      .disp(display);

    vert_offset += vert_step;
    vset.position(horiz_offset, vert_offset)
      .size(size)
      .color(GREEN, BLACK)
      .disp(display);

    vert_offset += vert_step;
    iset.position(horiz_offset, vert_offset).color(RED, BLACK).disp(display);
  }

  void update() {
    vread.print("3.341V");
    iread.print("0.001A");
    vset.print("3.34V");
    iset.print("1.00A");
  }
};

char printf_buf[20];
MyDisplay display;
Channel channel1(&display, 3);
Channel channel2(&display, 120 + 3);
Channel channel3(&display, 240 + 3);
Channel channel4(&display, 360 + 3);

static THD_WORKING_AREA(waDisplayThread, 256);
static __attribute__((noreturn)) THD_FUNCTION(DisplayThread, arg)
{
  (void)arg;
  chRegSetThreadName("display");
  display.init();

  display.border();

  display.vline(120, 0, display.max_y);
  display.vline(240, 0, display.max_y);
  display.vline(360, 0, display.max_y);

  channel1.update();
  channel2.update();
  channel3.update();
  channel4.update();

  while (1) {
    msg_t msg;
    if (chMBFetchTimeout(&input_mbox, &msg, 1000) == MSG_OK) {
      //        chsnprintf(printf_buf, sizeof(printf_buf), "%d", msg);
      //        encoder_widget.print(printf_buf);

    }

    led.toggle();

    if (chMBFetchI(&readback_mbox, &msg) == MSG_OK) {
      uint16_t raw_val = msg;
      double adc_res = 3.3 / 4096;
      double Vcur = raw_val * adc_res;
      double V25 = 1.3565;
      double slope = 0.0043;
      float temp = (V25 - Vcur) / slope + 25;
      chsnprintf(printf_buf, sizeof(printf_buf), "%.1f", temp);
      //        adc.print(printf_buf);
    }
    delay(1);
  }
}

static void
cmd_reboot(BaseSequentialStream* chp, int argc, char* argv[])
{
  (void)argc;
  (void)argv;
  chprintf(chp, "Rebooting...\r\n");
  chThdSleepMilliseconds(100);
  NVIC_SystemReset();
}

static const ShellCommand commands[] = { { "reboot", cmd_reboot },
                                         { NULL, NULL } };

static const ShellConfig shell_cfg1 = { (BaseSequentialStream*)&SDU1,
                                        commands };

int
main(void)
{
  halInit();
  chSysInit();
  sduObjectInit(&SDU1);
  sduStart(&SDU1, &serusbcfg);

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

//  chThdCreateStatic(waReadbackThread, sizeof(waReadbackThread), NORMALPRIO, ReadbackThread, NULL);
//  chThdCreateStatic(waInput, sizeof(waInput), NORMALPRIO, InputThread, NULL);
  chThdCreateStatic(waDisplayThread, sizeof(waDisplayThread), NORMALPRIO, DisplayThread, NULL);

  while (true) {
    if (SDU1.config->usbp->state == USB_ACTIVE) {
      thread_t* shelltp = chThdCreateFromHeap(NULL,
                                              SHELL_WA_SIZE,
                                              "shell",
                                              NORMALPRIO + 1,
                                              shellThread,
                                              (void*)&shell_cfg1);
      chThdWait(shelltp);
    }
    chThdSleepMilliseconds(1000);
  }
}
