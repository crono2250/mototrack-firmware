/*******************************************************************************
* File Name: Timer1_PM.c  
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

#include "Timer1.h"

static Timer1_backupStruct Timer1_backup;


/*******************************************************************************
* Function Name: Timer1_SaveConfig
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
*  Timer1_backup:  Variables of this global structure are modified to 
*  store the values of non retention configuration registers when Sleep() API is 
*  called.
*
*******************************************************************************/
void Timer1_SaveConfig(void) 
{
    #if (!Timer1_UsingFixedFunction)

        Timer1_backup.CounterUdb = Timer1_ReadCounter();

        #if(!Timer1_ControlRegRemoved)
            Timer1_backup.CounterControlRegister = Timer1_ReadControlRegister();
        #endif /* (!Timer1_ControlRegRemoved) */

    #endif /* (!Timer1_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Timer1_RestoreConfig
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
*  Timer1_backup:  Variables of this global structure are used to 
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void Timer1_RestoreConfig(void) 
{      
    #if (!Timer1_UsingFixedFunction)

       Timer1_WriteCounter(Timer1_backup.CounterUdb);

        #if(!Timer1_ControlRegRemoved)
            Timer1_WriteControlRegister(Timer1_backup.CounterControlRegister);
        #endif /* (!Timer1_ControlRegRemoved) */

    #endif /* (!Timer1_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Timer1_Sleep
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
*  Timer1_backup.enableState:  Is modified depending on the enable 
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void Timer1_Sleep(void) 
{
    #if(!Timer1_ControlRegRemoved)
        /* Save Counter's enable state */
        if(Timer1_CTRL_ENABLE == (Timer1_CONTROL & Timer1_CTRL_ENABLE))
        {
            /* Counter is enabled */
            Timer1_backup.CounterEnableState = 1u;
        }
        else
        {
            /* Counter is disabled */
            Timer1_backup.CounterEnableState = 0u;
        }
    #else
        Timer1_backup.CounterEnableState = 1u;
        if(Timer1_backup.CounterEnableState != 0u)
        {
            Timer1_backup.CounterEnableState = 0u;
        }
    #endif /* (!Timer1_ControlRegRemoved) */
    
    Timer1_Stop();
    Timer1_SaveConfig();
}


/*******************************************************************************
* Function Name: Timer1_Wakeup
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
*  Timer1_backup.enableState:  Is used to restore the enable state of 
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Timer1_Wakeup(void) 
{
    Timer1_RestoreConfig();
    #if(!Timer1_ControlRegRemoved)
        if(Timer1_backup.CounterEnableState == 1u)
        {
            /* Enable Counter's operation */
            Timer1_Enable();
        } /* Do nothing if Counter was disabled before */    
    #endif /* (!Timer1_ControlRegRemoved) */
    
}


/* [] END OF FILE */
