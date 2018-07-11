/*******************************************************************************
* File Name: .h
* Version 2.50
*
* Description:
*  This private header file contains internal definitions for the SPIM
*  component. Do not use these definitions directly in your application.
*
* Note:
*
********************************************************************************
* Copyright 2012-2015, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_SPIM_PVT_PERI_SPIM_H)
#define CY_SPIM_PVT_PERI_SPIM_H

#include "PERI_SPIM.h"


/**********************************
*   Functions with external linkage
**********************************/


/**********************************
*   Variables with external linkage
**********************************/

extern volatile uint8 PERI_SPIM_swStatusTx;
extern volatile uint8 PERI_SPIM_swStatusRx;

#if(PERI_SPIM_TX_SOFTWARE_BUF_ENABLED)
    extern volatile uint8 PERI_SPIM_txBuffer[PERI_SPIM_TX_BUFFER_SIZE];
    extern volatile uint8 PERI_SPIM_txBufferRead;
    extern volatile uint8 PERI_SPIM_txBufferWrite;
    extern volatile uint8 PERI_SPIM_txBufferFull;
#endif /* (PERI_SPIM_TX_SOFTWARE_BUF_ENABLED) */

#if(PERI_SPIM_RX_SOFTWARE_BUF_ENABLED)
    extern volatile uint8 PERI_SPIM_rxBuffer[PERI_SPIM_RX_BUFFER_SIZE];
    extern volatile uint8 PERI_SPIM_rxBufferRead;
    extern volatile uint8 PERI_SPIM_rxBufferWrite;
    extern volatile uint8 PERI_SPIM_rxBufferFull;
#endif /* (PERI_SPIM_RX_SOFTWARE_BUF_ENABLED) */

#endif /* CY_SPIM_PVT_PERI_SPIM_H */


/* [] END OF FILE */
