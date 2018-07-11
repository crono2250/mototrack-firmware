#include "SakuraIOc_spi.h"
#include "spi_interface.h"  //コンパイルできるようにするためのダミーファイル
#include "project.h"

#define SAKURA_CSn_IDLE()  SAKURA_CSn_Write(1)
#define SAKURA_CSn_Active()  SAKURA_CSn_Write(0)

inline void delay(uint32_t ms){
    CyDelay(ms);
}

void SakuraIOc_hw_init(){
    //元関数のコンストラクタ
    //SPIの初期化をする
    SAKURA_SPIM_Init();
    SAKURA_SPIM_Enable();
    SAKURA_SPIM_EnableInt();
    SAKURA_SPIM_Start();
}

void SakuraIOc_hw_begin(){
    //SPIを転送可能状態にする関数
    //CSをアサート
    SAKURA_CSn_Active();
}

void SakuraIOc_hw_end(){
    //SPIを転送終了状態にする関数
    //CSをネゲート
    //元関数では最後に20us待ってる
    SAKURA_CSn_IDLE();
    CyDelayUs(20);
}

void SakuraIOc_hw_sendByte(uint8_t data){
    //SPIで1byte送る関数
    //元関数では送る前に20us待ってる
    CyDelayUs(20);
    SAKURA_SPIM_WriteTxData(data);
	while(!(SAKURA_SPIM_ReadTxStatus() & SAKURA_SPIM_STS_SPI_DONE)){};
}

uint8_t SakuraIOc_hw_startReceive(uint8_t length){
    //I2C用の関数なのでSPIはダミー動作させる
    return length;
}

uint8_t SakuraIOc_hw_receiveByte(){
    //SPIから1byte読む関数
    //元の関数では読む前に20us待ってる
    //送信ダミーデータは0x00にしてある
	SAKURA_SPIM_WriteTxData(0x00);
	while(!(SAKURA_SPIM_ReadTxStatus() & SAKURA_SPIM_STS_SPI_DONE)){};

    return SAKURA_SPIM_ReadRxData();
}

uint8_t SakuraIOc_hw_receiveByte2(bool stop){
    //元の関数ではstopが使われてない．
    return 0;
}
