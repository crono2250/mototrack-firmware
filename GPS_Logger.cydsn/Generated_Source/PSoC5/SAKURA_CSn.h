/*******************************************************************************
* File Name: SAKURA_CSn.h  
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

#if !defined(CY_PINS_SAKURA_CSn_H) /* Pins SAKURA_CSn_H */
#define CY_PINS_SAKURA_CSn_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "SAKURA_CSn_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 SAKURA_CSn__PORT == 15 && ((SAKURA_CSn__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    SAKURA_CSn_Write(uint8 value);
void    SAKURA_CSn_SetDriveMode(uint8 mode);
uint8   SAKURA_CSn_ReadDataReg(void);
uint8   SAKURA_CSn_Read(void);
void    SAKURA_CSn_SetInterruptMode(uint16 position, uint16 mode);
uint8   SAKURA_CSn_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the SAKURA_CSn_SetDriveMode() function.
     *  @{
     */
        #define SAKURA_CSn_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define SAKURA_CSn_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define SAKURA_CSn_DM_RES_UP          PIN_DM_RES_UP
        #define SAKURA_CSn_DM_RES_DWN         PIN_DM_RES_DWN
        #define SAKURA_CSn_DM_OD_LO           PIN_DM_OD_LO
        #define SAKURA_CSn_DM_OD_HI           PIN_DM_OD_HI
        #define SAKURA_CSn_DM_STRONG          PIN_DM_STRONG
        #define SAKURA_CSn_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define SAKURA_CSn_MASK               SAKURA_CSn__MASK
#define SAKURA_CSn_SHIFT              SAKURA_CSn__SHIFT
#define SAKURA_CSn_WIDTH              1u

/* Interrupt constants */
#if defined(SAKURA_CSn__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in SAKURA_CSn_SetInterruptMode() function.
     *  @{
     */
        #define SAKURA_CSn_INTR_NONE      (uint16)(0x0000u)
        #define SAKURA_CSn_INTR_RISING    (uint16)(0x0001u)
        #define SAKURA_CSn_INTR_FALLING   (uint16)(0x0002u)
        #define SAKURA_CSn_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define SAKURA_CSn_INTR_MASK      (0x01u) 
#endif /* (SAKURA_CSn__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define SAKURA_CSn_PS                     (* (reg8 *) SAKURA_CSn__PS)
/* Data Register */
#define SAKURA_CSn_DR                     (* (reg8 *) SAKURA_CSn__DR)
/* Port Number */
#define SAKURA_CSn_PRT_NUM                (* (reg8 *) SAKURA_CSn__PRT) 
/* Connect to Analog Globals */                                                  
#define SAKURA_CSn_AG                     (* (reg8 *) SAKURA_CSn__AG)                       
/* Analog MUX bux enable */
#define SAKURA_CSn_AMUX                   (* (reg8 *) SAKURA_CSn__AMUX) 
/* Bidirectional Enable */                                                        
#define SAKURA_CSn_BIE                    (* (reg8 *) SAKURA_CSn__BIE)
/* Bit-mask for Aliased Register Access */
#define SAKURA_CSn_BIT_MASK               (* (reg8 *) SAKURA_CSn__BIT_MASK)
/* Bypass Enable */
#define SAKURA_CSn_BYP                    (* (reg8 *) SAKURA_CSn__BYP)
/* Port wide control signals */                                                   
#define SAKURA_CSn_CTL                    (* (reg8 *) SAKURA_CSn__CTL)
/* Drive Modes */
#define SAKURA_CSn_DM0                    (* (reg8 *) SAKURA_CSn__DM0) 
#define SAKURA_CSn_DM1                    (* (reg8 *) SAKURA_CSn__DM1)
#define SAKURA_CSn_DM2                    (* (reg8 *) SAKURA_CSn__DM2) 
/* Input Buffer Disable Override */
#define SAKURA_CSn_INP_DIS                (* (reg8 *) SAKURA_CSn__INP_DIS)
/* LCD Common or Segment Drive */
#define SAKURA_CSn_LCD_COM_SEG            (* (reg8 *) SAKURA_CSn__LCD_COM_SEG)
/* Enable Segment LCD */
#define SAKURA_CSn_LCD_EN                 (* (reg8 *) SAKURA_CSn__LCD_EN)
/* Slew Rate Control */
#define SAKURA_CSn_SLW                    (* (reg8 *) SAKURA_CSn__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define SAKURA_CSn_PRTDSI__CAPS_SEL       (* (reg8 *) SAKURA_CSn__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define SAKURA_CSn_PRTDSI__DBL_SYNC_IN    (* (reg8 *) SAKURA_CSn__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define SAKURA_CSn_PRTDSI__OE_SEL0        (* (reg8 *) SAKURA_CSn__PRTDSI__OE_SEL0) 
#define SAKURA_CSn_PRTDSI__OE_SEL1        (* (reg8 *) SAKURA_CSn__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define SAKURA_CSn_PRTDSI__OUT_SEL0       (* (reg8 *) SAKURA_CSn__PRTDSI__OUT_SEL0) 
#define SAKURA_CSn_PRTDSI__OUT_SEL1       (* (reg8 *) SAKURA_CSn__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define SAKURA_CSn_PRTDSI__SYNC_OUT       (* (reg8 *) SAKURA_CSn__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(SAKURA_CSn__SIO_CFG)
    #define SAKURA_CSn_SIO_HYST_EN        (* (reg8 *) SAKURA_CSn__SIO_HYST_EN)
    #define SAKURA_CSn_SIO_REG_HIFREQ     (* (reg8 *) SAKURA_CSn__SIO_REG_HIFREQ)
    #define SAKURA_CSn_SIO_CFG            (* (reg8 *) SAKURA_CSn__SIO_CFG)
    #define SAKURA_CSn_SIO_DIFF           (* (reg8 *) SAKURA_CSn__SIO_DIFF)
#endif /* (SAKURA_CSn__SIO_CFG) */

/* Interrupt Registers */
#if defined(SAKURA_CSn__INTSTAT)
    #define SAKURA_CSn_INTSTAT            (* (reg8 *) SAKURA_CSn__INTSTAT)
    #define SAKURA_CSn_SNAP               (* (reg8 *) SAKURA_CSn__SNAP)
    
	#define SAKURA_CSn_0_INTTYPE_REG 		(* (reg8 *) SAKURA_CSn__0__INTTYPE)
#endif /* (SAKURA_CSn__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_SAKURA_CSn_H */


/* [] END OF FILE */
