#ifndef SAKURAIOC_H
#define SAKURAIOC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
    
uint8_t getConnectionStatus();
uint8_t getSignalQuality();
uint64_t getUnixtime();
uint8_t echoback(uint8_t length, uint8_t *data, uint8_t *response);
uint16_t getADC(uint8_t channel); // Deprecated
uint8_t enqueueTxRaw(uint8_t ch, uint8_t type, uint8_t length, uint8_t *data, uint64_t offset);
uint8_t enqueueTx2_i32(uint8_t ch, int32_t value, uint64_t offset);
uint8_t enqueueTx2_ui32(uint8_t ch, uint32_t value, uint64_t offset);
uint8_t enqueueTx2_i64(uint8_t ch, int64_t value, uint64_t offset);
uint8_t enqueueTx2_ui64(uint8_t ch, uint64_t value, uint64_t offset);
uint8_t enqueueTx2_f(uint8_t ch, float value, uint64_t offset);
uint8_t enqueueTx2_d(uint8_t ch, double value, uint64_t offset);
uint8_t enqueueTx2_ui8a(uint8_t ch, uint8_t value[8], uint64_t offset);
uint8_t enqueueTx_i32(uint8_t ch, int32_t value);
uint8_t enqueueTx_ui32(uint8_t ch, uint32_t value);
uint8_t enqueueTx_i64(uint8_t ch, int64_t value);
uint8_t enqueueTx_ui64(uint8_t ch, uint64_t value);
uint8_t enqueueTx_f(uint8_t ch, float value);
uint8_t enqueueTx_d(uint8_t ch, double value);
uint8_t enqueueTx_ui8a(uint8_t ch, uint8_t value[8]);
uint8_t sendImmediatelyRaw(uint8_t ch, uint8_t type, uint8_t length, uint8_t *data, uint64_t offset);
uint8_t sendImmediately2_i32(uint8_t ch, int32_t value, uint64_t offset);
uint8_t sendImmediately2_ui32(uint8_t ch, uint32_t value, uint64_t offset);
uint8_t sendImmediately2_i64(uint8_t ch, int64_t value, uint64_t offset);
uint8_t sendImmediately2_ui64(uint8_t ch, uint64_t value, uint64_t offset);
uint8_t sendImmediately2_f(uint8_t ch, float value, uint64_t offset);
uint8_t sendImmediately2_d(uint8_t ch, double value, uint64_t offset);
uint8_t sendImmediately2_ui8a(uint8_t ch, uint8_t value[8], uint64_t offset);
uint8_t sendImmediately_i32(uint8_t ch, int32_t value);
uint8_t sendImmediately_ui32(uint8_t ch, uint32_t value);
uint8_t sendImmediately_i64(uint8_t ch, int64_t value);
uint8_t sendImmediately_ui64(uint8_t ch, uint64_t value);
uint8_t sendImmediately_f(uint8_t ch, float value);
uint8_t sendImmediately_d(uint8_t ch, double value);
uint8_t sendImmediately_ui8a(uint8_t ch, uint8_t value[8]);
uint8_t getTxQueueLength(uint8_t *available, uint8_t *queued);
uint8_t clearTx();
uint8_t getTxStatus(uint8_t *queue, uint8_t *immediate);
uint8_t send();
uint8_t dequeueRx(uint8_t *ch, uint8_t *type, uint8_t *value, int64_t *offset);
uint8_t peekRx(uint8_t *ch, uint8_t *type, uint8_t *value, int64_t *offset);
uint8_t getRxQueueLength(uint8_t *available, uint8_t *queued);
uint8_t clearRx();
uint8_t startFileDownload(uint16_t fileId);
uint8_t cancelFileDownload();
uint8_t getFileMetaData(uint8_t *status, uint32_t *totalSize, uint64_t *timestamp, uint32_t *crc);
uint8_t getFileDownloadStatus(uint8_t *status, uint32_t *currentSize);
uint8_t getFileData(uint8_t *size, uint8_t *data);
uint16_t getProductID();
uint8_t getUniqueID(char *data);
uint8_t getFirmwareVersion(char *data);
uint8_t unlock();
uint8_t updateFirmware();
uint8_t getFirmwareUpdateStatus();
uint8_t reset();
uint8_t setPowerSaveMode(uint8_t mode);
uint8_t getPowerSaveMode();


#ifdef __cplusplus
}
#endif

#endif /* SAKURAIOC_H */

