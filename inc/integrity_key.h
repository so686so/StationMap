#ifndef __INTEGRITY_KEY_H__
#define __INTEGRITY_KEY_H__

/*
    쓰레드 간 무결성 키 값 참조를 위한 CUSTOM Key Library
*/

#include "key.h"
#include "thread.h"

typedef struct _tKEY_CONTROL {
	BOOL    bKeyUpdated;
    int     nKeyValue;
    MUTEX_T *pMtxKey;
} KEY;

KEY *CreateIntegrityKey( MUTEX_T* mtx );
void DestroyIntegrityKey( KEY *pKey );

void SetKeyValueIntegrity( KEY *pKey, int nValue );
int  GetKeyValueIntegrity( KEY *pKey );

#endif