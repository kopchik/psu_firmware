#pragma once

#include <cstdint>

#include "hal.h"


class QEI {
public:
    ioportid_t port;
    uint32_t pad1;
    uint32_t pad2;
    uint8_t state = 0;
    uint16_t count = 0; // TODO: overflow?
    uint16_t value = 0;
    uint16_t old_value = 0;

    QEI(ioportid_t _port, uint32_t _pad1, uint32_t _pad2);

    int scan();
    int scan_relative();
};
