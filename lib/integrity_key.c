#include "integrity_key.h"

KEY *CreateIntegrityKey( MUTEX_T* mtx )
{
    KEY *pKey = (KEY *)malloc(sizeof(KEY));

    if( pKey == NULL ) {
        ERROR_TRACE("CreateIntegrityKey Failed.\n");
        return  NULL;
    }

    pKey->bKeyUpdated   = FALSE;
    pKey->nKeyValue     = KEY_NON_INPUT;    
    pKey->pMtxKey       = mtx;

    MUTEX_Init( pKey->pMtxKey );

    return pKey;
}

void DestroyIntegrityKey( KEY *pKey )
{
    if( pKey == NULL ) return;

	MUTEX_Term( pKey->pMtxKey );

    free( pKey );
    pKey = NULL;
}

void SetKeyValueIntegrity( KEY *pKey, int nValue )
{
    if( pKey == NULL ) return;

    MUTEX_Enter( pKey->pMtxKey );
    pKey->nKeyValue     = nValue;
    pKey->bKeyUpdated   = TRUE;
    MUTEX_Leave( pKey->pMtxKey );
}

int GetKeyValueIntegrity( KEY *pKey )
{
    if( pKey == NULL ) return KEY_NON_INPUT;

    int ret = KEY_NON_INPUT;

    MUTEX_Enter( pKey->pMtxKey );
    if( pKey->bKeyUpdated == FALSE ) {
        ret = KEY_NON_INPUT;
    } else {
        ret = pKey->nKeyValue;
        pKey->bKeyUpdated = FALSE;
    }
    MUTEX_Leave( pKey->pMtxKey );

    return ret;
}