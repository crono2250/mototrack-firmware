/*******************************************************************************
* File Name: Timer2_PM.c  
* Version 3.0
*
*  Description:
*    This file provides the power management source code to API for the
*    Counter.  
*
*   Note:
*     None
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "Timer2.h"

static Timer2_backupStruct Timer2_backup;


/*******************************************************************************
* Function Name: Timer2_SaveConfig
********************************************************************************
* Summary:
*     Save the current user configuration
*
* Parameters:  
*  void
*
* Return: 
*  void
*
* Global variables:
*  Timer2_backup:  Variables of this global structure are modified to 
*  store the values of non retention configuration registers when Sleep() API is 
*  called.
*
*******************************************************************************/
void Timer2_SaveConfig(void) 
{
    #if (!Timer2_UsingFixedFunction)

        Timer2_backup.CounterUdb = Timer2_ReadCounter();

        #if(!Timer2_ControlRegRemoved)
            Timer2_backup.CounterControlRegister = Timer2_ReadControlRegister();
        #endif /* (!Timer2_ControlRegRemoved) */

    #endif /* (!Timer2_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Timer2_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:  
*  void
*
* Return: 
*  void
*
* Global variables:
*  Timer2_backup:  Variables of this global structure are used to 
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Timer2_RestoreConfig(void) 
{      
    #if (!Timer2_UsingFixedFunction)

       Timer2_WriteCounter(Timer2_backup.CounterUdb);

        #if(!Timer2_ControlRegRemoved)
            Timer2_WriteControlRegister(Timer2_backup.CounterControlRegister);
        #endif /* (!Timer2_ControlRegRemoved) */

    #endif /* (!Timer2_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Timer2_Sleep
********************************************************************************
* Summary:
*     Stop and Save the user configuration
*
* Parameters:  
*  void
*
* Return: 
*  void
*
* Global variables:
*  Timer2_backup.enableState:  Is modified depending on the enable 
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void Timer2_Sleep(void) 
{
    #if(!Timer2_ControlRegRemoved)
        /* Save Counter's enable state */
        if(Timer2_CTRL_ENABLE == (Timer2_CONTROL & Timer2_CTRL_ENABLE))
        {
            /* Counter is enabled */
            Timer2_backup.CounterEnableState = 1u;
        }
        else
        {
            /* Counter is disabled */
            Timer2_backup.CounterEnableState = 0u;
        }
    #else
        Timer2_backup.CounterEnableState = 1u;
        if(Timer2_backup.CounterEnableState != 0u)
        {
            Timer2_backup.CounterEnableState = 0u;
        }
    #endif /* (!Timer2_ControlRegRemoved) */
    
    Timer2_Stop();
    Timer2_SaveConfig();
}


/*******************************************************************************
* Function Name: Timer2_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration
*  
* Parameters:  
*  void
*
* Return: 
*  void
*
* Global variables:
*  Timer2_backup.enableState:  Is used to restore the enable state of 
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Timer2_Wakeup(void) 
{
    Timer2_RestoreConfig();
    #if(!Timer2_ControlRegRemoved)
        if(Timer2_backup.CounterEnableState == 1u)
        {
            /* Enable Counter's operation */
            Timer2_Enable();
        } /* Do nothing if Counter was disabled before */    
    #endif /* (!Timer2_ControlRegRemoved) */
    
}


/* [] END OF FILE */
