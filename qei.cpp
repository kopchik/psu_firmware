#include "qei.h"


QEI::QEI(ioportid_t _port, uint32_t _pad1, uint32_t _pad2)
        : port(_port)
        , pad1(_pad1)
        , pad2(_pad2) {
    palSetPadMode(port, pad1, PAL_MODE_INPUT_PULLUP);
    palSetPadMode(port, pad2, PAL_MODE_INPUT_PULLUP);
}


int QEI::scan()
{
    // idea from
    // https://www.eevblog.com/forum/projects/quadrature-rotary-encoder/msg1291497/#msg1291497

    bool pad1_state = palReadPad(port, pad1);
    bool pad2_state = palReadPad(port, pad2);

    state = (state << 2) & 0b1111;
    if (pad1_state) {
        state |= 0b01;
    }
    if (pad2_state) {
        state |= 0b10;
    }

    switch (state) {
        // Forward
        case 0b0010:
        case 0b1011:
        case 0b1101:
        case 0b0100:
            count++;
            break;

            // Reverse
        case 0b0001:
        case 0b0111:
        case 0b1110:
        case 0b1000:
            count--;
            break;
    }

    if (count % 4 == 0) {
        value = count / 4;
    }

    return value;
}


int QEI::scan_relative() {
    int result = 0;
    int new_value = scan();
    if (new_value == old_value) {
        result = 0;
    } else if (new_value > old_value) {
        result = 1;
    } else {
        result = -1;
    }
    old_value = new_value;
    return result;
}
