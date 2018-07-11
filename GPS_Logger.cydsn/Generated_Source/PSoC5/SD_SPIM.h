/*******************************************************************************
* File Name: SD_SPIM.h
* Version 2.50
*
* Description:
*  Contains the function prototypes, constants and register definition
*  of the SPI Master Component.
*
* Note:
*  None
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_SPIM_SD_SPIM_H)
#define CY_SPIM_SD_SPIM_H

#include "cyfitter.h"
#include "cytypes.h"
#include "CyLib.h" /* For CyEnterCriticalSection() and CyExitCriticalSection() functions */


/***************************************
*   Conditional Compilation Parameters
***************************************/

#define SD_SPIM_INTERNAL_CLOCK             (0u)

#if(0u != SD_SPIM_INTERNAL_CLOCK)
    #include "SD_SPIM_IntClock.h"
#endif /* (0u != SD_SPIM_INTERNAL_CLOCK) */

#define SD_SPIM_MODE                       (1u)
#define SD_SPIM_DATA_WIDTH                 (8u)
#define SD_SPIM_MODE_USE_ZERO              (1u)
#define SD_SPIM_BIDIRECTIONAL_MODE         (0u)

/* Internal interrupt handling */
#define SD_SPIM_TX_BUFFER_SIZE             (4u)
#define SD_SPIM_RX_BUFFER_SIZE             (4u)
#define SD_SPIM_INTERNAL_TX_INT_ENABLED    (0u)
#define SD_SPIM_INTERNAL_RX_INT_ENABLED    (0u)

#define SD_SPIM_SINGLE_REG_SIZE            (8u)
#define SD_SPIM_USE_SECOND_DATAPATH        (SD_SPIM_DATA_WIDTH > SD_SPIM_SINGLE_REG_SIZE)

#define SD_SPIM_FIFO_SIZE                  (4u)
#define SD_SPIM_TX_SOFTWARE_BUF_ENABLED    ((0u != SD_SPIM_INTERNAL_TX_INT_ENABLED) && \
                                                     (SD_SPIM_TX_BUFFER_SIZE > SD_SPIM_FIFO_SIZE))

#define SD_SPIM_RX_SOFTWARE_BUF_ENABLED    ((0u != SD_SPIM_INTERNAL_RX_INT_ENABLED) && \
                                                     (SD_SPIM_RX_BUFFER_SIZE > SD_SPIM_FIFO_SIZE))


/***************************************
*        Data Struct Definition
***************************************/

/* Sleep Mode API Support */
typedef struct
{
    uint8 enableState;
    uint8 cntrPeriod;
} SD_SPIM_BACKUP_STRUCT;


/***************************************
*        Function Prototypes
***************************************/

void  SD_SPIM_Init(void)                           ;
void  SD_SPIM_Enable(void)                         ;
void  SD_SPIM_Start(void)                          ;
void  SD_SPIM_Stop(void)                           ;

void  SD_SPIM_EnableTxInt(void)                    ;
void  SD_SPIM_EnableRxInt(void)                    ;
void  SD_SPIM_DisableTxInt(void)                   ;
void  SD_SPIM_DisableRxInt(void)                   ;

void  SD_SPIM_Sleep(void)                          ;
void  SD_SPIM_Wakeup(void)                         ;
void  SD_SPIM_SaveConfig(void)                     ;
void  SD_SPIM_RestoreConfig(void)                  ;

void  SD_SPIM_SetTxInterruptMode(uint8 intSrc)     ;
void  SD_SPIM_SetRxInterruptMode(uint8 intSrc)     ;
uint8 SD_SPIM_ReadTxStatus(void)                   ;
uint8 SD_SPIM_ReadRxStatus(void)                   ;
void  SD_SPIM_WriteTxData(uint8 txData)  \
                                                            ;
uint8 SD_SPIM_ReadRxData(void) \
                                                            ;
uint8 SD_SPIM_GetRxBufferSize(void)                ;
uint8 SD_SPIM_GetTxBufferSize(void)                ;
void  SD_SPIM_ClearRxBuffer(void)                  ;
void  SD_SPIM_ClearTxBuffer(void)                  ;
void  SD_SPIM_ClearFIFO(void)                              ;
void  SD_SPIM_PutArray(const uint8 buffer[], uint8 byteCount) \
                                                            ;

#if(0u != SD_SPIM_BIDIRECTIONAL_MODE)
    void  SD_SPIM_TxEnable(void)                   ;
    void  SD_SPIM_TxDisable(void)                  ;
#endif /* (0u != SD_SPIM_BIDIRECTIONAL_MODE) */

CY_ISR_PROTO(SD_SPIM_TX_ISR);
CY_ISR_PROTO(SD_SPIM_RX_ISR);


/***************************************
*   Variable with external linkage
***************************************/

extern uint8 SD_SPIM_initVar;


/***************************************
*           API Constants
***************************************/

#define SD_SPIM_TX_ISR_NUMBER     ((uint8) (SD_SPIM_TxInternalInterrupt__INTC_NUMBER))
#define SD_SPIM_RX_ISR_NUMBER     ((uint8) (SD_SPIM_RxInternalInterrupt__INTC_NUMBER))

#define SD_SPIM_TX_ISR_PRIORITY   ((uint8) (SD_SPIM_TxInternalInterrupt__INTC_PRIOR_NUM))
#define SD_SPIM_RX_ISR_PRIORITY   ((uint8) (SD_SPIM_RxInternalInterrupt__INTC_PRIOR_NUM))


/***************************************
*    Initial Parameter Constants
***************************************/

#define SD_SPIM_INT_ON_SPI_DONE    ((uint8) (0u   << SD_SPIM_STS_SPI_DONE_SHIFT))
#define SD_SPIM_INT_ON_TX_EMPTY    ((uint8) (0u   << SD_SPIM_STS_TX_FIFO_EMPTY_SHIFT))
#define SD_SPIM_INT_ON_TX_NOT_FULL ((uint8) (0u << \
                                                                           SD_SPIM_STS_TX_FIFO_NOT_FULL_SHIFT))
#define SD_SPIM_INT_ON_BYTE_COMP   ((uint8) (0u  << SD_SPIM_STS_BYTE_COMPLETE_SHIFT))
#define SD_SPIM_INT_ON_SPI_IDLE    ((uint8) (0u   << SD_SPIM_STS_SPI_IDLE_SHIFT))

/* Disable TX_NOT_FULL if software buffer is used */
#define SD_SPIM_INT_ON_TX_NOT_FULL_DEF ((SD_SPIM_TX_SOFTWARE_BUF_ENABLED) ? \
                                                                        (0u) : (SD_SPIM_INT_ON_TX_NOT_FULL))

/* TX interrupt mask */
#define SD_SPIM_TX_INIT_INTERRUPTS_MASK    (SD_SPIM_INT_ON_SPI_DONE  | \
                                                     SD_SPIM_INT_ON_TX_EMPTY  | \
                                                     SD_SPIM_INT_ON_TX_NOT_FULL_DEF | \
                                                     SD_SPIM_INT_ON_BYTE_COMP | \
                                                     SD_SPIM_INT_ON_SPI_IDLE)

#define SD_SPIM_INT_ON_RX_FULL         ((uint8) (0u << \
                                                                          SD_SPIM_STS_RX_FIFO_FULL_SHIFT))
#define SD_SPIM_INT_ON_RX_NOT_EMPTY    ((uint8) (0u << \
                                                                          SD_SPIM_STS_RX_FIFO_NOT_EMPTY_SHIFT))
#define SD_SPIM_INT_ON_RX_OVER         ((uint8) (0u << \
                                                                          SD_SPIM_STS_RX_FIFO_OVERRUN_SHIFT))

/* RX interrupt mask */
#define SD_SPIM_RX_INIT_INTERRUPTS_MASK    (SD_SPIM_INT_ON_RX_FULL      | \
                                                     SD_SPIM_INT_ON_RX_NOT_EMPTY | \
                                                     SD_SPIM_INT_ON_RX_OVER)
/* Nubmer of bits to receive/transmit */
#define SD_SPIM_BITCTR_INIT            (((uint8) (SD_SPIM_DATA_WIDTH << 1u)) - 1u)


/***************************************
*             Registers
***************************************/
#if(CY_PSOC3 || CY_PSOC5)
    #define SD_SPIM_TXDATA_REG (* (reg8 *) \
                                                SD_SPIM_BSPIM_sR8_Dp_u0__F0_REG)
    #define SD_SPIM_TXDATA_PTR (  (reg8 *) \
                                                SD_SPIM_BSPIM_sR8_Dp_u0__F0_REG)
    #define SD_SPIM_RXDATA_REG (* (reg8 *) \
                                                SD_SPIM_BSPIM_sR8_Dp_u0__F1_REG)
    #define SD_SPIM_RXDATA_PTR (  (reg8 *) \
                                                SD_SPIM_BSPIM_sR8_Dp_u0__F1_REG)
#else   /* PSOC4 */
    #if(SD_SPIM_USE_SECOND_DATAPATH)
        #define SD_SPIM_TXDATA_REG (* (reg16 *) \
                                          SD_SPIM_BSPIM_sR8_Dp_u0__16BIT_F0_REG)
        #define SD_SPIM_TXDATA_PTR (  (reg16 *) \
                                          SD_SPIM_BSPIM_sR8_Dp_u0__16BIT_F0_REG)
        #define SD_SPIM_RXDATA_REG (* (reg16 *) \
                                          SD_SPIM_BSPIM_sR8_Dp_u0__16BIT_F1_REG)
        #define SD_SPIM_RXDATA_PTR (  (reg16 *) \
                                          SD_SPIM_BSPIM_sR8_Dp_u0__16BIT_F1_REG)
    #else
        #define SD_SPIM_TXDATA_REG (* (reg8 *) \
                                                SD_SPIM_BSPIM_sR8_Dp_u0__F0_REG)
        #define SD_SPIM_TXDATA_PTR (  (reg8 *) \
                                                SD_SPIM_BSPIM_sR8_Dp_u0__F0_REG)
        #define SD_SPIM_RXDATA_REG (* (reg8 *) \
                                                SD_SPIM_BSPIM_sR8_Dp_u0__F1_REG)
        #define SD_SPIM_RXDATA_PTR (  (reg8 *) \
                                                SD_SPIM_BSPIM_sR8_Dp_u0__F1_REG)
    #endif /* (SD_SPIM_USE_SECOND_DATAPATH) */
#endif     /* (CY_PSOC3 || CY_PSOC5) */

#define SD_SPIM_AUX_CONTROL_DP0_REG (* (reg8 *) \
                                        SD_SPIM_BSPIM_sR8_Dp_u0__DP_AUX_CTL_REG)
#define SD_SPIM_AUX_CONTROL_DP0_PTR (  (reg8 *) \
                                        SD_SPIM_BSPIM_sR8_Dp_u0__DP_AUX_CTL_REG)

#if(SD_SPIM_USE_SECOND_DATAPATH)
    #define SD_SPIM_AUX_CONTROL_DP1_REG  (* (reg8 *) \
                                        SD_SPIM_BSPIM_sR8_Dp_u1__DP_AUX_CTL_REG)
    #define SD_SPIM_AUX_CONTROL_DP1_PTR  (  (reg8 *) \
                                        SD_SPIM_BSPIM_sR8_Dp_u1__DP_AUX_CTL_REG)
#endif /* (SD_SPIM_USE_SECOND_DATAPATH) */

#define SD_SPIM_COUNTER_PERIOD_REG     (* (reg8 *) SD_SPIM_BSPIM_BitCounter__PERIOD_REG)
#define SD_SPIM_COUNTER_PERIOD_PTR     (  (reg8 *) SD_SPIM_BSPIM_BitCounter__PERIOD_REG)
#define SD_SPIM_COUNTER_CONTROL_REG    (* (reg8 *) SD_SPIM_BSPIM_BitCounter__CONTROL_AUX_CTL_REG)
#define SD_SPIM_COUNTER_CONTROL_PTR    (  (reg8 *) SD_SPIM_BSPIM_BitCounter__CONTROL_AUX_CTL_REG)

#define SD_SPIM_TX_STATUS_REG          (* (reg8 *) SD_SPIM_BSPIM_TxStsReg__STATUS_REG)
#define SD_SPIM_TX_STATUS_PTR          (  (reg8 *) SD_SPIM_BSPIM_TxStsReg__STATUS_REG)
#define SD_SPIM_RX_STATUS_REG          (* (reg8 *) SD_SPIM_BSPIM_RxStsReg__STATUS_REG)
#define SD_SPIM_RX_STATUS_PTR          (  (reg8 *) SD_SPIM_BSPIM_RxStsReg__STATUS_REG)

#define SD_SPIM_CONTROL_REG            (* (reg8 *) \
                                      SD_SPIM_BSPIM_BidirMode_CtrlReg__CONTROL_REG)
#define SD_SPIM_CONTROL_PTR            (  (reg8 *) \
                                      SD_SPIM_BSPIM_BidirMode_CtrlReg__CONTROL_REG)

#define SD_SPIM_TX_STATUS_MASK_REG     (* (reg8 *) SD_SPIM_BSPIM_TxStsReg__MASK_REG)
#define SD_SPIM_TX_STATUS_MASK_PTR     (  (reg8 *) SD_SPIM_BSPIM_TxStsReg__MASK_REG)
#define SD_SPIM_RX_STATUS_MASK_REG     (* (reg8 *) SD_SPIM_BSPIM_RxStsReg__MASK_REG)
#define SD_SPIM_RX_STATUS_MASK_PTR     (  (reg8 *) SD_SPIM_BSPIM_RxStsReg__MASK_REG)

#define SD_SPIM_TX_STATUS_ACTL_REG     (* (reg8 *) SD_SPIM_BSPIM_TxStsReg__STATUS_AUX_CTL_REG)
#define SD_SPIM_TX_STATUS_ACTL_PTR     (  (reg8 *) SD_SPIM_BSPIM_TxStsReg__STATUS_AUX_CTL_REG)
#define SD_SPIM_RX_STATUS_ACTL_REG     (* (reg8 *) SD_SPIM_BSPIM_RxStsReg__STATUS_AUX_CTL_REG)
#define SD_SPIM_RX_STATUS_ACTL_PTR     (  (reg8 *) SD_SPIM_BSPIM_RxStsReg__STATUS_AUX_CTL_REG)

#if(SD_SPIM_USE_SECOND_DATAPATH)
    #define SD_SPIM_AUX_CONTROLDP1     (SD_SPIM_AUX_CONTROL_DP1_REG)
#endif /* (SD_SPIM_USE_SECOND_DATAPATH) */


/***************************************
*       Register Constants
***************************************/

/* Status Register Definitions */
#define SD_SPIM_STS_SPI_DONE_SHIFT             (0x00u)
#define SD_SPIM_STS_TX_FIFO_EMPTY_SHIFT        (0x01u)
#define SD_SPIM_STS_TX_FIFO_NOT_FULL_SHIFT     (0x02u)
#define SD_SPIM_STS_BYTE_COMPLETE_SHIFT        (0x03u)
#define SD_SPIM_STS_SPI_IDLE_SHIFT             (0x04u)
#define SD_SPIM_STS_RX_FIFO_FULL_SHIFT         (0x04u)
#define SD_SPIM_STS_RX_FIFO_NOT_EMPTY_SHIFT    (0x05u)
#define SD_SPIM_STS_RX_FIFO_OVERRUN_SHIFT      (0x06u)

#define SD_SPIM_STS_SPI_DONE           ((uint8) (0x01u << SD_SPIM_STS_SPI_DONE_SHIFT))
#define SD_SPIM_STS_TX_FIFO_EMPTY      ((uint8) (0x01u << SD_SPIM_STS_TX_FIFO_EMPTY_SHIFT))
#define SD_SPIM_STS_TX_FIFO_NOT_FULL   ((uint8) (0x01u << SD_SPIM_STS_TX_FIFO_NOT_FULL_SHIFT))
#define SD_SPIM_STS_BYTE_COMPLETE      ((uint8) (0x01u << SD_SPIM_STS_BYTE_COMPLETE_SHIFT))
#define SD_SPIM_STS_SPI_IDLE           ((uint8) (0x01u << SD_SPIM_STS_SPI_IDLE_SHIFT))
#define SD_SPIM_STS_RX_FIFO_FULL       ((uint8) (0x01u << SD_SPIM_STS_RX_FIFO_FULL_SHIFT))
#define SD_SPIM_STS_RX_FIFO_NOT_EMPTY  ((uint8) (0x01u << SD_SPIM_STS_RX_FIFO_NOT_EMPTY_SHIFT))
#define SD_SPIM_STS_RX_FIFO_OVERRUN    ((uint8) (0x01u << SD_SPIM_STS_RX_FIFO_OVERRUN_SHIFT))

/* TX and RX masks for clear on read bits */
#define SD_SPIM_TX_STS_CLR_ON_RD_BYTES_MASK    (0x09u)
#define SD_SPIM_RX_STS_CLR_ON_RD_BYTES_MASK    (0x40u)

/* StatusI Register Interrupt Enable Control Bits */
/* As defined by the Register map for the AUX Control Register */
#define SD_SPIM_INT_ENABLE     (0x10u) /* Enable interrupt from statusi */
#define SD_SPIM_TX_FIFO_CLR    (0x01u) /* F0 - TX FIFO */
#define SD_SPIM_RX_FIFO_CLR    (0x02u) /* F1 - RX FIFO */
#define SD_SPIM_FIFO_CLR       (SD_SPIM_TX_FIFO_CLR | SD_SPIM_RX_FIFO_CLR)

/* Bit Counter (7-bit) Control Register Bit Definitions */
/* As defined by the Register map for the AUX Control Register */
#define SD_SPIM_CNTR_ENABLE    (0x20u) /* Enable CNT7 */

/* Bi-Directional mode control bit */
#define SD_SPIM_CTRL_TX_SIGNAL_EN  (0x01u)

/* Datapath Auxillary Control Register definitions */
#define SD_SPIM_AUX_CTRL_FIFO0_CLR         (0x01u)
#define SD_SPIM_AUX_CTRL_FIFO1_CLR         (0x02u)
#define SD_SPIM_AUX_CTRL_FIFO0_LVL         (0x04u)
#define SD_SPIM_AUX_CTRL_FIFO1_LVL         (0x08u)
#define SD_SPIM_STATUS_ACTL_INT_EN_MASK    (0x10u)

/* Component disabled */
#define SD_SPIM_DISABLED   (0u)


/***************************************
*       Macros
***************************************/

/* Returns true if componentn enabled */
#define SD_SPIM_IS_ENABLED (0u != (SD_SPIM_TX_STATUS_ACTL_REG & SD_SPIM_INT_ENABLE))

/* Retuns TX status register */
#define SD_SPIM_GET_STATUS_TX(swTxSts) ( (uint8)(SD_SPIM_TX_STATUS_REG | \
                                                          ((swTxSts) & SD_SPIM_TX_STS_CLR_ON_RD_BYTES_MASK)) )
/* Retuns RX status register */
#define SD_SPIM_GET_STATUS_RX(swRxSts) ( (uint8)(SD_SPIM_RX_STATUS_REG | \
                                                          ((swRxSts) & SD_SPIM_RX_STS_CLR_ON_RD_BYTES_MASK)) )


/***************************************
* The following code is DEPRECATED and 
* should not be used in new projects.
***************************************/

#define SD_SPIM_WriteByte   SD_SPIM_WriteTxData
#define SD_SPIM_ReadByte    SD_SPIM_ReadRxData
void  SD_SPIM_SetInterruptMode(uint8 intSrc)       ;
uint8 SD_SPIM_ReadStatus(void)                     ;
void  SD_SPIM_EnableInt(void)                      ;
void  SD_SPIM_DisableInt(void)                     ;

#define SD_SPIM_TXDATA                 (SD_SPIM_TXDATA_REG)
#define SD_SPIM_RXDATA                 (SD_SPIM_RXDATA_REG)
#define SD_SPIM_AUX_CONTROLDP0         (SD_SPIM_AUX_CONTROL_DP0_REG)
#define SD_SPIM_TXBUFFERREAD           (SD_SPIM_txBufferRead)
#define SD_SPIM_TXBUFFERWRITE          (SD_SPIM_txBufferWrite)
#define SD_SPIM_RXBUFFERREAD           (SD_SPIM_rxBufferRead)
#define SD_SPIM_RXBUFFERWRITE          (SD_SPIM_rxBufferWrite)

#define SD_SPIM_COUNTER_PERIOD         (SD_SPIM_COUNTER_PERIOD_REG)
#define SD_SPIM_COUNTER_CONTROL        (SD_SPIM_COUNTER_CONTROL_REG)
#define SD_SPIM_STATUS                 (SD_SPIM_TX_STATUS_REG)
#define SD_SPIM_CONTROL                (SD_SPIM_CONTROL_REG)
#define SD_SPIM_STATUS_MASK            (SD_SPIM_TX_STATUS_MASK_REG)
#define SD_SPIM_STATUS_ACTL            (SD_SPIM_TX_STATUS_ACTL_REG)

#define SD_SPIM_INIT_INTERRUPTS_MASK  (SD_SPIM_INT_ON_SPI_DONE     | \
                                                SD_SPIM_INT_ON_TX_EMPTY     | \
                                                SD_SPIM_INT_ON_TX_NOT_FULL_DEF  | \
                                                SD_SPIM_INT_ON_RX_FULL      | \
                                                SD_SPIM_INT_ON_RX_NOT_EMPTY | \
                                                SD_SPIM_INT_ON_RX_OVER      | \
                                                SD_SPIM_INT_ON_BYTE_COMP)
                                                
#define SD_SPIM_DataWidth                  (SD_SPIM_DATA_WIDTH)
#define SD_SPIM_InternalClockUsed          (SD_SPIM_INTERNAL_CLOCK)
#define SD_SPIM_InternalTxInterruptEnabled (SD_SPIM_INTERNAL_TX_INT_ENABLED)
#define SD_SPIM_InternalRxInterruptEnabled (SD_SPIM_INTERNAL_RX_INT_ENABLED)
#define SD_SPIM_ModeUseZero                (SD_SPIM_MODE_USE_ZERO)
#define SD_SPIM_BidirectionalMode          (SD_SPIM_BIDIRECTIONAL_MODE)
#define SD_SPIM_Mode                       (SD_SPIM_MODE)
#define SD_SPIM_DATAWIDHT                  (SD_SPIM_DATA_WIDTH)
#define SD_SPIM_InternalInterruptEnabled   (0u)

#define SD_SPIM_TXBUFFERSIZE   (SD_SPIM_TX_BUFFER_SIZE)
#define SD_SPIM_RXBUFFERSIZE   (SD_SPIM_RX_BUFFER_SIZE)

#define SD_SPIM_TXBUFFER       SD_SPIM_txBuffer
#define SD_SPIM_RXBUFFER       SD_SPIM_rxBuffer

#endif /* (CY_SPIM_SD_SPIM_H) */


/* [] END OF FILE */
