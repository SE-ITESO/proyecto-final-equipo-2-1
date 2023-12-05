/*
 * SPI.h
 *
 *  Created on: 19 jun. 2021
 *      Author: jlpe
 */

#ifndef SPI_H_
#define SPI_H_



#define PIN0_IDX                         0u   /*!< Pin number for pin 0 in a port */
#define PIN1_IDX                         1u   /*!< Pin number for pin 1 in a port */
#define PIN2_IDX                         2u   /*!< Pin number for pin 2 in a port */
#define PIN3_IDX                         3u   /*!< Pin number for pin 3 in a port */

#define LCD_BAUDRATE 1000000U /*! Transfer baudrate - 1MHz */
#define MEMORY_BAUDRATE 5000000U /*! Transfer baudrate - 2MHz */

void SPI_config(void);

#endif /* SPI_H_ */
