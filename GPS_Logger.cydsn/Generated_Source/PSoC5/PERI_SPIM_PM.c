/*******************************************************************************
* File Name: PERI_SPIM_PM.c
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

#include "PERI_SPIM_PVT.h"

static PERI_SPIM_BACKUP_STRUCT PERI_SPIM_backup =
{
    PERI_SPIM_DISABLED,
    PERI_SPIM_BITCTR_INIT,
};


/*******************************************************************************
* Function Name: PERI_SPIM_SaveConfig
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
void PERI_SPIM_SaveConfig(void) 
{

}


/*******************************************************************************
* Function Name: PERI_SPIM_RestoreConfig
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
void PERI_SPIM_RestoreConfig(void) 
{

}


/*******************************************************************************
* Function Name: PERI_SPIM_Sleep
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
*  PERI_SPIM_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void PERI_SPIM_Sleep(void) 
{
    /* Save components enable state */
    PERI_SPIM_backup.enableState = ((uint8) PERI_SPIM_IS_ENABLED);

    PERI_SPIM_Stop();
}


/*******************************************************************************
* Function Name: PERI_SPIM_Wakeup
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
*  PERI_SPIM_backup - used when non-retention registers are restored.
*  PERI_SPIM_txBufferWrite - modified every function call - resets to
*  zero.
*  PERI_SPIM_txBufferRead - modified every function call - resets to
*  zero.
*  PERI_SPIM_rxBufferWrite - modified every function call - resets to
*  zero.
*  PERI_SPIM_rxBufferRead - modified every function call - resets to
*  zero.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void PERI_SPIM_Wakeup(void) 
{
    #if(PERI_SPIM_RX_SOFTWARE_BUF_ENABLED)
        PERI_SPIM_rxBufferFull  = 0u;
        PERI_SPIM_rxBufferRead  = 0u;
        PERI_SPIM_rxBufferWrite = 0u;
    #endif /* (PERI_SPIM_RX_SOFTWARE_BUF_ENABLED) */

    #if(PERI_SPIM_TX_SOFTWARE_BUF_ENABLED)
        PERI_SPIM_txBufferFull  = 0u;
        PERI_SPIM_txBufferRead  = 0u;
        PERI_SPIM_txBufferWrite = 0u;
    #endif /* (PERI_SPIM_TX_SOFTWARE_BUF_ENABLED) */

    /* Clear any data from the RX and TX FIFO */
    PERI_SPIM_ClearFIFO();

    /* Restore components block enable state */
    if(0u != PERI_SPIM_backup.enableState)
    {
        PERI_SPIM_Enable();
    }
}


/* [] END OF FILE */
