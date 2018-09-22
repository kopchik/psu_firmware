#define ILI9341_GRAM 0x2C
#define ILI9341_MAC 0x36

void on(void) {
  writecommand(0x29);
}


void off(void) {
  writecommand(0x28);
}


void init(void) {
    reset();
    //SOFTWARE RESET
    writecommand(0x01);
    delay(100);

    //EXIT SLEEP
    writecommand(0x11);
    delay(100);

    //writecommand(ILI9341_DISPLAY_OFF);

    //POWER CONTROL A
    writecommand(0xCB);
    writedata(0x39);
    writedata(0x2C);
    writedata(0x00);
    writedata(0x34);
    writedata(0x02);

    //POWER CONTROL B
    writecommand(0xCF);
    writedata(0x00);
    writedata(0xC1);
    writedata(0x30);

    //DRIVER TIMING CONTROL A
    writecommand(0xE8);
    writedata(0x85);
    writedata(0x00);
    writedata(0x78);

    //DRIVER TIMING CONTROL B
    writecommand(0xEA);
    writedata(0x00);
    writedata(0x00);

    //POWER ON SEQUENCE CONTROL
    writecommand(0xED);
    writedata(0x64);
    writedata(0x03);
    writedata(0x12);
    writedata(0x81);

    //PUMP RATIO CONTROL
    writecommand(0xF7);
    writedata(0x20);
    //writedata(0x10);

    //POWER CONTROL,VRH[5:0]
    writecommand(0xC0);
    writedata(0x23);

    //POWER CONTROL,SAP[2:0];BT[3:0]
    writecommand(0xC1);
    writedata(0x10);

    //VCM CONTROL
    writecommand(0xC5);
    writedata(0x3E);
    writedata(0x28);

    //VCM CONTROL 2
    writecommand(0xC7);
    writedata(0x86);

    //MEMORY ACCESS CONTROL
    writecommand(0x36);
    writedata(0x48);

    //PIXEL FORMAT
    writecommand(0x3A);
    writedata(0x55);

    //FRAME RATIO CONTROL, STANDARD RGB COLOR
    writecommand(0xB1);
    writedata(0x00);
    writedata(0x18);

    //DISPLAY FUNCTION CONTROL
    writecommand(0xB6);
    writedata(0x08);
    writedata(0x82);
    writedata(0x27);

    //3GAMMA FUNCTION DISABLE
    writecommand(0xF2);
    writedata(0x00);

    //GAMMA CURVE SELECTED
    writecommand(0x26);
    writedata(0x01);

    //POSITIVE GAMMA CORRECTION
    writecommand(0xE0);
    writedata(0x0F);
    writedata(0x31);
    writedata(0x2B);
    writedata(0x0C);
    writedata(0x0E);
    writedata(0x08);
    writedata(0x4E);
    writedata(0xF1);
    writedata(0x37);
    writedata(0x07);
    writedata(0x10);
    writedata(0x03);
    writedata(0x0E);
    writedata(0x09);
    writedata(0x00);

    //NEGATIVE GAMMA CORRECTION
    writecommand(0xE1);
    writedata(0x00);
    writedata(0x0E);
    writedata(0x14);
    writedata(0x03);
    writedata(0x11);
    writedata(0x07);
    writedata(0x31);
    writedata(0xC1);
    writedata(0x48);
    writedata(0x08);
    writedata(0x0F);
    writedata(0x0C);
    writedata(0x31);
    writedata(0x36);
    writedata(0x0F);


    //TURN ON DISPLAY
    writecommand(0x29);
    delay(5);

    writecommand(ILI9341_GRAM);
}


void init2(void)
{

reset();
writecommand(0x11);
delay(100);

//SOFTWARE RESET
writecommand(0x01);
delay(200);

//POWER CONTROL A
writecommand(0xCB);
writedata(0x39);
writedata(0x2C);
writedata(0x00);
writedata(0x34);
writedata(0x02);

//POWER CONTROL B
writecommand(0xCF);
writedata(0x00);
writedata(0xC1);
writedata(0x30);

//DRIVER TIMING CONTROL A
writecommand(0xE8);
writedata(0x85);
writedata(0x00);
writedata(0x78);

//DRIVER TIMING CONTROL B
writecommand(0xEA);
writedata(0x00);
writedata(0x00);

//POWER ON SEQUENCE CONTROL
writecommand(0xED);
writedata(0x64);
writedata(0x03);
writedata(0x12);
writedata(0x81);

//PUMP RATIO CONTROL
writecommand(0xF7);
writedata(0x20);

//POWER CONTROL,VRH[5:0]
writecommand(0xC0);
writedata(0x23);

//POWER CONTROL,SAP[2:0];BT[3:0]
writecommand(0xC1);
writedata(0x10);

//VCM CONTROL
writecommand(0xC5);
writedata(0x3E);
writedata(0x28);

//VCM CONTROL 2
writecommand(0xC7);
writedata(0x86);

//MEMORY ACCESS CONTROL
writecommand(0x36);
writedata(0x48);

//PIXEL FORMAT
writecommand(0x3A);
writedata(0x55);

//FRAME RATIO CONTROL, STANDARD RGB COLOR
writecommand(0xB1);
writedata(0x00);
writedata(0x18);

//DISPLAY FUNCTION CONTROL
writecommand(0xB6);
writedata(0x08);
writedata(0x82);
writedata(0x27);

//3GAMMA FUNCTION DISABLE
writecommand(0xF2);
writedata(0x00);

//GAMMA CURVE SELECTED
writecommand(0x26);
writedata(0x01);

//POSITIVE GAMMA CORRECTION
writecommand(0xE0);
writedata(0x0F);
writedata(0x31);
writedata(0x2B);
writedata(0x0C);
writedata(0x0E);
writedata(0x08);
writedata(0x4E);
writedata(0xF1);
writedata(0x37);
writedata(0x07);
writedata(0x10);
writedata(0x03);
writedata(0x0E);
writedata(0x09);
writedata(0x00);

//NEGATIVE GAMMA CORRECTION
writecommand(0xE1);
writedata(0x00);
writedata(0x0E);
writedata(0x14);
writedata(0x03);
writedata(0x11);
writedata(0x07);
writedata(0x31);
writedata(0xC1);
writedata(0x48);
writedata(0x08);
writedata(0x0F);
writedata(0x0C);
writedata(0x31);
writedata(0x36);
writedata(0x0F);

//EXIT SLEEP
writecommand(0x11);
delay(100);

//TURN ON DISPLAY
writecommand(0x29);
}

void invert_on(void) {
  writecommand(0x21);
}

void invert_off(void) {
  writecommand(0x20);
}

void set_window(void) {
#define ILI9341_COLUMN_ADDR  0x2A
#define ILI9341_PAGE_ADDR    0x2B
uint16_t x_pos1=1, y_pos1=1, x_pos2=100, y_pos2=100;
    writecommand(ILI9341_COLUMN_ADDR);
    writedata((x_pos1 >> 8));
    writedata(x_pos1);
    writedata((x_pos2 >> 8));
    writedata(x_pos2);

    writecommand(ILI9341_PAGE_ADDR);
    writedata((y_pos1 >> 8));
    writedata(y_pos1);
    writedata((y_pos2 >> 8));
    writedata(y_pos2);

    writecommand(ILI9341_GRAM);
}
void set_rotation(void) {
  writecommand(ILI9341_MAC);
  delay(1);
  //writedata(0xE8);
  writedata(0x20|0x08);
}

