/*******************************************************************************
* File Name: UART_VMONINT.c
* Version 2.50
*
* Description:
*  This file provides all Interrupt Service functionality of the UART component
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "UART_VMON.h"
#include "cyapicallbacks.h"


/***************************************
* Custom Declarations
***************************************/
/* `#START CUSTOM_DECLARATIONS` Place your declaration here */

/* `#END` */

#if (UART_VMON_RX_INTERRUPT_ENABLED && (UART_VMON_RX_ENABLED || UART_VMON_HD_ENABLED))
    /*******************************************************************************
    * Function Name: UART_VMON_RXISR
    ********************************************************************************
    *
    * Summary:
    *  Interrupt Service Routine for RX portion of the UART
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  UART_VMON_rxBuffer - RAM buffer pointer for save received data.
    *  UART_VMON_rxBufferWrite - cyclic index for write to rxBuffer,
    *     increments after each byte saved to buffer.
    *  UART_VMON_rxBufferRead - cyclic index for read from rxBuffer,
    *     checked to detect overflow condition.
    *  UART_VMON_rxBufferOverflow - software overflow flag. Set to one
    *     when UART_VMON_rxBufferWrite index overtakes
    *     UART_VMON_rxBufferRead index.
    *  UART_VMON_rxBufferLoopDetect - additional variable to detect overflow.
    *     Set to one when UART_VMON_rxBufferWrite is equal to
    *    UART_VMON_rxBufferRead
    *  UART_VMON_rxAddressMode - this variable contains the Address mode,
    *     selected in customizer or set by UART_SetRxAddressMode() API.
    *  UART_VMON_rxAddressDetected - set to 1 when correct address received,
    *     and analysed to store following addressed data bytes to the buffer.
    *     When not correct address received, set to 0 to skip following data bytes.
    *
    *******************************************************************************/
    CY_ISR(UART_VMON_RXISR)
    {
        uint8 readData;
        uint8 readStatus;
        uint8 increment_pointer = 0u;

    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef UART_VMON_RXISR_ENTRY_CALLBACK
        UART_VMON_RXISR_EntryCallback();
    #endif /* UART_VMON_RXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START UART_VMON_RXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        do
        {
            /* Read receiver status register */
            readStatus = UART_VMON_RXSTATUS_REG;
            /* Copy the same status to readData variable for backward compatibility support 
            *  of the user code in UART_VMON_RXISR_ERROR` section. 
            */
            readData = readStatus;

            if((readStatus & (UART_VMON_RX_STS_BREAK | 
                            UART_VMON_RX_STS_PAR_ERROR |
                            UART_VMON_RX_STS_STOP_ERROR | 
                            UART_VMON_RX_STS_OVERRUN)) != 0u)
            {
                /* ERROR handling. */
                UART_VMON_errorStatus |= readStatus & ( UART_VMON_RX_STS_BREAK | 
                                                            UART_VMON_RX_STS_PAR_ERROR | 
                                                            UART_VMON_RX_STS_STOP_ERROR | 
                                                            UART_VMON_RX_STS_OVERRUN);
                /* `#START UART_VMON_RXISR_ERROR` */

                /* `#END` */
                
            #ifdef UART_VMON_RXISR_ERROR_CALLBACK
                UART_VMON_RXISR_ERROR_Callback();
            #endif /* UART_VMON_RXISR_ERROR_CALLBACK */
            }
            
            if((readStatus & UART_VMON_RX_STS_FIFO_NOTEMPTY) != 0u)
            {
                /* Read data from the RX data register */
                readData = UART_VMON_RXDATA_REG;
            #if (UART_VMON_RXHW_ADDRESS_ENABLED)
                if(UART_VMON_rxAddressMode == (uint8)UART_VMON__B_UART__AM_SW_DETECT_TO_BUFFER)
                {
                    if((readStatus & UART_VMON_RX_STS_MRKSPC) != 0u)
                    {
                        if ((readStatus & UART_VMON_RX_STS_ADDR_MATCH) != 0u)
                        {
                            UART_VMON_rxAddressDetected = 1u;
                        }
                        else
                        {
                            UART_VMON_rxAddressDetected = 0u;
                        }
                    }
                    if(UART_VMON_rxAddressDetected != 0u)
                    {   /* Store only addressed data */
                        UART_VMON_rxBuffer[UART_VMON_rxBufferWrite] = readData;
                        increment_pointer = 1u;
                    }
                }
                else /* Without software addressing */
                {
                    UART_VMON_rxBuffer[UART_VMON_rxBufferWrite] = readData;
                    increment_pointer = 1u;
                }
            #else  /* Without addressing */
                UART_VMON_rxBuffer[UART_VMON_rxBufferWrite] = readData;
                increment_pointer = 1u;
            #endif /* (UART_VMON_RXHW_ADDRESS_ENABLED) */

                /* Do not increment buffer pointer when skip not addressed data */
                if(increment_pointer != 0u)
                {
                    if(UART_VMON_rxBufferLoopDetect != 0u)
                    {   /* Set Software Buffer status Overflow */
                        UART_VMON_rxBufferOverflow = 1u;
                    }
                    /* Set next pointer. */
                    UART_VMON_rxBufferWrite++;

                    /* Check pointer for a loop condition */
                    if(UART_VMON_rxBufferWrite >= UART_VMON_RX_BUFFER_SIZE)
                    {
                        UART_VMON_rxBufferWrite = 0u;
                    }

                    /* Detect pre-overload condition and set flag */
                    if(UART_VMON_rxBufferWrite == UART_VMON_rxBufferRead)
                    {
                        UART_VMON_rxBufferLoopDetect = 1u;
                        /* When Hardware Flow Control selected */
                        #if (UART_VMON_FLOW_CONTROL != 0u)
                            /* Disable RX interrupt mask, it is enabled when user read data from the buffer using APIs */
                            UART_VMON_RXSTATUS_MASK_REG  &= (uint8)~UART_VMON_RX_STS_FIFO_NOTEMPTY;
                            CyIntClearPending(UART_VMON_RX_VECT_NUM);
                            break; /* Break the reading of the FIFO loop, leave the data there for generating RTS signal */
                        #endif /* (UART_VMON_FLOW_CONTROL != 0u) */
                    }
                }
            }
        }while((readStatus & UART_VMON_RX_STS_FIFO_NOTEMPTY) != 0u);

        /* User code required at end of ISR (Optional) */
        /* `#START UART_VMON_RXISR_END` */

        /* `#END` */

    #ifdef UART_VMON_RXISR_EXIT_CALLBACK
        UART_VMON_RXISR_ExitCallback();
    #endif /* UART_VMON_RXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
    }
    
#endif /* (UART_VMON_RX_INTERRUPT_ENABLED && (UART_VMON_RX_ENABLED || UART_VMON_HD_ENABLED)) */


#if (UART_VMON_TX_INTERRUPT_ENABLED && UART_VMON_TX_ENABLED)
    /*******************************************************************************
    * Function Name: UART_VMON_TXISR
    ********************************************************************************
    *
    * Summary:
    * Interrupt Service Routine for the TX portion of the UART
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  UART_VMON_txBuffer - RAM buffer pointer for transmit data from.
    *  UART_VMON_txBufferRead - cyclic index for read and transmit data
    *     from txBuffer, increments after each transmitted byte.
    *  UART_VMON_rxBufferWrite - cyclic index for write to txBuffer,
    *     checked to detect available for transmission bytes.
    *
    *******************************************************************************/
    CY_ISR(UART_VMON_TXISR)
    {
    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef UART_VMON_TXISR_ENTRY_CALLBACK
        UART_VMON_TXISR_EntryCallback();
    #endif /* UART_VMON_TXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START UART_VMON_TXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        while((UART_VMON_txBufferRead != UART_VMON_txBufferWrite) &&
             ((UART_VMON_TXSTATUS_REG & UART_VMON_TX_STS_FIFO_FULL) == 0u))
        {
            /* Check pointer wrap around */
            if(UART_VMON_txBufferRead >= UART_VMON_TX_BUFFER_SIZE)
            {
                UART_VMON_txBufferRead = 0u;
            }

            UART_VMON_TXDATA_REG = UART_VMON_txBuffer[UART_VMON_txBufferRead];

            /* Set next pointer */
            UART_VMON_txBufferRead++;
        }

        /* User code required at end of ISR (Optional) */
        /* `#START UART_VMON_TXISR_END` */

        /* `#END` */

    #ifdef UART_VMON_TXISR_EXIT_CALLBACK
        UART_VMON_TXISR_ExitCallback();
    #endif /* UART_VMON_TXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
   }
#endif /* (UART_VMON_TX_INTERRUPT_ENABLED && UART_VMON_TX_ENABLED) */


/* [] END OF FILE */
