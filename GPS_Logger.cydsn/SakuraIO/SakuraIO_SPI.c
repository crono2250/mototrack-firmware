#include <SPI.h>
#include "SakuraIO.h"
#include "SakuraIO/debug.h"

//#if SPI_HAS_TRANSACTION
//static SPISettings settings;
//#endif

void begin(){
//#if SPI_HAS_TRANSACTION
//  SPI.beginTransaction(settings);
//#endif
//  dbgln("CS=0");
  //digitalWrite(cs, LOW);
  SAKURA_CSn_Write(0);
}

void end(){
//  dbgln("CS=1");
//  digitalWrite(cs, HIGH);
  SAKURA_CSn_Write(1);
//#if SPI_HAS_TRANSACTION
//  endTransaction();
//#endif
  CyDelayUs(20);
}

void sendByte(uint8_t data){
  CyDelayUs(20);
//  dbg("Send=");
//  dbgln(data, HEX);
//  SPI.transfer(data);
  PERI_SPIM_WriteTxData(data);
  while(!(PERI_SPIM_ReadTxStatus() & PERI_SPIM_STS_SPI_DONE)){};
}


uint8_t receiveByte(bool stop){
  PERI_SPIM_WriteTxData(0x00);
  while(!(PERI_SPIM_ReadTxStatus() & PERI_SPIM_STS_SPI_DONE)){};
  ret = PERI_SPIM_ReadRxData();

  return ret;
}

uint8_t receiveByte(){
  uint8_t ret;
  CyDelayUs(20);
//  ret = SPI.transfer(0x00);
  PERI_SPIM_WriteTxData(0x00);
  while(!(PERI_SPIM_ReadTxStatus() & PERI_SPIM_STS_SPI_DONE)){};
  ret = PERI_SPIM_ReadRxData();

//  dbg("Recv=");
//  dbgln(ret, HEX);
  return ret;
}

SakuraIO_SPI(int _cs){
//  cs = _cs;
//  SAKURA_CSn_Write(0);
//#if SPI_HAS_TRANSACTION
//  settings = SPISettings(4000000, MSBFIRST, SPI_MODE0); // 4MHz, MSB First, SPI mode 0
//#endif
//  pinMode(cs, OUTPUT);
  SAKURA_CSn_Write(1);
}
