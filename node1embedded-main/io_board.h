

#ifndef IO_BOARD_H_
#define IO_BOARD_H_

#include <stdint.h>

// Official datasheet structure using unions and bit-fields
typedef struct __attribute__((packed)) {
    union {
        uint8_t right;
        struct {
            uint8_t R1:1;
            uint8_t R2:1;
            uint8_t R3:1;
            uint8_t R4:1;
            uint8_t R5:1;
            uint8_t R6:1;
        };
    };
    union {
        uint8_t left;
        struct {
            uint8_t L1:1;
            uint8_t L2:1;
            uint8_t L3:1;
            uint8_t L4:1;
            uint8_t L5:1;
            uint8_t L6:1;
            uint8_t L7:1;
        };
    };
    union {
        uint8_t nav;
        struct {
            uint8_t NB:1; // Nav Button
            uint8_t NR:1; // Nav Right
            uint8_t ND:1; // Nav Down
            uint8_t NL:1; // Nav Left
            uint8_t NU:1; // Nav Up
        };
    };
} Buttons;

/**
 * @brief Reads the IO Board button state via SPI.
 * @param buttons Pointer to the Buttons structure.
 */
void io_board_read_buttons(Buttons *buttons);

/**
 * @brief Prints a byte in 8-bit binary format.
 */
void io_print_bin(uint8_t byte);

/**
 * @brief Displays the binary state of the 3 bytes on the OLED.
 */
void io_print_buttons_bin(const Buttons *buttons);

#endif /* IO_BOARD_H_ */