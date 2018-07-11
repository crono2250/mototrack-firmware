/*******************************************************************************
* File Name: Timer1_Reset_PM.c
* Version 1.80
*
* Description:
*  This file contains the setup, control, and status commands to support 
*  the component operation in the low power mode. 
*
* Note:
*
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "Timer1_Reset.h"

/* Check for removal by optimization */
#if !defined(Timer1_Reset_Sync_ctrl_reg__REMOVED)

static Timer1_Reset_BACKUP_STRUCT  Timer1_Reset_backup = {0u};

    
/*******************************************************************************
* Function Name: Timer1_Reset_SaveConfig
********************************************************************************
*
* Summary:
*  Saves the control register value.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void Timer1_Reset_SaveConfig(void) 
{
    Timer1_Reset_backup.controlState = Timer1_Reset_Control;
}


/*******************************************************************************
* Function Name: Timer1_Reset_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the control register value.
*
* Parameters:
*  None
*
* Return:
*  None
*
*
*******************************************************************************/
void Timer1_Reset_RestoreConfig(void) 
{
     Timer1_Reset_Control = Timer1_Reset_backup.controlState;
}


/*******************************************************************************
* Function Name: Timer1_Reset_Sleep
********************************************************************************
*
* Summary:
*  Prepares the component for entering the low power mode.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void Timer1_Reset_Sleep(void) 
{
    Timer1_Reset_SaveConfig();
}


/*******************************************************************************
* Function Name: Timer1_Reset_Wakeup
********************************************************************************
*
* Summary:
*  Restores the component after waking up from the low power mode.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void Timer1_Reset_Wakeup(void)  
{
    Timer1_Reset_RestoreConfig();
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
