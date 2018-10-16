#pragma once

#define INPUT_QUEUE_SIZE 4
#define READBACK_QUEUE_SIZE 4


#define CONTROL_PORT GPIOA
#define COMMAND_DATA 15 // DC / RS
#define WRITE_STROBE 8  // WR  CHECK
#define RD 15           // read data, active low
#define CS 10           // chip select, active low
#define RESET 9

#define LED_PORT GPIOB
#define LED_PAD 1