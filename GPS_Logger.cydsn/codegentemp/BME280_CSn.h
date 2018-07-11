/*******************************************************************************
* File Name: BME280_CSn.h  
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

#if !defined(CY_PINS_BME280_CSn_H) /* Pins BME280_CSn_H */
#define CY_PINS_BME280_CSn_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "BME280_CSn_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 BME280_CSn__PORT == 15 && ((BME280_CSn__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    BME280_CSn_Write(uint8 value);
void    BME280_CSn_SetDriveMode(uint8 mode);
uint8   BME280_CSn_ReadDataReg(void);
uint8   BME280_CSn_Read(void);
void    BME280_CSn_SetInterruptMode(uint16 position, uint16 mode);
uint8   BME280_CSn_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the BME280_CSn_SetDriveMode() function.
     *  @{
     */
        #define BME280_CSn_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define BME280_CSn_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define BME280_CSn_DM_RES_UP          PIN_DM_RES_UP
        #define BME280_CSn_DM_RES_DWN         PIN_DM_RES_DWN
        #define BME280_CSn_DM_OD_LO           PIN_DM_OD_LO
        #define BME280_CSn_DM_OD_HI           PIN_DM_OD_HI
        #define BME280_CSn_DM_STRONG          PIN_DM_STRONG
        #define BME280_CSn_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define BME280_CSn_MASK               BME280_CSn__MASK
#define BME280_CSn_SHIFT              BME280_CSn__SHIFT
#define BME280_CSn_WIDTH              1u

/* Interrupt constants */
#if defined(BME280_CSn__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in BME280_CSn_SetInterruptMode() function.
     *  @{
     */
        #define BME280_CSn_INTR_NONE      (uint16)(0x0000u)
        #define BME280_CSn_INTR_RISING    (uint16)(0x0001u)
        #define BME280_CSn_INTR_FALLING   (uint16)(0x0002u)
        #define BME280_CSn_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define BME280_CSn_INTR_MASK      (0x01u) 
#endif /* (BME280_CSn__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define BME280_CSn_PS                     (* (reg8 *) BME280_CSn__PS)
/* Data Register */
#define BME280_CSn_DR                     (* (reg8 *) BME280_CSn__DR)
/* Port Number */
#define BME280_CSn_PRT_NUM                (* (reg8 *) BME280_CSn__PRT) 
/* Connect to Analog Globals */                                                  
#define BME280_CSn_AG                     (* (reg8 *) BME280_CSn__AG)                       
/* Analog MUX bux enable */
#define BME280_CSn_AMUX                   (* (reg8 *) BME280_CSn__AMUX) 
/* Bidirectional Enable */                                                        
#define BME280_CSn_BIE                    (* (reg8 *) BME280_CSn__BIE)
/* Bit-mask for Aliased Register Access */
#define BME280_CSn_BIT_MASK               (* (reg8 *) BME280_CSn__BIT_MASK)
/* Bypass Enable */
#define BME280_CSn_BYP                    (* (reg8 *) BME280_CSn__BYP)
/* Port wide control signals */                                                   
#define BME280_CSn_CTL                    (* (reg8 *) BME280_CSn__CTL)
/* Drive Modes */
#define BME280_CSn_DM0                    (* (reg8 *) BME280_CSn__DM0) 
#define BME280_CSn_DM1                    (* (reg8 *) BME280_CSn__DM1)
#define BME280_CSn_DM2                    (* (reg8 *) BME280_CSn__DM2) 
/* Input Buffer Disable Override */
#define BME280_CSn_INP_DIS                (* (reg8 *) BME280_CSn__INP_DIS)
/* LCD Common or Segment Drive */
#define BME280_CSn_LCD_COM_SEG            (* (reg8 *) BME280_CSn__LCD_COM_SEG)
/* Enable Segment LCD */
#define BME280_CSn_LCD_EN                 (* (reg8 *) BME280_CSn__LCD_EN)
/* Slew Rate Control */
#define BME280_CSn_SLW                    (* (reg8 *) BME280_CSn__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define BME280_CSn_PRTDSI__CAPS_SEL       (* (reg8 *) BME280_CSn__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define BME280_CSn_PRTDSI__DBL_SYNC_IN    (* (reg8 *) BME280_CSn__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define BME280_CSn_PRTDSI__OE_SEL0        (* (reg8 *) BME280_CSn__PRTDSI__OE_SEL0) 
#define BME280_CSn_PRTDSI__OE_SEL1        (* (reg8 *) BME280_CSn__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define BME280_CSn_PRTDSI__OUT_SEL0       (* (reg8 *) BME280_CSn__PRTDSI__OUT_SEL0) 
#define BME280_CSn_PRTDSI__OUT_SEL1       (* (reg8 *) BME280_CSn__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define BME280_CSn_PRTDSI__SYNC_OUT       (* (reg8 *) BME280_CSn__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(BME280_CSn__SIO_CFG)
    #define BME280_CSn_SIO_HYST_EN        (* (reg8 *) BME280_CSn__SIO_HYST_EN)
    #define BME280_CSn_SIO_REG_HIFREQ     (* (reg8 *) BME280_CSn__SIO_REG_HIFREQ)
    #define BME280_CSn_SIO_CFG            (* (reg8 *) BME280_CSn__SIO_CFG)
    #define BME280_CSn_SIO_DIFF           (* (reg8 *) BME280_CSn__SIO_DIFF)
#endif /* (BME280_CSn__SIO_CFG) */

/* Interrupt Registers */
#if defined(BME280_CSn__INTSTAT)
    #define BME280_CSn_INTSTAT            (* (reg8 *) BME280_CSn__INTSTAT)
    #define BME280_CSn_SNAP               (* (reg8 *) BME280_CSn__SNAP)
    
	#define BME280_CSn_0_INTTYPE_REG 		(* (reg8 *) BME280_CSn__0__INTTYPE)
#endif /* (BME280_CSn__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_BME280_CSn_H */


/* [] END OF FILE */
