#pragma once

#define INPUT_QUEUE_SIZE 4
#define READBACK_QUEUE_SIZE 4
#define SHELL_WA_SIZE THD_WORKING_AREA_SIZE(2048)

#define CONTROL_PORT GPIOA
#define COMMAND_DATA 15 // DC / RS
#define WRITE_PIN 8     // WRITE
#define CHIP_SELECT_PIN 10           // chip select, active low
#define RESET_PAD 9

#define LED_PORT GPIOB
#define LED_PAD 1