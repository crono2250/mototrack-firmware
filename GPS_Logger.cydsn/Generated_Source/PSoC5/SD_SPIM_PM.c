/*******************************************************************************
* File Name: SD_SPIM_PM.c
* Version 2.50
*
* Description:
*  This file contains the setup, control and status commands to support
*  component operations in low power mode.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "SD_SPIM_PVT.h"

static SD_SPIM_BACKUP_STRUCT SD_SPIM_backup =
{
    SD_SPIM_DISABLED,
    SD_SPIM_BITCTR_INIT,
};


/*******************************************************************************
* Function Name: SD_SPIM_SaveConfig
********************************************************************************
*
* Summary:
*  Empty function. Included for consistency with other components.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void SD_SPIM_SaveConfig(void) 
{

}


/*******************************************************************************
* Function Name: SD_SPIM_RestoreConfig
********************************************************************************
*
* Summary:
*  Empty function. Included for consistency with other components.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void SD_SPIM_RestoreConfig(void) 
{

}


/*******************************************************************************
* Function Name: SD_SPIM_Sleep
********************************************************************************
*
* Summary:
*  Prepare SPIM Component goes to sleep.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  SD_SPIM_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void SD_SPIM_Sleep(void) 
{
    /* Save components enable state */
    SD_SPIM_backup.enableState = ((uint8) SD_SPIM_IS_ENABLED);

    SD_SPIM_Stop();
}


/*******************************************************************************
* Function Name: SD_SPIM_Wakeup
********************************************************************************
*
* Summary:
*  Prepare SPIM Component to wake up.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  SD_SPIM_backup - used when non-retention registers are restored.
*  SD_SPIM_txBufferWrite - modified every function call - resets to
*  zero.
*  SD_SPIM_txBufferRead - modified every function call - resets to
*  zero.
*  SD_SPIM_rxBufferWrite - modified every function call - resets to
*  zero.
*  SD_SPIM_rxBufferRead - modified every function call - resets to
*  zero.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void SD_SPIM_Wakeup(void) 
{
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

    /* Clear any data from the RX and TX FIFO */
    SD_SPIM_ClearFIFO();

    /* Restore components block enable state */
    if(0u != SD_SPIM_backup.enableState)
    {
        SD_SPIM_Enable();
    }
}


/* [] END OF FILE */
