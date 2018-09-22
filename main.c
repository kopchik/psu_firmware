#include <stdio.h>
#include <string.h>

#include "ch.h"
#include "hal.h"

#include "shell.h"
#include "chprintf.h"

#include "usbcfg.h"

#define SHELL_WA_SIZE   THD_WORKING_AREA_SIZE(2048)

/*
DB9 not connected
*/


static const ShellCommand commands[] = {
  {NULL, NULL}
};

static const ShellConfig shell_cfg1 = {
  (BaseSequentialStream *)&SDU1,
  commands
};


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
  //CHIP_DISABLE;
}


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


void blink(ioportid_t port, iopadid_t pin) {
  while (true) {
    palClearPad(port, pin);
    chThdSleepMilliseconds(500);
    palSetPad(port, pin);
    chThdSleepMilliseconds(500);
  }

}


void init(void) {
    reset();
    
    writecommand(0x11);
    delay(20);
    
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

    delay(120);
    writecommand(0x29);

    delay(25);
}



void blink_led(void) {
    while (true) {
      palWriteBus(&busB, 0xFF);
      chThdSleepMilliseconds(1);

      palWriteBus(&busB, 0x00);
      chThdSleepMilliseconds(200);
    }
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

static THD_WORKING_AREA(waThread1, 128);
static __attribute__((noreturn)) THD_FUNCTION(Thread1, arg) {
  (void)arg;
  chRegSetThreadName("blinker");


  bus_init();
  // DEBUG
  //blink(CONTROL_PORT, WRITE_STROBE);
  //blink(CONTROL_PORT, RD);
  //blink(CONTROL_PORT, RESET);
  //blink(CONTROL_PORT, CS);
  //blink(CONTROL_PORT, COMMAND_DATA);
  init();
  SetAddrWindow(0, 0, 320-1, 320-1);
  for (int i=0; i<40000; i++) {
    wrihttps://ae01.alicdn.com/kf/HTB1sfchvr1YBuNjSszeq6yblFXaM/3-2-inch-TFT-LCD-Touch-Screen-Module-Display-Ultra-HD-320X240-ILI9341-for-3-2.jpg_50x50.jpgtedata(0xFFFF);
  }
  blink_led();

//  set_rotation();
//  set_window();

  systime_t time = 1000;
  while (true) {
    //on();
    //SEND_COMMAND;
    //WRITE_BEGIN;
    //invert_on();
    palWriteBus(&busB, 1);
    chThdSleepMilliseconds(time);

    //off();
    //invert_off();
    //SEND_DATA;
    //WRITE_END;
    palWriteBus(&busB, 0);
    chThdSleepMilliseconds(time);
  }
}


int main(void) {
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
