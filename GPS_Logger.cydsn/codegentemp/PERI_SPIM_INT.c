/*******************************************************************************
* File Name: PERI_SPIM_INT.c
* Version 2.50
*
* Description:
*  This file provides all Interrupt Service Routine (ISR) for the SPI Master
*  component.
*
* Note:
*  None.
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "PERI_SPIM_PVT.h"
#include "cyapicallbacks.h"

/* User code required at start of ISR */
/* `#START PERI_SPIM_ISR_START_DEF` */

/* `#END` */


/*******************************************************************************
* Function Name: PERI_SPIM_TX_ISR
********************************************************************************
*
* Summary:
*  Interrupt Service Routine for TX portion of the SPI Master.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  PERI_SPIM_txBufferWrite - used for the account of the bytes which
*  have been written down in the TX software buffer.
*  PERI_SPIM_txBufferRead - used for the account of the bytes which
*  have been read from the TX software buffer, modified when exist data to
*  sending and FIFO Not Full.
*  PERI_SPIM_txBuffer[PERI_SPIM_TX_BUFFER_SIZE] - used to store
*  data to sending.
*  All described above Global variables are used when Software Buffer is used.
*
*******************************************************************************/
CY_ISR(PERI_SPIM_TX_ISR)
{
    #if(PERI_SPIM_TX_SOFTWARE_BUF_ENABLED)
        uint8 tmpStatus;
    #endif /* (PERI_SPIM_TX_SOFTWARE_BUF_ENABLED) */

    #ifdef PERI_SPIM_TX_ISR_ENTRY_CALLBACK
        PERI_SPIM_TX_ISR_EntryCallback();
    #endif /* PERI_SPIM_TX_ISR_ENTRY_CALLBACK */

    /* User code required at start of ISR */
    /* `#START PERI_SPIM_TX_ISR_START` */

    /* `#END` */
    
    #if(PERI_SPIM_TX_SOFTWARE_BUF_ENABLED)
        /* Check if TX data buffer is not empty and there is space in TX FIFO */
        while(PERI_SPIM_txBufferRead != PERI_SPIM_txBufferWrite)
        {
            tmpStatus = PERI_SPIM_GET_STATUS_TX(PERI_SPIM_swStatusTx);
            PERI_SPIM_swStatusTx = tmpStatus;

            if(0u != (PERI_SPIM_swStatusTx & PERI_SPIM_STS_TX_FIFO_NOT_FULL))
            {
                if(0u == PERI_SPIM_txBufferFull)
                {
                   PERI_SPIM_txBufferRead++;

                    if(PERI_SPIM_txBufferRead >= PERI_SPIM_TX_BUFFER_SIZE)
                    {
                        PERI_SPIM_txBufferRead = 0u;
                    }
                }
                else
                {
                    PERI_SPIM_txBufferFull = 0u;
                }

                /* Put data element into the TX FIFO */
                CY_SET_REG8(PERI_SPIM_TXDATA_PTR, 
                                             PERI_SPIM_txBuffer[PERI_SPIM_txBufferRead]);
            }
            else
            {
                break;
            }
        }

        if(PERI_SPIM_txBufferRead == PERI_SPIM_txBufferWrite)
        {
            /* TX Buffer is EMPTY: disable interrupt on TX NOT FULL */
            PERI_SPIM_TX_STATUS_MASK_REG &= ((uint8) ~PERI_SPIM_STS_TX_FIFO_NOT_FULL);
        }

    #endif /* (PERI_SPIM_TX_SOFTWARE_BUF_ENABLED) */

    /* User code required at end of ISR (Optional) */
    /* `#START PERI_SPIM_TX_ISR_END` */

    /* `#END` */
    
    #ifdef PERI_SPIM_TX_ISR_EXIT_CALLBACK
        PERI_SPIM_TX_ISR_ExitCallback();
    #endif /* PERI_SPIM_TX_ISR_EXIT_CALLBACK */
}


/*******************************************************************************
* Function Name: PERI_SPIM_RX_ISR
********************************************************************************
*
* Summary:
*  Interrupt Service Routine for RX portion of the SPI Master.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  PERI_SPIM_rxBufferWrite - used for the account of the bytes which
*  have been written down in the RX software buffer modified when FIFO contains
*  new data.
*  PERI_SPIM_rxBufferRead - used for the account of the bytes which
*  have been read from the RX software buffer, modified when overflow occurred.
*  PERI_SPIM_rxBuffer[PERI_SPIM_RX_BUFFER_SIZE] - used to store
*  received data, modified when FIFO contains new data.
*  All described above Global variables are used when Software Buffer is used.
*
*******************************************************************************/
CY_ISR(PERI_SPIM_RX_ISR)
{
    #if(PERI_SPIM_RX_SOFTWARE_BUF_ENABLED)
        uint8 tmpStatus;
        uint8 rxData;
    #endif /* (PERI_SPIM_RX_SOFTWARE_BUF_ENABLED) */

    #ifdef PERI_SPIM_RX_ISR_ENTRY_CALLBACK
        PERI_SPIM_RX_ISR_EntryCallback();
    #endif /* PERI_SPIM_RX_ISR_ENTRY_CALLBACK */

    /* User code required at start of ISR */
    /* `#START PERI_SPIM_RX_ISR_START` */

    /* `#END` */
    
    #if(PERI_SPIM_RX_SOFTWARE_BUF_ENABLED)

        tmpStatus = PERI_SPIM_GET_STATUS_RX(PERI_SPIM_swStatusRx);
        PERI_SPIM_swStatusRx = tmpStatus;

        /* Check if RX data FIFO has some data to be moved into the RX Buffer */
        while(0u != (PERI_SPIM_swStatusRx & PERI_SPIM_STS_RX_FIFO_NOT_EMPTY))
        {
            rxData = CY_GET_REG8(PERI_SPIM_RXDATA_PTR);

            /* Set next pointer. */
            PERI_SPIM_rxBufferWrite++;
            if(PERI_SPIM_rxBufferWrite >= PERI_SPIM_RX_BUFFER_SIZE)
            {
                PERI_SPIM_rxBufferWrite = 0u;
            }

            if(PERI_SPIM_rxBufferWrite == PERI_SPIM_rxBufferRead)
            {
                PERI_SPIM_rxBufferRead++;
                if(PERI_SPIM_rxBufferRead >= PERI_SPIM_RX_BUFFER_SIZE)
                {
                    PERI_SPIM_rxBufferRead = 0u;
                }

                PERI_SPIM_rxBufferFull = 1u;
            }

            /* Move data from the FIFO to the Buffer */
            PERI_SPIM_rxBuffer[PERI_SPIM_rxBufferWrite] = rxData;

            tmpStatus = PERI_SPIM_GET_STATUS_RX(PERI_SPIM_swStatusRx);
            PERI_SPIM_swStatusRx = tmpStatus;
        }

    #endif /* (PERI_SPIM_RX_SOFTWARE_BUF_ENABLED) */

    /* User code required at end of ISR (Optional) */
    /* `#START PERI_SPIM_RX_ISR_END` */

    /* `#END` */
    
    #ifdef PERI_SPIM_RX_ISR_EXIT_CALLBACK
        PERI_SPIM_RX_ISR_ExitCallback();
    #endif /* PERI_SPIM_RX_ISR_EXIT_CALLBACK */
}

/* [] END OF FILE */
