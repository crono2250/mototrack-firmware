/*******************************************************************************
* File Name: Timer2.c  
* Version 3.0
*
*  Description:
*     The Counter component consists of a 8, 16, 24 or 32-bit counter with
*     a selectable period between 2 and 2^Width - 1.  
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

uint8 Timer2_initVar = 0u;


/*******************************************************************************
* Function Name: Timer2_Init
********************************************************************************
* Summary:
*     Initialize to the schematic state
* 
* Parameters:  
*  void  
*
* Return: 
*  void
*
*******************************************************************************/
void Timer2_Init(void) 
{
        #if (!Timer2_UsingFixedFunction && !Timer2_ControlRegRemoved)
            uint8 ctrl;
        #endif /* (!Timer2_UsingFixedFunction && !Timer2_ControlRegRemoved) */
        
        #if(!Timer2_UsingFixedFunction) 
            /* Interrupt State Backup for Critical Region*/
            uint8 Timer2_interruptState;
        #endif /* (!Timer2_UsingFixedFunction) */
        
        #if (Timer2_UsingFixedFunction)
            /* Clear all bits but the enable bit (if it's already set for Timer operation */
            Timer2_CONTROL &= Timer2_CTRL_ENABLE;
            
            /* Clear the mode bits for continuous run mode */
            #if (CY_PSOC5A)
                Timer2_CONTROL2 &= ((uint8)(~Timer2_CTRL_MODE_MASK));
            #endif /* (CY_PSOC5A) */
            #if (CY_PSOC3 || CY_PSOC5LP)
                Timer2_CONTROL3 &= ((uint8)(~Timer2_CTRL_MODE_MASK));                
            #endif /* (CY_PSOC3 || CY_PSOC5LP) */
            /* Check if One Shot mode is enabled i.e. RunMode !=0*/
            #if (Timer2_RunModeUsed != 0x0u)
                /* Set 3rd bit of Control register to enable one shot mode */
                Timer2_CONTROL |= Timer2_ONESHOT;
            #endif /* (Timer2_RunModeUsed != 0x0u) */
            
            /* Set the IRQ to use the status register interrupts */
            Timer2_CONTROL2 |= Timer2_CTRL2_IRQ_SEL;
            
            /* Clear and Set SYNCTC and SYNCCMP bits of RT1 register */
            Timer2_RT1 &= ((uint8)(~Timer2_RT1_MASK));
            Timer2_RT1 |= Timer2_SYNC;     
                    
            /*Enable DSI Sync all all inputs of the Timer*/
            Timer2_RT1 &= ((uint8)(~Timer2_SYNCDSI_MASK));
            Timer2_RT1 |= Timer2_SYNCDSI_EN;

        #else
            #if(!Timer2_ControlRegRemoved)
            /* Set the default compare mode defined in the parameter */
            ctrl = Timer2_CONTROL & ((uint8)(~Timer2_CTRL_CMPMODE_MASK));
            Timer2_CONTROL = ctrl | Timer2_DEFAULT_COMPARE_MODE;
            
            /* Set the default capture mode defined in the parameter */
            ctrl = Timer2_CONTROL & ((uint8)(~Timer2_CTRL_CAPMODE_MASK));
            
            #if( 0 != Timer2_CAPTURE_MODE_CONF)
                Timer2_CONTROL = ctrl | Timer2_DEFAULT_CAPTURE_MODE;
            #else
                Timer2_CONTROL = ctrl;
            #endif /* 0 != Timer2_CAPTURE_MODE */ 
            
            #endif /* (!Timer2_ControlRegRemoved) */
        #endif /* (Timer2_UsingFixedFunction) */
        
        /* Clear all data in the FIFO's */
        #if (!Timer2_UsingFixedFunction)
            Timer2_ClearFIFO();
        #endif /* (!Timer2_UsingFixedFunction) */
        
        /* Set Initial values from Configuration */
        Timer2_WritePeriod(Timer2_INIT_PERIOD_VALUE);
        #if (!(Timer2_UsingFixedFunction && (CY_PSOC5A)))
            Timer2_WriteCounter(Timer2_INIT_COUNTER_VALUE);
        #endif /* (!(Timer2_UsingFixedFunction && (CY_PSOC5A))) */
        Timer2_SetInterruptMode(Timer2_INIT_INTERRUPTS_MASK);
        
        #if (!Timer2_UsingFixedFunction)
            /* Read the status register to clear the unwanted interrupts */
            (void)Timer2_ReadStatusRegister();
            /* Set the compare value (only available to non-fixed function implementation */
            Timer2_WriteCompare(Timer2_INIT_COMPARE_VALUE);
            /* Use the interrupt output of the status register for IRQ output */
            
            /* CyEnterCriticalRegion and CyExitCriticalRegion are used to mark following region critical*/
            /* Enter Critical Region*/
            Timer2_interruptState = CyEnterCriticalSection();
            
            Timer2_STATUS_AUX_CTRL |= Timer2_STATUS_ACTL_INT_EN_MASK;
            
            /* Exit Critical Region*/
            CyExitCriticalSection(Timer2_interruptState);
            
        #endif /* (!Timer2_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Timer2_Enable
********************************************************************************
* Summary:
*     Enable the Counter
* 
* Parameters:  
*  void  
*
* Return: 
*  void
*
* Side Effects: 
*   If the Enable mode is set to Hardware only then this function has no effect 
*   on the operation of the counter.
*
*******************************************************************************/
void Timer2_Enable(void) 
{
    /* Globally Enable the Fixed Function Block chosen */
    #if (Timer2_UsingFixedFunction)
        Timer2_GLOBAL_ENABLE |= Timer2_BLOCK_EN_MASK;
        Timer2_GLOBAL_STBY_ENABLE |= Timer2_BLOCK_STBY_EN_MASK;
    #endif /* (Timer2_UsingFixedFunction) */  
        
    /* Enable the counter from the control register  */
    /* If Fixed Function then make sure Mode is set correctly */
    /* else make sure reset is clear */
    #if(!Timer2_ControlRegRemoved || Timer2_UsingFixedFunction)
        Timer2_CONTROL |= Timer2_CTRL_ENABLE;                
    #endif /* (!Timer2_ControlRegRemoved || Timer2_UsingFixedFunction) */
    
}


/*******************************************************************************
* Function Name: Timer2_Start
********************************************************************************
* Summary:
*  Enables the counter for operation 
*
* Parameters:  
*  void  
*
* Return: 
*  void
*
* Global variables:
*  Timer2_initVar: Is modified when this function is called for the  
*   first time. Is used to ensure that initialization happens only once.
*
*******************************************************************************/
void Timer2_Start(void) 
{
    if(Timer2_initVar == 0u)
    {
        Timer2_Init();
        
        Timer2_initVar = 1u; /* Clear this bit for Initialization */        
    }
    
    /* Enable the Counter */
    Timer2_Enable();        
}


/*******************************************************************************
* Function Name: Timer2_Stop
********************************************************************************
* Summary:
* Halts the counter, but does not change any modes or disable interrupts.
*
* Parameters:  
*  void  
*
* Return: 
*  void
*
* Side Effects: If the Enable mode is set to Hardware only then this function
*               has no effect on the operation of the counter.
*
*******************************************************************************/
void Timer2_Stop(void) 
{
    /* Disable Counter */
    #if(!Timer2_ControlRegRemoved || Timer2_UsingFixedFunction)
        Timer2_CONTROL &= ((uint8)(~Timer2_CTRL_ENABLE));        
    #endif /* (!Timer2_ControlRegRemoved || Timer2_UsingFixedFunction) */
    
    /* Globally disable the Fixed Function Block chosen */
    #if (Timer2_UsingFixedFunction)
        Timer2_GLOBAL_ENABLE &= ((uint8)(~Timer2_BLOCK_EN_MASK));
        Timer2_GLOBAL_STBY_ENABLE &= ((uint8)(~Timer2_BLOCK_STBY_EN_MASK));
    #endif /* (Timer2_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Timer2_SetInterruptMode
********************************************************************************
* Summary:
* Configures which interrupt sources are enabled to generate the final interrupt
*
* Parameters:  
*  InterruptsMask: This parameter is an or'd collection of the status bits
*                   which will be allowed to generate the counters interrupt.   
*
* Return: 
*  void
*
*******************************************************************************/
void Timer2_SetInterruptMode(uint8 interruptsMask) 
{
    Timer2_STATUS_MASK = interruptsMask;
}


/*******************************************************************************
* Function Name: Timer2_ReadStatusRegister
********************************************************************************
* Summary:
*   Reads the status register and returns it's state. This function should use
*       defined types for the bit-field information as the bits in this
*       register may be permuteable.
*
* Parameters:  
*  void
*
* Return: 
*  (uint8) The contents of the status register
*
* Side Effects:
*   Status register bits may be clear on read. 
*
*******************************************************************************/
uint8   Timer2_ReadStatusRegister(void) 
{
    return Timer2_STATUS;
}


#if(!Timer2_ControlRegRemoved)
/*******************************************************************************
* Function Name: Timer2_ReadControlRegister
********************************************************************************
* Summary:
*   Reads the control register and returns it's state. This function should use
*       defined types for the bit-field information as the bits in this
*       register may be permuteable.
*
* Parameters:  
*  void
*
* Return: 
*  (uint8) The contents of the control register
*
*******************************************************************************/
uint8   Timer2_ReadControlRegister(void) 
{
    return Timer2_CONTROL;
}


/*******************************************************************************
* Function Name: Timer2_WriteControlRegister
********************************************************************************
* Summary:
*   Sets the bit-field of the control register.  This function should use
*       defined types for the bit-field information as the bits in this
*       register may be permuteable.
*
* Parameters:  
*  void
*
* Return: 
*  (uint8) The contents of the control register
*
*******************************************************************************/
void    Timer2_WriteControlRegister(uint8 control) 
{
    Timer2_CONTROL = control;
}

#endif  /* (!Timer2_ControlRegRemoved) */


#if (!(Timer2_UsingFixedFunction && (CY_PSOC5A)))
/*******************************************************************************
* Function Name: Timer2_WriteCounter
********************************************************************************
* Summary:
*   This funtion is used to set the counter to a specific value
*
* Parameters:  
*  counter:  New counter value. 
*
* Return: 
*  void 
*
*******************************************************************************/
void Timer2_WriteCounter(uint16 counter) \
                                   
{
    #if(Timer2_UsingFixedFunction)
        /* assert if block is already enabled */
        CYASSERT (0u == (Timer2_GLOBAL_ENABLE & Timer2_BLOCK_EN_MASK));
        /* If block is disabled, enable it and then write the counter */
        Timer2_GLOBAL_ENABLE |= Timer2_BLOCK_EN_MASK;
        CY_SET_REG16(Timer2_COUNTER_LSB_PTR, (uint16)counter);
        Timer2_GLOBAL_ENABLE &= ((uint8)(~Timer2_BLOCK_EN_MASK));
    #else
        CY_SET_REG16(Timer2_COUNTER_LSB_PTR, counter);
    #endif /* (Timer2_UsingFixedFunction) */
}
#endif /* (!(Timer2_UsingFixedFunction && (CY_PSOC5A))) */


/*******************************************************************************
* Function Name: Timer2_ReadCounter
********************************************************************************
* Summary:
* Returns the current value of the counter.  It doesn't matter
* if the counter is enabled or running.
*
* Parameters:  
*  void:  
*
* Return: 
*  (uint16) The present value of the counter.
*
*******************************************************************************/
uint16 Timer2_ReadCounter(void) 
{
    /* Force capture by reading Accumulator */
    /* Must first do a software capture to be able to read the counter */
    /* It is up to the user code to make sure there isn't already captured data in the FIFO */
    #if(Timer2_UsingFixedFunction)
		(void)CY_GET_REG16(Timer2_COUNTER_LSB_PTR);
	#else
		(void)CY_GET_REG8(Timer2_COUNTER_LSB_PTR_8BIT);
	#endif/* (Timer2_UsingFixedFunction) */
    
    /* Read the data from the FIFO (or capture register for Fixed Function)*/
    #if(Timer2_UsingFixedFunction)
        return ((uint16)CY_GET_REG16(Timer2_STATICCOUNT_LSB_PTR));
    #else
        return (CY_GET_REG16(Timer2_STATICCOUNT_LSB_PTR));
    #endif /* (Timer2_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Timer2_ReadCapture
********************************************************************************
* Summary:
*   This function returns the last value captured.
*
* Parameters:  
*  void
*
* Return: 
*  (uint16) Present Capture value.
*
*******************************************************************************/
uint16 Timer2_ReadCapture(void) 
{
    #if(Timer2_UsingFixedFunction)
        return ((uint16)CY_GET_REG16(Timer2_STATICCOUNT_LSB_PTR));
    #else
        return (CY_GET_REG16(Timer2_STATICCOUNT_LSB_PTR));
    #endif /* (Timer2_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Timer2_WritePeriod
********************************************************************************
* Summary:
* Changes the period of the counter.  The new period 
* will be loaded the next time terminal count is detected.
*
* Parameters:  
*  period: (uint16) A value of 0 will result in
*         the counter remaining at zero.  
*
* Return: 
*  void
*
*******************************************************************************/
void Timer2_WritePeriod(uint16 period) 
{
    #if(Timer2_UsingFixedFunction)
        CY_SET_REG16(Timer2_PERIOD_LSB_PTR,(uint16)period);
    #else
        CY_SET_REG16(Timer2_PERIOD_LSB_PTR, period);
    #endif /* (Timer2_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Timer2_ReadPeriod
********************************************************************************
* Summary:
* Reads the current period value without affecting counter operation.
*
* Parameters:  
*  void:  
*
* Return: 
*  (uint16) Present period value.
*
*******************************************************************************/
uint16 Timer2_ReadPeriod(void) 
{
    #if(Timer2_UsingFixedFunction)
        return ((uint16)CY_GET_REG16(Timer2_PERIOD_LSB_PTR));
    #else
        return (CY_GET_REG16(Timer2_PERIOD_LSB_PTR));
    #endif /* (Timer2_UsingFixedFunction) */
}


#if (!Timer2_UsingFixedFunction)
/*******************************************************************************
* Function Name: Timer2_WriteCompare
********************************************************************************
* Summary:
* Changes the compare value.  The compare output will 
* reflect the new value on the next UDB clock.  The compare output will be 
* driven high when the present counter value compares true based on the 
* configured compare mode setting. 
*
* Parameters:  
*  Compare:  New compare value. 
*
* Return: 
*  void
*
*******************************************************************************/
void Timer2_WriteCompare(uint16 compare) \
                                   
{
    #if(Timer2_UsingFixedFunction)
        CY_SET_REG16(Timer2_COMPARE_LSB_PTR, (uint16)compare);
    #else
        CY_SET_REG16(Timer2_COMPARE_LSB_PTR, compare);
    #endif /* (Timer2_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Timer2_ReadCompare
********************************************************************************
* Summary:
* Returns the compare value.
*
* Parameters:  
*  void:
*
* Return: 
*  (uint16) Present compare value.
*
*******************************************************************************/
uint16 Timer2_ReadCompare(void) 
{
    return (CY_GET_REG16(Timer2_COMPARE_LSB_PTR));
}


#if (Timer2_COMPARE_MODE_SOFTWARE)
/*******************************************************************************
* Function Name: Timer2_SetCompareMode
********************************************************************************
* Summary:
*  Sets the software controlled Compare Mode.
*
* Parameters:
*  compareMode:  Compare Mode Enumerated Type.
*
* Return:
*  void
*
*******************************************************************************/
void Timer2_SetCompareMode(uint8 compareMode) 
{
    /* Clear the compare mode bits in the control register */
    Timer2_CONTROL &= ((uint8)(~Timer2_CTRL_CMPMODE_MASK));
    
    /* Write the new setting */
    Timer2_CONTROL |= compareMode;
}
#endif  /* (Timer2_COMPARE_MODE_SOFTWARE) */


#if (Timer2_CAPTURE_MODE_SOFTWARE)
/*******************************************************************************
* Function Name: Timer2_SetCaptureMode
********************************************************************************
* Summary:
*  Sets the software controlled Capture Mode.
*
* Parameters:
*  captureMode:  Capture Mode Enumerated Type.
*
* Return:
*  void
*
*******************************************************************************/
void Timer2_SetCaptureMode(uint8 captureMode) 
{
    /* Clear the capture mode bits in the control register */
    Timer2_CONTROL &= ((uint8)(~Timer2_CTRL_CAPMODE_MASK));
    
    /* Write the new setting */
    Timer2_CONTROL |= ((uint8)((uint8)captureMode << Timer2_CTRL_CAPMODE0_SHIFT));
}
#endif  /* (Timer2_CAPTURE_MODE_SOFTWARE) */


/*******************************************************************************
* Function Name: Timer2_ClearFIFO
********************************************************************************
* Summary:
*   This function clears all capture data from the capture FIFO
*
* Parameters:  
*  void:
*
* Return: 
*  None
*
*******************************************************************************/
void Timer2_ClearFIFO(void) 
{

    while(0u != (Timer2_ReadStatusRegister() & Timer2_STATUS_FIFONEMP))
    {
        (void)Timer2_ReadCapture();
    }

}
#endif  /* (!Timer2_UsingFixedFunction) */


/* [] END OF FILE */

