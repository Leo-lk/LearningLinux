#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "lktmp.h"

static void QTEE_RSA_append_deinit(QTEE_RSA *rsa)
{
    if (rsa != NULL)
    {
        for(int i; i < 4; i++){
        if((((QTEE_RSA_NUM *)(*((uint32_t *)(rsa->token) + i)))) != NULL){
            free((QTEE_RSA_NUM *)(*((uint32_t *)(rsa->token) + i)));
            }
        }
    }
}
static QTEE_RSA *QTEE_RSA_append_init(QTEE_RSA *rsa)
{
    QTEE_RSA_NUM *buf = NULL;

    if (rsa == NULL)
    {
        CA_ERROR("memory is not enough\n");
        goto err_exit;
    }

    memset(rsa->ED.enc, 0x00, sizeof(rsa->ED.enc));//p
    memset(rsa->token, 0x00, sizeof(rsa->token));
    for(int i = 0; i < 4; i++){
        buf = (QTEE_RSA_NUM *)malloc(sizeof(QTEE_RSA_NUM));
        if (buf == NULL){
            CA_ERROR("memory is not enough\n");
            goto err_exit;
        }

        for (int k = 0; k < 5; k++)
            buf->key[k] = 0xFF-k-i;

        CA_DEBUG("buf to token\n");
        *((uint32_t *)(rsa->token) + i) = buf;
         CA_DEBUG("token%d toaddr = %p, bufaddr = %p\n",i, ((QTEE_RSA_NUM *)(*((uint32_t *)(rsa->token) + i))), buf);
        QTEE_RSA_NUM *p_qtee =  ((QTEE_RSA_NUM *)(*((uint32_t *)(rsa->token) + i)));
        
        CA_DEBUG("token%d qtaddr = %p, bufaddr = %p\n",i, p_qtee, buf);
        CA_DEBUG("token%d keyvalue is :\n",i);
        for (int j = 0; j < 5; j++)
            CA_DEBUG("%x \n", buf->key[j]);
        for (int j = 0; j < 5; j++)
            CA_DEBUG("%x \n", p_qtee->key[j]);
        
    }

    return rsa;

err_exit:
    QTEE_RSA_append_deinit(rsa);
    return NULL;
}

QTEE_RSA *QTEE_RSA_init(void)
{
    QTEE_RSA *rsa = NULL;

    if ((rsa = malloc(sizeof(QTEE_RSA))) == NULL) {
        CA_ERROR("memory is not enough\n");
        return NULL;
    }

    
    memset(rsa->N.key, 0x00, sizeof(rsa->N.key));
    memset(rsa->E.key, 0x00, sizeof(rsa->E.key));
    memset(rsa->D.key, 0x00, sizeof(rsa->D.key));
    CA_DEBUG("start append init\n");

    if(QTEE_RSA_append_init(rsa) == NULL){
        CA_ERROR("memory is not enough\n");
        return NULL;
    }

    return rsa;
}

void QTEE_RSA_deinit(QTEE_RSA *rsa)
{
    if (rsa != NULL)
    {
        QTEE_RSA_append_deinit(rsa);
        free(rsa);
    }
}


void rw_tmp(QTEE_RSA *key)
{
    QTEE_RSA_NUM *p_qtee =  (QTEE_RSA_NUM *)(*((uint32_t *)(key->token) + 0));
    QTEE_RSA_NUM *q_qtee =  (QTEE_RSA_NUM *)(*((uint32_t *)(key->token) + 1));
    QTEE_RSA_NUM *dmp1_qtee =  (QTEE_RSA_NUM *)(*((uint32_t *)(key->token) + 2));
    QTEE_RSA_NUM *dmq1_qtee =  (QTEE_RSA_NUM *)(*((uint32_t *)(key->token) + 3));
    QTEE_RSA_NUM *iqmp_qtee =  (QTEE_RSA_NUM *)(*((uint32_t *)(key->token) + 4));

    for(int i; i < 4; i++)
        CA_DEBUG("start import :p, addr = %p, %x\n", p_qtee, (*((uint32_t *)(key->token) + i)));

    memset(q_qtee, 0x34, q_qtee->key_len);
    for (int j = 0; j < 5; j++)
            CA_DEBUG("%x \n", q_qtee->key[j]);

}


int main(int argc, char *argv[])
{
    CA_DEBUG("start \n");

    QTEE_RSA *rsa = QTEE_RSA_init();

    rw_tmp(rsa);

    QTEE_RSA_append_deinit(rsa);

    return 0;
}



