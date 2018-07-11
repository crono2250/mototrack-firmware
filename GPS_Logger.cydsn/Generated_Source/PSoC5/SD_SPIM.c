/*******************************************************************************
* File Name: SD_SPIM.c
* Version 2.50
*
* Description:
*  This file provides all API functionality of the SPI Master component.
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

#if(SD_SPIM_TX_SOFTWARE_BUF_ENABLED)
    volatile uint8 SD_SPIM_txBuffer[SD_SPIM_TX_BUFFER_SIZE];
    volatile uint8 SD_SPIM_txBufferFull;
    volatile uint8 SD_SPIM_txBufferRead;
    volatile uint8 SD_SPIM_txBufferWrite;
#endif /* (SD_SPIM_TX_SOFTWARE_BUF_ENABLED) */

#if(SD_SPIM_RX_SOFTWARE_BUF_ENABLED)
    volatile uint8 SD_SPIM_rxBuffer[SD_SPIM_RX_BUFFER_SIZE];
    volatile uint8 SD_SPIM_rxBufferFull;
    volatile uint8 SD_SPIM_rxBufferRead;
    volatile uint8 SD_SPIM_rxBufferWrite;
#endif /* (SD_SPIM_RX_SOFTWARE_BUF_ENABLED) */

uint8 SD_SPIM_initVar = 0u;

volatile uint8 SD_SPIM_swStatusTx;
volatile uint8 SD_SPIM_swStatusRx;


/*******************************************************************************
* Function Name: SD_SPIM_Init
********************************************************************************
*
* Summary:
*  Inits/Restores default SPIM configuration provided with customizer.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Side Effects:
*  When this function is called it initializes all of the necessary parameters
*  for execution. i.e. setting the initial interrupt mask, configuring the
*  interrupt service routine, configuring the bit-counter parameters and
*  clearing the FIFO and Status Register.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void SD_SPIM_Init(void) 
{
    /* Initialize the Bit counter */
    SD_SPIM_COUNTER_PERIOD_REG = SD_SPIM_BITCTR_INIT;

    /* Init TX ISR  */
    #if(0u != SD_SPIM_INTERNAL_TX_INT_ENABLED)
        CyIntDisable         (SD_SPIM_TX_ISR_NUMBER);
        CyIntSetPriority     (SD_SPIM_TX_ISR_NUMBER,  SD_SPIM_TX_ISR_PRIORITY);
        (void) CyIntSetVector(SD_SPIM_TX_ISR_NUMBER, &SD_SPIM_TX_ISR);
    #endif /* (0u != SD_SPIM_INTERNAL_TX_INT_ENABLED) */

    /* Init RX ISR  */
    #if(0u != SD_SPIM_INTERNAL_RX_INT_ENABLED)
        CyIntDisable         (SD_SPIM_RX_ISR_NUMBER);
        CyIntSetPriority     (SD_SPIM_RX_ISR_NUMBER,  SD_SPIM_RX_ISR_PRIORITY);
        (void) CyIntSetVector(SD_SPIM_RX_ISR_NUMBER, &SD_SPIM_RX_ISR);
    #endif /* (0u != SD_SPIM_INTERNAL_RX_INT_ENABLED) */

    /* Clear any stray data from the RX and TX FIFO */
    SD_SPIM_ClearFIFO();

    #if(SD_SPIM_RX_SOFTWARE_BUF_ENABLED)
        SD_SPIM_rxBufferFull  = 0u;
        SD_SPIM_rxBufferRead  = 0u;
        SD_SPIM_rxBufferWrite = 0u;
    #endif /* (SD_SPIM_RX_SOFTWARE_BUF_ENABLED) */

    #if(SD_SPIM_TX_SOFTWARE_BUF_ENABLED)
        SD_SPIM_txBufferFull  = 0u;
        SD_SPIM_txBufferRead  = 0u;
        SD_SPIM_txBufferWrite = 0u;
    #endif /* (SD_SPIM_TX_SOFTWARE_BUF_ENABLED) */

    (void) SD_SPIM_ReadTxStatus(); /* Clear Tx status and swStatusTx */
    (void) SD_SPIM_ReadRxStatus(); /* Clear Rx status and swStatusRx */

    /* Configure TX and RX interrupt mask */
    SD_SPIM_TX_STATUS_MASK_REG = SD_SPIM_TX_INIT_INTERRUPTS_MASK;
    SD_SPIM_RX_STATUS_MASK_REG = SD_SPIM_RX_INIT_INTERRUPTS_MASK;
}


/*******************************************************************************
* Function Name: SD_SPIM_Enable
********************************************************************************
*
* Summary:
*  Enable SPIM component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void SD_SPIM_Enable(void) 
{
    uint8 enableInterrupts;

    enableInterrupts = CyEnterCriticalSection();
    SD_SPIM_COUNTER_CONTROL_REG |= SD_SPIM_CNTR_ENABLE;
    SD_SPIM_TX_STATUS_ACTL_REG  |= SD_SPIM_INT_ENABLE;
    SD_SPIM_RX_STATUS_ACTL_REG  |= SD_SPIM_INT_ENABLE;
    CyExitCriticalSection(enableInterrupts);

    #if(0u != SD_SPIM_INTERNAL_CLOCK)
        SD_SPIM_IntClock_Enable();
    #endif /* (0u != SD_SPIM_INTERNAL_CLOCK) */

    SD_SPIM_EnableTxInt();
    SD_SPIM_EnableRxInt();
}


/*******************************************************************************
* Function Name: SD_SPIM_Start
********************************************************************************
*
* Summary:
*  Initialize and Enable the SPI Master component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  SD_SPIM_initVar - used to check initial configuration, modified on
*  first function call.
*
* Theory:
*  Enable the clock input to enable operation.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void SD_SPIM_Start(void) 
{
    if(0u == SD_SPIM_initVar)
    {
        SD_SPIM_Init();
        SD_SPIM_initVar = 1u;
    }

    SD_SPIM_Enable();
}


/*******************************************************************************
* Function Name: SD_SPIM_Stop
********************************************************************************
*
* Summary:
*  Disable the SPI Master component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Theory:
*  Disable the clock input to enable operation.
*
*******************************************************************************/
void SD_SPIM_Stop(void) 
{
    uint8 enableInterrupts;

    enableInterrupts = CyEnterCriticalSection();
    SD_SPIM_TX_STATUS_ACTL_REG &= ((uint8) ~SD_SPIM_INT_ENABLE);
    SD_SPIM_RX_STATUS_ACTL_REG &= ((uint8) ~SD_SPIM_INT_ENABLE);
    CyExitCriticalSection(enableInterrupts);

    #if(0u != SD_SPIM_INTERNAL_CLOCK)
        SD_SPIM_IntClock_Disable();
    #endif /* (0u != SD_SPIM_INTERNAL_CLOCK) */

    SD_SPIM_DisableTxInt();
    SD_SPIM_DisableRxInt();
}


/*******************************************************************************
* Function Name: SD_SPIM_EnableTxInt
********************************************************************************
*
* Summary:
*  Enable internal Tx interrupt generation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Theory:
*  Enable the internal Tx interrupt output -or- the interrupt component itself.
*
*******************************************************************************/
void SD_SPIM_EnableTxInt(void) 
{
    #if(0u != SD_SPIM_INTERNAL_TX_INT_ENABLED)
        CyIntEnable(SD_SPIM_TX_ISR_NUMBER);
    #endif /* (0u != SD_SPIM_INTERNAL_TX_INT_ENABLED) */
}


/*******************************************************************************
* Function Name: SD_SPIM_EnableRxInt
********************************************************************************
*
* Summary:
*  Enable internal Rx interrupt generation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Theory:
*  Enable the internal Rx interrupt output -or- the interrupt component itself.
*
*******************************************************************************/
void SD_SPIM_EnableRxInt(void) 
{
    #if(0u != SD_SPIM_INTERNAL_RX_INT_ENABLED)
        CyIntEnable(SD_SPIM_RX_ISR_NUMBER);
    #endif /* (0u != SD_SPIM_INTERNAL_RX_INT_ENABLED) */
}


/*******************************************************************************
* Function Name: SD_SPIM_DisableTxInt
********************************************************************************
*
* Summary:
*  Disable internal Tx interrupt generation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Theory:
*  Disable the internal Tx interrupt output -or- the interrupt component itself.
*
*******************************************************************************/
void SD_SPIM_DisableTxInt(void) 
{
    #if(0u != SD_SPIM_INTERNAL_TX_INT_ENABLED)
        CyIntDisable(SD_SPIM_TX_ISR_NUMBER);
    #endif /* (0u != SD_SPIM_INTERNAL_TX_INT_ENABLED) */
}


/*******************************************************************************
* Function Name: SD_SPIM_DisableRxInt
********************************************************************************
*
* Summary:
*  Disable internal Rx interrupt generation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Theory:
*  Disable the internal Rx interrupt output -or- the interrupt component itself.
*
*******************************************************************************/
void SD_SPIM_DisableRxInt(void) 
{
    #if(0u != SD_SPIM_INTERNAL_RX_INT_ENABLED)
        CyIntDisable(SD_SPIM_RX_ISR_NUMBER);
    #endif /* (0u != SD_SPIM_INTERNAL_RX_INT_ENABLED) */
}


/*******************************************************************************
* Function Name: SD_SPIM_SetTxInterruptMode
********************************************************************************
*
* Summary:
*  Configure which status bits trigger an interrupt event.
*
* Parameters:
*  intSrc: An or'd combination of the desired status bit masks (defined in the
*  header file).
*
* Return:
*  None.
*
* Theory:
*  Enables the output of specific status bits to the interrupt controller.
*
*******************************************************************************/
void SD_SPIM_SetTxInterruptMode(uint8 intSrc) 
{
    SD_SPIM_TX_STATUS_MASK_REG = intSrc;
}


/*******************************************************************************
* Function Name: SD_SPIM_SetRxInterruptMode
********************************************************************************
*
* Summary:
*  Configure which status bits trigger an interrupt event.
*
* Parameters:
*  intSrc: An or'd combination of the desired status bit masks (defined in the
*  header file).
*
* Return:
*  None.
*
* Theory:
*  Enables the output of specific status bits to the interrupt controller.
*
*******************************************************************************/
void SD_SPIM_SetRxInterruptMode(uint8 intSrc) 
{
    SD_SPIM_RX_STATUS_MASK_REG  = intSrc;
}


/*******************************************************************************
* Function Name: SD_SPIM_ReadTxStatus
********************************************************************************
*
* Summary:
*  Read the Tx status register for the component.
*
* Parameters:
*  None.
*
* Return:
*  Contents of the Tx status register.
*
* Global variables:
*  SD_SPIM_swStatusTx - used to store in software status register,
*  modified every function call - resets to zero.
*
* Theory:
*  Allows the user and the API to read the Tx status register for error
*  detection and flow control.
*
* Side Effects:
*  Clear Tx status register of the component.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 SD_SPIM_ReadTxStatus(void) 
{
    uint8 tmpStatus;

    #if(SD_SPIM_TX_SOFTWARE_BUF_ENABLED)
        /* Disable TX interrupt to protect global veriables */
        SD_SPIM_DisableTxInt();

        tmpStatus = SD_SPIM_GET_STATUS_TX(SD_SPIM_swStatusTx);
        SD_SPIM_swStatusTx = 0u;

        SD_SPIM_EnableTxInt();

    #else

        tmpStatus = SD_SPIM_TX_STATUS_REG;

    #endif /* (SD_SPIM_TX_SOFTWARE_BUF_ENABLED) */

    return(tmpStatus);
}


/*******************************************************************************
* Function Name: SD_SPIM_ReadRxStatus
********************************************************************************
*
* Summary:
*  Read the Rx status register for the component.
*
* Parameters:
*  None.
*
* Return:
*  Contents of the Rx status register.
*
* Global variables:
*  SD_SPIM_swStatusRx - used to store in software Rx status register,
*  modified every function call - resets to zero.
*
* Theory:
*  Allows the user and the API to read the Rx status register for error
*  detection and flow control.
*
* Side Effects:
*  Clear Rx status register of the component.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 SD_SPIM_ReadRxStatus(void) 
{
    uint8 tmpStatus;

    #if(SD_SPIM_RX_SOFTWARE_BUF_ENABLED)
        /* Disable RX interrupt to protect global veriables */
        SD_SPIM_DisableRxInt();

        tmpStatus = SD_SPIM_GET_STATUS_RX(SD_SPIM_swStatusRx);
        SD_SPIM_swStatusRx = 0u;

        SD_SPIM_EnableRxInt();

    #else

        tmpStatus = SD_SPIM_RX_STATUS_REG;

    #endif /* (SD_SPIM_RX_SOFTWARE_BUF_ENABLED) */

    return(tmpStatus);
}


/*******************************************************************************
* Function Name: SD_SPIM_WriteTxData
********************************************************************************
*
* Summary:
*  Write a byte of data to be sent across the SPI.
*
* Parameters:
*  txDataByte: The data value to send across the SPI.
*
* Return:
*  None.
*
* Global variables:
*  SD_SPIM_txBufferWrite - used for the account of the bytes which
*  have been written down in the TX software buffer, modified every function
*  call if TX Software Buffer is used.
*  SD_SPIM_txBufferRead - used for the account of the bytes which
*  have been read from the TX software buffer.
*  SD_SPIM_txBuffer[SD_SPIM_TX_BUFFER_SIZE] - used to store
*  data to sending, modified every function call if TX Software Buffer is used.
*
* Theory:
*  Allows the user to transmit any byte of data in a single transfer.
*
* Side Effects:
*  If this function is called again before the previous byte is finished then
*  the next byte will be appended to the transfer with no time between
*  the byte transfers. Clear Tx status register of the component.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void SD_SPIM_WriteTxData(uint8 txData) 
{
    #if(SD_SPIM_TX_SOFTWARE_BUF_ENABLED)

        uint8 tempStatus;
        uint8 tmpTxBufferRead;

        /* Block if TX buffer is FULL: don't overwrite */
        do
        {
            tmpTxBufferRead = SD_SPIM_txBufferRead;
            if(0u == tmpTxBufferRead)
            {
                tmpTxBufferRead = (SD_SPIM_TX_BUFFER_SIZE - 1u);
            }
            else
            {
                tmpTxBufferRead--;
            }

        }while(tmpTxBufferRead == SD_SPIM_txBufferWrite);

        /* Disable TX interrupt to protect global veriables */
        SD_SPIM_DisableTxInt();

        tempStatus = SD_SPIM_GET_STATUS_TX(SD_SPIM_swStatusTx);
        SD_SPIM_swStatusTx = tempStatus;


        if((SD_SPIM_txBufferRead == SD_SPIM_txBufferWrite) &&
           (0u != (SD_SPIM_swStatusTx & SD_SPIM_STS_TX_FIFO_NOT_FULL)))
        {
            /* Put data element into the TX FIFO */
            CY_SET_REG8(SD_SPIM_TXDATA_PTR, txData);
        }
        else
        {
            /* Add to the TX software buffer */
            SD_SPIM_txBufferWrite++;
            if(SD_SPIM_txBufferWrite >= SD_SPIM_TX_BUFFER_SIZE)
            {
                SD_SPIM_txBufferWrite = 0u;
            }

            if(SD_SPIM_txBufferWrite == SD_SPIM_txBufferRead)
            {
                SD_SPIM_txBufferRead++;
                if(SD_SPIM_txBufferRead >= SD_SPIM_TX_BUFFER_SIZE)
                {
                    SD_SPIM_txBufferRead = 0u;
                }
                SD_SPIM_txBufferFull = 1u;
            }

            SD_SPIM_txBuffer[SD_SPIM_txBufferWrite] = txData;

            SD_SPIM_TX_STATUS_MASK_REG |= SD_SPIM_STS_TX_FIFO_NOT_FULL;
        }

        SD_SPIM_EnableTxInt();

    #else
        /* Wait until TX FIFO has a place */
        while(0u == (SD_SPIM_TX_STATUS_REG & SD_SPIM_STS_TX_FIFO_NOT_FULL))
        {
        }

        /* Put data element into the TX FIFO */
        CY_SET_REG8(SD_SPIM_TXDATA_PTR, txData);

    #endif /* (SD_SPIM_TX_SOFTWARE_BUF_ENABLED) */
}


/*******************************************************************************
* Function Name: SD_SPIM_ReadRxData
********************************************************************************
*
* Summary:
*  Read the next byte of data received across the SPI.
*
* Parameters:
*  None.
*
* Return:
*  The next byte of data read from the FIFO.
*
* Global variables:
*  SD_SPIM_rxBufferWrite - used for the account of the bytes which
*  have been written down in the RX software buffer.
*  SD_SPIM_rxBufferRead - used for the account of the bytes which
*  have been read from the RX software buffer, modified every function
*  call if RX Software Buffer is used.
*  SD_SPIM_rxBuffer[SD_SPIM_RX_BUFFER_SIZE] - used to store
*  received data.
*
* Theory:
*  Allows the user to read a byte of data received.
*
* Side Effects:
*  Will return invalid data if the FIFO is empty. The user should Call
*  GetRxBufferSize() and if it returns a non-zero value then it is safe to call
*  ReadByte() function.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 SD_SPIM_ReadRxData(void) 
{
    uint8 rxData;

    #if(SD_SPIM_RX_SOFTWARE_BUF_ENABLED)

        /* Disable RX interrupt to protect global veriables */
        SD_SPIM_DisableRxInt();

        if(SD_SPIM_rxBufferRead != SD_SPIM_rxBufferWrite)
        {
            if(0u == SD_SPIM_rxBufferFull)
            {
                SD_SPIM_rxBufferRead++;
                if(SD_SPIM_rxBufferRead >= SD_SPIM_RX_BUFFER_SIZE)
                {
                    SD_SPIM_rxBufferRead = 0u;
                }
            }
            else
            {
                SD_SPIM_rxBufferFull = 0u;
            }
        }

        rxData = SD_SPIM_rxBuffer[SD_SPIM_rxBufferRead];

        SD_SPIM_EnableRxInt();

    #else

        rxData = CY_GET_REG8(SD_SPIM_RXDATA_PTR);

    #endif /* (SD_SPIM_RX_SOFTWARE_BUF_ENABLED) */

    return(rxData);
}


/*******************************************************************************
* Function Name: SD_SPIM_GetRxBufferSize
********************************************************************************
*
* Summary:
*  Returns the number of bytes/words of data currently held in the RX buffer.
*  If RX Software Buffer not used then function return 0 if FIFO empty or 1 if
*  FIFO not empty. In another case function return size of RX Software Buffer.
*
* Parameters:
*  None.
*
* Return:
*  Integer count of the number of bytes/words in the RX buffer.
*
* Global variables:
*  SD_SPIM_rxBufferWrite - used for the account of the bytes which
*  have been written down in the RX software buffer.
*  SD_SPIM_rxBufferRead - used for the account of the bytes which
*  have been read from the RX software buffer.
*
* Side Effects:
*  Clear status register of the component.
*
*******************************************************************************/
uint8 SD_SPIM_GetRxBufferSize(void) 
{
    uint8 size;

    #if(SD_SPIM_RX_SOFTWARE_BUF_ENABLED)

        /* Disable RX interrupt to protect global veriables */
        SD_SPIM_DisableRxInt();

        if(SD_SPIM_rxBufferRead == SD_SPIM_rxBufferWrite)
        {
            size = 0u;
        }
        else if(SD_SPIM_rxBufferRead < SD_SPIM_rxBufferWrite)
        {
            size = (SD_SPIM_rxBufferWrite - SD_SPIM_rxBufferRead);
        }
        else
        {
            size = (SD_SPIM_RX_BUFFER_SIZE - SD_SPIM_rxBufferRead) + SD_SPIM_rxBufferWrite;
        }

        SD_SPIM_EnableRxInt();

    #else

        /* We can only know if there is data in the RX FIFO */
        size = (0u != (SD_SPIM_RX_STATUS_REG & SD_SPIM_STS_RX_FIFO_NOT_EMPTY)) ? 1u : 0u;

    #endif /* (SD_SPIM_TX_SOFTWARE_BUF_ENABLED) */

    return(size);
}


/*******************************************************************************
* Function Name: SD_SPIM_GetTxBufferSize
********************************************************************************
*
* Summary:
*  Returns the number of bytes/words of data currently held in the TX buffer.
*  If TX Software Buffer not used then function return 0 - if FIFO empty, 1 - if
*  FIFO not full, 4 - if FIFO full. In another case function return size of TX
*  Software Buffer.
*
* Parameters:
*  None.
*
* Return:
*  Integer count of the number of bytes/words in the TX buffer.
*
* Global variables:
*  SD_SPIM_txBufferWrite - used for the account of the bytes which
*  have been written down in the TX software buffer.
*  SD_SPIM_txBufferRead - used for the account of the bytes which
*  have been read from the TX software buffer.
*
* Side Effects:
*  Clear status register of the component.
*
*******************************************************************************/
uint8  SD_SPIM_GetTxBufferSize(void) 
{
    uint8 size;

    #if(SD_SPIM_TX_SOFTWARE_BUF_ENABLED)
        /* Disable TX interrupt to protect global veriables */
        SD_SPIM_DisableTxInt();

        if(SD_SPIM_txBufferRead == SD_SPIM_txBufferWrite)
        {
            size = 0u;
        }
        else if(SD_SPIM_txBufferRead < SD_SPIM_txBufferWrite)
        {
            size = (SD_SPIM_txBufferWrite - SD_SPIM_txBufferRead);
        }
        else
        {
            size = (SD_SPIM_TX_BUFFER_SIZE - SD_SPIM_txBufferRead) + SD_SPIM_txBufferWrite;
        }

        SD_SPIM_EnableTxInt();

    #else

        size = SD_SPIM_TX_STATUS_REG;

        if(0u != (size & SD_SPIM_STS_TX_FIFO_EMPTY))
        {
            size = 0u;
        }
        else if(0u != (size & SD_SPIM_STS_TX_FIFO_NOT_FULL))
        {
            size = 1u;
        }
        else
        {
            size = SD_SPIM_FIFO_SIZE;
        }

    #endif /* (SD_SPIM_TX_SOFTWARE_BUF_ENABLED) */

    return(size);
}


/*******************************************************************************
* Function Name: SD_SPIM_ClearRxBuffer
********************************************************************************
*
* Summary:
*  Clear the RX RAM buffer by setting the read and write pointers both to zero.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  SD_SPIM_rxBufferWrite - used for the account of the bytes which
*  have been written down in the RX software buffer, modified every function
*  call - resets to zero.
*  SD_SPIM_rxBufferRead - used for the account of the bytes which
*  have been read from the RX software buffer, modified every function call -
*  resets to zero.
*
* Theory:
*  Setting the pointers to zero makes the system believe there is no data to
*  read and writing will resume at address 0 overwriting any data that may have
*  remained in the RAM.
*
* Side Effects:
*  Any received data not read from the RAM buffer will be lost when overwritten.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void SD_SPIM_ClearRxBuffer(void) 
{
    /* Clear Hardware RX FIFO */
    while(0u !=(SD_SPIM_RX_STATUS_REG & SD_SPIM_STS_RX_FIFO_NOT_EMPTY))
    {
        (void) CY_GET_REG8(SD_SPIM_RXDATA_PTR);
    }

    #if(SD_SPIM_RX_SOFTWARE_BUF_ENABLED)
        /* Disable RX interrupt to protect global veriables */
        SD_SPIM_DisableRxInt();

        SD_SPIM_rxBufferFull  = 0u;
        SD_SPIM_rxBufferRead  = 0u;
        SD_SPIM_rxBufferWrite = 0u;

        SD_SPIM_EnableRxInt();
    #endif /* (SD_SPIM_RX_SOFTWARE_BUF_ENABLED) */
}


/*******************************************************************************
* Function Name: SD_SPIM_ClearTxBuffer
********************************************************************************
*
* Summary:
*  Clear the TX RAM buffer by setting the read and write pointers both to zero.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  SD_SPIM_txBufferWrite - used for the account of the bytes which
*  have been written down in the TX software buffer, modified every function
*  call - resets to zero.
*  SD_SPIM_txBufferRead - used for the account of the bytes which
*  have been read from the TX software buffer, modified every function call -
*  resets to zero.
*
* Theory:
*  Setting the pointers to zero makes the system believe there is no data to
*  read and writing will resume at address 0 overwriting any data that may have
*  remained in the RAM.
*
* Side Effects:
*  Any data not yet transmitted from the RAM buffer will be lost when
*  overwritten.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void SD_SPIM_ClearTxBuffer(void) 
{
    uint8 enableInterrupts;

    enableInterrupts = CyEnterCriticalSection();
    /* Clear TX FIFO */
    SD_SPIM_AUX_CONTROL_DP0_REG |= ((uint8)  SD_SPIM_TX_FIFO_CLR);
    SD_SPIM_AUX_CONTROL_DP0_REG &= ((uint8) ~SD_SPIM_TX_FIFO_CLR);

    #if(SD_SPIM_USE_SECOND_DATAPATH)
        /* Clear TX FIFO for 2nd Datapath */
        SD_SPIM_AUX_CONTROL_DP1_REG |= ((uint8)  SD_SPIM_TX_FIFO_CLR);
        SD_SPIM_AUX_CONTROL_DP1_REG &= ((uint8) ~SD_SPIM_TX_FIFO_CLR);
    #endif /* (SD_SPIM_USE_SECOND_DATAPATH) */
    CyExitCriticalSection(enableInterrupts);

    #if(SD_SPIM_TX_SOFTWARE_BUF_ENABLED)
        /* Disable TX interrupt to protect global veriables */
        SD_SPIM_DisableTxInt();

        SD_SPIM_txBufferFull  = 0u;
        SD_SPIM_txBufferRead  = 0u;
        SD_SPIM_txBufferWrite = 0u;

        /* Buffer is EMPTY: disable TX FIFO NOT FULL interrupt */
        SD_SPIM_TX_STATUS_MASK_REG &= ((uint8) ~SD_SPIM_STS_TX_FIFO_NOT_FULL);

        SD_SPIM_EnableTxInt();
    #endif /* (SD_SPIM_TX_SOFTWARE_BUF_ENABLED) */
}


#if(0u != SD_SPIM_BIDIRECTIONAL_MODE)
    /*******************************************************************************
    * Function Name: SD_SPIM_TxEnable
    ********************************************************************************
    *
    * Summary:
    *  If the SPI master is configured to use a single bi-directional pin then this
    *  will set the bi-directional pin to transmit.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void SD_SPIM_TxEnable(void) 
    {
        SD_SPIM_CONTROL_REG |= SD_SPIM_CTRL_TX_SIGNAL_EN;
    }


    /*******************************************************************************
    * Function Name: SD_SPIM_TxDisable
    ********************************************************************************
    *
    * Summary:
    *  If the SPI master is configured to use a single bi-directional pin then this
    *  will set the bi-directional pin to receive.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void SD_SPIM_TxDisable(void) 
    {
        SD_SPIM_CONTROL_REG &= ((uint8) ~SD_SPIM_CTRL_TX_SIGNAL_EN);
    }

#endif /* (0u != SD_SPIM_BIDIRECTIONAL_MODE) */


/*******************************************************************************
* Function Name: SD_SPIM_PutArray
********************************************************************************
*
* Summary:
*  Write available data from ROM/RAM to the TX buffer while space is available
*  in the TX buffer. Keep trying until all data is passed to the TX buffer.
*
* Parameters:
*  *buffer: Pointer to the location in RAM containing the data to send
*  byteCount: The number of bytes to move to the transmit buffer.
*
* Return:
*  None.
*
* Side Effects:
*  Will stay in this routine until all data has been sent.  May get locked in
*  this loop if data is not being initiated by the master if there is not
*  enough room in the TX FIFO.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void SD_SPIM_PutArray(const uint8 buffer[], uint8 byteCount)
                                                                          
{
    uint8 bufIndex;

    bufIndex = 0u;

    while(byteCount > 0u)
    {
        SD_SPIM_WriteTxData(buffer[bufIndex]);
        bufIndex++;
        byteCount--;
    }
}


/*******************************************************************************
* Function Name: SD_SPIM_ClearFIFO
********************************************************************************
*
* Summary:
*  Clear the RX and TX FIFO's of all data for a fresh start.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Side Effects:
*  Clear status register of the component.
*
*******************************************************************************/
void SD_SPIM_ClearFIFO(void) 
{
    uint8 enableInterrupts;

    /* Clear Hardware RX FIFO */
    while(0u !=(SD_SPIM_RX_STATUS_REG & SD_SPIM_STS_RX_FIFO_NOT_EMPTY))
    {
        (void) CY_GET_REG8(SD_SPIM_RXDATA_PTR);
    }

    enableInterrupts = CyEnterCriticalSection();
    /* Clear TX FIFO */
    SD_SPIM_AUX_CONTROL_DP0_REG |= ((uint8)  SD_SPIM_TX_FIFO_CLR);
    SD_SPIM_AUX_CONTROL_DP0_REG &= ((uint8) ~SD_SPIM_TX_FIFO_CLR);

    #if(SD_SPIM_USE_SECOND_DATAPATH)
        /* Clear TX FIFO for 2nd Datapath */
        SD_SPIM_AUX_CONTROL_DP1_REG |= ((uint8)  SD_SPIM_TX_FIFO_CLR);
        SD_SPIM_AUX_CONTROL_DP1_REG &= ((uint8) ~SD_SPIM_TX_FIFO_CLR);
    #endif /* (SD_SPIM_USE_SECOND_DATAPATH) */
    CyExitCriticalSection(enableInterrupts);
}


/* Following functions are for version Compatibility, they are obsolete.
*  Please do not use it in new projects.
*/


/*******************************************************************************
* Function Name: SD_SPIM_EnableInt
********************************************************************************
*
* Summary:
*  Enable internal interrupt generation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Theory:
*  Enable the internal interrupt output -or- the interrupt component itself.
*
*******************************************************************************/
void SD_SPIM_EnableInt(void) 
{
    SD_SPIM_EnableRxInt();
    SD_SPIM_EnableTxInt();
}


/*******************************************************************************
* Function Name: SD_SPIM_DisableInt
********************************************************************************
*
* Summary:
*  Disable internal interrupt generation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Theory:
*  Disable the internal interrupt output -or- the interrupt component itself.
*
*******************************************************************************/
void SD_SPIM_DisableInt(void) 
{
    SD_SPIM_DisableTxInt();
    SD_SPIM_DisableRxInt();
}


/*******************************************************************************
* Function Name: SD_SPIM_SetInterruptMode
********************************************************************************
*
* Summary:
*  Configure which status bits trigger an interrupt event.
*
* Parameters:
*  intSrc: An or'd combination of the desired status bit masks (defined in the
*  header file).
*
* Return:
*  None.
*
* Theory:
*  Enables the output of specific status bits to the interrupt controller.
*
*******************************************************************************/
void SD_SPIM_SetInterruptMode(uint8 intSrc) 
{
    SD_SPIM_TX_STATUS_MASK_REG  = (intSrc & ((uint8) ~SD_SPIM_STS_SPI_IDLE));
    SD_SPIM_RX_STATUS_MASK_REG  =  intSrc;
}


/*******************************************************************************
* Function Name: SD_SPIM_ReadStatus
********************************************************************************
*
* Summary:
*  Read the status register for the component.
*
* Parameters:
*  None.
*
* Return:
*  Contents of the status register.
*
* Global variables:
*  SD_SPIM_swStatus - used to store in software status register,
*  modified every function call - resets to zero.
*
* Theory:
*  Allows the user and the API to read the status register for error detection
*  and flow control.
*
* Side Effects:
*  Clear status register of the component.
*
* Reentrant:
*  No.
*
*******************************************************************************/
uint8 SD_SPIM_ReadStatus(void) 
{
    uint8 tmpStatus;

    #if(SD_SPIM_TX_SOFTWARE_BUF_ENABLED || SD_SPIM_RX_SOFTWARE_BUF_ENABLED)

        SD_SPIM_DisableInt();

        tmpStatus  = SD_SPIM_GET_STATUS_RX(SD_SPIM_swStatusRx);
        tmpStatus |= SD_SPIM_GET_STATUS_TX(SD_SPIM_swStatusTx);
        tmpStatus &= ((uint8) ~SD_SPIM_STS_SPI_IDLE);

        SD_SPIM_swStatusTx = 0u;
        SD_SPIM_swStatusRx = 0u;

        SD_SPIM_EnableInt();

    #else

        tmpStatus  = SD_SPIM_RX_STATUS_REG;
        tmpStatus |= SD_SPIM_TX_STATUS_REG;
        tmpStatus &= ((uint8) ~SD_SPIM_STS_SPI_IDLE);

    #endif /* (SD_SPIM_TX_SOFTWARE_BUF_ENABLED || SD_SPIM_RX_SOFTWARE_BUF_ENABLED) */

    return(tmpStatus);
}


/* [] END OF FILE */
