#ifndef __LKTMP_H
#define __LKTMP_H

#include <stdio.h>
#include <stdint.h>

#define CA_DEBUG(format,...) \
do {\
		printf("[DEBUG] %s: "format, __func__, ##__VA_ARGS__); \
	} while(0)

#define CA_ERROR(format,...) \
do {\
		printf("[ERROR] %s: "format, __func__, ##__VA_ARGS__); \
	} while(0)

#define QTEE_RSA_MAX_BITS		2048
#define QTEE_RSA_MAX_LENGTH		(QTEE_RSA_MAX_BITS/8)


typedef struct {
	uint8_t  key[QTEE_RSA_MAX_LENGTH];
	uint32_t key_len;
}QTEE_RSA_NUM;

typedef struct {
	uint8_t  enc[QTEE_RSA_MAX_LENGTH+32];
	uint32_t enc_len;
}QTEE_RSA_ENC;

typedef struct QTEE_RSA {
	char magic[8];
	uint32_t flags;
	QTEE_RSA_NUM E;
	QTEE_RSA_NUM N;
	QTEE_RSA_NUM D;
	QTEE_RSA_ENC ED;
    uint8_t token[32];
}QTEE_RSA;




#endif
