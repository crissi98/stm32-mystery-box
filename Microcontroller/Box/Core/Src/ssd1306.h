#ifndef SSD1306_H
#define SSD1306_H

#include <stdint.h>

#include "ssd1306_enum.h"

#include <cstring>

#include <string>

#include <bits/stdc++.h>

static const char init_cmds[] = {
    COMMANDS::SET_DISPLAY_OFF,
    COMMANDS::SET_DISPLAY_CLK_DIVIDER_CONTROL,
    COMMANDS::SET_DISPLAY_CLK_DIVIDER_128,
    COMMANDS::SET_MULTIPLEX_RATIO_CONTROL,
    COMMANDS::SET_MULTIPLEX_RATIO_64,
    COMMANDS::SET_DISPLAY_OFFSET_CONTROL,
    0x00,  // no offset
    COMMANDS::SET_START_LINE | 0x00,
    COMMANDS::SET_CHARGE_BUMP_CONTROL,
    COMMANDS::SET_CHARGE_BUMP_ON,
    COMMANDS::SET_SEG_REMAP_ON,
    COMMANDS::SET_COM_SCAN_DECREMENT,
    COMMANDS::SET_COM_PINS_CONFIG_CONTROL,
    COMMANDS::SET_COM_PINS_ALT_NO_REMAP,
    COMMANDS::SET_CONTRAST_CONTROL,
    COMMANDS::SET_CONTRAST_207,
    COMMANDS::SET_PRECHARGE_CONTROL,
    COMMANDS::SET_PRECHARGE_PHASE1_1_PHASE2_15,
    COMMANDS::SET_VCOM_DESELECT_LEVEL_CONTROL,
    COMMANDS::SET_VCOM_DESELECT_LEVEL_07VCC,
    COMMANDS::SET_MEMORY_ADDRESSING_MODE_CONTROL,
    COMMANDS::SET_HORIZONTAL_MEMORY_ADDRESSING_MODE,
    COMMANDS::SET_PAGE_ADDR_CONTROL,
    0x00,  // page start address = 0
    0x07,  // page end address = 7;
    COMMANDS::SET_COLUMN_ADDR,
    0x00,  // column start address = 0
    0x7F,  // column end address = 127
    COMMANDS::SET_DISPLAY_BY_RAM,
    COMMANDS::SET_NORMAL_DISPLAY,
    COMMANDS::SET_DEACTIVATE_SCROLL,
    COMMANDS::SET_DISPLAY_ON,
};

static char characters[][8] = {
		{0x3C,0x62,0x52,0x4A,0x46,0x3C,0x00,0x00},  // 0
		{0x44,0x42,0x7E,0x40,0x40,0x00,0x00,0x00},  // 1
		{0x64,0x52,0x52,0x52,0x52,0x4C,0x00,0x00},  // 2
		{0x24,0x42,0x42,0x4A,0x4A,0x34,0x00,0x00},  // 3
		{0x30,0x28,0x24,0x7E,0x20,0x20,0x00,0x00},  // 4
		{0x2E,0x4A,0x4A,0x4A,0x4A,0x32,0x00,0x00},  // 5
		{0x3C,0x4A,0x4A,0x4A,0x4A,0x30,0x00,0x00},  // 6
		{0x02,0x02,0x62,0x12,0x0A,0x06,0x00,0x00},  // 7
		{0x34,0x4A,0x4A,0x4A,0x4A,0x34,0x00,0x00},  // 8
		{0x0C,0x52,0x52,0x52,0x52,0x3C,0x00,0x00},  // 9
		{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},  // <space>
		{0x08,0x08,0x08,0x08,0x08,0x08,0x00,0x00},  // -
		{0x7C,0x12,0x12,0x12,0x12,0x7C,0x00,0x00},  // A
		{0x7E,0x4A,0x4A,0x4A,0x4A,0x34,0x00,0x00},  // B
		{0x3C,0x42,0x42,0x42,0x42,0x24,0x00,0x00},  // C
		{0x7E,0x42,0x42,0x42,0x24,0x18,0x00,0x00},  // D
		{0x7E,0x4A,0x4A,0x4A,0x4A,0x42,0x00,0x00},  // E
		{0x7E,0x0A,0x0A,0x0A,0x0A,0x02,0x00,0x00},  // F
		{0x3C,0x42,0x42,0x52,0x52,0x34,0x00,0x00},  // G
		{0x7E,0x08,0x08,0x08,0x08,0x7E,0x00,0x00},  // H
		{0x00,0x42,0x42,0x7E,0x42,0x42,0x00,0x00},  // I
		{0x30,0x40,0x40,0x40,0x40,0x3E,0x00,0x00},  // J
		{0x7E,0x08,0x08,0x14,0x22,0x40,0x00,0x00},  // K
		{0x7E,0x40,0x40,0x40,0x40,0x40,0x00,0x00},  // L
		{0x7E,0x04,0x08,0x08,0x04,0x7E,0x00,0x00},  // M
		{0x7E,0x04,0x08,0x10,0x20,0x7E,0x00,0x00},  // N
		{0x3C,0x42,0x42,0x42,0x42,0x3C,0x00,0x00},  // O
		{0x7E,0x12,0x12,0x12,0x12,0x0C,0x00,0x00},  // P
		{0x3C,0x42,0x52,0x62,0x42,0x3C,0x00,0x00},  // Q
		{0x7E,0x12,0x12,0x12,0x32,0x4C,0x00,0x00},  // R
		{0x24,0x4A,0x4A,0x4A,0x4A,0x30,0x00,0x00},  // S
		{0x02,0x02,0x02,0x7E,0x02,0x02,0x02,0x00},  // T
		{0x3E,0x40,0x40,0x40,0x40,0x3E,0x00,0x00},  // U
		{0x1E,0x20,0x40,0x40,0x20,0x1E,0x00,0x00},  // V
		{0x3E,0x40,0x20,0x20,0x40,0x3E,0x00,0x00},  // W
		{0x42,0x24,0x18,0x18,0x24,0x42,0x00,0x00},  // X
		{0x02,0x04,0x08,0x70,0x08,0x04,0x02,0x00},  // Y
		{0x42,0x62,0x52,0x4A,0x46,0x42,0x00,0x00},  // Z
		{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}   // Error block

};

template <char WIDTH, char HEIGHT, char PPB,
          int (*i2c_write)(const char* buf, uint32_t len)>
class SSD1306 {
 private:
  char framebuffer[(WIDTH * HEIGHT) / PPB];
  char current_page, current_column;

  char command(const char command) {
    char buf[2] = {CONTROL_BYTE_COMMANDS::SEND_SINGLE_CMD, command};
    return i2c_write(buf, 2);
  }

  char commandLine(const char* command_buf, char len) {
    char buf[len] = {CONTROL_BYTE_COMMANDS::SEND_MULTIPLE_CMDS};
    memcpy(buf + 1, command_buf, len);
    return i2c_write(buf, len + 1);
  }

  char data(const char data) {
    char buf[2] = {CONTROL_BYTE_COMMANDS::SEND_SINGLE_DATA, data};
    return i2c_write(buf, 2);
  }

  char multipleData(const char* data_buf, char len) {
    char buf[len] = {CONTROL_BYTE_COMMANDS::SEND_MULTIPLE_DATA};
    memcpy(buf + 1, data_buf, len);
    return i2c_write(buf, len + 1);
  }

 public:
  SSD1306() : framebuffer{0}, current_page(0), current_column(0) {}

  void init() { this->commandLine(init_cmds, sizeof(init_cmds)); }

  void showDisplay() {
    char buf_out[(WIDTH * HEIGHT) / PPB + 1] = {
        CONTROL_BYTE_COMMANDS::SEND_MULTIPLE_DATA};

    if (this->current_column != 0 || this->current_page != 0) {
      this->setPosition(0, 0);
    }

    memcpy(buf_out + 1, this->framebuffer, (WIDTH * HEIGHT) / PPB);
    i2c_write(buf_out, sizeof(buf_out));

    if (this->current_column != 0 || this->current_page != 0) {
      this->setPosition(this->current_page, this->current_column);
    }
  }

  void setPosition(char page, char column) {
    const char cmds[] = {
        COMMANDS::SET_PAGE_ADDR_CONTROL,
        page &
            ((HEIGHT / PPB) - 1),  // page start address = page % (height/ppb)
        0x07,                      // page end address = 7;
        COMMANDS::SET_COLUMN_ADDR,
        column & (WIDTH - 1),  // column start address = column % width
        0x7F,                  // column end address = 127
    };

    if (this->commandLine(cmds, sizeof(cmds)) == 0) {
      this->current_page = cmds[1];
      this->current_column = cmds[4];
    }
  }

  void setAndShowByteAtCurrentPosition(const char data) {
    if (this->data(data) != 0) return;

    this->framebuffer[this->current_column + (this->current_page * WIDTH)] =
        data;

    this->current_column = (++this->current_column) & (WIDTH - 1);
    this->current_page = (this->current_column != 0)
                             ? this->current_page
                             : (this->current_page + 1) & ((HEIGHT / PPB) - 1);
  }

  void setAndShowBytesFromCurrentPosition(const char* data_buf,
                                          char len) {
    if (this->multipleData(data_buf, len) != 0) return;

    memcpy((this->framebuffer + this->current_column +
            (this->current_page * WIDTH)),
           data_buf, len);

    this->current_page =
        (this->current_page + (this->current_column + len) / WIDTH) &
        ((HEIGHT / PPB) - 1);
    this->current_column = (this->current_column + len) & (WIDTH - 1);
  }

  void setPixel(char pos_x, char pos_y, char value) {
    if (pos_x >= WIDTH) return;
    if (pos_y >= HEIGHT) return;

    this->framebuffer[pos_x + (pos_y / PPB) * WIDTH] =
        (this->framebuffer[pos_x + (pos_y / PPB) * WIDTH] &
         ~(1 << pos_y % PPB)) |
        (value << pos_y % PPB);
  }

  void clearDisplay() {
    for (uint16_t i = 0; i < sizeof(framebuffer); i++) {
      this->framebuffer[i] = 0;
    }
    this->showDisplay();
  }

  void showTestGrid() {
    for (uint16_t i = 0; i < sizeof(this->framebuffer) * PPB; i++) {
      this->framebuffer[(i / PPB)] =
          (i % 2 && (i / PPB) % 2)
              ? this->framebuffer[i / PPB] & ~(1 << (i % PPB))
              : this->framebuffer[i / PPB] | 1 << (i % PPB);
    }
    this->showDisplay();
  }

  void displayText(std::string text) {
	  std::transform(text.begin(), text.end(),text.begin(), ::toupper);
	  int length = text.length();
	  char chars[length + 1];
	  std::strcpy(chars, text.c_str());
	  for (int i = 0; i < length; i++) {
		  displayCharacter(chars[i]);
	  }
  }

  void displayCharacter(char c) {
	  int index = -1;

	  if (c == ' ') {
		  index = 10;
	  } else if (c == '-') {
		  index = 11;
	  } else if (c >= '0' && c <= '9') {
		  index = c - '0';
	  } else if (c >= 'A' && c <= 'Z') {
		  index = c - 'A' + 12;
	  }

	  char* charData = characters[38];
	  if (index != -1) {
		  charData = characters[index];
	  }

	  this->setAndShowBytesFromCurrentPosition(charData, 8);
  }

};
#endif /* SSD1306_H */
