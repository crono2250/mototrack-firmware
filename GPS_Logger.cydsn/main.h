// UART
#define SCI_RX_SIZE 64
#define GPS_RX_SIZE 80
#define str2dec_buf_size 64				// SCI_RX_SIZE > str2dec_buf_size にしないこと。
										// 解析の時にバッファが溢れてハマるぞ。
// BME280
// REGISTER READ/WRITE //
#define BME280_WRITE    0x00
#define BME280_READ    0x80

//POWER MODE DEFINITIONS  //
#define BME280_SLEEP_MODE      0x00
#define BME280_FORCED_MODE      0x01
#define BME280_NORMAL_MODE      0x03
#define BME280_SOFT_RESET_CODE   0xB6
//STANDBY DEFINITIONS  //
#define BME280_STANDBY_TIME_1_MS   0x00
#define BME280_STANDBY_TIME_63_MS   0x01
#define BME280_STANDBY_TIME_125_MS   0x02
#define BME280_STANDBY_TIME_250_MS   0x03
#define BME280_STANDBY_TIME_500_MS   0x04
#define BME280_STANDBY_TIME_1000_MS   0x05
#define BME280_STANDBY_TIME_10_MS   0x06
#define BME280_STANDBY_TIME_20_MS    0x07
//OVER SAMPLING //
#define BME280_OVERSAMP_SKIPPED   0x00
#define BME280_OVERSAMP_1X      0x01
#define BME280_OVERSAMP_2X      0x02
#define BME280_OVERSAMP_4X      0x03
#define BME280_OVERSAMP_8X      0x04
#define BME280_OVERSAMP_16X      0x05
//WORK MODE//
#define BME280_ULTRALOWPOWER_MODE       0x00
#define BME280_LOWPOWER_MODE         0x01
#define BME280_STANDARDRESOLUTION_MODE   0x02
#define BME280_HIGHRESOLUTION_MODE      0x03
#define BME280_ULTRAHIGHRESOLUTION_MODE   0x04
//FILTER //
#define BME280_FILTER_COEFF_OFF   0x00
#define BME280_FILTER_COEFF_2   0x01
#define BME280_FILTER_COEFF_4   0x02
#define BME280_FILTER_COEFF_8   0x03
#define BME280_FILTER_COEFF_16   0x04
//CALRATION REGISTER//
#define BME280_DIG_T1_LSB_REG   0x88
#define BME280_DIG_T1_MSB_REG   0x89
#define BME280_DIG_T2_LSB_REG   0x8A
#define BME280_DIG_T2_MSB_REG   0x8B
#define BME280_DIG_T3_LSB_REG   0x8C
#define BME280_DIG_T3_MSB_REG   0x8D
#define BME280_DIG_P1_LSB_REG   0x8E
#define BME280_DIG_P1_MSB_REG   0x8F
#define BME280_DIG_P2_LSB_REG   0x90
#define BME280_DIG_P2_MSB_REG   0x91
#define BME280_DIG_P3_LSB_REG   0x92
#define BME280_DIG_P3_MSB_REG   0x93
#define BME280_DIG_P4_LSB_REG   0x94
#define BME280_DIG_P4_MSB_REG   0x95
#define BME280_DIG_P5_LSB_REG   0x96
#define BME280_DIG_P5_MSB_REG   0x97
#define BME280_DIG_P6_LSB_REG   0x98
#define BME280_DIG_P6_MSB_REG   0x99
#define BME280_DIG_P7_LSB_REG   0x9A
#define BME280_DIG_P7_MSB_REG   0x9B
#define BME280_DIG_P8_LSB_REG   0x9C
#define BME280_DIG_P8_MSB_REG   0x9D
#define BME280_DIG_P9_LSB_REG   0x9E
#define BME280_DIG_P9_MSB_REG   0x9F
#define BME280_DIG_H1_REG      0xA1
#define BME280_DIG_H2_LSB_REG   0xE1
#define BME280_DIG_H2_MSB_REG   0xE2
#define BME280_DIG_H3_REG      0xE3
#define BME280_DIG_H4_MSB_REG   0xE4
#define BME280_DIG_H4_H5_REG   0xE5
#define BME280_DIG_H5_REG       0xE6
#define BME280_DIG_H6_REG      0xE7
//REGISTER ADDRESS //
#define BME280_CHIPID_REG      0xD0  //CHIP ID
#define BME280_RST_REG         0xE0  //SOFTRESET
#define BME280_CTRL_HUMI_REG   0xF2  //CTRL HUMI
#define BME280_STAT_REG         0xF3  //STATUS REGISTER
#define BME280_CTRL_MEAS_REG   0xF4  //CTRL MEASURE REGISTER
#define BME280_CONFIG_REG      0xF5  //CONFIGURATION
#define BME280_PRES_MSB_REG      0xF7  //PRES MSB REGISTER
#define BME280_PRES_LSB_REG      0xF8  //PRES LSB REGISTER
#define BME280_PRES_XLSB_REG   0xF9  //PRES XLSB REGISTER
#define BME280_TEMP_MSB_REG      0xFA  //TEMP MSB REG
#define BME280_TEMP_LSB_REG      0xFB  //TEMP LSB REG
#define BME280_TEMP_XLSB_REG   0xFC  //TEMP XLSB REG
#define BME280_HUMI_MSB_REG      0xFD  //HUMI MSB REG
#define BME280_HUMI_LSB_REG      0xFE  //HUMI LSB REG



//----------------------------------------------------------------
// 関数プロトタイプ宣言
//----------------------------------------------------------------

void sci_recvln(void);
uint16 str2dec(uint16 start_byte);
void DEF_Check(void);
uint8 EEPROM_Write_Word(uint16 dat, uint16 adr);
uint16 EEPROM_Read_Word(uint16 adr);
uint8 EEPROM_Write_TByte(uint32 dat, uint16 adr);
uint32 EEPROM_Read_TByte(uint16 adr);
void clreeprom(void);
void eedump(void);
void PERI_SPIM_RW(uint8 size);
void BME_READ(void);
void GPS_PARSE(void);
void SAKURA_IO_DataSend(void);
void ACC_Sleep_FUNC(void);