#ifndef SPI_INTERFACE_H
#define SPI_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

//#define SAKURA_CSn_Active()    SakuraIO_CSn_Write(0)
//#define SAKURA_CSn_IDLE()    SakuraIO_CSn_Write(1)

//uint8_t    SAKURA_SPIM_READBUF[16] = {0x00};   //バッファの深さは変更可
//uint8_t    SAKURA_SPIM_SENDBUF[16] = {0x00};

//uint8_t SAKURA_SPIM_ReadTxStatus(void);
//uint8_t SAKURA_SPIM_ReadRxStatus(void);
//void SAKURA_SPIM_WriteTxData(uint8_t txData);
//void SAKURA_SPIM_PutArray(const uint8_t buffer[], uint8_t byteCount);
//uint8_t SAKURA_SPIM_ReadRxData(void);
//uint8_t SAKURA_SPIM_GetRxBufferSize(void);
//void CyDelay(uint32_t ms);
//void CyDelayUs(uint32_t us); 

#ifdef __cplusplus
}
#endif

#endif /* SPI_INTERFACE_H */
