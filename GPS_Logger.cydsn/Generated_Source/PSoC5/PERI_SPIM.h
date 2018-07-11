/*******************************************************************************
* File Name: PERI_SPIM.h
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

#if !defined(CY_SPIM_PERI_SPIM_H)
#define CY_SPIM_PERI_SPIM_H

#include "cyfitter.h"
#include "cytypes.h"
#include "CyLib.h" /* For CyEnterCriticalSection() and CyExitCriticalSection() functions */


/***************************************
*   Conditional Compilation Parameters
***************************************/

#define PERI_SPIM_INTERNAL_CLOCK             (0u)

#if(0u != PERI_SPIM_INTERNAL_CLOCK)
    #include "PERI_SPIM_IntClock.h"
#endif /* (0u != PERI_SPIM_INTERNAL_CLOCK) */

#define PERI_SPIM_MODE                       (4u)
#define PERI_SPIM_DATA_WIDTH                 (8u)
#define PERI_SPIM_MODE_USE_ZERO              (0u)
#define PERI_SPIM_BIDIRECTIONAL_MODE         (0u)

/* Internal interrupt handling */
#define PERI_SPIM_TX_BUFFER_SIZE             (4u)
#define PERI_SPIM_RX_BUFFER_SIZE             (4u)
#define PERI_SPIM_INTERNAL_TX_INT_ENABLED    (0u)
#define PERI_SPIM_INTERNAL_RX_INT_ENABLED    (0u)

#define PERI_SPIM_SINGLE_REG_SIZE            (8u)
#define PERI_SPIM_USE_SECOND_DATAPATH        (PERI_SPIM_DATA_WIDTH > PERI_SPIM_SINGLE_REG_SIZE)

#define PERI_SPIM_FIFO_SIZE                  (4u)
#define PERI_SPIM_TX_SOFTWARE_BUF_ENABLED    ((0u != PERI_SPIM_INTERNAL_TX_INT_ENABLED) && \
                                                     (PERI_SPIM_TX_BUFFER_SIZE > PERI_SPIM_FIFO_SIZE))

#define PERI_SPIM_RX_SOFTWARE_BUF_ENABLED    ((0u != PERI_SPIM_INTERNAL_RX_INT_ENABLED) && \
                                                     (PERI_SPIM_RX_BUFFER_SIZE > PERI_SPIM_FIFO_SIZE))


/***************************************
*        Data Struct Definition
***************************************/

/* Sleep Mode API Support */
typedef struct
{
    uint8 enableState;
    uint8 cntrPeriod;
} PERI_SPIM_BACKUP_STRUCT;


/***************************************
*        Function Prototypes
***************************************/

void  PERI_SPIM_Init(void)                           ;
void  PERI_SPIM_Enable(void)                         ;
void  PERI_SPIM_Start(void)                          ;
void  PERI_SPIM_Stop(void)                           ;

void  PERI_SPIM_EnableTxInt(void)                    ;
void  PERI_SPIM_EnableRxInt(void)                    ;
void  PERI_SPIM_DisableTxInt(void)                   ;
void  PERI_SPIM_DisableRxInt(void)                   ;

void  PERI_SPIM_Sleep(void)                          ;
void  PERI_SPIM_Wakeup(void)                         ;
void  PERI_SPIM_SaveConfig(void)                     ;
void  PERI_SPIM_RestoreConfig(void)                  ;

void  PERI_SPIM_SetTxInterruptMode(uint8 intSrc)     ;
void  PERI_SPIM_SetRxInterruptMode(uint8 intSrc)     ;
uint8 PERI_SPIM_ReadTxStatus(void)                   ;
uint8 PERI_SPIM_ReadRxStatus(void)                   ;
void  PERI_SPIM_WriteTxData(uint8 txData)  \
                                                            ;
uint8 PERI_SPIM_ReadRxData(void) \
                                                            ;
uint8 PERI_SPIM_GetRxBufferSize(void)                ;
uint8 PERI_SPIM_GetTxBufferSize(void)                ;
void  PERI_SPIM_ClearRxBuffer(void)                  ;
void  PERI_SPIM_ClearTxBuffer(void)                  ;
void  PERI_SPIM_ClearFIFO(void)                              ;
void  PERI_SPIM_PutArray(const uint8 buffer[], uint8 byteCount) \
                                                            ;

#if(0u != PERI_SPIM_BIDIRECTIONAL_MODE)
    void  PERI_SPIM_TxEnable(void)                   ;
    void  PERI_SPIM_TxDisable(void)                  ;
#endif /* (0u != PERI_SPIM_BIDIRECTIONAL_MODE) */

CY_ISR_PROTO(PERI_SPIM_TX_ISR);
CY_ISR_PROTO(PERI_SPIM_RX_ISR);


/***************************************
*   Variable with external linkage
***************************************/

extern uint8 PERI_SPIM_initVar;


/***************************************
*           API Constants
***************************************/

#define PERI_SPIM_TX_ISR_NUMBER     ((uint8) (PERI_SPIM_TxInternalInterrupt__INTC_NUMBER))
#define PERI_SPIM_RX_ISR_NUMBER     ((uint8) (PERI_SPIM_RxInternalInterrupt__INTC_NUMBER))

#define PERI_SPIM_TX_ISR_PRIORITY   ((uint8) (PERI_SPIM_TxInternalInterrupt__INTC_PRIOR_NUM))
#define PERI_SPIM_RX_ISR_PRIORITY   ((uint8) (PERI_SPIM_RxInternalInterrupt__INTC_PRIOR_NUM))


/***************************************
*    Initial Parameter Constants
***************************************/

#define PERI_SPIM_INT_ON_SPI_DONE    ((uint8) (0u   << PERI_SPIM_STS_SPI_DONE_SHIFT))
#define PERI_SPIM_INT_ON_TX_EMPTY    ((uint8) (0u   << PERI_SPIM_STS_TX_FIFO_EMPTY_SHIFT))
#define PERI_SPIM_INT_ON_TX_NOT_FULL ((uint8) (0u << \
                                                                           PERI_SPIM_STS_TX_FIFO_NOT_FULL_SHIFT))
#define PERI_SPIM_INT_ON_BYTE_COMP   ((uint8) (0u  << PERI_SPIM_STS_BYTE_COMPLETE_SHIFT))
#define PERI_SPIM_INT_ON_SPI_IDLE    ((uint8) (0u   << PERI_SPIM_STS_SPI_IDLE_SHIFT))

/* Disable TX_NOT_FULL if software buffer is used */
#define PERI_SPIM_INT_ON_TX_NOT_FULL_DEF ((PERI_SPIM_TX_SOFTWARE_BUF_ENABLED) ? \
                                                                        (0u) : (PERI_SPIM_INT_ON_TX_NOT_FULL))

/* TX interrupt mask */
#define PERI_SPIM_TX_INIT_INTERRUPTS_MASK    (PERI_SPIM_INT_ON_SPI_DONE  | \
                                                     PERI_SPIM_INT_ON_TX_EMPTY  | \
                                                     PERI_SPIM_INT_ON_TX_NOT_FULL_DEF | \
                                                     PERI_SPIM_INT_ON_BYTE_COMP | \
                                                     PERI_SPIM_INT_ON_SPI_IDLE)

#define PERI_SPIM_INT_ON_RX_FULL         ((uint8) (0u << \
                                                                          PERI_SPIM_STS_RX_FIFO_FULL_SHIFT))
#define PERI_SPIM_INT_ON_RX_NOT_EMPTY    ((uint8) (0u << \
                                                                          PERI_SPIM_STS_RX_FIFO_NOT_EMPTY_SHIFT))
#define PERI_SPIM_INT_ON_RX_OVER         ((uint8) (0u << \
                                                                          PERI_SPIM_STS_RX_FIFO_OVERRUN_SHIFT))

/* RX interrupt mask */
#define PERI_SPIM_RX_INIT_INTERRUPTS_MASK    (PERI_SPIM_INT_ON_RX_FULL      | \
                                                     PERI_SPIM_INT_ON_RX_NOT_EMPTY | \
                                                     PERI_SPIM_INT_ON_RX_OVER)
/* Nubmer of bits to receive/transmit */
#define PERI_SPIM_BITCTR_INIT            (((uint8) (PERI_SPIM_DATA_WIDTH << 1u)) - 1u)


/***************************************
*             Registers
***************************************/
#if(CY_PSOC3 || CY_PSOC5)
    #define PERI_SPIM_TXDATA_REG (* (reg8 *) \
                                                PERI_SPIM_BSPIM_sR8_Dp_u0__F0_REG)
    #define PERI_SPIM_TXDATA_PTR (  (reg8 *) \
                                                PERI_SPIM_BSPIM_sR8_Dp_u0__F0_REG)
    #define PERI_SPIM_RXDATA_REG (* (reg8 *) \
                                                PERI_SPIM_BSPIM_sR8_Dp_u0__F1_REG)
    #define PERI_SPIM_RXDATA_PTR (  (reg8 *) \
                                                PERI_SPIM_BSPIM_sR8_Dp_u0__F1_REG)
#else   /* PSOC4 */
    #if(PERI_SPIM_USE_SECOND_DATAPATH)
        #define PERI_SPIM_TXDATA_REG (* (reg16 *) \
                                          PERI_SPIM_BSPIM_sR8_Dp_u0__16BIT_F0_REG)
        #define PERI_SPIM_TXDATA_PTR (  (reg16 *) \
                                          PERI_SPIM_BSPIM_sR8_Dp_u0__16BIT_F0_REG)
        #define PERI_SPIM_RXDATA_REG (* (reg16 *) \
                                          PERI_SPIM_BSPIM_sR8_Dp_u0__16BIT_F1_REG)
        #define PERI_SPIM_RXDATA_PTR (  (reg16 *) \
                                          PERI_SPIM_BSPIM_sR8_Dp_u0__16BIT_F1_REG)
    #else
        #define PERI_SPIM_TXDATA_REG (* (reg8 *) \
                                                PERI_SPIM_BSPIM_sR8_Dp_u0__F0_REG)
        #define PERI_SPIM_TXDATA_PTR (  (reg8 *) \
                                                PERI_SPIM_BSPIM_sR8_Dp_u0__F0_REG)
        #define PERI_SPIM_RXDATA_REG (* (reg8 *) \
                                                PERI_SPIM_BSPIM_sR8_Dp_u0__F1_REG)
        #define PERI_SPIM_RXDATA_PTR (  (reg8 *) \
                                                PERI_SPIM_BSPIM_sR8_Dp_u0__F1_REG)
    #endif /* (PERI_SPIM_USE_SECOND_DATAPATH) */
#endif     /* (CY_PSOC3 || CY_PSOC5) */

#define PERI_SPIM_AUX_CONTROL_DP0_REG (* (reg8 *) \
                                        PERI_SPIM_BSPIM_sR8_Dp_u0__DP_AUX_CTL_REG)
#define PERI_SPIM_AUX_CONTROL_DP0_PTR (  (reg8 *) \
                                        PERI_SPIM_BSPIM_sR8_Dp_u0__DP_AUX_CTL_REG)

#if(PERI_SPIM_USE_SECOND_DATAPATH)
    #define PERI_SPIM_AUX_CONTROL_DP1_REG  (* (reg8 *) \
                                        PERI_SPIM_BSPIM_sR8_Dp_u1__DP_AUX_CTL_REG)
    #define PERI_SPIM_AUX_CONTROL_DP1_PTR  (  (reg8 *) \
                                        PERI_SPIM_BSPIM_sR8_Dp_u1__DP_AUX_CTL_REG)
#endif /* (PERI_SPIM_USE_SECOND_DATAPATH) */

#define PERI_SPIM_COUNTER_PERIOD_REG     (* (reg8 *) PERI_SPIM_BSPIM_BitCounter__PERIOD_REG)
#define PERI_SPIM_COUNTER_PERIOD_PTR     (  (reg8 *) PERI_SPIM_BSPIM_BitCounter__PERIOD_REG)
#define PERI_SPIM_COUNTER_CONTROL_REG    (* (reg8 *) PERI_SPIM_BSPIM_BitCounter__CONTROL_AUX_CTL_REG)
#define PERI_SPIM_COUNTER_CONTROL_PTR    (  (reg8 *) PERI_SPIM_BSPIM_BitCounter__CONTROL_AUX_CTL_REG)

#define PERI_SPIM_TX_STATUS_REG          (* (reg8 *) PERI_SPIM_BSPIM_TxStsReg__STATUS_REG)
#define PERI_SPIM_TX_STATUS_PTR          (  (reg8 *) PERI_SPIM_BSPIM_TxStsReg__STATUS_REG)
#define PERI_SPIM_RX_STATUS_REG          (* (reg8 *) PERI_SPIM_BSPIM_RxStsReg__STATUS_REG)
#define PERI_SPIM_RX_STATUS_PTR          (  (reg8 *) PERI_SPIM_BSPIM_RxStsReg__STATUS_REG)

#define PERI_SPIM_CONTROL_REG            (* (reg8 *) \
                                      PERI_SPIM_BSPIM_BidirMode_CtrlReg__CONTROL_REG)
#define PERI_SPIM_CONTROL_PTR            (  (reg8 *) \
                                      PERI_SPIM_BSPIM_BidirMode_CtrlReg__CONTROL_REG)

#define PERI_SPIM_TX_STATUS_MASK_REG     (* (reg8 *) PERI_SPIM_BSPIM_TxStsReg__MASK_REG)
#define PERI_SPIM_TX_STATUS_MASK_PTR     (  (reg8 *) PERI_SPIM_BSPIM_TxStsReg__MASK_REG)
#define PERI_SPIM_RX_STATUS_MASK_REG     (* (reg8 *) PERI_SPIM_BSPIM_RxStsReg__MASK_REG)
#define PERI_SPIM_RX_STATUS_MASK_PTR     (  (reg8 *) PERI_SPIM_BSPIM_RxStsReg__MASK_REG)

#define PERI_SPIM_TX_STATUS_ACTL_REG     (* (reg8 *) PERI_SPIM_BSPIM_TxStsReg__STATUS_AUX_CTL_REG)
#define PERI_SPIM_TX_STATUS_ACTL_PTR     (  (reg8 *) PERI_SPIM_BSPIM_TxStsReg__STATUS_AUX_CTL_REG)
#define PERI_SPIM_RX_STATUS_ACTL_REG     (* (reg8 *) PERI_SPIM_BSPIM_RxStsReg__STATUS_AUX_CTL_REG)
#define PERI_SPIM_RX_STATUS_ACTL_PTR     (  (reg8 *) PERI_SPIM_BSPIM_RxStsReg__STATUS_AUX_CTL_REG)

#if(PERI_SPIM_USE_SECOND_DATAPATH)
    #define PERI_SPIM_AUX_CONTROLDP1     (PERI_SPIM_AUX_CONTROL_DP1_REG)
#endif /* (PERI_SPIM_USE_SECOND_DATAPATH) */


/***************************************
*       Register Constants
***************************************/

/* Status Register Definitions */
#define PERI_SPIM_STS_SPI_DONE_SHIFT             (0x00u)
#define PERI_SPIM_STS_TX_FIFO_EMPTY_SHIFT        (0x01u)
#define PERI_SPIM_STS_TX_FIFO_NOT_FULL_SHIFT     (0x02u)
#define PERI_SPIM_STS_BYTE_COMPLETE_SHIFT        (0x03u)
#define PERI_SPIM_STS_SPI_IDLE_SHIFT             (0x04u)
#define PERI_SPIM_STS_RX_FIFO_FULL_SHIFT         (0x04u)
#define PERI_SPIM_STS_RX_FIFO_NOT_EMPTY_SHIFT    (0x05u)
#define PERI_SPIM_STS_RX_FIFO_OVERRUN_SHIFT      (0x06u)

#define PERI_SPIM_STS_SPI_DONE           ((uint8) (0x01u << PERI_SPIM_STS_SPI_DONE_SHIFT))
#define PERI_SPIM_STS_TX_FIFO_EMPTY      ((uint8) (0x01u << PERI_SPIM_STS_TX_FIFO_EMPTY_SHIFT))
#define PERI_SPIM_STS_TX_FIFO_NOT_FULL   ((uint8) (0x01u << PERI_SPIM_STS_TX_FIFO_NOT_FULL_SHIFT))
#define PERI_SPIM_STS_BYTE_COMPLETE      ((uint8) (0x01u << PERI_SPIM_STS_BYTE_COMPLETE_SHIFT))
#define PERI_SPIM_STS_SPI_IDLE           ((uint8) (0x01u << PERI_SPIM_STS_SPI_IDLE_SHIFT))
#define PERI_SPIM_STS_RX_FIFO_FULL       ((uint8) (0x01u << PERI_SPIM_STS_RX_FIFO_FULL_SHIFT))
#define PERI_SPIM_STS_RX_FIFO_NOT_EMPTY  ((uint8) (0x01u << PERI_SPIM_STS_RX_FIFO_NOT_EMPTY_SHIFT))
#define PERI_SPIM_STS_RX_FIFO_OVERRUN    ((uint8) (0x01u << PERI_SPIM_STS_RX_FIFO_OVERRUN_SHIFT))

/* TX and RX masks for clear on read bits */
#define PERI_SPIM_TX_STS_CLR_ON_RD_BYTES_MASK    (0x09u)
#define PERI_SPIM_RX_STS_CLR_ON_RD_BYTES_MASK    (0x40u)

/* StatusI Register Interrupt Enable Control Bits */
/* As defined by the Register map for the AUX Control Register */
#define PERI_SPIM_INT_ENABLE     (0x10u) /* Enable interrupt from statusi */
#define PERI_SPIM_TX_FIFO_CLR    (0x01u) /* F0 - TX FIFO */
#define PERI_SPIM_RX_FIFO_CLR    (0x02u) /* F1 - RX FIFO */
#define PERI_SPIM_FIFO_CLR       (PERI_SPIM_TX_FIFO_CLR | PERI_SPIM_RX_FIFO_CLR)

/* Bit Counter (7-bit) Control Register Bit Definitions */
/* As defined by the Register map for the AUX Control Register */
#define PERI_SPIM_CNTR_ENABLE    (0x20u) /* Enable CNT7 */

/* Bi-Directional mode control bit */
#define PERI_SPIM_CTRL_TX_SIGNAL_EN  (0x01u)

/* Datapath Auxillary Control Register definitions */
#define PERI_SPIM_AUX_CTRL_FIFO0_CLR         (0x01u)
#define PERI_SPIM_AUX_CTRL_FIFO1_CLR         (0x02u)
#define PERI_SPIM_AUX_CTRL_FIFO0_LVL         (0x04u)
#define PERI_SPIM_AUX_CTRL_FIFO1_LVL         (0x08u)
#define PERI_SPIM_STATUS_ACTL_INT_EN_MASK    (0x10u)

/* Component disabled */
#define PERI_SPIM_DISABLED   (0u)


/***************************************
*       Macros
***************************************/

/* Returns true if componentn enabled */
#define PERI_SPIM_IS_ENABLED (0u != (PERI_SPIM_TX_STATUS_ACTL_REG & PERI_SPIM_INT_ENABLE))

/* Retuns TX status register */
#define PERI_SPIM_GET_STATUS_TX(swTxSts) ( (uint8)(PERI_SPIM_TX_STATUS_REG | \
                                                          ((swTxSts) & PERI_SPIM_TX_STS_CLR_ON_RD_BYTES_MASK)) )
/* Retuns RX status register */
#define PERI_SPIM_GET_STATUS_RX(swRxSts) ( (uint8)(PERI_SPIM_RX_STATUS_REG | \
                                                          ((swRxSts) & PERI_SPIM_RX_STS_CLR_ON_RD_BYTES_MASK)) )


/***************************************
* The following code is DEPRECATED and 
* should not be used in new projects.
***************************************/

#define PERI_SPIM_WriteByte   PERI_SPIM_WriteTxData
#define PERI_SPIM_ReadByte    PERI_SPIM_ReadRxData
void  PERI_SPIM_SetInterruptMode(uint8 intSrc)       ;
uint8 PERI_SPIM_ReadStatus(void)                     ;
void  PERI_SPIM_EnableInt(void)                      ;
void  PERI_SPIM_DisableInt(void)                     ;

#define PERI_SPIM_TXDATA                 (PERI_SPIM_TXDATA_REG)
#define PERI_SPIM_RXDATA                 (PERI_SPIM_RXDATA_REG)
#define PERI_SPIM_AUX_CONTROLDP0         (PERI_SPIM_AUX_CONTROL_DP0_REG)
#define PERI_SPIM_TXBUFFERREAD           (PERI_SPIM_txBufferRead)
#define PERI_SPIM_TXBUFFERWRITE          (PERI_SPIM_txBufferWrite)
#define PERI_SPIM_RXBUFFERREAD           (PERI_SPIM_rxBufferRead)
#define PERI_SPIM_RXBUFFERWRITE          (PERI_SPIM_rxBufferWrite)

#define PERI_SPIM_COUNTER_PERIOD         (PERI_SPIM_COUNTER_PERIOD_REG)
#define PERI_SPIM_COUNTER_CONTROL        (PERI_SPIM_COUNTER_CONTROL_REG)
#define PERI_SPIM_STATUS                 (PERI_SPIM_TX_STATUS_REG)
#define PERI_SPIM_CONTROL                (PERI_SPIM_CONTROL_REG)
#define PERI_SPIM_STATUS_MASK            (PERI_SPIM_TX_STATUS_MASK_REG)
#define PERI_SPIM_STATUS_ACTL            (PERI_SPIM_TX_STATUS_ACTL_REG)

#define PERI_SPIM_INIT_INTERRUPTS_MASK  (PERI_SPIM_INT_ON_SPI_DONE     | \
                                                PERI_SPIM_INT_ON_TX_EMPTY     | \
                                                PERI_SPIM_INT_ON_TX_NOT_FULL_DEF  | \
                                                PERI_SPIM_INT_ON_RX_FULL      | \
                                                PERI_SPIM_INT_ON_RX_NOT_EMPTY | \
                                                PERI_SPIM_INT_ON_RX_OVER      | \
                                                PERI_SPIM_INT_ON_BYTE_COMP)
                                                
#define PERI_SPIM_DataWidth                  (PERI_SPIM_DATA_WIDTH)
#define PERI_SPIM_InternalClockUsed          (PERI_SPIM_INTERNAL_CLOCK)
#define PERI_SPIM_InternalTxInterruptEnabled (PERI_SPIM_INTERNAL_TX_INT_ENABLED)
#define PERI_SPIM_InternalRxInterruptEnabled (PERI_SPIM_INTERNAL_RX_INT_ENABLED)
#define PERI_SPIM_ModeUseZero                (PERI_SPIM_MODE_USE_ZERO)
#define PERI_SPIM_BidirectionalMode          (PERI_SPIM_BIDIRECTIONAL_MODE)
#define PERI_SPIM_Mode                       (PERI_SPIM_MODE)
#define PERI_SPIM_DATAWIDHT                  (PERI_SPIM_DATA_WIDTH)
#define PERI_SPIM_InternalInterruptEnabled   (0u)

#define PERI_SPIM_TXBUFFERSIZE   (PERI_SPIM_TX_BUFFER_SIZE)
#define PERI_SPIM_RXBUFFERSIZE   (PERI_SPIM_RX_BUFFER_SIZE)

#define PERI_SPIM_TXBUFFER       PERI_SPIM_txBuffer
#define PERI_SPIM_RXBUFFER       PERI_SPIM_rxBuffer

#endif /* (CY_SPIM_PERI_SPIM_H) */


/* [] END OF FILE */
