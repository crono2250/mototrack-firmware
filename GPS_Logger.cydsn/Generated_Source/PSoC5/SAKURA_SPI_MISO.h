/*******************************************************************************
* File Name: SAKURA_SPI_MISO.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_SAKURA_SPI_MISO_H) /* Pins SAKURA_SPI_MISO_H */
#define CY_PINS_SAKURA_SPI_MISO_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "SAKURA_SPI_MISO_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 SAKURA_SPI_MISO__PORT == 15 && ((SAKURA_SPI_MISO__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    SAKURA_SPI_MISO_Write(uint8 value);
void    SAKURA_SPI_MISO_SetDriveMode(uint8 mode);
uint8   SAKURA_SPI_MISO_ReadDataReg(void);
uint8   SAKURA_SPI_MISO_Read(void);
void    SAKURA_SPI_MISO_SetInterruptMode(uint16 position, uint16 mode);
uint8   SAKURA_SPI_MISO_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the SAKURA_SPI_MISO_SetDriveMode() function.
     *  @{
     */
        #define SAKURA_SPI_MISO_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define SAKURA_SPI_MISO_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define SAKURA_SPI_MISO_DM_RES_UP          PIN_DM_RES_UP
        #define SAKURA_SPI_MISO_DM_RES_DWN         PIN_DM_RES_DWN
        #define SAKURA_SPI_MISO_DM_OD_LO           PIN_DM_OD_LO
        #define SAKURA_SPI_MISO_DM_OD_HI           PIN_DM_OD_HI
        #define SAKURA_SPI_MISO_DM_STRONG          PIN_DM_STRONG
        #define SAKURA_SPI_MISO_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define SAKURA_SPI_MISO_MASK               SAKURA_SPI_MISO__MASK
#define SAKURA_SPI_MISO_SHIFT              SAKURA_SPI_MISO__SHIFT
#define SAKURA_SPI_MISO_WIDTH              1u

/* Interrupt constants */
#if defined(SAKURA_SPI_MISO__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in SAKURA_SPI_MISO_SetInterruptMode() function.
     *  @{
     */
        #define SAKURA_SPI_MISO_INTR_NONE      (uint16)(0x0000u)
        #define SAKURA_SPI_MISO_INTR_RISING    (uint16)(0x0001u)
        #define SAKURA_SPI_MISO_INTR_FALLING   (uint16)(0x0002u)
        #define SAKURA_SPI_MISO_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define SAKURA_SPI_MISO_INTR_MASK      (0x01u) 
#endif /* (SAKURA_SPI_MISO__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define SAKURA_SPI_MISO_PS                     (* (reg8 *) SAKURA_SPI_MISO__PS)
/* Data Register */
#define SAKURA_SPI_MISO_DR                     (* (reg8 *) SAKURA_SPI_MISO__DR)
/* Port Number */
#define SAKURA_SPI_MISO_PRT_NUM                (* (reg8 *) SAKURA_SPI_MISO__PRT) 
/* Connect to Analog Globals */                                                  
#define SAKURA_SPI_MISO_AG                     (* (reg8 *) SAKURA_SPI_MISO__AG)                       
/* Analog MUX bux enable */
#define SAKURA_SPI_MISO_AMUX                   (* (reg8 *) SAKURA_SPI_MISO__AMUX) 
/* Bidirectional Enable */                                                        
#define SAKURA_SPI_MISO_BIE                    (* (reg8 *) SAKURA_SPI_MISO__BIE)
/* Bit-mask for Aliased Register Access */
#define SAKURA_SPI_MISO_BIT_MASK               (* (reg8 *) SAKURA_SPI_MISO__BIT_MASK)
/* Bypass Enable */
#define SAKURA_SPI_MISO_BYP                    (* (reg8 *) SAKURA_SPI_MISO__BYP)
/* Port wide control signals */                                                   
#define SAKURA_SPI_MISO_CTL                    (* (reg8 *) SAKURA_SPI_MISO__CTL)
/* Drive Modes */
#define SAKURA_SPI_MISO_DM0                    (* (reg8 *) SAKURA_SPI_MISO__DM0) 
#define SAKURA_SPI_MISO_DM1                    (* (reg8 *) SAKURA_SPI_MISO__DM1)
#define SAKURA_SPI_MISO_DM2                    (* (reg8 *) SAKURA_SPI_MISO__DM2) 
/* Input Buffer Disable Override */
#define SAKURA_SPI_MISO_INP_DIS                (* (reg8 *) SAKURA_SPI_MISO__INP_DIS)
/* LCD Common or Segment Drive */
#define SAKURA_SPI_MISO_LCD_COM_SEG            (* (reg8 *) SAKURA_SPI_MISO__LCD_COM_SEG)
/* Enable Segment LCD */
#define SAKURA_SPI_MISO_LCD_EN                 (* (reg8 *) SAKURA_SPI_MISO__LCD_EN)
/* Slew Rate Control */
#define SAKURA_SPI_MISO_SLW                    (* (reg8 *) SAKURA_SPI_MISO__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define SAKURA_SPI_MISO_PRTDSI__CAPS_SEL       (* (reg8 *) SAKURA_SPI_MISO__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define SAKURA_SPI_MISO_PRTDSI__DBL_SYNC_IN    (* (reg8 *) SAKURA_SPI_MISO__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define SAKURA_SPI_MISO_PRTDSI__OE_SEL0        (* (reg8 *) SAKURA_SPI_MISO__PRTDSI__OE_SEL0) 
#define SAKURA_SPI_MISO_PRTDSI__OE_SEL1        (* (reg8 *) SAKURA_SPI_MISO__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define SAKURA_SPI_MISO_PRTDSI__OUT_SEL0       (* (reg8 *) SAKURA_SPI_MISO__PRTDSI__OUT_SEL0) 
#define SAKURA_SPI_MISO_PRTDSI__OUT_SEL1       (* (reg8 *) SAKURA_SPI_MISO__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define SAKURA_SPI_MISO_PRTDSI__SYNC_OUT       (* (reg8 *) SAKURA_SPI_MISO__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(SAKURA_SPI_MISO__SIO_CFG)
    #define SAKURA_SPI_MISO_SIO_HYST_EN        (* (reg8 *) SAKURA_SPI_MISO__SIO_HYST_EN)
    #define SAKURA_SPI_MISO_SIO_REG_HIFREQ     (* (reg8 *) SAKURA_SPI_MISO__SIO_REG_HIFREQ)
    #define SAKURA_SPI_MISO_SIO_CFG            (* (reg8 *) SAKURA_SPI_MISO__SIO_CFG)
    #define SAKURA_SPI_MISO_SIO_DIFF           (* (reg8 *) SAKURA_SPI_MISO__SIO_DIFF)
#endif /* (SAKURA_SPI_MISO__SIO_CFG) */

/* Interrupt Registers */
#if defined(SAKURA_SPI_MISO__INTSTAT)
    #define SAKURA_SPI_MISO_INTSTAT            (* (reg8 *) SAKURA_SPI_MISO__INTSTAT)
    #define SAKURA_SPI_MISO_SNAP               (* (reg8 *) SAKURA_SPI_MISO__SNAP)
    
	#define SAKURA_SPI_MISO_0_INTTYPE_REG 		(* (reg8 *) SAKURA_SPI_MISO__0__INTTYPE)
#endif /* (SAKURA_SPI_MISO__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_SAKURA_SPI_MISO_H */


/* [] END OF FILE */
