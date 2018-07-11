/*******************************************************************************
* File Name: Timer1.c  
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

#include "Timer1.h"

uint8 Timer1_initVar = 0u;


/*******************************************************************************
* Function Name: Timer1_Init
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
void Timer1_Init(void) 
{
        #if (!Timer1_UsingFixedFunction && !Timer1_ControlRegRemoved)
            uint8 ctrl;
        #endif /* (!Timer1_UsingFixedFunction && !Timer1_ControlRegRemoved) */
        
        #if(!Timer1_UsingFixedFunction) 
            /* Interrupt State Backup for Critical Region*/
            uint8 Timer1_interruptState;
        #endif /* (!Timer1_UsingFixedFunction) */
        
        #if (Timer1_UsingFixedFunction)
            /* Clear all bits but the enable bit (if it's already set for Timer operation */
            Timer1_CONTROL &= Timer1_CTRL_ENABLE;
            
            /* Clear the mode bits for continuous run mode */
            #if (CY_PSOC5A)
                Timer1_CONTROL2 &= ((uint8)(~Timer1_CTRL_MODE_MASK));
            #endif /* (CY_PSOC5A) */
            #if (CY_PSOC3 || CY_PSOC5LP)
                Timer1_CONTROL3 &= ((uint8)(~Timer1_CTRL_MODE_MASK));                
            #endif /* (CY_PSOC3 || CY_PSOC5LP) */
            /* Check if One Shot mode is enabled i.e. RunMode !=0*/
            #if (Timer1_RunModeUsed != 0x0u)
                /* Set 3rd bit of Control register to enable one shot mode */
                Timer1_CONTROL |= Timer1_ONESHOT;
            #endif /* (Timer1_RunModeUsed != 0x0u) */
            
            /* Set the IRQ to use the status register interrupts */
            Timer1_CONTROL2 |= Timer1_CTRL2_IRQ_SEL;
            
            /* Clear and Set SYNCTC and SYNCCMP bits of RT1 register */
            Timer1_RT1 &= ((uint8)(~Timer1_RT1_MASK));
            Timer1_RT1 |= Timer1_SYNC;     
                    
            /*Enable DSI Sync all all inputs of the Timer*/
            Timer1_RT1 &= ((uint8)(~Timer1_SYNCDSI_MASK));
            Timer1_RT1 |= Timer1_SYNCDSI_EN;

        #else
            #if(!Timer1_ControlRegRemoved)
            /* Set the default compare mode defined in the parameter */
            ctrl = Timer1_CONTROL & ((uint8)(~Timer1_CTRL_CMPMODE_MASK));
            Timer1_CONTROL = ctrl | Timer1_DEFAULT_COMPARE_MODE;
            
            /* Set the default capture mode defined in the parameter */
            ctrl = Timer1_CONTROL & ((uint8)(~Timer1_CTRL_CAPMODE_MASK));
            
            #if( 0 != Timer1_CAPTURE_MODE_CONF)
                Timer1_CONTROL = ctrl | Timer1_DEFAULT_CAPTURE_MODE;
            #else
                Timer1_CONTROL = ctrl;
            #endif /* 0 != Timer1_CAPTURE_MODE */ 
            
            #endif /* (!Timer1_ControlRegRemoved) */
        #endif /* (Timer1_UsingFixedFunction) */
        
        /* Clear all data in the FIFO's */
        #if (!Timer1_UsingFixedFunction)
            Timer1_ClearFIFO();
        #endif /* (!Timer1_UsingFixedFunction) */
        
        /* Set Initial values from Configuration */
        Timer1_WritePeriod(Timer1_INIT_PERIOD_VALUE);
        #if (!(Timer1_UsingFixedFunction && (CY_PSOC5A)))
            Timer1_WriteCounter(Timer1_INIT_COUNTER_VALUE);
        #endif /* (!(Timer1_UsingFixedFunction && (CY_PSOC5A))) */
        Timer1_SetInterruptMode(Timer1_INIT_INTERRUPTS_MASK);
        
        #if (!Timer1_UsingFixedFunction)
            /* Read the status register to clear the unwanted interrupts */
            (void)Timer1_ReadStatusRegister();
            /* Set the compare value (only available to non-fixed function implementation */
            Timer1_WriteCompare(Timer1_INIT_COMPARE_VALUE);
            /* Use the interrupt output of the status register for IRQ output */
            
            /* CyEnterCriticalRegion and CyExitCriticalRegion are used to mark following region critical*/
            /* Enter Critical Region*/
            Timer1_interruptState = CyEnterCriticalSection();
            
            Timer1_STATUS_AUX_CTRL |= Timer1_STATUS_ACTL_INT_EN_MASK;
            
            /* Exit Critical Region*/
            CyExitCriticalSection(Timer1_interruptState);
            
        #endif /* (!Timer1_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Timer1_Enable
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
void Timer1_Enable(void) 
{
    /* Globally Enable the Fixed Function Block chosen */
    #if (Timer1_UsingFixedFunction)
        Timer1_GLOBAL_ENABLE |= Timer1_BLOCK_EN_MASK;
        Timer1_GLOBAL_STBY_ENABLE |= Timer1_BLOCK_STBY_EN_MASK;
    #endif /* (Timer1_UsingFixedFunction) */  
        
    /* Enable the counter from the control register  */
    /* If Fixed Function then make sure Mode is set correctly */
    /* else make sure reset is clear */
    #if(!Timer1_ControlRegRemoved || Timer1_UsingFixedFunction)
        Timer1_CONTROL |= Timer1_CTRL_ENABLE;                
    #endif /* (!Timer1_ControlRegRemoved || Timer1_UsingFixedFunction) */
    
}


/*******************************************************************************
* Function Name: Timer1_Start
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
*  Timer1_initVar: Is modified when this function is called for the  
*   first time. Is used to ensure that initialization happens only once.
*
*******************************************************************************/
void Timer1_Start(void) 
{
    if(Timer1_initVar == 0u)
    {
        Timer1_Init();
        
        Timer1_initVar = 1u; /* Clear this bit for Initialization */        
    }
    
    /* Enable the Counter */
    Timer1_Enable();        
}


/*******************************************************************************
* Function Name: Timer1_Stop
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
void Timer1_Stop(void) 
{
    /* Disable Counter */
    #if(!Timer1_ControlRegRemoved || Timer1_UsingFixedFunction)
        Timer1_CONTROL &= ((uint8)(~Timer1_CTRL_ENABLE));        
    #endif /* (!Timer1_ControlRegRemoved || Timer1_UsingFixedFunction) */
    
    /* Globally disable the Fixed Function Block chosen */
    #if (Timer1_UsingFixedFunction)
        Timer1_GLOBAL_ENABLE &= ((uint8)(~Timer1_BLOCK_EN_MASK));
        Timer1_GLOBAL_STBY_ENABLE &= ((uint8)(~Timer1_BLOCK_STBY_EN_MASK));
    #endif /* (Timer1_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Timer1_SetInterruptMode
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
void Timer1_SetInterruptMode(uint8 interruptsMask) 
{
    Timer1_STATUS_MASK = interruptsMask;
}


/*******************************************************************************
* Function Name: Timer1_ReadStatusRegister
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
uint8   Timer1_ReadStatusRegister(void) 
{
    return Timer1_STATUS;
}


#if(!Timer1_ControlRegRemoved)
/*******************************************************************************
* Function Name: Timer1_ReadControlRegister
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
uint8   Timer1_ReadControlRegister(void) 
{
    return Timer1_CONTROL;
}


/*******************************************************************************
* Function Name: Timer1_WriteControlRegister
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
void    Timer1_WriteControlRegister(uint8 control) 
{
    Timer1_CONTROL = control;
}

#endif  /* (!Timer1_ControlRegRemoved) */


#if (!(Timer1_UsingFixedFunction && (CY_PSOC5A)))
/*******************************************************************************
* Function Name: Timer1_WriteCounter
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
void Timer1_WriteCounter(uint16 counter) \
                                   
{
    #if(Timer1_UsingFixedFunction)
        /* assert if block is already enabled */
        CYASSERT (0u == (Timer1_GLOBAL_ENABLE & Timer1_BLOCK_EN_MASK));
        /* If block is disabled, enable it and then write the counter */
        Timer1_GLOBAL_ENABLE |= Timer1_BLOCK_EN_MASK;
        CY_SET_REG16(Timer1_COUNTER_LSB_PTR, (uint16)counter);
        Timer1_GLOBAL_ENABLE &= ((uint8)(~Timer1_BLOCK_EN_MASK));
    #else
        CY_SET_REG16(Timer1_COUNTER_LSB_PTR, counter);
    #endif /* (Timer1_UsingFixedFunction) */
}
#endif /* (!(Timer1_UsingFixedFunction && (CY_PSOC5A))) */


/*******************************************************************************
* Function Name: Timer1_ReadCounter
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
uint16 Timer1_ReadCounter(void) 
{
    /* Force capture by reading Accumulator */
    /* Must first do a software capture to be able to read the counter */
    /* It is up to the user code to make sure there isn't already captured data in the FIFO */
    #if(Timer1_UsingFixedFunction)
		(void)CY_GET_REG16(Timer1_COUNTER_LSB_PTR);
	#else
		(void)CY_GET_REG8(Timer1_COUNTER_LSB_PTR_8BIT);
	#endif/* (Timer1_UsingFixedFunction) */
    
    /* Read the data from the FIFO (or capture register for Fixed Function)*/
    #if(Timer1_UsingFixedFunction)
        return ((uint16)CY_GET_REG16(Timer1_STATICCOUNT_LSB_PTR));
    #else
        return (CY_GET_REG16(Timer1_STATICCOUNT_LSB_PTR));
    #endif /* (Timer1_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Timer1_ReadCapture
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
uint16 Timer1_ReadCapture(void) 
{
    #if(Timer1_UsingFixedFunction)
        return ((uint16)CY_GET_REG16(Timer1_STATICCOUNT_LSB_PTR));
    #else
        return (CY_GET_REG16(Timer1_STATICCOUNT_LSB_PTR));
    #endif /* (Timer1_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Timer1_WritePeriod
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
void Timer1_WritePeriod(uint16 period) 
{
    #if(Timer1_UsingFixedFunction)
        CY_SET_REG16(Timer1_PERIOD_LSB_PTR,(uint16)period);
    #else
        CY_SET_REG16(Timer1_PERIOD_LSB_PTR, period);
    #endif /* (Timer1_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Timer1_ReadPeriod
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
uint16 Timer1_ReadPeriod(void) 
{
    #if(Timer1_UsingFixedFunction)
        return ((uint16)CY_GET_REG16(Timer1_PERIOD_LSB_PTR));
    #else
        return (CY_GET_REG16(Timer1_PERIOD_LSB_PTR));
    #endif /* (Timer1_UsingFixedFunction) */
}


#if (!Timer1_UsingFixedFunction)
/*******************************************************************************
* Function Name: Timer1_WriteCompare
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
void Timer1_WriteCompare(uint16 compare) \
                                   
{
    #if(Timer1_UsingFixedFunction)
        CY_SET_REG16(Timer1_COMPARE_LSB_PTR, (uint16)compare);
    #else
        CY_SET_REG16(Timer1_COMPARE_LSB_PTR, compare);
    #endif /* (Timer1_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: Timer1_ReadCompare
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
uint16 Timer1_ReadCompare(void) 
{
    return (CY_GET_REG16(Timer1_COMPARE_LSB_PTR));
}


#if (Timer1_COMPARE_MODE_SOFTWARE)
/*******************************************************************************
* Function Name: Timer1_SetCompareMode
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
void Timer1_SetCompareMode(uint8 compareMode) 
{
    /* Clear the compare mode bits in the control register */
    Timer1_CONTROL &= ((uint8)(~Timer1_CTRL_CMPMODE_MASK));
    
    /* Write the new setting */
    Timer1_CONTROL |= compareMode;
}
#endif  /* (Timer1_COMPARE_MODE_SOFTWARE) */


#if (Timer1_CAPTURE_MODE_SOFTWARE)
/*******************************************************************************
* Function Name: Timer1_SetCaptureMode
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
void Timer1_SetCaptureMode(uint8 captureMode) 
{
    /* Clear the capture mode bits in the control register */
    Timer1_CONTROL &= ((uint8)(~Timer1_CTRL_CAPMODE_MASK));
    
    /* Write the new setting */
    Timer1_CONTROL |= ((uint8)((uint8)captureMode << Timer1_CTRL_CAPMODE0_SHIFT));
}
#endif  /* (Timer1_CAPTURE_MODE_SOFTWARE) */


/*******************************************************************************
* Function Name: Timer1_ClearFIFO
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
void Timer1_ClearFIFO(void) 
{

    while(0u != (Timer1_ReadStatusRegister() & Timer1_STATUS_FIFONEMP))
    {
        (void)Timer1_ReadCapture();
    }

}
#endif  /* (!Timer1_UsingFixedFunction) */


/* [] END OF FILE */

