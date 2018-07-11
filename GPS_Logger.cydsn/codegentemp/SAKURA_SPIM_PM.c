/*******************************************************************************
* File Name: SAKURA_SPIM_PM.c
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

#include "SAKURA_SPIM_PVT.h"

static SAKURA_SPIM_BACKUP_STRUCT SAKURA_SPIM_backup =
{
    SAKURA_SPIM_DISABLED,
    SAKURA_SPIM_BITCTR_INIT,
};


/*******************************************************************************
* Function Name: SAKURA_SPIM_SaveConfig
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
void SAKURA_SPIM_SaveConfig(void) 
{

}


/*******************************************************************************
* Function Name: SAKURA_SPIM_RestoreConfig
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
void SAKURA_SPIM_RestoreConfig(void) 
{

}


/*******************************************************************************
* Function Name: SAKURA_SPIM_Sleep
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
*  SAKURA_SPIM_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void SAKURA_SPIM_Sleep(void) 
{
    /* Save components enable state */
    SAKURA_SPIM_backup.enableState = ((uint8) SAKURA_SPIM_IS_ENABLED);

    SAKURA_SPIM_Stop();
}


/*******************************************************************************
* Function Name: SAKURA_SPIM_Wakeup
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
*  SAKURA_SPIM_backup - used when non-retention registers are restored.
*  SAKURA_SPIM_txBufferWrite - modified every function call - resets to
*  zero.
*  SAKURA_SPIM_txBufferRead - modified every function call - resets to
*  zero.
*  SAKURA_SPIM_rxBufferWrite - modified every function call - resets to
*  zero.
*  SAKURA_SPIM_rxBufferRead - modified every function call - resets to
*  zero.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void SAKURA_SPIM_Wakeup(void) 
{
    #if(SAKURA_SPIM_RX_SOFTWARE_BUF_ENABLED)
        SAKURA_SPIM_rxBufferFull  = 0u;
        SAKURA_SPIM_rxBufferRead  = 0u;
        SAKURA_SPIM_rxBufferWrite = 0u;
    #endif /* (SAKURA_SPIM_RX_SOFTWARE_BUF_ENABLED) */

    #if(SAKURA_SPIM_TX_SOFTWARE_BUF_ENABLED)
        SAKURA_SPIM_txBufferFull  = 0u;
        SAKURA_SPIM_txBufferRead  = 0u;
        SAKURA_SPIM_txBufferWrite = 0u;
    #endif /* (SAKURA_SPIM_TX_SOFTWARE_BUF_ENABLED) */

    /* Clear any data from the RX and TX FIFO */
    SAKURA_SPIM_ClearFIFO();

    /* Restore components block enable state */
    if(0u != SAKURA_SPIM_backup.enableState)
    {
        SAKURA_SPIM_Enable();
    }
}


/* [] END OF FILE */
