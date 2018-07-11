/*------------------------------------------------------------------------/
/  MMCv3/SDv1/SDv2 (in SPI mode) control module
/-------------------------------------------------------------------------/
/
/  Copyright (C) 2010, ChaN, all right reserved.
/
/ * This software is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/
/-------------------------------------------------------------------------*/

//#include <p24FJ64GA002.h>
#include "device.h"		// This file includes device configurations and API definitions for this project (PSoC3 CY8C3866)
#include "diskio.h"
#include "../xprintf.h"


/* Definitions for MMC/SDC command */
#define CMD0   (0)			/* GO_IDLE_STATE */
#define CMD1   (1)			/* SEND_OP_COND */
#define ACMD41 (41|0x80)	/* SEND_OP_COND (SDC) */
#define CMD8   (8)			/* SEND_IF_COND */
#define CMD9   (9)			/* SEND_CSD */
#define CMD10  (10)			/* SEND_CID */
#define CMD12  (12)			/* STOP_TRANSMISSION */
#define ACMD13 (13|0x80)	/* SD_STATUS (SDC) */
#define CMD16  (16)			/* SET_BLOCKLEN */
#define CMD17  (17)			/* READ_SINGLE_BLOCK */
#define CMD18  (18)			/* READ_MULTIPLE_BLOCK */
#define CMD23  (23)			/* SET_BLOCK_COUNT */
#define ACMD23 (23|0x80)	/* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24  (24)			/* WRITE_BLOCK */
#define CMD25  (25)			/* WRITE_MULTIPLE_BLOCK */
#define CMD41  (41)			/* SEND_OP_COND (ACMD) */
#define CMD55  (55)			/* APP_CMD */
#define CMD58  (58)			/* READ_OCR */


/* Port Controls  (Platform dependent) */
#if 0
#define CS_LOW()  _LATB15 = 0	/* MMC CS = L */
#define CS_HIGH() _LATB15 = 1	/* MMC CS = H */

#define SOCKPORT	PORTB		/* Socket contact port */
#define SOCKWP	(1<<10)		/* Write protect switch (RB10) */
#define SOCKINS	(1<<11)		/* Card detect switch (RB11) */
#endif
//#define	FCLK_SLOW()			/* Set slow clock (100k-400k) */
//#define	FCLK_FAST()			/* Set fast clock (depends on the CSD) */
#define	FCLK_SLOW()			Clock_3_SetDividerValue(24)	/* Set slow clock (100k-400k) */
#define	FCLK_FAST()			Clock_3_SetDividerValue(2)	/* Set fast clock (depends on the CSD) */

//#define	CS_LOW()	CS_Write(0)
//#define	CS_HIGH()	CS_Write(1)
#define	CS_LOW()			SD_CSn_Write(0)
#define	CS_HIGH()			SD_CSn_Write(1)
//#define SD_Acc_LED_ON()		SD_Access_Write(1)
//#define SD_Acc_LED_OFF()	SD_Access_Write(0)


/*--------------------------------------------------------------------------

   Module Private Functions

---------------------------------------------------------------------------*/

static volatile
DSTATUS Stat = STA_NOINIT;	/* Disk status */

//static volatile
//UINT Timer1, Timer2;		/* 1000Hz decrement timer */

static
UINT CardType;



/*-----------------------------------------------------------------------*/
/* Exchange a byte between PSoC3 and MMC via SPI  (Platform dependent)   */
/*-----------------------------------------------------------------------*/

#define xmit_spi(dat) 	xchg_spi(dat)
#define rcvr_spi()		xchg_spi(0xFF)
//#define rcvr_spi_m(p)	SPI1BUF = 0xFF; while (!_SPIRBF); *(p) = (BYTE)SPI1BUF;

static
void rcvr_spi_m(BYTE *dat)
{
	SD_SPIM_WriteTxData(0xFF);
//	while(SD_SPIM_GetTxBufferSize()) {}
	while(!(SD_SPIM_ReadStatus() & SD_SPIM_STS_BYTE_COMPLETE)) {}
	*dat = (BYTE)SD_SPIM_ReadRxData();
	return;
}

static
BYTE xchg_spi (BYTE dat)
{
//	BYTE i;
/*	SPI1BUF = dat;
	while (!_SPIRBF);
	return (BYTE)SPI1BUF;*/
	SD_SPIM_WriteTxData(dat);
	while(!(SD_SPIM_ReadStatus() & SD_SPIM_STS_BYTE_COMPLETE)) {}
	return(SD_SPIM_ReadRxData());
/*	if(SD_SPIM_GetRxBufferSize())
	{
		return(SD_SPIM_ReadRxData());
	}
	else
	{
		xprintf("No data\r\n");
		return 0;
	}*/
}


	void Timer2_Set(unsigned int wait)
	{
		Timer2_Reset_Write(1);
		Timer2_Reset_Write(0);
		Timer2_WriteCounter(wait);
		return;
	}



	void Timer1_Set(unsigned int wait)
	{
		Timer1_Reset_Write(1);
		Timer1_Reset_Write(0);
		Timer1_WriteCounter(wait);
		return;
	}


/*-----------------------------------------------------------------------*/
/* Wait for card ready                                                   */
/*-----------------------------------------------------------------------*/

static
BYTE wait_ready (void)
{
	BYTE res;
	WORD timer;

//	Timer2 = 500;	/* Wait for ready in timeout of 500ms */
	Timer2_Set(50000);
	rcvr_spi();
	do
	{
		res = rcvr_spi();
		timer = Timer2_ReadCounter();
//		xprintf("%u\r\n", timer);
	}
	while ((res != 0xFF) && timer/* Timer2 */);
//	xprintf("%u, %u\r\n", timer, 0, res);
	return res;
}



/*-----------------------------------------------------------------------*/
/* Deselect the card and release SPI bus                                 */
/*-----------------------------------------------------------------------*/

static
void deselect (void)
{
	CS_HIGH();
//	SD_Acc_LED_OFF();
	rcvr_spi();
}



/*-----------------------------------------------------------------------*/
/* Select the card and wait ready                                        */
/*-----------------------------------------------------------------------*/

static
int select (void)	/* 1:Successful, 0:Timeout */
{
	CS_LOW();
//	SD_Acc_LED_ON();
	if (wait_ready() != 0xFF) {
		deselect();
		return 0;
	}
	return 1;
}



/*-----------------------------------------------------------------------*/
/* Power Control  (Platform dependent)                                   */
/*-----------------------------------------------------------------------*/
/* When the target system does not support socket power control, there   */
/* is nothing to do in these functions and chk_power always returns 1.   */

static
void power_on (void)
{
						/* Enable SPI1 */
/*	SPI1CON1 = 0x013B;
	SPI1CON2 = 0x0000;
	_SPIEN = 1;*/
}

static
void power_off (void)
{
	select();			/* Wait for card ready */
	deselect();

//	_SPIEN = 0;			/* Disable SPI1 */

	Stat |= STA_NOINIT;	/* Set STA_NOINIT */
}



/*-----------------------------------------------------------------------*/
/* Receive a data packet from MMC                                        */
/*-----------------------------------------------------------------------*/

static
int rcvr_datablock (	/* 1:OK, 0:Failed */
	BYTE *buff,			/* Data buffer to store received data */
	UINT btr			/* Byte count (must be multiple of 4) */
)
{
	BYTE token;

//	Timer1 = 100;
	Timer1_Set(5000);
	do {							/* Wait for data packet in timeout of 100ms */
		token = rcvr_spi();
	} while ((token == 0xFF) && Timer1_ReadCounter()/* Timer1 */);

	if(token != 0xFE) return 0;		/* If not valid data token, retutn with error */

	do {							/* Receive the data block into buffer */
		rcvr_spi_m(buff++);
		rcvr_spi_m(buff++);
		rcvr_spi_m(buff++);
		rcvr_spi_m(buff++);
//		if(btr > 497) xprintf("%02X", (unsigned int)*buff);
	} while (btr -= 4);
	rcvr_spi();						/* Discard CRC */
	rcvr_spi();

	return 1;						/* Return with success */
}



/*-----------------------------------------------------------------------*/
/* Send a data packet to MMC                                             */
/*-----------------------------------------------------------------------*/

#if _READONLY == 0
static
int xmit_datablock (	/* 1:OK, 0:Failed */
	const BYTE *buff,	/* 512 byte data block to be transmitted */
	BYTE token			/* Data token */
)
{
	BYTE resp;
	UINT bc = 512;


	if (wait_ready() != 0xFF) return 0;

	xmit_spi(token);		/* Xmit a token */
	if (token != 0xFD) {	/* Not StopTran token */
		do {						/* Xmit the 512 byte data block to the MMC */
			xmit_spi(*buff++);
			xmit_spi(*buff++);
		} while (bc -= 2);
		xmit_spi(0xFF);				/* CRC (Dummy) */
		xmit_spi(0xFF);
		resp = rcvr_spi();			/* Receive a data response */
		if ((resp & 0x1F) != 0x05)	/* If not accepted, return with error */
			return 0;
	}

	return 1;
}
#endif	/* _READONLY */



/*-----------------------------------------------------------------------*/
/* Send a command packet to MMC                                          */
/*-----------------------------------------------------------------------*/

static
BYTE send_cmd (
	BYTE cmd,		/* Command byte */
	DWORD arg		/* Argument */
)
{
	BYTE n, res;


	if (cmd & 0x80)
	{	/* ACMD<n> is the command sequense of CMD55-CMD<n> */
		cmd &= 0x7F;
//		res = send_cmd(CMD55, 0);
		/* Select the card and wait for ready */
		deselect();
		if (!select()) return 0xFF;
		/* Send command packet */
		xmit_spi(0x40 | CMD55);			/* Start + Command index */
		xmit_spi((BYTE)0);				/* Argument[31..24] */
		xmit_spi((BYTE)0);				/* Argument[23..16] */
		xmit_spi((BYTE)0);				/* Argument[15..8] */
		xmit_spi((BYTE)0);				/* Argument[7..0] */
		xmit_spi(0x01);					/* Dummy CRC + Stop */
	
		/* Receive command response */
		n = 10;							/* Wait for a valid response in timeout of 10 attempts */
		do
			res = rcvr_spi();
		while ((res & 0x80) && --n);
	
//		return res;			/* Return with the response value */
		if (res > 1) return res;
	}

	/* Select the card and wait for ready */
	deselect();
	if(!select())
	{
		return 0xFF;
	}
	/* Send command packet */
	xmit_spi(0x40 | cmd);			/* Start + Command index */
	xmit_spi((BYTE)(arg >> 24));	/* Argument[31..24] */
	xmit_spi((BYTE)(arg >> 16));	/* Argument[23..16] */
	xmit_spi((BYTE)(arg >> 8));		/* Argument[15..8] */
	xmit_spi((BYTE)arg);			/* Argument[7..0] */
	n = 0x01;						/* Dummy CRC + Stop */
	if (cmd == CMD0) n = 0x95;		/* Valid CRC for CMD0(0) */
	if (cmd == CMD8) n = 0x87;		/* Valid CRC for CMD8(0x1AA) */
	xmit_spi(n);
	/* Receive command response */
	if (cmd == CMD12) rcvr_spi();	/* Skip a stuff byte when stop reading */
	n = 20;							/* Wait for a valid response in timeout of 10 attempts */
	do
		res = rcvr_spi();
	while ((res & 0x80) && --n);

	return res;			/* Return with the response value */
}



/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE drv		/* Physical drive nmuber (0) */
)
{
	BYTE n, cmd, ty, ocr[4], res;

#ifdef	_DEBUG
	xprintf("INIT\r\n");
#endif
	if (drv) return STA_NOINIT;			/* Supports only single drive */
	if (Stat & STA_NODISK) return Stat;	/* No card in the socket */


	power_on();							/* Force socket power on */
	FCLK_SLOW();
	for(n = 0; n < 30; n++)
	{
		rcvr_spi();						/* 80 dummy clocks */
	}
//		xprintf("ST:%u\r\n", (unsigned int)m);

	ty = 0;
	if((res = send_cmd(CMD0, 0)) == 1)
	{									/* Enter Idle state */
#ifdef	_DEBUG
		xprintf("CMD0 \r\n");
#endif
//		Timer1 = 1000;					/* Initialization timeout of 1000 msec */
		Timer1_Set(1000);
		if(send_cmd(CMD8, 0x1AA) == 1)
		{								/* SDv2? */
#ifdef	_DEBUG
			xprintf("CMD8 \r\n");
#endif
			for(n = 0; n < 4; n++) ocr[n] = rcvr_spi();			/* Get trailing return value of R7 resp */
			if(ocr[2] == 0x01 && ocr[3] == 0xAA)
			{							/* The card can work at vdd range of 2.7-3.6V */
#ifdef	_DEBUG
				xprintf("Vdd OK \r\n");
#endif
				while(Timer1_ReadCounter()/* Timer1 */ && send_cmd(ACMD41, 0x40000000));	/* Wait for leaving idle state (ACMD41 with HCS bit) */
				if(Timer1_ReadCounter()/* Timer1 */ && send_cmd(CMD58, 0) == 0) {			/* Check CCS bit in the OCR */
					for(n = 0; n < 4; n++) ocr[n] = rcvr_spi();
					ty = (ocr[0] & 0x40) ? CT_SD2|CT_BLOCK : CT_SD2;	/* SDv2 */
				}
			}
		}
		else
		{								/* SDv1 or MMCv3 */
#ifdef	_DEBUG
			xprintf("!CMD8 ");
#endif
			if(send_cmd(ACMD41, 0) <= 1)
			{
				ty = CT_SD1; cmd = ACMD41;	/* SDv1 */
			}
			else
			{
				ty = CT_MMC; cmd = CMD1;	/* MMCv3 */
			}
			while(Timer1_ReadCounter()/* Timer1 */ && send_cmd(cmd, 0));		/* Wait for leaving idle state */
			if(!Timer1_ReadCounter()/* Timer1 */ || send_cmd(CMD16, 512) != 0)	/* Set read/write block length to 512 */
				ty = 0;
		}
	}
	CardType = ty;
	deselect();
	
//	xprintf("%u\r\n", 0, res);
	
	if(ty)
	{							/* Initialization succeded */
#ifdef	_DEBUG
		xprintf("NOINIT, %u\r\n", 0, res);
#endif
		Stat &= ~STA_NOINIT;	/* Clear STA_NOINIT */
		FCLK_FAST();
	}
	else
	{							/* Initialization failed */
		xprintf("FAILED!, %u\r\n", 0, res);
		power_off();
	}
#ifdef	_DEBUG
	xprintf("RETRN\r\n");
#endif
	return Stat;
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0) */
)
{
	if (drv) return STA_NOINIT;		/* Supports only single drive */
	return Stat;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,			/* Physical drive nmuber (0) */
	BYTE *buff,			/* Pointer to the data buffer to store read data */
	DWORD sector,		/* Start sector number (LBA) */
	UINT count			/* Sector count (1..128) */
)
{
	BYTE cmd;


	if (pdrv || !count) return RES_PARERR;
	if (Stat & STA_NOINIT) return RES_NOTRDY;

	if (!(CardType & CT_BLOCK)) sector *= 512;	/* Convert to byte address if needed */

	cmd = count > 1 ? CMD18 : CMD17;			/*  READ_MULTIPLE_BLOCK : READ_SINGLE_BLOCK */
	if (send_cmd(cmd, sector) == 0) {
		do {
			if (!rcvr_datablock(buff, 512)) break;
			buff += 512;
		} while (--count);
		if (cmd == CMD18) send_cmd(CMD12, 0);	/* STOP_TRANSMISSION */
	}
	deselect();

	return count ? RES_ERROR : RES_OK;
}

// ##### R0.09b #####
//DRESULT disk_read (
//	BYTE drv,		/* Physical drive nmuber (0) */
//	BYTE *buff,		/* Pointer to the data buffer to store read data */
//	DWORD sector,	/* Start sector number (LBA) */
//	BYTE count		/* Sector count (1..255) */
//)
//{
//	if (drv || !count) return RES_PARERR;
//	if (Stat & STA_NOINIT) return RES_NOTRDY;
//
//	if (!(CardType & CT_BLOCK)) sector *= 512;	/* Convert to byte address if needed */
//
//	if (count == 1) {		/* Single block read */
//		if ((send_cmd(CMD17, sector) == 0)	/* READ_SINGLE_BLOCK */
//			&& rcvr_datablock(buff, 512))
//			count = 0;
//	}
//	else {				/* Multiple block read */
//		if (send_cmd(CMD18, sector) == 0) {	/* READ_MULTIPLE_BLOCK */
//			do {
//				if (!rcvr_datablock(buff, 512)) break;
//				buff += 512;
//			} while (--count);
//			send_cmd(CMD12, 0);				/* STOP_TRANSMISSION */
//		}
//	}
//	deselect();
//
//	return count ? RES_ERROR : RES_OK;
//}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber (0) */
	const BYTE *buff,	/* Pointer to the data to be written */
	DWORD sector,		/* Start sector number (LBA) */
	UINT count			/* Sector count (1..128) */
)
{
	if (pdrv || !count) return RES_PARERR;
	if (Stat & STA_NOINIT) return RES_NOTRDY;
	if (Stat & STA_PROTECT) return RES_WRPRT;

	if (!(CardType & CT_BLOCK)) sector *= 512;	/* Convert to byte address if needed */

	if (count == 1) {	/* Single block write */
		if ((send_cmd(CMD24, sector) == 0)	/* WRITE_BLOCK */
			&& xmit_datablock(buff, 0xFE))
			count = 0;
	}
	else {				/* Multiple block write */
		if (CardType & CT_SDC) send_cmd(ACMD23, count);
		if (send_cmd(CMD25, sector) == 0) {	/* WRITE_MULTIPLE_BLOCK */
			do {
				if (!xmit_datablock(buff, 0xFC)) break;
				buff += 512;
			} while (--count);
			if (!xmit_datablock(0, 0xFD))	/* STOP_TRAN token */
				count = 1;
		}
	}
	deselect();

	return count ? RES_ERROR : RES_OK;
}
#endif

// ##### R0.09b #####
//#if _READONLY == 0
//DRESULT disk_write (
//	BYTE drv,				/* Physical drive nmuber (0) */
//	const BYTE *buff,		/* Pointer to the data to be written */
//	DWORD sector,			/* Start sector number (LBA) */
//	BYTE count				/* Sector count (1..255) */
//)
//{
//	if (drv || !count) return RES_PARERR;
//	if (Stat & STA_NOINIT) return RES_NOTRDY;
//	if (Stat & STA_PROTECT) return RES_WRPRT;
//
//	if (!(CardType & CT_BLOCK)) sector *= 512;	/* Convert to byte address if needed */
//
//	if (count == 1) {		/* Single block write */
//		if ((send_cmd(CMD24, sector) == 0)	/* WRITE_BLOCK */
//			&& xmit_datablock(buff, 0xFE))
//			count = 0;
//	}
//	else {				/* Multiple block write */
//		if (CardType & CT_SDC) send_cmd(ACMD23, count);
//		if (send_cmd(CMD25, sector) == 0) {	/* WRITE_MULTIPLE_BLOCK */
//			do {
//				if (!xmit_datablock(buff, 0xFC)) break;
//				buff += 512;
//			} while (--count);
//			if (!xmit_datablock(0, 0xFD))	/* STOP_TRAN token */
//				count = 1;
//		}
//	}
//	deselect();
//
//	return count ? RES_ERROR : RES_OK;
//}
//#endif /* _READONLY */



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive data block */
)
{
	DRESULT res;
	BYTE n, csd[16], *ptr = buff;
	DWORD csize;


	if (drv) return RES_PARERR;
	if (Stat & STA_NOINIT) return RES_NOTRDY;

	res = RES_ERROR;
	switch (ctrl) {
		case CTRL_SYNC :	/* Flush dirty buffer if present */
			if (select()) {
				deselect();
				res = RES_OK;
			}
			break;

		case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (WORD) */
			if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) {
				if ((csd[0] >> 6) == 1) {	/* SDv2? */
					csize = csd[9] + ((WORD)csd[8] << 8) + 1;
					*(DWORD*)buff = (DWORD)csize << 10;
				} else {					/* SDv1 or MMCv2 */
					n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
					csize = (csd[8] >> 6) + ((WORD)csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
					*(DWORD*)buff = (DWORD)csize << (n - 9);
				}
				res = RES_OK;
			}
			break;

		case GET_SECTOR_SIZE :	/* Get sectors on the disk (WORD) */
			*(WORD*)buff = 512;
			res = RES_OK;
			break;

		case GET_BLOCK_SIZE :	/* Get erase block size in unit of sectors (DWORD) */
			if (CardType & CT_SD2) {	/* SDv2? */
				if (send_cmd(ACMD13, 0) == 0) {		/* Read SD status */
					rcvr_spi();
					if (rcvr_datablock(csd, 16)) {				/* Read partial block */
						for (n = 64 - 16; n; n--) rcvr_spi();	/* Purge trailing data */
						*(DWORD*)buff = 16UL << (csd[10] >> 4);
						res = RES_OK;
					}
				}
			} else {					/* SDv1 or MMCv3 */
				if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) {	/* Read CSD */
					if (CardType & CT_SD1) {	/* SDv1 */
						*(DWORD*)buff = (((csd[10] & 63) << 1) + ((WORD)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
					} else {					/* MMCv3 */
						*(DWORD*)buff = ((WORD)((csd[10] & 124) >> 2) + 1) * (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
					}
					res = RES_OK;
				}
			}
			break;

		case MMC_GET_TYPE :		/* Get card type flags (1 byte) */
			*ptr = CardType;
			res = RES_OK;
			break;

		case MMC_GET_CSD :	/* Receive CSD as a data block (16 bytes) */
			if ((send_cmd(CMD9, 0) == 0)	/* READ_CSD */
				&& rcvr_datablock(buff, 16))
				res = RES_OK;
			break;

		case MMC_GET_CID :	/* Receive CID as a data block (16 bytes) */
			if ((send_cmd(CMD10, 0) == 0)	/* READ_CID */
				&& rcvr_datablock(buff, 16))
				res = RES_OK;
			break;

		case MMC_GET_OCR :	/* Receive OCR as an R3 resp (4 bytes) */
			if (send_cmd(CMD58, 0) == 0) {	/* READ_OCR */
				for (n = 0; n < 4; n++)
					*((BYTE*)buff+n) = rcvr_spi();
				res = RES_OK;
			}
			break;

		case MMC_GET_SDSTAT :	/* Receive SD statsu as a data block (64 bytes) */
			if (send_cmd(ACMD13, 0) == 0) {	/* SD_STATUS */
				rcvr_spi();
				if (rcvr_datablock(buff, 64))
					res = RES_OK;
			}
			break;

		default:
			res = RES_PARERR;
	}

	deselect();

	return res;
}



/*-----------------------------------------------------------------------*/
/* Device Timer Interrupt Procedure  (Platform dependent)                */
/*-----------------------------------------------------------------------*/
/* This function must be called in period of 1ms                         */

#if 0
void disk_timerproc (void)
{
	static WORD pv;
	WORD p;
	BYTE s;
	UINT n;


	n = Timer1;						/* 1000Hz decrement timer */
	if (n) Timer1 = --n;
	n = Timer2;
	if (n) Timer2 = --n;

	p = pv;
	pv = SOCKPORT & (SOCKWP | SOCKINS);	/* Sample socket switch */

	if (p == pv) {						/* Have contacts stabled? */
		s = Stat;

		if (p & SOCKWP)					/* WP is H (write protected) */
			s |= STA_PROTECT;
		else							/* WP is L (write enabled) */
			s &= ~STA_PROTECT;

		if (p & SOCKINS)				/* INS = H (Socket empty) */
			s |= (STA_NODISK | STA_NOINIT);
		else							/* INS = L (Card inserted) */
			s &= ~STA_NODISK;

		Stat = s;
	}
}
#endif
