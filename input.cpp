#include "config.h"
#include "macros.h"

#include "ch.h"
#include "hal.h"

#include "qei.h"

STATIC_MAILBOX(input_mbox, INPUT_QUEUE_SIZE);
static QEI encoder = QEI(GPIOC, 13, 14);

THD_WORKING_AREA(waInput, 256);
__attribute__((noreturn)) THD_FUNCTION(InputThread, arg) {
    (void) arg;
    chRegSetThreadName("input");

    int value = 0;
    while (1) {
        value = encoder.scan_relative();

        if (value != 0) {
            chMBPostI(&input_mbox, value);
        }

        chThdSleepMilliseconds(1);
    }
}