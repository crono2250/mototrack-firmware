// #define _WDT_ENB
#define _ECHOBACK
#define _DEBUG
/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include <stdlib.h>
#include <string.h>
#include "project.h"
#include "main.h"
#include "sdc\diskio.h"
#include "sdc\ff.h"
#include "xprintf.h"
#include "SakuraIO\SakuraIOc.h"
#include "SakuraIO\SakuraIOc_spi.h"
#include "SakuraIO\spi_interface.h"
#include "minmea\minmea.h"

#define	DEV_DEBUG	0		// sci_recvln()受信メディア指定
#define	DEV_LTE	1
#define	DEV_GPS	2
#define	DEV_VMON	3

#define LED0_ON()		LED0_Write(1)
#define LED0_OFF()		LED0_Write(0)
//#define LED1_ON()		LED1_Write(1)
//#define LED1_OFF()		LED1_Write(0)
#define SDA_ON()		SD_Access_Write(1)
#define SDA_OFF()		SD_Access_Write(0)

#define BME_ACTIVE()	BME280_CSn_Write(0)
#define BME_IDLE()		BME280_CSn_Write(1)

#define SAKURA_ACTIVE() SakuraIO_WAKE_OUT_Write(1)
#define SAKURA_IDLE()    SakuraIO_WAKE_OUT_Write(0)

#define INDENT_SPACES "  "

#define TimerValue_WORK 588
#define TimerValue_Sleep    529200

//----------------------------------------------------------------
// 変数定義
//----------------------------------------------------------------

reg8 SD_DET_FLAG = 0x00;
//uint16 buff;

 // for str2dec
reg8 DEC_count = 0x00;						// デコード桁数

 // DEBUG UART
reg8 SCI_STR_EXIST = 0x00;					// 行データがある場合に"1"
reg8 SCI_BYTES_READ = 0x00;					// 受信データサイズ(ワーク)
reg8 SCI_DATASIZE = 0x00;					// 受信データサイズ
reg8 SCI_RX_DAT[SCI_RX_SIZE] = {0x00};		// 受信データ

// GPS
reg8 GPS_STR_EXIST = 0x00;					// 行データがある場合に"1"
reg8 GPS_BYTES_READ = 0x00;					// 受信データサイズ(ワーク)
reg8 GPS_DATASIZE = 0x00;					// 受信データサイズ
char GPS_RX_DAT[GPS_RX_SIZE] = {0x00};		// GPS受信データ

char line[MINMEA_MAX_LENGTH];
char GPSDATA[10][80];
uint8 hoge_GPSDATA_COUNT = 0;

struct minmea_sentence_rmc rmc_frame;
struct minmea_sentence_gga gga_frame;
struct minmea_sentence_gst gst_frame;
struct minmea_sentence_vtg vtg_frame;
struct minmea_sentence_gsa gsa_frame;
struct minmea_sentence_gll gll_frame;

struct minmea_sentence_gsv gsv_frame;
struct minmea_sentence_zda zda_frame;

// Real Time Clock functions
DWORD get_fattime(void);
static FRESULT scan_files(char *) CYREENTRANT;

// BME280
uint8   BME_INIT_DONE = 0;

// Work registers
FILINFO Finfo;
DWORD acc_size;			/* Work register for fs command */
WORD acc_files, acc_dirs;
	// fatfs related variables
	FATFS fatfs, *fs;
	DIR rootdir;
	FILINFO fi;
	FIL file;
	unsigned int i = 0, j = 0;
	static char buf[128];
	unsigned char b, res;
	unsigned int w1, write_bytes;
	unsigned long p2;
uint32_t	TickTimer1Hz	=	0;
uint32_t    TickValue   =   0;

uint8	PERI_SPIM_READBUF[16] = {0x00};			// TCデータ受信バッファ
uint8	PERI_SPIM_SENDBUF[16] = {0x00};

uint8   sakura_io_INIT_DONE = 0;

//----------------------------------------------------------------
// Interrupt (100mSec_Tick)

CY_ISR(isr_Tick_Timer_isr) {
//    LED1_Write(LED1_Read()?(0):(1));

// SDC マウント処理
//	if(!SD_DET_Read() && !SD_DET_FLAG)			// カードがスロットに挿入され、かつ前ステップではSDマウントされていない場合
    if(!SD_DET_FLAG)			// カードがスロットに挿入され、かつ前ステップではSDマウントされていない場合
	{
//		res = f_mount(0, &fatfs);				// for R0.09b
		res = f_mount(&fatfs, "", 1);			// Mount drive #0
		if(res == FR_OK)
			res = f_opendir(&rootdir, "");		// Open root directory
		if(res == FR_OK)
		{
			fs = &fatfs;
//			xputs("SDC_Mounted\n");
			SD_DET_FLAG = 1;					// Mounted
//			LED0_ON();
		}
//	} else {
////		if(SD_DET_Read() && SD_DET_FLAG)		// カードがスロットから抜去され、かつ前ステップでSDマウントされている場合
//		if(SD_DET_FLAG)		// カードがスロットから抜去され、かつ前ステップでSDマウントされている場合
//        {
//			f_close(&file);
////			res = f_mount(0, NULL);				// for R0.09b
//			res = f_mount(NULL, "", 1);
//			if(res == FR_OK)
//			{
////				xputs("SDC_Unmounted\n");
//				SD_DET_FLAG = 0;				// UnMounted
//				LED0_OFF();
//			}
//		}
	}
    
    if(TickTimer1Hz == TickValue)  // 1秒
    {
        TickTimer1Hz = 0;
        if(sakura_io_INIT_DONE==1)
        {
//            xputs("Send ");
            SAKURA_IO_DataSend();   // GPS情報をsakura.ioに送信
//            LED0_Write(LED0_Read()?(0):(1));
        }
    } else
    {
        TickTimer1Hz++;
    }

    if(gsa_frame.fix_type == 3)     // 3D FixしたらLED点灯
    {
        LED0_ON();
    } else
    {
        LED0_OFF();
    }
    
    
}

//----------------------------------------------------------------
// Interrupt (574_Rx Interrupt)

CY_ISR(isr_UART_DEBUG_RXI_isr) {
	uint8 d;

    xfunc_out = UART_DEBUG_PutChar;		/* xprintf出力デバイス指定(UART使用時) */

	for(d=0;d==0;)
	{
		d = UART_DEBUG_GetChar();

		if(d==0){									// データなし(null)
			return;
		}
		else if(d == '\r'){								// 改行(CR)が来た
			SCI_RX_DAT[SCI_BYTES_READ] = '\0';		// Nullをエンドコードとして書く。
			SCI_DATASIZE = SCI_BYTES_READ;
			SCI_BYTES_READ = 0;						// 次回受信に備え受信データサイズをクリア
			SCI_STR_EXIST = 1;						// データありますフラグを立てる
#ifdef	_ECHOBACK
			xputs("\r");	// エコーバック(改行)
#endif
		}
		else if(d == '\b'){							// バックスペース(BS)が来た
			if(SCI_BYTES_READ > 0){					// バッファにデータが有れば
#ifdef	_ECHOBACK
				xputs("\b \b");						// 画面上でもデータ消去
#endif
				SCI_BYTES_READ--;					// バッファのデータも1つ減らす
			}
		}
		else if(SCI_BYTES_READ >= SCI_RX_SIZE){		// 受信サイズがバッファサイズを超える
#ifdef	_ECHOBACK
			xputs("Size Over\n");					// データ来すぎ！
#endif
			SCI_BYTES_READ = 0;						// 次回受信に備え受信データサイズをクリア
		}
		else if(d >= 0x20 && d <= 0x7E){			// 文字(英数字)が来た
			SCI_RX_DAT[SCI_BYTES_READ] = d;			// そのままデータとして積む。
			SCI_BYTES_READ++;
#ifdef	_ECHOBACK
			xprintf("%c", d);	// echo back
#endif
		}
	}	
	return;
}

//----------------------------------------------------------------
// Interrupt (GPS_Rx Interrupt)

CY_ISR(isr_UART_GPS_RXI_isr) {
	uint8 d,i;

//    xfunc_out = UART_GPS_PutChar;		/* xprintf出力デバイス指定(UART使用時) */
if(GPS_STR_EXIST == 0)
{
	for(d=0;d==0;)
	{
		d = UART_GPS_GetChar();

		if(d==0){									// データなし(null)
			return;
		}
		else if(d == '\r'){								// 改行(CR)が来た
			GPS_RX_DAT[GPS_BYTES_READ] = '\0';		// Nullをエンドコードとして書く。
			GPS_DATASIZE = GPS_BYTES_READ;
			GPS_BYTES_READ = 0;						// 次回受信に備え受信データサイズをクリア
			GPS_STR_EXIST = 1;						// データありますフラグを立てる
//#ifdef	_ECHOBACK
//			xputs("\r");	// エコーバック(改行)
//#endif
		}
		else if(GPS_BYTES_READ >= GPS_RX_SIZE){		// 受信サイズがバッファサイズを超える
//#ifdef	_ECHOBACK
//			xputs("Size Over\n");					// データ来すぎ！
//#endif
			GPS_BYTES_READ = 0;						// 次回受信に備え受信データサイズをクリア
		}
		else if(d >= 0x20 && d <= 0x7E){			// 文字(英数字)が来た
			GPS_RX_DAT[GPS_BYTES_READ] = d;			// そのままデータとして積む。
			GPS_BYTES_READ++;
//#ifdef	_ECHOBACK
//			xprintf("%c", d);	// echo back
//#endif
		}
	}	

    if(GPS_STR_EXIST == 1)
    {
        GPS_PARSE();
        for(i=0; i<=MINMEA_MAX_LENGTH; i++)
        {
            GPS_RX_DAT[i] = 0x0;
        }
        GPS_STR_EXIST = 0;
    }
}
    return;
}


int main(void)
{
    
UART_DEBUG_Start();         // デバッグ用UARTポート
UART_GPS_Start();           // GPS
UART_VMON_Start();          // 電圧モニタ

//xfunc_in = UART_1_GetChar;		/* xgets入力デバイス指定 */
xfunc_out = UART_DEBUG_PutChar;		/* xprintf出力デバイス指定(UART使用時) */
//xfunc_out = USBUART_putc;		/* xprintf出力デバイス指定(USB_UART使用時) */

// Initialize SPI (used to communicate with SD card) running at 1 MHz
SD_SPIM_Init();
SD_SPIM_Enable();
SD_SPIM_EnableInt();
SD_SPIM_Start();

PERI_SPIM_Init();
PERI_SPIM_Enable();
PERI_SPIM_EnableInt();
PERI_SPIM_Start();

SakuraIOc_hw_init();

isr_Tick_Timer_StartEx(isr_Tick_Timer_isr);
isr_UART_DEBUG_RXI_StartEx(isr_UART_DEBUG_RXI_isr);
isr_UART_GPS_RXI_StartEx(isr_UART_GPS_RXI_isr);
//isr_UART_VMON_RXI_StartEx(isr_UART_VMON_RXI_isr);

CyGlobalIntEnable; /* Enable all interrupts by the processor. */

DEF_Check();
SD_CSn_Write(0);

SAKURA_ACTIVE();        // LTE電源ON

uint8_t count = 0;
CyDelay(1000);
xputs("Sakura.io Init...\n");
while((getConnectionStatus() & 0x80) != 0x80)
{
//    xprintf("SakuraIO_Stat = %02X\n" ,getConnectionStatus());
    xprintf("%d ", count);
    count++;
    CyDelay(1000);
}
sakura_io_INIT_DONE = 1;
xputs("Sakura.io Init_Done\n");

SAKURA_SPIM_ClearRxBuffer();
CyDelay(500);

uint32_t time=0;
time = getUnixtime() / 1000;
xprintf("UnixTime:%ld\n", time);



#ifdef	_WDT_ENB
	CyWdtStart(CYWDT_1024_TICKS,CYWDT_LPMODE_NOCHANGE);
//	CyWdtStart(CYWDT_4096_TICKS,CYWDT_LPMODE_NOCHANGE);
#endif

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    for(;;)
    {
        ACC_Sleep_FUNC();
//#ifdef	_DEBUG
//		xprintf("DataSize = %u\n",SCI_DATASIZE);	// データ数出力
//#endif
        if (SCI_STR_EXIST == 1)
        {
    		if(SCI_DATASIZE == 0){
    			xputs("ERROR\n");						// データ無し
    		} else
    		{
    			switch(SCI_RX_DAT[0])
    			{
    				case 'm':							// SDカードマウント
    				case 'M':
//#ifdef	_DEBUG
//					xputs("SDC_DET\n");
//#endif
    											// SDカード情報取得
    					if(disk_ioctl(0, GET_SECTOR_COUNT, &p2) == RES_OK)
    					{
    						xprintf("Drive: %lu scts\r\n", p2);
    					}
    					if(disk_ioctl(0, GET_SECTOR_SIZE, &w1) == RES_OK)
    					{
    						xprintf("Sector size: %u\r\n", w1);
    					}
    					if(disk_ioctl(0, GET_BLOCK_SIZE, &p2) == RES_OK)
    					{
    						xprintf("Erase block: %lu sectors\r\n", p2);
    					}
    					if(disk_ioctl(0, MMC_GET_TYPE, &b) == RES_OK)
    					{
    						xprintf("MMC/SDC type: %u\r\n", (unsigned int)b);
    					}

    //							fs = &fatfs;
    						acc_files = 0;
    						acc_size = 0;
    						acc_dirs = 0;
    						res = scan_files("");						// Scan files stored at the root directory in the SD card
    					xprintf("%u files, %lu bytes.\r\n%u folders.\r\n",
    							acc_files, acc_size, acc_dirs);
    					res = f_stat("test01.txt", &fi);				// Get status of the file
    					if(res == FR_OK)
    					{
    						xprintf("%s\r\n", fi.fname);
    					}

    					file.fs = &fatfs;
    					res = f_open(&file, "test01.txt", FA_READ);		// Open file

    					if(res == FR_OK)
    					{
    						xprintf("%lu bytes\r\n", file.fsize);
    						while(1)
    						{
    							f_read(&file, (char *)buf, 120, &i);
    							for(j = 0; j < i; j++)
    								xprintf("%c", (unsigned int)buf[j]);
    							if(i < 120) break;
    						}
    						xprintf("\r\nEnd of File\r\n");
    						f_close(&file);
    					}
    					else
    					{
    						xprintf("Failed to Open\r\n");
    						f_close(&file);
    					}
    				break;

    				case 'a':							// SDカードテストアクセス(既存ファイルへデータ追記)
    				case 'A':
//#ifdef	_DEBUG
//    						xputs("SDC_DET\n");
//#endif
    						res = f_stat("test01.txt", &fi);		// Get status of the file
    						if(res == FR_OK)
    						{
    							xprintf("%s\r\n", fi.fname);
    						}
    						
    						res = f_open(&file, "test01.txt", FA_OPEN_ALWAYS | FA_WRITE);		// Open file
    						if(res == FR_OK)
    						{
    							res = f_lseek(&file, f_size(&file));
    							if(res == FR_OK){
    								write_bytes = f_putc('C', &file);
    								f_sync(&file);
    									xprintf("write:%dBytes\n", write_bytes);

    								write_bytes = f_printf(&file, "%s", "fatfs_WriteTest\n");
    								f_sync(&file);
    									xprintf("write:%dBytes\n", write_bytes);

    								write_bytes = f_puts("Test_Write\n", &file);
    								f_sync(&file);
    									xprintf("write:%dBytes\n", write_bytes);
    							}
    							res = f_close(&file);
    							if(res == FR_DISK_ERR) xprintf("FR_DISK_ERR\n");
    						} else {
    							xprintf("Failed to Open\n");
    						}
    					break;

    				case 'r':							// BME280生データ取得部テスト
    				case 'R':
							BME_READ();
					break;

                    case 's':							// Sakura.io データ送信テスト
    				case 'S':
							SAKURA_IO_DataSend();   // GPS情報をsakura.ioに送信
					break;

    				default:
    					xputs("ERROR\n");
    					break;
    			}
    		}
    		SCI_STR_EXIST = 0;							// データありますフラグを下ろす
        }
    }
}

//----------------------------------------------------------------
// ASCII配列データ→int変数化
//　やりたい：UARTから得たコマンド列より、引数で指定した開始バイトから
//　　　　　　終了文字(\r or \0 or space)までの値を数値として返す。
//　　　　　　得たデータは、戻り値として返す。
//----------------------------------------------------------------
uint16 str2dec(uint16 start_byte)
{
	uint16	i, j, wk;
	uint16	dec_num = 0;
	int16	end_byte = 0;
	char8	num_dat[str2dec_buf_size] = {0x00};

	for (i=start_byte;i<=str2dec_buf_size;i++)
	{
		if(SCI_RX_DAT[i] == '\r' || SCI_RX_DAT[i] == '\0' || SCI_RX_DAT[i] == ' ')
		{
			end_byte = i - 1;
			break;
		}
	}

	if(end_byte == start_byte){
		num_dat[0] = SCI_RX_DAT[start_byte];
		dec_num = atoi(num_dat);			// 固定値0として処理
		DEC_count = 1;
		return dec_num;

	} else if(end_byte == -1)				// いきなり終わった
	{
		return 0;
	}

	DEC_count = i - start_byte;				// それ以外の時は何文字来たかカウント

	if(end_byte < DEC_count)
	{
		wk = 0;
	} else
	{
		wk = (end_byte - DEC_count);
	}

	if (wk == 0)
	{
		for(i=0;i<=end_byte;i++)
		{
			num_dat[i] = SCI_RX_DAT[i];
		}
	} else
	{
		j = 0;
		for(i=wk;i<=end_byte;i++)
		{
			num_dat[j] = SCI_RX_DAT[i+1];
			j++;
		}
	}


	dec_num = atoi(num_dat);
	return dec_num;
}

void DEF_Check(void)
{
	if (SCI_RX_SIZE > str2dec_buf_size)
	{
		xputs("[ ERR ] str2dec_buf_size too small.\n");
	}
}

//------------------------------------------------------------
// EEPROMワード書き込み
// やりたいこと： 任意アドレスから2Byteの書き込みを行う。
// ret = EEPROM_Write_Word(uint16 dat,adress)   // こんな感じで使う。
//------------------------------------------------------------
uint8 EEPROM_Write_Word(uint16 dat, uint16 adr)
{
	uint16 buf = 0;
	uint8 ret = 0xFF;

	EEPROM_UpdateTemperature();

	buf = dat>>8;
	ret = EEPROM_WriteByte(buf , adr);

	if(ret != CYRET_SUCCESS){
		return ret;
	}

	buf = dat & 0xFF;
	ret = EEPROM_WriteByte(buf , adr+1);

	if(ret != CYRET_SUCCESS){
		return ret;
	}

	return ret;
}

//------------------------------------------------------------
// EEPROMワード読み込み
// やりたいこと： 任意アドレスから2Byteの読み込みを行う。
// dat = EEPROM_Read_Word(adress)   // こんな感じで使う。
//------------------------------------------------------------
uint16 EEPROM_Read_Word(uint16 adr)
{
	uint16 buf = 0;
	uint16 dat = 0;

	buf = EEPROM_ReadByte(adr);
	dat = buf<<8;

	buf = EEPROM_ReadByte(adr+1);
	dat = dat + (buf & 0xFF);

//    xprintf("##DBG EE_ReadWord: %04X - %04X\n",adr,dat);
	return dat;
}

//------------------------------------------------------------
// EEPROMワード書き込み
// やりたいこと： 任意アドレスから3Byteの書き込みを行う。
// ret = EEPROM_Write_TByte(uint32 dat,adress)   // こんな感じで使う。
//------------------------------------------------------------
uint8 EEPROM_Write_TByte(uint32 dat, uint16 adr)
{
	uint16 buf = 0;
	uint8 ret = 0xFF;

	EEPROM_UpdateTemperature();

	buf = dat>>16;
	ret = EEPROM_WriteByte(buf , adr);

	if(ret != CYRET_SUCCESS){
		return ret;
	}

    buf = dat>>8 & 0xFF;
	ret = EEPROM_WriteByte(buf , adr+1);

	if(ret != CYRET_SUCCESS){
		return ret;
	}
    
	buf = dat & 0xFF;
	ret = EEPROM_WriteByte(buf , adr+2);

	if(ret != CYRET_SUCCESS){
		return ret;
	}

	return ret;
}

//------------------------------------------------------------
// EEPROMワード読み込み
// やりたいこと： 任意アドレスから2Byteの読み込みを行う。
// dat = EEPROM_Read_TByte(adress)   // こんな感じで使う。
//------------------------------------------------------------
uint32 EEPROM_Read_TByte(uint16 adr)
{
	uint16 buf = 0;
	uint32 dat = 0;

	buf = EEPROM_ReadByte(adr);
	dat = buf<<16;

	buf = EEPROM_ReadByte(adr+1);
	dat = dat + ((buf & 0xFF)<<8);

	buf = EEPROM_ReadByte(adr+2);
	dat = dat + (buf & 0xFF);

//    xprintf("##DBG EE_ReadTByte: %04X - %04X\n",adr,dat);
	return dat;
}



////----------------------------------------------------------------
//// EEPROM全域イレース
////----------------------------------------------------------------
//void clreeprom(void)
//{
//	uint16 i,j;
//
//	for(i=0;i<Monitor_Channel;i++)
//	{
//        	EEPROM_Write_Word(NG , VCAL_CH1_FG + i*2);
//            CyWdtClear();
//
//       	for(j=0;j<VCAL_POINT;j++)
//        {
//            EEPROM_Write_TByte(0, (VCAL_CH1V_P0 + ((Monitor_Channel * ADC_Data_Size) * j) + i * ADC_Data_Size));
//            CyWdtClear();
//    		EEPROM_Write_TByte(0, (VCAL_CH1D_P0 + ((Monitor_Channel * ADC_Data_Size) * j) + i * ADC_Data_Size));
//            CyWdtClear();
//    	}
//    }
//}
//
////----------------------------------------------------------------
//// *EEPROMデータダンプ
//// 表示内容は調整の必要あり
////----------------------------------------------------------------
//void eedump(void)
//{
//	uint16 i,j;
//    uint32 wk = 0;
//
//		xputs("CAL Flag : ");
//		for (i=0; i<Monitor_Channel; i++)
//		{
//            wk = EEPROM_Read_Word(VCAL_CH1_FG + i*2);
//			xprintf("%X ",wk);
//			if(i==7||i==15||i==23)
//			{
//				xputs("\n");
//			}
//		}
//        xputs("\n");
//
//		for (i=0; i<Monitor_Channel; i++)
//		{
//       		xprintf("CAL Data for ch%2d :\n",i+1);
//            for (j=0; j<VCAL_POINT; j++)
//			{
//				wk = EEPROM_Read_TByte(VCAL_CH1V_P0 + ((Monitor_Channel * ADC_Data_Size) * j) + i * ADC_Data_Size);
//				xprintf("  %4d ",wk);
//			}
//				xputs("\n");
//
//			for (j=0; j<VCAL_POINT; j++)
//			{
//				wk = EEPROM_Read_TByte(VCAL_CH1D_P0 + ((Monitor_Channel * ADC_Data_Size) * j) + i * ADC_Data_Size);
//				xprintf("  %04X ",wk);
//			}
//				xputs("\n");
//		}
//}

//----------------------------------------------------------------
// SPI Data RW
//----------------------------------------------------------------
void PERI_SPIM_RW(uint8 size)
{
	uint8 i;

	PERI_SPIM_ClearRxBuffer();

	for(i=0; i<=size; i++)
	{
		PERI_SPIM_WriteTxData(PERI_SPIM_SENDBUF[i]);
		while(!(PERI_SPIM_ReadTxStatus() & PERI_SPIM_STS_SPI_DONE)){};
        PERI_SPIM_READBUF[i] = PERI_SPIM_ReadRxData();
	}

//	if(!(SPI_TC_GetRxBufferSize()))
//	{
//		xputs("SPI_Buf is Empty\n");
//	} else
//	{
//		xputs("SPI_Buf is NOT Empty\n");
//	}

	return;
}

//----------------------------------------------------------------
// BME280 Read
//----------------------------------------------------------------
void BME_READ(void)
{
	uint8 i, BME_TRANS_SIZE;
 
    if(BME_INIT_DONE == 0)
    {
#ifdef	_DEBUG
		xputs("BME_INIT\n");
#endif
		BME_ACTIVE();
			CyDelay(1);
			PERI_SPIM_SENDBUF[0] = (BME280_WRITE + (BME280_CTRL_HUMI_REG & 0x7F)); // Register Write
			PERI_SPIM_SENDBUF[1] = 0x01;	// Humidity oversampling x1

			PERI_SPIM_SENDBUF[2] = (BME280_WRITE + (BME280_CTRL_MEAS_REG & 0x7F)); // Register Write
			PERI_SPIM_SENDBUF[3] = 0x27;	//Temp oversampl x1, Press oversampl x1, Normal mode
			
			PERI_SPIM_SENDBUF[4] = (BME280_WRITE + (BME280_CONFIG_REG & 0x7F)); // Register Write
			PERI_SPIM_SENDBUF[5] = 0xA8;	// Standby 1000ms, Filter coef 4
			PERI_SPIM_RW(5);
			CyDelay(1);
		BME_IDLE();
        BME_INIT_DONE = 1;
    }

	BME_TRANS_SIZE = (BME280_HUMI_LSB_REG - BME280_STAT_REG);

    PERI_SPIM_SENDBUF[0] = (BME280_READ + (BME280_STAT_REG & 0x7F));
	for(i=1;i<=BME_TRANS_SIZE;i++)
	{
		PERI_SPIM_SENDBUF[i] = 0xFF;
	}
	BME_ACTIVE();
		CyDelay(1);
		PERI_SPIM_RW(1 + BME_TRANS_SIZE);
		CyDelay(1);
	BME_IDLE();

#ifdef	_DEBUG
		xputs("F3,F4,F5,F6,F7,F8,F9,FA,FB,FC,FD,FE\n");
		for(i=1;i<=(BME_TRANS_SIZE+1);i++)
		{
			xprintf("%02X,",PERI_SPIM_READBUF[i]);	// 取得データ表示
		}
        xputs("\b \b\n\n");
#endif

	return;
}

void GPS_PARSE(void)
{
//--------------
// NMEA Parce
//--------------
// char line[MINMEA_MAX_LENGTH];
//while (xfgets(UART_GPS_GetChar, line, sizeof(line)) != 0) {
//        xprintf("%s", line);

    uint8 i,j;

    if(hoge_GPSDATA_COUNT < 8)
    {
        for(i=0; i<80; i++)
        {
            GPSDATA[hoge_GPSDATA_COUNT][i] = GPS_RX_DAT[i];
            GPS_RX_DAT[i] = 0x0;
        }
        hoge_GPSDATA_COUNT++;
    } else
    {
//        xputs("###GPSLOG_captured\n");
        hoge_GPSDATA_COUNT = 0;
    }

    for(i=0; i<8; i++)
    {
//        xprintf("i = %d\n", i);
        for(j=0; j<MINMEA_MAX_LENGTH; j++)
        {
            line[j] = GPSDATA[i][j];
            GPSDATA[i][j] = 0x0;
        }
        
        switch (minmea_sentence_id(line, false)) {
            case MINMEA_SENTENCE_RMC: {
//                struct minmea_sentence_rmc frame;
                if (minmea_parse_rmc(&rmc_frame, line)) {
//                    xprintf("$xxRMC: raw coordinates and speed: (%ld/%ld,%ld/%ld) %ld/%ld\n",
//                            rmc_frame.latitude.value, rmc_frame.latitude.scale,
//                            rmc_frame.longitude.value, rmc_frame.longitude.scale,
//                            rmc_frame.speed.value, rmc_frame.speed.scale);
//                    xprintf( "$xxRMC fixed-point coordinates and speed scaled to three decimal places: (%ld,%ld) %ld\n",
//                            minmea_rescale(&rmc_frame.latitude, 1000),
//                            minmea_rescale(&rmc_frame.longitude, 1000),
//                            minmea_rescale(&rmc_frame.speed, 1000));
//                    xprintf( "$xxRMC floating point degree coordinates and speed: (%f,%f) %f\n",
//                            minmea_tocoord(&rmc_frame.latitude),
//                            minmea_tocoord(&rmc_frame.longitude),
//                            minmea_tofloat(&rmc_frame.speed));
                }
                else {
                    xputs( "$xxRMC sentence is not parsed\n");
                }
            } break;

            case MINMEA_SENTENCE_GGA: {
//                struct minmea_sentence_gga frame;
                if (minmea_parse_gga(&gga_frame, line)) {
//                    xprintf( "$xxGGA: fix quality: %d\n", gga_frame.fix_quality);
                }
                else {
                    xputs( "$xxGGA sentence is not parsed\n");
                }
            } break;

            case MINMEA_SENTENCE_GST: {
//                struct minmea_sentence_gst frame;
                if (minmea_parse_gst(&gst_frame, line)) {
//                    xprintf( "$xxGST: raw latitude,longitude and altitude error deviation: (%d/%d,%d/%d,%d/%d)\n",
//                            gst_frame.latitude_error_deviation.value, gst_frame.latitude_error_deviation.scale,
//                            gst_frame.longitude_error_deviation.value, gst_frame.longitude_error_deviation.scale,
//                            gst_frame.altitude_error_deviation.value, gst_frame.altitude_error_deviation.scale);
//                    xprintf( "$xxGST fixed point latitude,longitude and altitude error deviation"
//                           " scaled to one decimal place: (%d,%d,%d)\n",
//                            minmea_rescale(&gst_frame.latitude_error_deviation, 10),
//                            minmea_rescale(&gst_frame.longitude_error_deviation, 10),
//                            minmea_rescale(&gst_frame.altitude_error_deviation, 10));
//                    xprintf( "$xxGST floating point degree latitude, longitude and altitude error deviation: (%f,%f,%f)",
//                            minmea_tofloat(&gst_frame.latitude_error_deviation),
//                            minmea_tofloat(&gst_frame.longitude_error_deviation),
//                            minmea_tofloat(&gst_frame.altitude_error_deviation));
                }
                else {
                    xputs( "$xxGST sentence is not parsed\n");
                }
            } break;

            case MINMEA_SENTENCE_GSA: {
//               struct minmea_sentence_gsa frame;
               if (minmea_parse_gsa(&gsa_frame, line)) {
//                    xprintf( "$xxGSA: fix_type = %d\n",
//                           gsa_frame.fix_type);
//                    xprintf( "        PDOP = %d/%d\n",
//                           gsa_frame.pdop.value, gsa_frame.pdop.scale);
//                    xprintf( "        HDOP = %d/%d\n",
//                           gsa_frame.hdop.value, gsa_frame.hdop.scale);
//                    xprintf( "        VDOP = %d/%d\n",
//                           gsa_frame.vdop.value, gsa_frame.vdop.scale);
               }
               else {
                    xputs( "$xxGSA sentence is not parsed\n");
               }
            } break;

            case MINMEA_SENTENCE_GLL: {
//               struct minmea_sentence_gll frame;
               if (minmea_parse_gll(&gll_frame, line)) {
//                    xprintf( "$xxGLL: Latitude = %f\n",
//                           gll_frame.latitude.value, gll_frame.latitude.scale);
//                    xprintf( "        Longitude = %f\n",
//                           gll_frame.longitude.value, gll_frame.longitude.scale);
//                    xprintf( "        time = %02d:%02d:%02d.%02d\n",
//                           gll_frame.time.hours, gll_frame.time.minutes, gll_frame.time.seconds, gll_frame.time.microseconds);
//                    if(gll_frame.status == 'A')
//                    {
//                        xputs("        status = VALID\n");
//                    } else
//                    {
//                        xputs("        status = NOT_VALID\n");
//                    }
               }
               else {
                    xputs( "$xxGLL sentence is not parsed\n");
               }
            } break;

//            case MINMEA_SENTENCE_GSV: {
//                struct minmea_sentence_gsv frame;
//                if (minmea_parse_gsv(&gsv_frame, line)) {
//                    xprintf( "$xxGSV: message %d of %d\n", gsv_frame.msg_nr, gsv_frame.total_msgs);
//                    xprintf( "$xxGSV: sattelites in view: %d\n", gsv_frame.total_sats);
//                    for (int i = 0; i < 4; i++)
//                        xprintf( "$xxGSV: sat nr %d, elevation: %d, azimuth: %d, snr: %d dbm\n",
//                            gsv_frame.sats[i].nr,
//                            gsv_frame.sats[i].elevation,
//                            gsv_frame.sats[i].azimuth,
//                            gsv_frame.sats[i].snr);
//                }
//                else {
//                    xputs( "$xxGSV sentence is not parsed\n");
//                }
//            } break;

            case MINMEA_SENTENCE_VTG: {
//               struct minmea_sentence_vtg frame;
               if (minmea_parse_vtg(&vtg_frame, line)) {
//                    xprintf( "$xxVTG: true track degrees = %f\n",
//                           minmea_tofloat(&vtg_frame.true_track_degrees));
//                    xprintf( "        magnetic track degrees = %f\n",
//                           minmea_tofloat(&vtg_frame.magnetic_track_degrees));
//                    xprintf( "        speed knots = %f\n",
//                            minmea_tofloat(&vtg_frame.speed_knots));
//                    xprintf( "        speed kph = %f\n",
//                            minmea_tofloat(&vtg_frame.speed_kph));
               }
               else {
                    xputs( "$xxVTG sentence is not parsed\n");
               }
            } break;

//            case MINMEA_SENTENCE_ZDA: {
//                struct minmea_sentence_zda frame;
//                if (minmea_parse_zda(&zda_frame, line)) {
//                    xprintf( "$xxZDA: %d:%d:%d %02d.%02d.%d UTC%+03d:%02d\n",
//                           zda_frame.time.hours,
//                           zda_frame.time.minutes,
//                           zda_frame.time.seconds,
//                           zda_frame.date.day,
//                           zda_frame.date.month,
//                           zda_frame.date.year,
//                           zda_frame.hour_offset,
//                           zda_frame.minute_offset);
//                }
//                else {
//                    xputs( "$xxZDA sentence is not parsed\n");
//                }
//            } break;

            case MINMEA_INVALID: {
//                xputs( "$xxxxx sentence is not valid\n");
            } break;

            default: {
//                xputs( "$xxxxx sentence is not parsed\n");
            } break;
        }
      }
//    }

//--------------
}

//----------------------------------------------------------------
// Sakura.io DataSend
//----------------------------------------------------------------
void SAKURA_IO_DataSend(void)
{
//	uint8_t i;
    uint32_t Time = 0;

    uint8_t avail;
    uint8_t queued;

    
    if(TickTimer1Hz == TimerValue_Sleep)    //スリープモード時は起こして、送信して、寝かす。
    {
        SAKURA_ACTIVE();    //sakura.io モデムON
        CyDelay(2000);
        while((getConnectionStatus() & 0x80) != 0x80){}
    } else
    {
        if(SakuraIO_WAKE_IN_Read() == 0x0)  // スリープモード
        SAKURA_ACTIVE();
        CyDelay(2000);
        while((getConnectionStatus() & 0x80) != 0x80){}
    }

    if(gsa_frame.fix_type != 1)     // GPS未Fixの時は送信しない
    {
//        xputs("Sakura_IO Send\n");
        enqueueTx_ui32(0,1);
        enqueueTx_f(1,minmea_tocoord(&gll_frame.longitude));
        enqueueTx_f(2,minmea_tocoord(&gll_frame.latitude));
        enqueueTx_ui32(3,gsa_frame.fix_type);
        Time = getUnixtime() / 1000;
        enqueueTx_ui32(4,Time);
        send();
        xprintf("SakuraIO_DataSend : %02d:%02d:%02d\n", gll_frame.time.hours+9, gll_frame.time.minutes, gll_frame.time.seconds);
    }

    if(TickTimer1Hz == TimerValue_Sleep)    //スリープモード設定の場合
    {
        do
        {
            getTxQueueLength(&avail, &queued);
        } while(queued != 0);
        CyDelay(200);
        SAKURA_IDLE();
    }
    
	return;
}

//----------------------------------------------------------------
// ACC_Sleep Function
//----------------------------------------------------------------
void ACC_Sleep_FUNC(void)
{
//	uint8_t i;
    uint32_t Time = 0;

    if(ACC_Sleep_Read() == 0)     // ACCがONの時
    {
        TickValue = TimerValue_WORK;
    } else
    {
        TickValue = TimerValue_Sleep;
    }

	return;
}

/*---------------------------------------------------------*/
/* User Provided RTC Function for FatFs module             */
/*---------------------------------------------------------*/
/* This is a real time clock service to be called from     */
/* FatFs module. Any valid time must be returned even if   */
/* the system does not support an RTC.                     */
/* This function is not required in read-only cfg.         */

DWORD get_fattime(void)
{
	DWORD tmr;


//	_DI();
	/* Pack date and time into a DWORD variable */
	tmr =	  (((DWORD)2010 - 1980) << 25)
			| ((DWORD)11 << 21)
			| ((DWORD)1 << 16)
			| (WORD)(0 << 11)
			| (WORD)(0 << 5)
			| (WORD)(0 >> 1);
//	_EI();

	return tmr;
}

static FRESULT scan_files(
	char* path		/* Pointer to the path name working buffer */
) CYREENTRANT
{
	DIR dirs;
	FRESULT res;
	BYTE i;
	char *fn;

	if ((res = f_opendir(&dirs, path)) == FR_OK) {
		i = strlen(path);
		while (((res = f_readdir(&dirs, &Finfo)) == FR_OK) && Finfo.fname[0]) {
			if (_FS_RPATH && Finfo.fname[0] == '.') continue;
#if _USE_LFN
			fn = *Finfo.lfname ? Finfo.lfname : Finfo.fname;
#else
			fn = Finfo.fname;
#endif
			if (Finfo.fattrib & AM_DIR) {
				acc_dirs++;
				*(path+i) = '/'; strcpy(path+i+1, fn);
//				res = scan_files(path);
				*(path+i) = '\0';
				if (res != FR_OK) break;
			} else {
/*				xprintf("%s/%s\n", path, fn);	*/
				acc_files++;
				acc_size += Finfo.fsize;
			}
		}
	}

	return res;
}
/* [] END OF FILE */
