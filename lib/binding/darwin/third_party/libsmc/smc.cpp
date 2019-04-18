/*
 * Apple System Management Control (SMC) Tool
 * Copyright (C) 2006 devnull
 * Portions Copyright (C) 2013 Michael Wilber
 * Portions Copyright (C) 2019 Youchen Du
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 USA.
 */

#include <IOKit/IOKitLib.h>
#include <os/lock.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <unordered_map>

#include "smc.hpp"

using namespace std;

namespace {

#define OP_NONE 0
#define OP_LIST 1
#define OP_READ 2
#define OP_READ_FAN 3
#define OP_WRITE 4
#define OP_READ_TEMPS 5

#define KERNEL_INDEX_SMC 2

#define SMC_CMD_READ_BYTES 5
#define SMC_CMD_WRITE_BYTES 6
#define SMC_CMD_READ_INDEX 8
#define SMC_CMD_READ_KEYINFO 9
#define SMC_CMD_READ_PLIMIT 11
#define SMC_CMD_READ_VERS 12

#define DATATYPE_FLT "flt "
#define DATATYPE_FP1F "fp1f"
#define DATATYPE_FP4C "fp4c"
#define DATATYPE_FP5B "fp5b"
#define DATATYPE_FP6A "fp6a"
#define DATATYPE_FP79 "fp79"
#define DATATYPE_FP88 "fp88"
#define DATATYPE_FPA6 "fpa6"
#define DATATYPE_FPC4 "fpc4"
#define DATATYPE_FPE2 "fpe2"

#define DATATYPE_SP1E "sp1e"
#define DATATYPE_SP3C "sp3c"
#define DATATYPE_SP4B "sp4b"
#define DATATYPE_SP5A "sp5a"
#define DATATYPE_SP69 "sp69"
#define DATATYPE_SP78 "sp78"
#define DATATYPE_SP87 "sp87"
#define DATATYPE_SP96 "sp96"
#define DATATYPE_SPB4 "spb4"
#define DATATYPE_SPF0 "spf0"

#define DATATYPE_UINT8 "ui8 "
#define DATATYPE_UINT16 "ui16"
#define DATATYPE_UINT32 "ui32"

#define DATATYPE_SI8 "si8 "
#define DATATYPE_SI16 "si16"

#define DATATYPE_FLT "flt "

#define DATATYPE_PWM "{pwm"

struct SMCVersion {
  char major;
  char minor;
  char build;
  char reserved[1];
  unsigned short release;
};

struct SMCPLimitData {
  unsigned short version;
  unsigned short length;
  uint32_t cpuPLimit;
  uint32_t gpuPLimit;
  uint32_t memPLimit;
};

struct SMCKeyInfoData {
  uint32_t dataSize;
  uint32_t dataType;
  char dataAttributes;
};

struct SMCKeyData {
  uint32_t key;
  SMCVersion vers;
  SMCPLimitData pLimitData;
  SMCKeyInfoData keyInfo;
  char result;
  char status;
  char data8;
  uint32_t data32;
  unsigned char bytes[32];
};

struct SMCVal {
  char key[5];
  uint32_t dataSize;
  char dataType[5];
  unsigned char bytes[32];
};

class LockGuard {
 public:
  explicit LockGuard(os_unfair_lock &mutex_arg) : mutex{mutex_arg} {
    os_unfair_lock_lock(&mutex);
  }
  ~LockGuard() { os_unfair_lock_unlock(&mutex); }

 private:
  os_unfair_lock &mutex;
};

std::unordered_map<uint32_t, SMCKeyInfoData> key_info_cache;

os_unfair_lock mutex_ = OS_UNFAIR_LOCK_INIT;

io_connect_t g_conn{};

kern_return_t SMCCall2(int index, SMCKeyData *inputStructure,
                       SMCKeyData *outputStructure, io_connect_t conn);

uint32_t _strtoul(char *str, int size, int base) {
  uint32_t total = 0;
  int i;

  for (i = 0; i < size; i++) {
    if (base == 16)
      total += str[i] << (size - 1 - i) * 8;
    else
      total += ((unsigned char)(str[i]) << (size - 1 - i) * 8);
  }
  return total;
}

void _ultostr(char *str, uint32_t val) {
  str[0] = '\0';
  sprintf(str, "%c%c%c%c", (unsigned int)val >> 24, (unsigned int)val >> 16,
          (unsigned int)val >> 8, (unsigned int)val);
}

float _strtof(unsigned char *str, int size, int e) {
  float total = 0;
  int i;

  for (i = 0; i < size; i++) {
    if (i == (size - 1))
      total += (str[i] & 0xff) >> e;
    else
      total += str[i] << (size - 1 - i) * (8 - e);
  }

  total += (str[size - 1] & 0x03) * 0.25;

  return total;
}

void printFLT(SMCVal val) {
  float fval;
  memcpy(&fval, val.bytes, sizeof(float));
  printf("%.0f ", fval);
}

void printFP1F(SMCVal val) {
  printf("%.5f ", ntohs(*(unsigned short *)val.bytes) / 32768.0);
}

void printFP4C(SMCVal val) {
  printf("%.5f ", ntohs(*(unsigned short *)val.bytes) / 4096.0);
}

void printFP5B(SMCVal val) {
  printf("%.5f ", ntohs(*(unsigned short *)val.bytes) / 2048.0);
}

void printFP6A(SMCVal val) {
  printf("%.4f ", ntohs(*(unsigned short *)val.bytes) / 1024.0);
}

void printFP79(SMCVal val) {
  printf("%.4f ", ntohs(*(unsigned short *)val.bytes) / 512.0);
}

void printFP88(SMCVal val) {
  printf("%.3f ", ntohs(*(unsigned short *)val.bytes) / 256.0);
}

void printFPA6(SMCVal val) {
  printf("%.2f ", ntohs(*(unsigned short *)val.bytes) / 64.0);
}

void printFPC4(SMCVal val) {
  printf("%.2f ", ntohs(*(unsigned short *)val.bytes) / 16.0);
}

void printFPE2(SMCVal val) {
  printf("%.2f ", ntohs(*(unsigned short *)val.bytes) / 4.0);
}

void printUInt(SMCVal val) {
  printf("%u ", (unsigned int)_strtoul((char *)val.bytes, val.dataSize, 10));
}

void printSP1E(SMCVal val) {
  printf("%.5f ",
         ((signed short)ntohs(*(unsigned short *)val.bytes)) / 16384.0);
}

void printSP3C(SMCVal val) {
  printf("%.5f ", ((signed short)ntohs(*(unsigned short *)val.bytes)) / 4096.0);
}

void printSP4B(SMCVal val) {
  printf("%.4f ", ((signed short)ntohs(*(unsigned short *)val.bytes)) / 2048.0);
}

void printSP5A(SMCVal val) {
  printf("%.4f ", ((signed short)ntohs(*(unsigned short *)val.bytes)) / 1024.0);
}

void printSP69(SMCVal val) {
  printf("%.3f ", ((signed short)ntohs(*(unsigned short *)val.bytes)) / 512.0);
}

int ConvertSP78(SMCVal *val) {
  return static_cast<signed short>(
             ntohs(*reinterpret_cast<unsigned short *>(val->bytes))) /
         256;
}

void printSP78(SMCVal *val) { printf("%d ", ConvertSP78(val)); }

void printSP87(SMCVal val) {
  printf("%.3f ", ((signed short)ntohs(*(unsigned short *)val.bytes)) / 128.0);
}

void printSP96(SMCVal val) {
  printf("%.2f ", ((signed short)ntohs(*(unsigned short *)val.bytes)) / 64.0);
}

void printSPB4(SMCVal val) {
  printf("%.2f ", ((signed short)ntohs(*(unsigned short *)val.bytes)) / 16.0);
}

void printSPF0(SMCVal val) {
  printf("%.0f ", (float)ntohs(*(unsigned short *)val.bytes));
}

void printSI8(SMCVal val) { printf("%d ", (signed char)*val.bytes); }

void printSI16(SMCVal val) { printf("%d ", ntohs(*(signed short *)val.bytes)); }

void printPWM(SMCVal val) {
  printf("%.1f%% ", ntohs(*(unsigned short *)val.bytes) * 100 / 65536.0);
}

void printBytesHex(SMCVal val) {
  uint32_t i;

  printf("(bytes");
  for (i = 0; i < val.dataSize; i++)
    printf(" %02x", (unsigned char)val.bytes[i]);
  printf(")\n");
}

void printVal(SMCVal val) {
  printf("  %-4s  [%-4s]  ", val.key, val.dataType);
  if (val.dataSize > 0) {
    if ((strcmp(val.dataType, DATATYPE_UINT8) == 0) ||
        (strcmp(val.dataType, DATATYPE_UINT16) == 0) ||
        (strcmp(val.dataType, DATATYPE_UINT32) == 0))
      printUInt(val);
    else if (strcmp(val.dataType, DATATYPE_FLT) == 0 && val.dataSize == 4)
      printFLT(val);
    else if (strcmp(val.dataType, DATATYPE_FP1F) == 0 && val.dataSize == 2)
      printFP1F(val);
    else if (strcmp(val.dataType, DATATYPE_FP4C) == 0 && val.dataSize == 2)
      printFP4C(val);
    else if (strcmp(val.dataType, DATATYPE_FP5B) == 0 && val.dataSize == 2)
      printFP5B(val);
    else if (strcmp(val.dataType, DATATYPE_FP6A) == 0 && val.dataSize == 2)
      printFP6A(val);
    else if (strcmp(val.dataType, DATATYPE_FP79) == 0 && val.dataSize == 2)
      printFP79(val);
    else if (strcmp(val.dataType, DATATYPE_FP88) == 0 && val.dataSize == 2)
      printFP88(val);
    else if (strcmp(val.dataType, DATATYPE_FPA6) == 0 && val.dataSize == 2)
      printFPA6(val);
    else if (strcmp(val.dataType, DATATYPE_FPC4) == 0 && val.dataSize == 2)
      printFPC4(val);
    else if (strcmp(val.dataType, DATATYPE_FPE2) == 0 && val.dataSize == 2)
      printFPE2(val);
    else if (strcmp(val.dataType, DATATYPE_SP1E) == 0 && val.dataSize == 2)
      printSP1E(val);
    else if (strcmp(val.dataType, DATATYPE_SP3C) == 0 && val.dataSize == 2)
      printSP3C(val);
    else if (strcmp(val.dataType, DATATYPE_SP4B) == 0 && val.dataSize == 2)
      printSP4B(val);
    else if (strcmp(val.dataType, DATATYPE_SP5A) == 0 && val.dataSize == 2)
      printSP5A(val);
    else if (strcmp(val.dataType, DATATYPE_SP69) == 0 && val.dataSize == 2)
      printSP69(val);
    else if (strcmp(val.dataType, DATATYPE_SP78) == 0 && val.dataSize == 2)
      printSP78(&val);
    else if (strcmp(val.dataType, DATATYPE_SP87) == 0 && val.dataSize == 2)
      printSP87(val);
    else if (strcmp(val.dataType, DATATYPE_SP96) == 0 && val.dataSize == 2)
      printSP96(val);
    else if (strcmp(val.dataType, DATATYPE_SPB4) == 0 && val.dataSize == 2)
      printSPB4(val);
    else if (strcmp(val.dataType, DATATYPE_SPF0) == 0 && val.dataSize == 2)
      printSPF0(val);
    else if (strcmp(val.dataType, DATATYPE_SI8) == 0 && val.dataSize == 1)
      printSI8(val);
    else if (strcmp(val.dataType, DATATYPE_SI16) == 0 && val.dataSize == 2)
      printSI16(val);
    else if (strcmp(val.dataType, DATATYPE_PWM) == 0 && val.dataSize == 2)
      printPWM(val);
    else if (strcmp(val.dataType, DATATYPE_FLT) == 0 && val.dataSize == 4)
      printFLT(val);

    printBytesHex(val);
  } else {
    printf("no data\n");
  }
}

kern_return_t SMCOpen(io_connect_t *conn) {
  kern_return_t result;
  mach_port_t masterPort;
  io_iterator_t iterator;
  io_object_t device;

  IOMasterPort(MACH_PORT_NULL, &masterPort);

  CFMutableDictionaryRef matchingDictionary = IOServiceMatching("AppleSMC");
  result =
      IOServiceGetMatchingServices(masterPort, matchingDictionary, &iterator);
  if (result != kIOReturnSuccess) {
    printf("Error: IOServiceGetMatchingServices() = %08x\n", result);
    return 1;
  }

  device = IOIteratorNext(iterator);
  IOObjectRelease(iterator);
  if (device == 0) {
    printf("Error: no SMC found\n");
    return 1;
  }

  result = IOServiceOpen(device, mach_task_self(), 0, conn);
  IOObjectRelease(device);
  if (result != kIOReturnSuccess) {
    printf("Error: IOServiceOpen() = %08x\n", result);
    return 1;
  }

  return kIOReturnSuccess;
}

kern_return_t SMCClose(io_connect_t conn) { return IOServiceClose(conn); }

kern_return_t SMCCall2(int index, SMCKeyData *inputStructure,
                       SMCKeyData *outputStructure, io_connect_t conn) {
  size_t structureInputSize;
  size_t structureOutputSize;
  structureInputSize = sizeof(SMCKeyData);
  structureOutputSize = sizeof(SMCKeyData);

  return IOConnectCallStructMethod(conn, index, inputStructure,
                                   structureInputSize, outputStructure,
                                   &structureOutputSize);
}

// Provides key info, using a cache to dramatically improve the energy impact of
// smcFanControl
kern_return_t SMCGetKeyInfo(uint32_t key, SMCKeyInfoData *keyInfo,
                            io_connect_t conn) {
  LockGuard lock(mutex_);

  auto it = key_info_cache.find(key);
  if (it != key_info_cache.end()) {
    *keyInfo = it->second;
    return kIOReturnSuccess;
  }

  SMCKeyData inputStructure{};
  SMCKeyData outputStructure{};

  inputStructure.key = key;
  inputStructure.data8 = SMC_CMD_READ_KEYINFO;

  kern_return_t result =
      SMCCall2(KERNEL_INDEX_SMC, &inputStructure, &outputStructure, conn);
  if (result == kIOReturnSuccess) {
    *keyInfo = outputStructure.keyInfo;
    key_info_cache[key] = outputStructure.keyInfo;
  }

  return result;
}

kern_return_t SMCReadKey2(SMCVal *val, io_connect_t conn) {
  kern_return_t result;
  SMCKeyData inputStructure{};
  SMCKeyData outputStructure{};

  inputStructure.key = _strtoul(val->key, 4, 16);

  result = SMCGetKeyInfo(inputStructure.key, &outputStructure.keyInfo, conn);
  if (result != kIOReturnSuccess) {
    return result;
  }

  val->dataSize = outputStructure.keyInfo.dataSize;
  _ultostr(val->dataType, outputStructure.keyInfo.dataType);
  inputStructure.keyInfo.dataSize = val->dataSize;
  inputStructure.data8 = SMC_CMD_READ_BYTES;

  result = SMCCall2(KERNEL_INDEX_SMC, &inputStructure, &outputStructure, conn);
  if (result != kIOReturnSuccess) {
    return result;
  }

  memcpy(val->bytes, outputStructure.bytes, sizeof(outputStructure.bytes));

  return kIOReturnSuccess;
}

void smc_init() { SMCOpen(&g_conn); }

void smc_close() { SMCClose(g_conn); }

kern_return_t SMCCall(int index, SMCKeyData *inputStructure,
                      SMCKeyData *outputStructure) {
  return SMCCall2(index, inputStructure, outputStructure, g_conn);
}

kern_return_t SMCReadKey(const char *key, SMCVal *val) {
  strcpy(val->key, key);
  return SMCReadKey2(val, g_conn);
}

kern_return_t SMCWriteKey2(SMCVal &writeVal, io_connect_t conn) {
  kern_return_t result;
  SMCKeyData inputStructure;
  SMCKeyData outputStructure;

  SMCVal readVal;

  strcpy(readVal.key, writeVal.key);
  result = SMCReadKey2(&readVal, conn);
  if (result != kIOReturnSuccess) return result;

  if (readVal.dataSize != writeVal.dataSize) return kIOReturnError;

  memset(&inputStructure, 0, sizeof(SMCKeyData));
  memset(&outputStructure, 0, sizeof(SMCKeyData));

  inputStructure.key = _strtoul(writeVal.key, 4, 16);
  inputStructure.data8 = SMC_CMD_WRITE_BYTES;
  inputStructure.keyInfo.dataSize = writeVal.dataSize;
  memcpy(inputStructure.bytes, writeVal.bytes, sizeof(writeVal.bytes));
  result = SMCCall2(KERNEL_INDEX_SMC, &inputStructure, &outputStructure, conn);

  if (result != kIOReturnSuccess) return result;
  return kIOReturnSuccess;
}

kern_return_t SMCWriteKey(SMCVal writeVal) {
  return SMCWriteKey2(writeVal, g_conn);
}

uint32_t SMCReadIndexCount() {
  SMCVal val{};

  SMCReadKey("#KEY", &val);
  return _strtoul((char *)val.bytes, val.dataSize, 10);
}

// Fix me with other types
float getFloatFromVal(SMCVal val) {
  float fval = -1.0f;

  if (val.dataSize > 0) {
    if (strcmp(val.dataType, DATATYPE_FLT) == 0 && val.dataSize == 4) {
      memcpy(&fval, val.bytes, sizeof(float));
    } else if (strcmp(val.dataType, DATATYPE_FPE2) == 0 && val.dataSize == 2) {
      fval = _strtof(val.bytes, val.dataSize, 2);
    } else if (strcmp(val.dataType, DATATYPE_UINT16) == 0 &&
               val.dataSize == 2) {
      fval = (float)_strtoul((char *)val.bytes, val.dataSize, 10);
    } else if (strcmp(val.dataType, DATATYPE_UINT8) == 0 && val.dataSize == 1) {
      fval = (float)_strtoul((char *)val.bytes, val.dataSize, 10);
    }
  }

  return fval;
}

kern_return_t SMCPrintFans(void) {
  kern_return_t result;
  SMCVal val{};
  char key[5];
  int totalFans, i;

  result = SMCReadKey("FNum", &val);
  if (result != kIOReturnSuccess) return kIOReturnError;

  totalFans = _strtoul((char *)val.bytes, val.dataSize, 10);
  printf("Total fans in system: %d\n", totalFans);

  for (i = 0; i < totalFans; i++) {
    printf("\nFan #%d:\n", i);
    sprintf(key, "F%dID", i);
    SMCReadKey(key, &val);
    if (val.dataSize > 0) {
      printf("    Fan ID       : %s\n", val.bytes + 4);
    }
    sprintf(key, "F%dAc", i);
    SMCReadKey(key, &val);
    printf("    Actual speed : %.0f\n", getFloatFromVal(val));
    sprintf(key, "F%dMn", i);
    SMCReadKey(key, &val);
    printf("    Minimum speed: %.0f\n", getFloatFromVal(val));
    sprintf(key, "F%dMx", i);
    SMCReadKey(key, &val);
    printf("    Maximum speed: %.0f\n", getFloatFromVal(val));
    sprintf(key, "F%dSf", i);
    SMCReadKey(key, &val);
    printf("    Safe speed   : %.0f\n", getFloatFromVal(val));
    sprintf(key, "F%dTg", i);
    SMCReadKey(key, &val);
    printf("    Target speed : %.0f\n", getFloatFromVal(val));
    SMCReadKey("FS! ", &val);
    if (val.dataSize > 0) {
      if ((_strtoul((char *)val.bytes, 2, 16) & (1 << i)) == 0)
        printf("    Mode         : auto\n");
      else
        printf("    Mode         : forced\n");
    } else {
      sprintf(key, "F%dMd", i);
      SMCReadKey(key, &val);
      if (getFloatFromVal(val))
        printf("    Mode         : forced\n");
      else
        printf("    Mode         : auto\n");
    }
  }

  return kIOReturnSuccess;
}

kern_return_t SMCWriteSimple(char key[5], char *wvalue, io_connect_t conn) {
  kern_return_t result;
  SMCVal val;
  uint32_t i;
  char c[3];
  for (i = 0; i < strlen(wvalue); i++) {
    sprintf(c, "%c%c", wvalue[i * 2], wvalue[(i * 2) + 1]);
    val.bytes[i] = (int)strtol(c, NULL, 16);
  }
  val.dataSize = i / 2;
  strcpy(val.key, key);
  result = SMCWriteKey2(val, conn);
  if (result != kIOReturnSuccess)
    printf("Error: SMCWriteKey() = %08x\n", result);

  return result;
}

}  // namespace

namespace MacGenius {
namespace Darwin {
namespace SMC {
static bool initialized_ = false;

void SMCInitialize() {
  if (initialized_) {
    return;
  }
  smc_init();
  initialized_ = true;
}

void SMCDestroy() {
  if (!initialized_) {
    return;
  }
  smc_close();
  initialized_ = false;
}

vector<pair<string, int>> SMCGetCPUTemperatures() {
  if (!initialized_) {
    return {};
  }
  kern_return_t result;
  SMCKeyData inputStructure;
  SMCKeyData outputStructure;

  int totalKeys, i;
  char key[5];
  SMCVal val{};

  totalKeys = SMCReadIndexCount();
  vector<pair<string, int>> temps;
  for (i = 0; i < totalKeys; i++) {
    memset(&inputStructure, 0, sizeof(SMCKeyData));
    memset(&outputStructure, 0, sizeof(SMCKeyData));
    memset(&val, 0, sizeof(SMCVal));

    inputStructure.data8 = SMC_CMD_READ_INDEX;
    inputStructure.data32 = i;

    result = SMCCall(KERNEL_INDEX_SMC, &inputStructure, &outputStructure);
    if (result != kIOReturnSuccess) continue;

    _ultostr(key, outputStructure.key);
    if (strncmp(key, "TC", 2) != 0 || !isdigit(key[2]) || key[3] != 'C')
      continue;

    SMCReadKey(key, &val);
    if (strcmp(val.dataType, DATATYPE_SP78) == 0 && val.dataSize == 2) {
      temps.push_back({string(key), ConvertSP78(&val)});
    }
  }

  return temps;
}

}  // namespace SMC
}  // namespace Darwin
}  // namespace MacGenius
