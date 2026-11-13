

#ifndef OLED_H_
#define OLED_H_

#include <stdio.h>
#include <stdint.h>

/**
 * @brief Initializes the SPI bus and applies the startup procedure
 *        to the OLED controller.
 */
void oled_init(void);

/**
 * @brief Resets the display controller and restores the configuration.
 */
void oled_reset(void);

/**
 * @brief Moves the graphics cursor to the initial position (Line 0, Column 0).
 */
void oled_home(void);

/**
 * @brief Changes the current active line/page.
 * @param line Line number (0 to 7).
 */
void oled_goto_line(uint8_t line);

/**
 * @brief Changes the active column within the current line.
 * @param column Column number (0 to 127).
 */
void oled_goto_column(uint8_t column);

/**
 * @brief Positions the cursor at a specific line and column.
 * @param row Target line/page (0 to 7).
 * @param column Target column (0 to 127).
 */
void oled_pos(uint8_t row, uint8_t column);

/**
 * @brief Clears the 128 horizontal pixels of a specific line.
 * @param line Line number to clear (0 to 7).
 */
void oled_clear_line(uint8_t line);

/**
 * @brief Prints a text string stored in RAM at the current cursor position.
 * @param str Pointer to the text string to render.
 */
void oled_print(const char* str);

/**
 * @brief Output function for integrating the OLED with stdout / printf.
 */
int oled_putchar(char c, FILE *stream);

#endif /* OLED_H_ */