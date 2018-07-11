/*******************************************************************************
* File Name: SD_SPIM_INT.c
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

#include "SD_SPIM_PVT.h"
#include "cyapicallbacks.h"

/* User code required at start of ISR */
/* `#START SD_SPIM_ISR_START_DEF` */

/* `#END` */


/*******************************************************************************
* Function Name: SD_SPIM_TX_ISR
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
*  SD_SPIM_txBufferWrite - used for the account of the bytes which
*  have been written down in the TX software buffer.
*  SD_SPIM_txBufferRead - used for the account of the bytes which
*  have been read from the TX software buffer, modified when exist data to
*  sending and FIFO Not Full.
*  SD_SPIM_txBuffer[SD_SPIM_TX_BUFFER_SIZE] - used to store
*  data to sending.
*  All described above Global variables are used when Software Buffer is used.
*
*******************************************************************************/
CY_ISR(SD_SPIM_TX_ISR)
{
    #if(SD_SPIM_TX_SOFTWARE_BUF_ENABLED)
        uint8 tmpStatus;
    #endif /* (SD_SPIM_TX_SOFTWARE_BUF_ENABLED) */

    #ifdef SD_SPIM_TX_ISR_ENTRY_CALLBACK
        SD_SPIM_TX_ISR_EntryCallback();
    #endif /* SD_SPIM_TX_ISR_ENTRY_CALLBACK */

    /* User code required at start of ISR */
    /* `#START SD_SPIM_TX_ISR_START` */

    /* `#END` */
    
    #if(SD_SPIM_TX_SOFTWARE_BUF_ENABLED)
        /* Check if TX data buffer is not empty and there is space in TX FIFO */
        while(SD_SPIM_txBufferRead != SD_SPIM_txBufferWrite)
        {
            tmpStatus = SD_SPIM_GET_STATUS_TX(SD_SPIM_swStatusTx);
            SD_SPIM_swStatusTx = tmpStatus;

            if(0u != (SD_SPIM_swStatusTx & SD_SPIM_STS_TX_FIFO_NOT_FULL))
            {
                if(0u == SD_SPIM_txBufferFull)
                {
                   SD_SPIM_txBufferRead++;

                    if(SD_SPIM_txBufferRead >= SD_SPIM_TX_BUFFER_SIZE)
                    {
                        SD_SPIM_txBufferRead = 0u;
                    }
                }
                else
                {
                    SD_SPIM_txBufferFull = 0u;
                }

                /* Put data element into the TX FIFO */
                CY_SET_REG8(SD_SPIM_TXDATA_PTR, 
                                             SD_SPIM_txBuffer[SD_SPIM_txBufferRead]);
            }
            else
            {
                break;
            }
        }

        if(SD_SPIM_txBufferRead == SD_SPIM_txBufferWrite)
        {
            /* TX Buffer is EMPTY: disable interrupt on TX NOT FULL */
            SD_SPIM_TX_STATUS_MASK_REG &= ((uint8) ~SD_SPIM_STS_TX_FIFO_NOT_FULL);
        }

    #endif /* (SD_SPIM_TX_SOFTWARE_BUF_ENABLED) */

    /* User code required at end of ISR (Optional) */
    /* `#START SD_SPIM_TX_ISR_END` */

    /* `#END` */
    
    #ifdef SD_SPIM_TX_ISR_EXIT_CALLBACK
        SD_SPIM_TX_ISR_ExitCallback();
    #endif /* SD_SPIM_TX_ISR_EXIT_CALLBACK */
}


/*******************************************************************************
* Function Name: SD_SPIM_RX_ISR
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
*  SD_SPIM_rxBufferWrite - used for the account of the bytes which
*  have been written down in the RX software buffer modified when FIFO contains
*  new data.
*  SD_SPIM_rxBufferRead - used for the account of the bytes which
*  have been read from the RX software buffer, modified when overflow occurred.
*  SD_SPIM_rxBuffer[SD_SPIM_RX_BUFFER_SIZE] - used to store
*  received data, modified when FIFO contains new data.
*  All described above Global variables are used when Software Buffer is used.
*
*******************************************************************************/
CY_ISR(SD_SPIM_RX_ISR)
{
    #if(SD_SPIM_RX_SOFTWARE_BUF_ENABLED)
        uint8 tmpStatus;
        uint8 rxData;
    #endif /* (SD_SPIM_RX_SOFTWARE_BUF_ENABLED) */

    #ifdef SD_SPIM_RX_ISR_ENTRY_CALLBACK
        SD_SPIM_RX_ISR_EntryCallback();
    #endif /* SD_SPIM_RX_ISR_ENTRY_CALLBACK */

    /* User code required at start of ISR */
    /* `#START SD_SPIM_RX_ISR_START` */

    /* `#END` */
    
    #if(SD_SPIM_RX_SOFTWARE_BUF_ENABLED)

        tmpStatus = SD_SPIM_GET_STATUS_RX(SD_SPIM_swStatusRx);
        SD_SPIM_swStatusRx = tmpStatus;

        /* Check if RX data FIFO has some data to be moved into the RX Buffer */
        while(0u != (SD_SPIM_swStatusRx & SD_SPIM_STS_RX_FIFO_NOT_EMPTY))
        {
            rxData = CY_GET_REG8(SD_SPIM_RXDATA_PTR);

            /* Set next pointer. */
            SD_SPIM_rxBufferWrite++;
            if(SD_SPIM_rxBufferWrite >= SD_SPIM_RX_BUFFER_SIZE)
            {
                SD_SPIM_rxBufferWrite = 0u;
            }

            if(SD_SPIM_rxBufferWrite == SD_SPIM_rxBufferRead)
            {
                SD_SPIM_rxBufferRead++;
                if(SD_SPIM_rxBufferRead >= SD_SPIM_RX_BUFFER_SIZE)
                {
                    SD_SPIM_rxBufferRead = 0u;
                }

                SD_SPIM_rxBufferFull = 1u;
            }

            /* Move data from the FIFO to the Buffer */
            SD_SPIM_rxBuffer[SD_SPIM_rxBufferWrite] = rxData;

            tmpStatus = SD_SPIM_GET_STATUS_RX(SD_SPIM_swStatusRx);
            SD_SPIM_swStatusRx = tmpStatus;
        }

    #endif /* (SD_SPIM_RX_SOFTWARE_BUF_ENABLED) */

    /* User code required at end of ISR (Optional) */
    /* `#START SD_SPIM_RX_ISR_END` */

    /* `#END` */
    
    #ifdef SD_SPIM_RX_ISR_EXIT_CALLBACK
        SD_SPIM_RX_ISR_ExitCallback();
    #endif /* SD_SPIM_RX_ISR_EXIT_CALLBACK */
}

/* [] END OF FILE */
