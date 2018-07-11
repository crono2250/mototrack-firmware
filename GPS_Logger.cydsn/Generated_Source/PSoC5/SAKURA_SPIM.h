/*******************************************************************************
* File Name: SAKURA_SPIM.h
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

#if !defined(CY_SPIM_SAKURA_SPIM_H)
#define CY_SPIM_SAKURA_SPIM_H

#include "cyfitter.h"
#include "cytypes.h"
#include "CyLib.h" /* For CyEnterCriticalSection() and CyExitCriticalSection() functions */


/***************************************
*   Conditional Compilation Parameters
***************************************/

#define SAKURA_SPIM_INTERNAL_CLOCK             (1u)

#if(0u != SAKURA_SPIM_INTERNAL_CLOCK)
    #include "SAKURA_SPIM_IntClock.h"
#endif /* (0u != SAKURA_SPIM_INTERNAL_CLOCK) */

#define SAKURA_SPIM_MODE                       (1u)
#define SAKURA_SPIM_DATA_WIDTH                 (8u)
#define SAKURA_SPIM_MODE_USE_ZERO              (1u)
#define SAKURA_SPIM_BIDIRECTIONAL_MODE         (0u)

/* Internal interrupt handling */
#define SAKURA_SPIM_TX_BUFFER_SIZE             (4u)
#define SAKURA_SPIM_RX_BUFFER_SIZE             (4u)
#define SAKURA_SPIM_INTERNAL_TX_INT_ENABLED    (0u)
#define SAKURA_SPIM_INTERNAL_RX_INT_ENABLED    (0u)

#define SAKURA_SPIM_SINGLE_REG_SIZE            (8u)
#define SAKURA_SPIM_USE_SECOND_DATAPATH        (SAKURA_SPIM_DATA_WIDTH > SAKURA_SPIM_SINGLE_REG_SIZE)

#define SAKURA_SPIM_FIFO_SIZE                  (4u)
#define SAKURA_SPIM_TX_SOFTWARE_BUF_ENABLED    ((0u != SAKURA_SPIM_INTERNAL_TX_INT_ENABLED) && \
                                                     (SAKURA_SPIM_TX_BUFFER_SIZE > SAKURA_SPIM_FIFO_SIZE))

#define SAKURA_SPIM_RX_SOFTWARE_BUF_ENABLED    ((0u != SAKURA_SPIM_INTERNAL_RX_INT_ENABLED) && \
                                                     (SAKURA_SPIM_RX_BUFFER_SIZE > SAKURA_SPIM_FIFO_SIZE))


/***************************************
*        Data Struct Definition
***************************************/

/* Sleep Mode API Support */
typedef struct
{
    uint8 enableState;
    uint8 cntrPeriod;
} SAKURA_SPIM_BACKUP_STRUCT;


/***************************************
*        Function Prototypes
***************************************/

void  SAKURA_SPIM_Init(void)                           ;
void  SAKURA_SPIM_Enable(void)                         ;
void  SAKURA_SPIM_Start(void)                          ;
void  SAKURA_SPIM_Stop(void)                           ;

void  SAKURA_SPIM_EnableTxInt(void)                    ;
void  SAKURA_SPIM_EnableRxInt(void)                    ;
void  SAKURA_SPIM_DisableTxInt(void)                   ;
void  SAKURA_SPIM_DisableRxInt(void)                   ;

void  SAKURA_SPIM_Sleep(void)                          ;
void  SAKURA_SPIM_Wakeup(void)                         ;
void  SAKURA_SPIM_SaveConfig(void)                     ;
void  SAKURA_SPIM_RestoreConfig(void)                  ;

void  SAKURA_SPIM_SetTxInterruptMode(uint8 intSrc)     ;
void  SAKURA_SPIM_SetRxInterruptMode(uint8 intSrc)     ;
uint8 SAKURA_SPIM_ReadTxStatus(void)                   ;
uint8 SAKURA_SPIM_ReadRxStatus(void)                   ;
void  SAKURA_SPIM_WriteTxData(uint8 txData)  \
                                                            ;
uint8 SAKURA_SPIM_ReadRxData(void) \
                                                            ;
uint8 SAKURA_SPIM_GetRxBufferSize(void)                ;
uint8 SAKURA_SPIM_GetTxBufferSize(void)                ;
void  SAKURA_SPIM_ClearRxBuffer(void)                  ;
void  SAKURA_SPIM_ClearTxBuffer(void)                  ;
void  SAKURA_SPIM_ClearFIFO(void)                              ;
void  SAKURA_SPIM_PutArray(const uint8 buffer[], uint8 byteCount) \
                                                            ;

#if(0u != SAKURA_SPIM_BIDIRECTIONAL_MODE)
    void  SAKURA_SPIM_TxEnable(void)                   ;
    void  SAKURA_SPIM_TxDisable(void)                  ;
#endif /* (0u != SAKURA_SPIM_BIDIRECTIONAL_MODE) */

CY_ISR_PROTO(SAKURA_SPIM_TX_ISR);
CY_ISR_PROTO(SAKURA_SPIM_RX_ISR);


/***************************************
*   Variable with external linkage
***************************************/

extern uint8 SAKURA_SPIM_initVar;


/***************************************
*           API Constants
***************************************/

#define SAKURA_SPIM_TX_ISR_NUMBER     ((uint8) (SAKURA_SPIM_TxInternalInterrupt__INTC_NUMBER))
#define SAKURA_SPIM_RX_ISR_NUMBER     ((uint8) (SAKURA_SPIM_RxInternalInterrupt__INTC_NUMBER))

#define SAKURA_SPIM_TX_ISR_PRIORITY   ((uint8) (SAKURA_SPIM_TxInternalInterrupt__INTC_PRIOR_NUM))
#define SAKURA_SPIM_RX_ISR_PRIORITY   ((uint8) (SAKURA_SPIM_RxInternalInterrupt__INTC_PRIOR_NUM))


/***************************************
*    Initial Parameter Constants
***************************************/

#define SAKURA_SPIM_INT_ON_SPI_DONE    ((uint8) (0u   << SAKURA_SPIM_STS_SPI_DONE_SHIFT))
#define SAKURA_SPIM_INT_ON_TX_EMPTY    ((uint8) (0u   << SAKURA_SPIM_STS_TX_FIFO_EMPTY_SHIFT))
#define SAKURA_SPIM_INT_ON_TX_NOT_FULL ((uint8) (0u << \
                                                                           SAKURA_SPIM_STS_TX_FIFO_NOT_FULL_SHIFT))
#define SAKURA_SPIM_INT_ON_BYTE_COMP   ((uint8) (0u  << SAKURA_SPIM_STS_BYTE_COMPLETE_SHIFT))
#define SAKURA_SPIM_INT_ON_SPI_IDLE    ((uint8) (0u   << SAKURA_SPIM_STS_SPI_IDLE_SHIFT))

/* Disable TX_NOT_FULL if software buffer is used */
#define SAKURA_SPIM_INT_ON_TX_NOT_FULL_DEF ((SAKURA_SPIM_TX_SOFTWARE_BUF_ENABLED) ? \
                                                                        (0u) : (SAKURA_SPIM_INT_ON_TX_NOT_FULL))

/* TX interrupt mask */
#define SAKURA_SPIM_TX_INIT_INTERRUPTS_MASK    (SAKURA_SPIM_INT_ON_SPI_DONE  | \
                                                     SAKURA_SPIM_INT_ON_TX_EMPTY  | \
                                                     SAKURA_SPIM_INT_ON_TX_NOT_FULL_DEF | \
                                                     SAKURA_SPIM_INT_ON_BYTE_COMP | \
                                                     SAKURA_SPIM_INT_ON_SPI_IDLE)

#define SAKURA_SPIM_INT_ON_RX_FULL         ((uint8) (0u << \
                                                                          SAKURA_SPIM_STS_RX_FIFO_FULL_SHIFT))
#define SAKURA_SPIM_INT_ON_RX_NOT_EMPTY    ((uint8) (0u << \
                                                                          SAKURA_SPIM_STS_RX_FIFO_NOT_EMPTY_SHIFT))
#define SAKURA_SPIM_INT_ON_RX_OVER         ((uint8) (0u << \
                                                                          SAKURA_SPIM_STS_RX_FIFO_OVERRUN_SHIFT))

/* RX interrupt mask */
#define SAKURA_SPIM_RX_INIT_INTERRUPTS_MASK    (SAKURA_SPIM_INT_ON_RX_FULL      | \
                                                     SAKURA_SPIM_INT_ON_RX_NOT_EMPTY | \
                                                     SAKURA_SPIM_INT_ON_RX_OVER)
/* Nubmer of bits to receive/transmit */
#define SAKURA_SPIM_BITCTR_INIT            (((uint8) (SAKURA_SPIM_DATA_WIDTH << 1u)) - 1u)


/***************************************
*             Registers
***************************************/
#if(CY_PSOC3 || CY_PSOC5)
    #define SAKURA_SPIM_TXDATA_REG (* (reg8 *) \
                                                SAKURA_SPIM_BSPIM_sR8_Dp_u0__F0_REG)
    #define SAKURA_SPIM_TXDATA_PTR (  (reg8 *) \
                                                SAKURA_SPIM_BSPIM_sR8_Dp_u0__F0_REG)
    #define SAKURA_SPIM_RXDATA_REG (* (reg8 *) \
                                                SAKURA_SPIM_BSPIM_sR8_Dp_u0__F1_REG)
    #define SAKURA_SPIM_RXDATA_PTR (  (reg8 *) \
                                                SAKURA_SPIM_BSPIM_sR8_Dp_u0__F1_REG)
#else   /* PSOC4 */
    #if(SAKURA_SPIM_USE_SECOND_DATAPATH)
        #define SAKURA_SPIM_TXDATA_REG (* (reg16 *) \
                                          SAKURA_SPIM_BSPIM_sR8_Dp_u0__16BIT_F0_REG)
        #define SAKURA_SPIM_TXDATA_PTR (  (reg16 *) \
                                          SAKURA_SPIM_BSPIM_sR8_Dp_u0__16BIT_F0_REG)
        #define SAKURA_SPIM_RXDATA_REG (* (reg16 *) \
                                          SAKURA_SPIM_BSPIM_sR8_Dp_u0__16BIT_F1_REG)
        #define SAKURA_SPIM_RXDATA_PTR (  (reg16 *) \
                                          SAKURA_SPIM_BSPIM_sR8_Dp_u0__16BIT_F1_REG)
    #else
        #define SAKURA_SPIM_TXDATA_REG (* (reg8 *) \
                                                SAKURA_SPIM_BSPIM_sR8_Dp_u0__F0_REG)
        #define SAKURA_SPIM_TXDATA_PTR (  (reg8 *) \
                                                SAKURA_SPIM_BSPIM_sR8_Dp_u0__F0_REG)
        #define SAKURA_SPIM_RXDATA_REG (* (reg8 *) \
                                                SAKURA_SPIM_BSPIM_sR8_Dp_u0__F1_REG)
        #define SAKURA_SPIM_RXDATA_PTR (  (reg8 *) \
                                                SAKURA_SPIM_BSPIM_sR8_Dp_u0__F1_REG)
    #endif /* (SAKURA_SPIM_USE_SECOND_DATAPATH) */
#endif     /* (CY_PSOC3 || CY_PSOC5) */

#define SAKURA_SPIM_AUX_CONTROL_DP0_REG (* (reg8 *) \
                                        SAKURA_SPIM_BSPIM_sR8_Dp_u0__DP_AUX_CTL_REG)
#define SAKURA_SPIM_AUX_CONTROL_DP0_PTR (  (reg8 *) \
                                        SAKURA_SPIM_BSPIM_sR8_Dp_u0__DP_AUX_CTL_REG)

#if(SAKURA_SPIM_USE_SECOND_DATAPATH)
    #define SAKURA_SPIM_AUX_CONTROL_DP1_REG  (* (reg8 *) \
                                        SAKURA_SPIM_BSPIM_sR8_Dp_u1__DP_AUX_CTL_REG)
    #define SAKURA_SPIM_AUX_CONTROL_DP1_PTR  (  (reg8 *) \
                                        SAKURA_SPIM_BSPIM_sR8_Dp_u1__DP_AUX_CTL_REG)
#endif /* (SAKURA_SPIM_USE_SECOND_DATAPATH) */

#define SAKURA_SPIM_COUNTER_PERIOD_REG     (* (reg8 *) SAKURA_SPIM_BSPIM_BitCounter__PERIOD_REG)
#define SAKURA_SPIM_COUNTER_PERIOD_PTR     (  (reg8 *) SAKURA_SPIM_BSPIM_BitCounter__PERIOD_REG)
#define SAKURA_SPIM_COUNTER_CONTROL_REG    (* (reg8 *) SAKURA_SPIM_BSPIM_BitCounter__CONTROL_AUX_CTL_REG)
#define SAKURA_SPIM_COUNTER_CONTROL_PTR    (  (reg8 *) SAKURA_SPIM_BSPIM_BitCounter__CONTROL_AUX_CTL_REG)

#define SAKURA_SPIM_TX_STATUS_REG          (* (reg8 *) SAKURA_SPIM_BSPIM_TxStsReg__STATUS_REG)
#define SAKURA_SPIM_TX_STATUS_PTR          (  (reg8 *) SAKURA_SPIM_BSPIM_TxStsReg__STATUS_REG)
#define SAKURA_SPIM_RX_STATUS_REG          (* (reg8 *) SAKURA_SPIM_BSPIM_RxStsReg__STATUS_REG)
#define SAKURA_SPIM_RX_STATUS_PTR          (  (reg8 *) SAKURA_SPIM_BSPIM_RxStsReg__STATUS_REG)

#define SAKURA_SPIM_CONTROL_REG            (* (reg8 *) \
                                      SAKURA_SPIM_BSPIM_BidirMode_CtrlReg__CONTROL_REG)
#define SAKURA_SPIM_CONTROL_PTR            (  (reg8 *) \
                                      SAKURA_SPIM_BSPIM_BidirMode_CtrlReg__CONTROL_REG)

#define SAKURA_SPIM_TX_STATUS_MASK_REG     (* (reg8 *) SAKURA_SPIM_BSPIM_TxStsReg__MASK_REG)
#define SAKURA_SPIM_TX_STATUS_MASK_PTR     (  (reg8 *) SAKURA_SPIM_BSPIM_TxStsReg__MASK_REG)
#define SAKURA_SPIM_RX_STATUS_MASK_REG     (* (reg8 *) SAKURA_SPIM_BSPIM_RxStsReg__MASK_REG)
#define SAKURA_SPIM_RX_STATUS_MASK_PTR     (  (reg8 *) SAKURA_SPIM_BSPIM_RxStsReg__MASK_REG)

#define SAKURA_SPIM_TX_STATUS_ACTL_REG     (* (reg8 *) SAKURA_SPIM_BSPIM_TxStsReg__STATUS_AUX_CTL_REG)
#define SAKURA_SPIM_TX_STATUS_ACTL_PTR     (  (reg8 *) SAKURA_SPIM_BSPIM_TxStsReg__STATUS_AUX_CTL_REG)
#define SAKURA_SPIM_RX_STATUS_ACTL_REG     (* (reg8 *) SAKURA_SPIM_BSPIM_RxStsReg__STATUS_AUX_CTL_REG)
#define SAKURA_SPIM_RX_STATUS_ACTL_PTR     (  (reg8 *) SAKURA_SPIM_BSPIM_RxStsReg__STATUS_AUX_CTL_REG)

#if(SAKURA_SPIM_USE_SECOND_DATAPATH)
    #define SAKURA_SPIM_AUX_CONTROLDP1     (SAKURA_SPIM_AUX_CONTROL_DP1_REG)
#endif /* (SAKURA_SPIM_USE_SECOND_DATAPATH) */


/***************************************
*       Register Constants
***************************************/

/* Status Register Definitions */
#define SAKURA_SPIM_STS_SPI_DONE_SHIFT             (0x00u)
#define SAKURA_SPIM_STS_TX_FIFO_EMPTY_SHIFT        (0x01u)
#define SAKURA_SPIM_STS_TX_FIFO_NOT_FULL_SHIFT     (0x02u)
#define SAKURA_SPIM_STS_BYTE_COMPLETE_SHIFT        (0x03u)
#define SAKURA_SPIM_STS_SPI_IDLE_SHIFT             (0x04u)
#define SAKURA_SPIM_STS_RX_FIFO_FULL_SHIFT         (0x04u)
#define SAKURA_SPIM_STS_RX_FIFO_NOT_EMPTY_SHIFT    (0x05u)
#define SAKURA_SPIM_STS_RX_FIFO_OVERRUN_SHIFT      (0x06u)

#define SAKURA_SPIM_STS_SPI_DONE           ((uint8) (0x01u << SAKURA_SPIM_STS_SPI_DONE_SHIFT))
#define SAKURA_SPIM_STS_TX_FIFO_EMPTY      ((uint8) (0x01u << SAKURA_SPIM_STS_TX_FIFO_EMPTY_SHIFT))
#define SAKURA_SPIM_STS_TX_FIFO_NOT_FULL   ((uint8) (0x01u << SAKURA_SPIM_STS_TX_FIFO_NOT_FULL_SHIFT))
#define SAKURA_SPIM_STS_BYTE_COMPLETE      ((uint8) (0x01u << SAKURA_SPIM_STS_BYTE_COMPLETE_SHIFT))
#define SAKURA_SPIM_STS_SPI_IDLE           ((uint8) (0x01u << SAKURA_SPIM_STS_SPI_IDLE_SHIFT))
#define SAKURA_SPIM_STS_RX_FIFO_FULL       ((uint8) (0x01u << SAKURA_SPIM_STS_RX_FIFO_FULL_SHIFT))
#define SAKURA_SPIM_STS_RX_FIFO_NOT_EMPTY  ((uint8) (0x01u << SAKURA_SPIM_STS_RX_FIFO_NOT_EMPTY_SHIFT))
#define SAKURA_SPIM_STS_RX_FIFO_OVERRUN    ((uint8) (0x01u << SAKURA_SPIM_STS_RX_FIFO_OVERRUN_SHIFT))

/* TX and RX masks for clear on read bits */
#define SAKURA_SPIM_TX_STS_CLR_ON_RD_BYTES_MASK    (0x09u)
#define SAKURA_SPIM_RX_STS_CLR_ON_RD_BYTES_MASK    (0x40u)

/* StatusI Register Interrupt Enable Control Bits */
/* As defined by the Register map for the AUX Control Register */
#define SAKURA_SPIM_INT_ENABLE     (0x10u) /* Enable interrupt from statusi */
#define SAKURA_SPIM_TX_FIFO_CLR    (0x01u) /* F0 - TX FIFO */
#define SAKURA_SPIM_RX_FIFO_CLR    (0x02u) /* F1 - RX FIFO */
#define SAKURA_SPIM_FIFO_CLR       (SAKURA_SPIM_TX_FIFO_CLR | SAKURA_SPIM_RX_FIFO_CLR)

/* Bit Counter (7-bit) Control Register Bit Definitions */
/* As defined by the Register map for the AUX Control Register */
#define SAKURA_SPIM_CNTR_ENABLE    (0x20u) /* Enable CNT7 */

/* Bi-Directional mode control bit */
#define SAKURA_SPIM_CTRL_TX_SIGNAL_EN  (0x01u)

/* Datapath Auxillary Control Register definitions */
#define SAKURA_SPIM_AUX_CTRL_FIFO0_CLR         (0x01u)
#define SAKURA_SPIM_AUX_CTRL_FIFO1_CLR         (0x02u)
#define SAKURA_SPIM_AUX_CTRL_FIFO0_LVL         (0x04u)
#define SAKURA_SPIM_AUX_CTRL_FIFO1_LVL         (0x08u)
#define SAKURA_SPIM_STATUS_ACTL_INT_EN_MASK    (0x10u)

/* Component disabled */
#define SAKURA_SPIM_DISABLED   (0u)


/***************************************
*       Macros
***************************************/

/* Returns true if componentn enabled */
#define SAKURA_SPIM_IS_ENABLED (0u != (SAKURA_SPIM_TX_STATUS_ACTL_REG & SAKURA_SPIM_INT_ENABLE))

/* Retuns TX status register */
#define SAKURA_SPIM_GET_STATUS_TX(swTxSts) ( (uint8)(SAKURA_SPIM_TX_STATUS_REG | \
                                                          ((swTxSts) & SAKURA_SPIM_TX_STS_CLR_ON_RD_BYTES_MASK)) )
/* Retuns RX status register */
#define SAKURA_SPIM_GET_STATUS_RX(swRxSts) ( (uint8)(SAKURA_SPIM_RX_STATUS_REG | \
                                                          ((swRxSts) & SAKURA_SPIM_RX_STS_CLR_ON_RD_BYTES_MASK)) )


/***************************************
* The following code is DEPRECATED and 
* should not be used in new projects.
***************************************/

#define SAKURA_SPIM_WriteByte   SAKURA_SPIM_WriteTxData
#define SAKURA_SPIM_ReadByte    SAKURA_SPIM_ReadRxData
void  SAKURA_SPIM_SetInterruptMode(uint8 intSrc)       ;
uint8 SAKURA_SPIM_ReadStatus(void)                     ;
void  SAKURA_SPIM_EnableInt(void)                      ;
void  SAKURA_SPIM_DisableInt(void)                     ;

#define SAKURA_SPIM_TXDATA                 (SAKURA_SPIM_TXDATA_REG)
#define SAKURA_SPIM_RXDATA                 (SAKURA_SPIM_RXDATA_REG)
#define SAKURA_SPIM_AUX_CONTROLDP0         (SAKURA_SPIM_AUX_CONTROL_DP0_REG)
#define SAKURA_SPIM_TXBUFFERREAD           (SAKURA_SPIM_txBufferRead)
#define SAKURA_SPIM_TXBUFFERWRITE          (SAKURA_SPIM_txBufferWrite)
#define SAKURA_SPIM_RXBUFFERREAD           (SAKURA_SPIM_rxBufferRead)
#define SAKURA_SPIM_RXBUFFERWRITE          (SAKURA_SPIM_rxBufferWrite)

#define SAKURA_SPIM_COUNTER_PERIOD         (SAKURA_SPIM_COUNTER_PERIOD_REG)
#define SAKURA_SPIM_COUNTER_CONTROL        (SAKURA_SPIM_COUNTER_CONTROL_REG)
#define SAKURA_SPIM_STATUS                 (SAKURA_SPIM_TX_STATUS_REG)
#define SAKURA_SPIM_CONTROL                (SAKURA_SPIM_CONTROL_REG)
#define SAKURA_SPIM_STATUS_MASK            (SAKURA_SPIM_TX_STATUS_MASK_REG)
#define SAKURA_SPIM_STATUS_ACTL            (SAKURA_SPIM_TX_STATUS_ACTL_REG)

#define SAKURA_SPIM_INIT_INTERRUPTS_MASK  (SAKURA_SPIM_INT_ON_SPI_DONE     | \
                                                SAKURA_SPIM_INT_ON_TX_EMPTY     | \
                                                SAKURA_SPIM_INT_ON_TX_NOT_FULL_DEF  | \
                                                SAKURA_SPIM_INT_ON_RX_FULL      | \
                                                SAKURA_SPIM_INT_ON_RX_NOT_EMPTY | \
                                                SAKURA_SPIM_INT_ON_RX_OVER      | \
                                                SAKURA_SPIM_INT_ON_BYTE_COMP)
                                                
#define SAKURA_SPIM_DataWidth                  (SAKURA_SPIM_DATA_WIDTH)
#define SAKURA_SPIM_InternalClockUsed          (SAKURA_SPIM_INTERNAL_CLOCK)
#define SAKURA_SPIM_InternalTxInterruptEnabled (SAKURA_SPIM_INTERNAL_TX_INT_ENABLED)
#define SAKURA_SPIM_InternalRxInterruptEnabled (SAKURA_SPIM_INTERNAL_RX_INT_ENABLED)
#define SAKURA_SPIM_ModeUseZero                (SAKURA_SPIM_MODE_USE_ZERO)
#define SAKURA_SPIM_BidirectionalMode          (SAKURA_SPIM_BIDIRECTIONAL_MODE)
#define SAKURA_SPIM_Mode                       (SAKURA_SPIM_MODE)
#define SAKURA_SPIM_DATAWIDHT                  (SAKURA_SPIM_DATA_WIDTH)
#define SAKURA_SPIM_InternalInterruptEnabled   (0u)

#define SAKURA_SPIM_TXBUFFERSIZE   (SAKURA_SPIM_TX_BUFFER_SIZE)
#define SAKURA_SPIM_RXBUFFERSIZE   (SAKURA_SPIM_RX_BUFFER_SIZE)

#define SAKURA_SPIM_TXBUFFER       SAKURA_SPIM_txBuffer
#define SAKURA_SPIM_RXBUFFER       SAKURA_SPIM_rxBuffer

#endif /* (CY_SPIM_SAKURA_SPIM_H) */


/* [] END OF FILE */
