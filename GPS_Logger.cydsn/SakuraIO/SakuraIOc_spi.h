#ifndef SAKURAIOC_SPI_H
#define SAKURAIOC_SPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "commands.h"
#include "debug.h"
    
void delay(uint32_t ms);
void SakuraIOc_hw_init();
void SakuraIOc_hw_begin();
void SakuraIOc_hw_end();
void SakuraIOc_hw_sendByte(uint8_t data);
uint8_t SakuraIOc_hw_startReceive(uint8_t length);
uint8_t SakuraIOc_hw_receiveByte();
uint8_t SakuraIOc_hw_receiveByte2(bool stop);

#ifdef __cplusplus
}
#endif

#endif /* SAKURAIOC_SPI_H */

