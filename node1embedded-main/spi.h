#ifndef SPI_DRIVER_H_
#define SPI_DRIVER_H_

#include <stdint.h>
#include <stdbool.h>

// -----------------------------------------------------------------------------
// CHIP SELECT (CS) AND CONTROL PIN DEFINITIONS
// -----------------------------------------------------------------------------
#define DISPLAY_DC_PIN	DDB2
#define CS_IO_PIN		DDB4 
#define CS_DISPLAY_PIN	DDB1
#define CS_CAN_PIN		DDB3 

// Standard SPI bus pin definitions
#define SPI_SS_HARDWARE DDB4
#define SPI_MOSI		DDB5
#define SPI_MISO		DDB6
#define SPI_SCK			DDB7

// Supported slave types
typedef enum {
	SPI_SLAVE_IO,
    SPI_SLAVE_DISPLAY_COMMAND,
	SPI_SLAVE_DISPLAY_DATA,
	SPI_SLAVE_CAN
} spi_slave_t;

/**
 * @brief Initializes the SPI peripheral and configures CS pins as inactive outputs (HIGH).
 */
void spi_init(void);

/**
 * @brief Selects (drives LOW) the CS pin of the specified slave.
 * @param slave Slave to activate.
 */
void spi_select_slave(spi_slave_t slave);

/**
 * @brief Deselects (drives HIGH) the CS pin of the specified slave.
 * @param slave Slave to deactivate.
 */
void spi_deselect_slave(spi_slave_t slave);

/**
 * @brief Writes a single byte over the SPI bus.
 * @param data Byte to transmit.
 */
void spi_write_byte(uint8_t data);

/**
 * @brief Reads a single byte by sending a dummy byte (0xFF).
 * @return uint8_t Byte received through MISO.
 */
uint8_t spi_read_byte(void);

/**
 * @brief Transmits one byte and returns the received byte (Full-Duplex Primitive).
 * @param data Byte to send through MOSI.
 * @return uint8_t Byte received simultaneously through MISO.
 */
uint8_t spi_master_transfer(uint8_t data);

/**
 * @brief Block transmission/reception of n bytes.
 * @param tx_buffer Pointer to the data buffer to transmit.
 * @param rx_buffer Pointer to the buffer where the received data will be stored.
 * @param n Number of bytes to transfer.
 * @return uint8_t* Pointer to the receive buffer rx_buffer.
 */
uint8_t* spi_master_transfer_buffer(uint8_t* tx_buffer, uint8_t* rx_buffer, uint8_t n);

/**
 * @brief Writes a block of data to a specific slave while automatically managing its CS.
 * @param slave Target slave.
 * @param data_out Pointer to the data buffer to transmit.
 * @param length Number of bytes to send.
 */
void spi_write_packet(spi_slave_t slave, const uint8_t *data_out, uint16_t length);

/**
 * @brief Reads a block of data from a specific slave by sending dummy bytes.
 * @param slave Target slave.
 * @param data_in Pointer to the buffer where the response will be stored.
 * @param length Number of bytes to receive.
 */
void spi_read_packet(spi_slave_t slave, uint8_t *data_in, uint16_t length);

#endif /* SPI_DRIVER_H_ */