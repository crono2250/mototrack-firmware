// https://github.com/sakuraio/SakuraIOArduino

#include "SakuraIO.h"
#include "SakuraIO/commands.h"
#include "SakuraIO/debug.h"

uint8_t executeCommand(uint8_t cmd,uint8_t requestLength, uint8_t *request, uint8_t *responseLength, uint8_t *response)
{
  uint8_t parity = 0x00;
  uint8_t result = 0x00;
  uint8_t reservedResponseLength, tmpResponse, receivedResponseLength;

  xputs("executeCommand");

  begin();

  // request
  sendByte(cmd);
  sendByte(requestLength);
  parity = cmd ^ requestLength;
  for(int16_t i=0; i<requestLength; i++)
  {
    parity ^= request[i];
    sendByte(request[i]);
  }
  sendByte(parity);
  //finishSending();

  reservedResponseLength = 0;
  if(responseLength != NULL){
    reservedResponseLength = *responseLength;
  }

  CyDelay(10);

  // response
  startReceive(reservedResponseLength+3);
  result = receiveByte();
  if(result != CMD_ERROR_NONE){
    xputs("Invalid status");
    end();
    return result;
  }
  receivedResponseLength = receiveByte();
  if(responseLength != NULL){
    *responseLength = receivedResponseLength;
  }

  parity = result ^ receivedResponseLength;
  for(int16_t i=0; i<receivedResponseLength; i++){
    tmpResponse = receiveByte();
    parity ^= tmpResponse;
    if(response != NULL && i<reservedResponseLength){
      response[i] = tmpResponse;
    }
  }
  xputs("Parity");
  uint8_t p = receiveByte(true);
  parity ^= p;
//  xputs("Parity=");
//  xprintf("%02X", p);
  if(parity != 0x00){
    result = CMD_ERROR_PARITY;
    xputs("Invalid parity");
  }else{
    xputs("Success");
  }
  //finishReceiving();

  end();
  return result;
}

/* Common Commands */

uint8_t getConnectionStatus(){
  uint8_t responseLength = 1;
  uint8_t response[1] = {0x00};
  if(executeCommand(CMD_GET_CONNECTION_STATUS, 0, NULL, &responseLength, response) != CMD_ERROR_NONE){
    return 0x7F;
  }
  return response[0];
}

uint8_t getSignalQuality(){
  uint8_t responseLength = 1;
  uint8_t response[1] = {0x00};

  if(executeCommand(CMD_GET_SIGNAL_QUALITY, 0, NULL, &responseLength, response) != CMD_ERROR_NONE){
    return 0x00;
  }
  return response[0];
}

uint8_t getSignalQuarity(){
  // deprecated
  return getSignalQuality();
}

uint64_t getUnixtime(){
  uint8_t responseLength = 8;
  uint8_t response[8] = {0x00};
  if(executeCommand(CMD_GET_DATETIME, 0, NULL, &responseLength, response) != CMD_ERROR_NONE){
    return 0x00;
  }
  return *((uint64_t *)response);
}

uint8_t echoback(uint8_t length, uint8_t *data, uint8_t *response){
  uint8_t responseLength = length;
  if(executeCommand(CMD_ECHO_BACK, length, data, &responseLength, response) != CMD_ERROR_NONE){
    return 0x00;
  }
  return responseLength;
}

/* IO Commands */

uint16_t getADC(uint8_t channel){
  uint8_t request[1] = {channel};
  uint8_t response[2] = {0x00};
  uint8_t responseLength = sizeof(response);
  if(executeCommand(CMD_READ_ADC, 1, request, &responseLength, response) != CMD_ERROR_NONE){
    return 0xffff;
  }
  return *((uint16_t *)response);
}

/* TX Commands */
uint8_t enqueueTxRaw(uint8_t ch, uint8_t type, uint8_t length, uint8_t *data, uint64_t offset){
  uint8_t request[18] = {0x00};
  uint8_t requestLength = 10;
  request[0] = ch;
  request[1] = type;
  for(uint8_t i=0;i<length;i++){
    request[2+i] = data[i];
  }
  if(offset != 0){
    requestLength = 18;
    for(uint8_t i=0;i<8;i++){
      request[10+i] = ((uint8_t *)&offset)[i];
    }
  }
  return executeCommand(CMD_TX_ENQUEUE, requestLength, request, NULL, NULL);
}

uint8_t enqueueTx(uint8_t ch, int32_t value, uint64_t offset){
  return enqueueTxRaw(ch, 'i', 4, (uint8_t *)&value, offset);
}

uint8_t enqueueTx(uint8_t ch, uint32_t value, uint64_t offset){
  return enqueueTxRaw(ch, 'I', 4, (uint8_t *)&value, offset);
}

uint8_t enqueueTx(uint8_t ch, int64_t value, uint64_t offset){
  return enqueueTxRaw(ch, 'l', 8, (uint8_t *)&value, offset);
}

uint8_t enqueueTx(uint8_t ch, uint64_t value, uint64_t offset){
  return enqueueTxRaw(ch, 'L', 8, (uint8_t *)&value, offset);
}

uint8_t enqueueTx(uint8_t ch, float value, uint64_t offset){
  return enqueueTxRaw(ch, 'f', 4, (uint8_t *)&value, offset);
}

uint8_t enqueueTx(uint8_t ch, double value, uint64_t offset){
  return enqueueTxRaw(ch, 'd', 8, (uint8_t *)&value, offset);
}

uint8_t enqueueTx(uint8_t ch, uint8_t value[8], uint64_t offset){
  return enqueueTxRaw(ch, 'b', 8, (uint8_t *)value, offset);
}

uint8_t enqueueTx(uint8_t ch, int32_t value){
  return enqueueTx(ch, value, (uint32_t)0);
}

uint8_t enqueueTx(uint8_t ch, uint32_t value){
  return enqueueTx(ch, value, (uint32_t)0);
}

uint8_t enqueueTx(uint8_t ch, int64_t value){
  return enqueueTx(ch, value, (uint32_t)0);
}

uint8_t enqueueTx(uint8_t ch, uint64_t value){
  return enqueueTx(ch, value, (uint32_t)0);
}

uint8_t enqueueTx(uint8_t ch, float value){
  return enqueueTx(ch, value, (uint32_t)0);
}

uint8_t enqueueTx(uint8_t ch, double value){
  return enqueueTx(ch, value, (uint32_t)0);
}

uint8_t enqueueTx(uint8_t ch, uint8_t value[8]){
  return enqueueTx(ch, value, (uint32_t)0);
}

uint8_t sendImmediatelyRaw(uint8_t ch, uint8_t type, uint8_t length, uint8_t *data, uint64_t offset){
  uint8_t request[18] = {0x00};
  uint8_t requestLength = 10;
  request[0] = ch;
  request[1] = type;
  for(uint8_t i=0;i<length;i++){
    request[2+i] = data[i];
  }
  if(offset != 0){
    requestLength = 18;
    for(uint8_t i=0;i<8;i++){
      request[10+i] = ((uint8_t *)&offset)[i];
    }
  }
  return executeCommand(CMD_TX_SENDIMMED, requestLength, request, NULL, NULL);
}

uint8_t sendImmediately(uint8_t ch, int32_t value, uint64_t offset){
  return sendImmediatelyRaw(ch, 'i', 4, (uint8_t *)&value, offset);
}

uint8_t sendImmediately(uint8_t ch, uint32_t value, uint64_t offset){
  return sendImmediatelyRaw(ch, 'I', 4, (uint8_t *)&value, offset);
}

uint8_t sendImmediately(uint8_t ch, int64_t value, uint64_t offset){
  return sendImmediatelyRaw(ch, 'l', 8, (uint8_t *)&value, offset);
}

uint8_t sendImmediately(uint8_t ch, uint64_t value, uint64_t offset){
  return sendImmediatelyRaw(ch, 'L', 8, (uint8_t *)&value, offset);
}

uint8_t sendImmediately(uint8_t ch, float value, uint64_t offset){
  return sendImmediatelyRaw(ch, 'f', 4, (uint8_t *)&value, offset);
}

uint8_t sendImmediately(uint8_t ch, double value, uint64_t offset){
  return sendImmediatelyRaw(ch, 'd', 8, (uint8_t *)&value, offset);
}

uint8_t sendImmediately(uint8_t ch, uint8_t value[8], uint64_t offset){
  return sendImmediatelyRaw(ch, 'b', 8, (uint8_t *)value, offset);
}

uint8_t sendImmediately(uint8_t ch, int32_t value){
  return sendImmediately(ch, value, (uint32_t)0);
}

uint8_t sendImmediately(uint8_t ch, uint32_t value){
  return sendImmediately(ch, value, (uint32_t)0);
}

uint8_t sendImmediately(uint8_t ch, int64_t value){
  return sendImmediately(ch, value, (uint32_t)0);
}

uint8_t sendImmediately(uint8_t ch, uint64_t value){
  return sendImmediately(ch, value, (uint32_t)0);
}

uint8_t sendImmediately(uint8_t ch, float value){
  return sendImmediately(ch, value, (uint32_t)0);
}

uint8_t sendImmediately(uint8_t ch, double value){
  return sendImmediately(ch, value, (uint32_t)0);
}

uint8_t sendImmediately(uint8_t ch, uint8_t value[8]){
  return sendImmediately(ch, value, (uint32_t)0);
}



uint8_t getTxQueueLength(uint8_t *available, uint8_t *queued){
  uint8_t response[2] = {0x00};
  uint8_t responseLength = 2;
  uint8_t ret = executeCommand(CMD_TX_LENGTH, 0, NULL, &responseLength, response);
  *available = response[0];
  *queued = response[1];
  return ret;
}

uint8_t clearTx(){
  return executeCommand(CMD_TX_CLEAR, 0, NULL, NULL, NULL);
}

uint8_t send(){
  return executeCommand(CMD_TX_SEND, 0, NULL, NULL, NULL);
}

uint8_t getTxStatus(uint8_t *queue, uint8_t *immediate){
  uint8_t response[2] = {0x00};
  uint8_t responseLength = 2;
  uint8_t ret = executeCommand(CMD_TX_STAT, 0, NULL, &responseLength, response);
  *queue = response[0];
  *immediate = response[1];
  return ret;
}

/* RX Commands */

uint8_t dequeueRx(uint8_t *ch, uint8_t *type, uint8_t *value, int64_t *offset){
  uint8_t response[18] = {0x00};
  uint8_t responseLength = 18;
  uint8_t ret = executeCommand(CMD_RX_DEQUEUE, 0, NULL, &responseLength, response);
  if(ret != CMD_ERROR_NONE){
    return ret;
  }

  *ch = response[0];
  *type = response[1];
  for(uint8_t i=0; i<8; i++){
    value[i] = response[2+i];
  }
  for(uint8_t i=0; i<8; i++){
    ((uint8_t *)offset)[i] = response[10+i];
  }

  return ret;
}

uint8_t peekRx(uint8_t *ch, uint8_t *type, uint8_t *value, int64_t *offset){
  uint8_t response[18] = {0x00};
  uint8_t responseLength = 18;
  uint8_t ret = executeCommand(CMD_RX_PEEK, 0, NULL, &responseLength, response);
  if(ret != CMD_ERROR_NONE){
    return ret;
  }

  *ch = response[0];
  *type = response[1];
  for(uint8_t i=0; i<8; i++){
    value[i] = response[2+i];
  }
  for(uint8_t i=0; i<8; i++){
    ((uint8_t *)offset)[i] = response[10+i];
  }

  return ret;
}

uint8_t getRxQueueLength(uint8_t *available, uint8_t *queued){
  uint8_t response[2] = {0x00};
  uint8_t responseLength = 2;
  uint8_t ret = executeCommand(CMD_RX_LENGTH, 0, NULL, &responseLength, response);
  *available = response[0];
  *queued = response[1];
  return ret;
}

uint8_t clearRx(){
  return executeCommand(CMD_RX_CLEAR, 0, NULL, NULL, NULL);
}

/* File command */
uint8_t startFileDownload(uint16_t fileId){
  return executeCommand(CMD_START_FILE_DOWNLOAD, 2, (uint8_t *)&fileId, NULL, NULL);
}

uint8_t cancelFileDownload(){
  return executeCommand(CMD_CANCEL_FILE_DOWNLOAD, 0, NULL, NULL, NULL);
}

uint8_t getFileMetaData(uint8_t *status, uint32_t *totalSize, uint64_t *timestamp, uint32_t *crc){
  uint8_t response[17] = {0x00};
  uint8_t responseLength = 17;
  uint8_t ret = executeCommand(CMD_GET_FILE_METADATA, 0, NULL, &responseLength, response);
  *status = response[0];
  *totalSize = *(uint32_t *)(response+1);
  *timestamp = *(uint64_t *)(response+5);
  *crc = *(uint32_t *)(response+13);
  return ret;
}

uint8_t getFileDownloadStatus(uint8_t *status, uint32_t *currentSize){
  uint8_t response[5] = {0x00};
  uint8_t responseLength = 5;
  uint8_t ret = executeCommand(CMD_GET_FILE_DOWNLOAD_STATUS, 0, NULL, &responseLength, response);
  *status = response[0];
  *currentSize = *(uint32_t *)(response+1);
  return ret;
}

uint8_t getFileData(uint8_t *size, uint8_t *data){
  return executeCommand(CMD_GET_FILE_DATA, 1, size, size, data);
}

/* Operation command */

uint16_t getProductID(){
  uint8_t response[2] = {0x00};
  uint8_t responseLength = 2;
  uint8_t ret = executeCommand(CMD_GET_PRODUCT_ID, 0, NULL, &responseLength, response);
  if(ret != CMD_ERROR_NONE){
    return 0x00;
  }
  return *((uint16_t *)response);
}

uint8_t getUniqueID(char *data){
  uint8_t response[11] = {0x00};
  uint8_t responseLength = 10;
  uint8_t ret = executeCommand(CMD_GET_UNIQUE_ID, 0, NULL, &responseLength, response);
  if(ret != CMD_ERROR_NONE){
    return ret;
  }
  for(uint8_t i=0; i<responseLength; i++){
    data[i] = (char)response[i];
  }
  data[responseLength] = 0x00;
  return ret;
}

uint8_t getFirmwareVersion(char *data){
  uint8_t response[33] = {0x00};
  uint8_t responseLength = 32;
  uint8_t ret = executeCommand(CMD_GET_FIRMWARE_VERSION, 0, NULL, &responseLength, response);
  if(ret != CMD_ERROR_NONE){
    return ret;
  }
  for(uint8_t i=0; i<responseLength; i++){
    data[i] = (char)response[i];
  }
  data[responseLength] = 0x00;
  return ret;
}

uint8_t unlock(){
  uint8_t request[4] = {0x53, 0x6B, 0x72, 0x61};
  return executeCommand(CMD_UNLOCK, 4, request, NULL, NULL);
}

uint8_t updateFirmware(){
  return executeCommand(CMD_UPDATE_FIRMWARE, 0, 0, NULL, NULL);
}

uint8_t getFirmwareUpdateStatus(){
  uint8_t response[1] = {0x00};
  uint8_t responseLength = 1;
  if(executeCommand(CMD_GET_UPDATE_FIRMWARE_STATUS, 0, 0, &responseLength, response) != CMD_ERROR_NONE){
      return 0xff;
  }
  return response[0];
}

uint8_t reset(){
  return executeCommand(CMD_SOFTWARE_RESET, 0, 0, NULL, NULL);
}

uint8_t setPowerSaveMode(uint8_t mode)
{
  uint8_t request[1] = {mode};
  return executeCommand(CMD_SET_POWER_SAVE_MODE, 1, request, NULL, NULL);
}

uint8_t getPowerSaveMode()
{
  uint8_t response[1] = {0x00};
  uint8_t responseLength = 1;
  if (executeCommand(CMD_GET_POWER_SAVE_MODE, 0, NULL, &responseLength, response) != CMD_ERROR_NONE) {
    return 0xff;
  }
  return response[0];
}
