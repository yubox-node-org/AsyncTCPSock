#pragma once

// TODO: se debe quitar este #define para que pueda habilitarse a voluntad el SSL
// según el proyecto. Se coloca aquí para probar el desarrollo
#define ASYNC_TCP_SSL_ENABLED 1

#if ASYNC_TCP_SSL_ENABLED

#include "mbedtls/platform.h"
#include "mbedtls/net.h"
#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"

#define ASYNCTCP_TLS_CAN_RETRY(r) (((r) == MBEDTLS_ERR_SSL_WANT_READ) || ((r) == MBEDTLS_ERR_SSL_WANT_WRITE))

class AsyncTCP_TLS_Context
{
private:
    // These fields must persist for the life of the encrypted connection, destroyed on
    // object destructor.
    mbedtls_ssl_context ssl_ctx;
    mbedtls_ssl_config ssl_conf;
    mbedtls_ctr_drbg_context drbg_ctx;
    mbedtls_entropy_context entropy_ctx;

    // These allocate memory during handshake but must be freed on either success or failure
    mbedtls_x509_crt ca_cert;
    mbedtls_x509_crt client_cert;
    mbedtls_pk_context client_key;
    bool _have_ca_cert;
    bool _have_client_cert;
    bool _have_client_key;

    unsigned long handshake_timeout;
    unsigned long handshake_start_time;

    int _socket;



    // Delete certificates used in handshake
    void _deleteHandshakeCerts(void);
public:
    AsyncTCP_TLS_Context(void);
    virtual ~AsyncTCP_TLS_Context();

    int startSSLClient(int sck, const char * host_or_ip, const char *rootCABuff,
        const char *cli_cert, const char *cli_key, const char *pskIdent,
        const char *psKey, bool insecure);

    int runSSLHandshake(void);

    int write(const uint8_t *data, size_t len);

    int read(uint8_t * data, size_t len);
};

#endif // ASYNC_TCP_SSL_ENABLED