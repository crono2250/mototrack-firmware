/*******************************************************************************
* File Name: MPU9250_INTn.h  
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

#if !defined(CY_PINS_MPU9250_INTn_H) /* Pins MPU9250_INTn_H */
#define CY_PINS_MPU9250_INTn_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "MPU9250_INTn_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 MPU9250_INTn__PORT == 15 && ((MPU9250_INTn__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    MPU9250_INTn_Write(uint8 value);
void    MPU9250_INTn_SetDriveMode(uint8 mode);
uint8   MPU9250_INTn_ReadDataReg(void);
uint8   MPU9250_INTn_Read(void);
void    MPU9250_INTn_SetInterruptMode(uint16 position, uint16 mode);
uint8   MPU9250_INTn_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the MPU9250_INTn_SetDriveMode() function.
     *  @{
     */
        #define MPU9250_INTn_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define MPU9250_INTn_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define MPU9250_INTn_DM_RES_UP          PIN_DM_RES_UP
        #define MPU9250_INTn_DM_RES_DWN         PIN_DM_RES_DWN
        #define MPU9250_INTn_DM_OD_LO           PIN_DM_OD_LO
        #define MPU9250_INTn_DM_OD_HI           PIN_DM_OD_HI
        #define MPU9250_INTn_DM_STRONG          PIN_DM_STRONG
        #define MPU9250_INTn_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define MPU9250_INTn_MASK               MPU9250_INTn__MASK
#define MPU9250_INTn_SHIFT              MPU9250_INTn__SHIFT
#define MPU9250_INTn_WIDTH              1u

/* Interrupt constants */
#if defined(MPU9250_INTn__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in MPU9250_INTn_SetInterruptMode() function.
     *  @{
     */
        #define MPU9250_INTn_INTR_NONE      (uint16)(0x0000u)
        #define MPU9250_INTn_INTR_RISING    (uint16)(0x0001u)
        #define MPU9250_INTn_INTR_FALLING   (uint16)(0x0002u)
        #define MPU9250_INTn_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define MPU9250_INTn_INTR_MASK      (0x01u) 
#endif /* (MPU9250_INTn__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define MPU9250_INTn_PS                     (* (reg8 *) MPU9250_INTn__PS)
/* Data Register */
#define MPU9250_INTn_DR                     (* (reg8 *) MPU9250_INTn__DR)
/* Port Number */
#define MPU9250_INTn_PRT_NUM                (* (reg8 *) MPU9250_INTn__PRT) 
/* Connect to Analog Globals */                                                  
#define MPU9250_INTn_AG                     (* (reg8 *) MPU9250_INTn__AG)                       
/* Analog MUX bux enable */
#define MPU9250_INTn_AMUX                   (* (reg8 *) MPU9250_INTn__AMUX) 
/* Bidirectional Enable */                                                        
#define MPU9250_INTn_BIE                    (* (reg8 *) MPU9250_INTn__BIE)
/* Bit-mask for Aliased Register Access */
#define MPU9250_INTn_BIT_MASK               (* (reg8 *) MPU9250_INTn__BIT_MASK)
/* Bypass Enable */
#define MPU9250_INTn_BYP                    (* (reg8 *) MPU9250_INTn__BYP)
/* Port wide control signals */                                                   
#define MPU9250_INTn_CTL                    (* (reg8 *) MPU9250_INTn__CTL)
/* Drive Modes */
#define MPU9250_INTn_DM0                    (* (reg8 *) MPU9250_INTn__DM0) 
#define MPU9250_INTn_DM1                    (* (reg8 *) MPU9250_INTn__DM1)
#define MPU9250_INTn_DM2                    (* (reg8 *) MPU9250_INTn__DM2) 
/* Input Buffer Disable Override */
#define MPU9250_INTn_INP_DIS                (* (reg8 *) MPU9250_INTn__INP_DIS)
/* LCD Common or Segment Drive */
#define MPU9250_INTn_LCD_COM_SEG            (* (reg8 *) MPU9250_INTn__LCD_COM_SEG)
/* Enable Segment LCD */
#define MPU9250_INTn_LCD_EN                 (* (reg8 *) MPU9250_INTn__LCD_EN)
/* Slew Rate Control */
#define MPU9250_INTn_SLW                    (* (reg8 *) MPU9250_INTn__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define MPU9250_INTn_PRTDSI__CAPS_SEL       (* (reg8 *) MPU9250_INTn__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define MPU9250_INTn_PRTDSI__DBL_SYNC_IN    (* (reg8 *) MPU9250_INTn__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define MPU9250_INTn_PRTDSI__OE_SEL0        (* (reg8 *) MPU9250_INTn__PRTDSI__OE_SEL0) 
#define MPU9250_INTn_PRTDSI__OE_SEL1        (* (reg8 *) MPU9250_INTn__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define MPU9250_INTn_PRTDSI__OUT_SEL0       (* (reg8 *) MPU9250_INTn__PRTDSI__OUT_SEL0) 
#define MPU9250_INTn_PRTDSI__OUT_SEL1       (* (reg8 *) MPU9250_INTn__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define MPU9250_INTn_PRTDSI__SYNC_OUT       (* (reg8 *) MPU9250_INTn__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(MPU9250_INTn__SIO_CFG)
    #define MPU9250_INTn_SIO_HYST_EN        (* (reg8 *) MPU9250_INTn__SIO_HYST_EN)
    #define MPU9250_INTn_SIO_REG_HIFREQ     (* (reg8 *) MPU9250_INTn__SIO_REG_HIFREQ)
    #define MPU9250_INTn_SIO_CFG            (* (reg8 *) MPU9250_INTn__SIO_CFG)
    #define MPU9250_INTn_SIO_DIFF           (* (reg8 *) MPU9250_INTn__SIO_DIFF)
#endif /* (MPU9250_INTn__SIO_CFG) */

/* Interrupt Registers */
#if defined(MPU9250_INTn__INTSTAT)
    #define MPU9250_INTn_INTSTAT            (* (reg8 *) MPU9250_INTn__INTSTAT)
    #define MPU9250_INTn_SNAP               (* (reg8 *) MPU9250_INTn__SNAP)
    
	#define MPU9250_INTn_0_INTTYPE_REG 		(* (reg8 *) MPU9250_INTn__0__INTTYPE)
#endif /* (MPU9250_INTn__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_MPU9250_INTn_H */


/* [] END OF FILE */
