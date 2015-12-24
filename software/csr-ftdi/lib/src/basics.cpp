#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>

#include "dllmain.h"
#include "spifns.h"
#include "spi.h"
#include "compat.h"
#include "logging.h"

/*
 * This is based on CsrSpiDrivers/spilpt.fixed, converted to our SPI impl and
 * with SPI API 1.4 spifns_stream_* functions added as wrappers around 1.3 API
 */

#define VARLIST_SPISPORT 0
#define VARLIST_SPIMUL 1
#define VARLIST_SPICLOCK 3
#define VARLIST_SPICMDBITS 4
#define VARLIST_SPICMDREADBITS 5
#define VARLIST_SPICMDWRITEBITS 6
#define VARLIST_SPIMAXCLOCK 7
#define VARLIST_FTDI_BASE_CLOCK 8
#define VARLIST_FTDI_LOG_LEVEL 9
#define VARLIST_FTDI_LOG_FILE 10

const SPIVARDEF g_pVarList[]={
    {"SPIPORT","1",1},
    {"SPIMUL","0",0},
    {"SPISHIFTPERIOD","0",0},   /* Unused */
    {"SPICLOCK","0",0},
    {"SPICMDBITS","0",0},
    {"SPICMDREADBITS","0",0},
    {"SPICMDWRITEBITS","0",0},
    {"SPIMAXCLOCK","1000",0},
    {"FTDI_BASE_CLOCK","2000000",0},
    {"FTDI_LOG_LEVEL","warn",0},
    {"FTDI_LOG_FILE","stderr",0}
};

int g_nSpiPort=1;
char g_szErrorString[256]="No error";
unsigned int g_nError=SPIERR_NO_ERROR;
unsigned short g_nErrorAddress=0;
static uint32_t spifns_api_version = 0;

/* We support only one stream currently - stream 0 */
#define STREAM      ((spifns_stream_t)0)
#define NSTREAMS    1

#define SET_ERROR(n, s) do { \
        g_nError = (n); \
        strncpy(g_szErrorString, (s), sizeof(g_szErrorString)); \
        g_szErrorString[sizeof(g_szErrorString) - 1] = '\0'; \
    } while (0)

static int spifns_sequence_setvar(const char *szName, const char *szValue);

DLLEXPORT void spifns_getvarlist(const SPIVARDEF **ppList, unsigned int *pnCount) {
    LOG(DEBUG, "");

    *ppList=g_pVarList;
    *pnCount=sizeof(g_pVarList)/sizeof(*g_pVarList);
}

/* Try to initialize variables from environment earlier than pttransport
 * will send them to us */
static int spifns_init_vars_from_env(void)
{
    const char *var, *val;
    unsigned int ii;

    for (ii = 0; ii < sizeof(g_pVarList)/sizeof(g_pVarList[0]); ii++) {
        var = g_pVarList[ii].szName;
        val = getenv(var);
        if (val != NULL && val[0] != '\0') {
            if (spifns_sequence_setvar(var, val) !=0)
                return -1;
        }
    }
    return 0;
}

DLLEXPORT int DLLEXPORT spifns_init() {
    LOG(DEBUG, "");

    spi_set_err_buf(g_szErrorString, sizeof(g_szErrorString));

    if (spifns_init_vars_from_env() < 0)
        return -1;

    if (!spifns_api_version) {
        if (pttrans_api_version)
            spifns_api_version = pttrans_api_version;
        else
            spifns_api_version = SPIFNS_API_1_4;
    }

    LOG(INFO, "Detected SPI API version 0x%04x, using version 0x%04x",
            pttrans_api_version, spifns_api_version);

    if (spi_init() < 0)
        return -1;

    return 0;
}

DLLEXPORT const char * spifns_getvar(const char *szName) {
    LOG(DEBUG, "(%s)", szName);
    if (!szName) {
        return "";
    } else if (stricmp(szName,"SPIPORT")==0) {
        static char szReturn[20];
        snprintf(szReturn, sizeof(szReturn), "%d", g_nSpiPort);
        return szReturn;
    } else if (stricmp(szName,"SPIMUL")==0) {
        return "-1";
    } else if (stricmp(szName,"SPISHIFTPERIOD")==0) {
        return "1";
    } else if (stricmp(szName,"SPICLOCK")==0) {
        static char szReturn[64];
        snprintf(szReturn, sizeof(szReturn), "%lu", spi_get_clock());
        return szReturn;
    } else if (stricmp(szName,"SPIMAXCLOCK")==0) {
        static char szReturn[24];
        snprintf(szReturn, sizeof(szReturn), "%lu", spi_get_max_clock());
        return szReturn;
    } else {
        return "";
    }
}

DLLEXPORT unsigned int spifns_get_last_error(unsigned short *pnErrorAddress, const char **pszErrorString) {
    LOG(DEBUG, "");
    if (pnErrorAddress)
        *pnErrorAddress=g_nErrorAddress;
    if (pszErrorString)
        *pszErrorString=g_szErrorString;
    return g_nError;
}

DLLEXPORT void spifns_clear_last_error(void)
{
    LOG(DEBUG, "");
    static const char szError[]="No error";
    memcpy(g_szErrorString,szError,sizeof(szError));
    g_nErrorAddress=0;
    g_nError=SPIERR_NO_ERROR;
}

/* Don't use pttransport's debug callback, causes crash in pttransport.dll */
DLLEXPORT void spifns_set_debug_callback(spifns_debug_callback pCallback) {
}

/* In BlueSuite 2.6 this is called before spifns_init(), in BlueSuite 2.3 - after. */
DLLEXPORT uint32_t spifns_get_version() {
    if (!spifns_api_version) {
        if (pttrans_api_version)
            spifns_api_version = pttrans_api_version;
        else
            spifns_api_version = SPIFNS_API_1_4;
    }

    return spifns_api_version;
}

static void spifns_close_port() {
    LOG(DEBUG, "");
    /*
     * Some CSR apps (e.g. BlueFlash 2.6.0) init SPI and open/close SPI port in
     * chaotic fashion. For now keep port always open until other port is
     * selected or DLL is detached.
     */
    /*spi_close();*/
}

DLLEXPORT void spifns_close() {
    LOG(DEBUG, "");
    spifns_close_port();

    /* XXX See comment in spifns_close_port() */
    /*spi_deinit();*/
}

DLLEXPORT void spifns_chip_select(int nChip) {
    LOG(DEBUG, "(%d)", nChip);
}

DLLEXPORT const char* spifns_command(const char *szCmd) {
    LOG(DEBUG, "(%s)", szCmd);
    if (stricmp(szCmd,"SPISLOWER")==0) {
        if (spi_clock_slowdown() < 0) {
            LOG(ERR, "spi_clock_slowdown() failed");
            return 0;
        }
    }
    return 0;
}

DLLEXPORT void spifns_enumerate_ports(spifns_enumerate_ports_callback pCallback, void *pData) {
    struct spi_port *portlist;
    int nports, nport;

    LOG(DEBUG, "");

    if (spi_get_port_list(&portlist, &nports) < 0)
        return;

    if (nports == 0) {
        /* Some apps, CSR86XX ROM ConfigTool 3.0.48 in particular, crash when
         * no ports present. Return some dummy port for it if we can't find
         * any. */
        LOG(WARN, "No FTDI device found, calling port enum callback "
                "(1, \"No FTDI device found\", %p)", pData);
        pCallback(1, "No FTDI device found", pData);
        return;
    }

    for (nport = 0; nport < nports; nport++) {
        LOG(DEBUG, "Calling port enum callback (%d, \"%s\", %p)",
                nport + 1, portlist[nport].name, pData);
        /* Ports start with 1 */
        pCallback(nport + 1, portlist[nport].name, pData);
    }
}

static bool spifns_sequence_setvar_spiport(int nPort) {
    LOG(DEBUG, "(%d)", nPort);

    if (spi_isopen())
        spi_close();
    if (spi_open(nPort - 1) < 0)
        return false;
    g_nSpiPort=nPort;
    return true;
}

static int spifns_sequence_write(unsigned short nAddress, unsigned short nLength, unsigned short *pnInput) {
    uint8_t outbuf1[] = {
        0x02,                       /* Command: write */
        (uint8_t)(nAddress >> 8),   /* Address high byte */
        (uint8_t)(nAddress & 0xff), /* Address low byte */
    };

    LOG(DEBUG, "(0x%04x, %d, %p)", nAddress, nLength, pnInput);

#define _ERR_RETURN(n, s) do { \
        SET_ERROR((n), (s)); \
        goto error; \
    } while (0)

    if (!spi_isopen())
        _ERR_RETURN(SPIERR_NO_LPT_PORT_SELECTED, "No FTDI device selected");

    DUMP(pnInput, nLength << 1, "write16(addr=0x%04x, len16=%d)",
            nAddress, nLength);

#ifdef ENABLE_LEDS
    spi_led(SPI_LED_WRITE);
#endif

    if (spi_xfer_begin(0) < 0)
        _ERR_RETURN(SPIERR_READ_FAILED, "Unable to begin transfer");

    if (spi_xfer(SPI_XFER_WRITE, 8, outbuf1, 3) < 0)
        _ERR_RETURN(SPIERR_READ_FAILED, "Unable to start write");

    if (spi_xfer(SPI_XFER_WRITE, 16, pnInput, nLength) < 0) {
        _ERR_RETURN(SPIERR_READ_FAILED, "Unable to write (writing buffer)");
    }

    if (spi_xfer_end() < 0)
        _ERR_RETURN(SPIERR_READ_FAILED, "Unable to end transfer");

    return 0;

#undef _ERR_RETURN

error:
    if (g_nError != SPIERR_NO_ERROR) {
        g_nErrorAddress=nAddress;
        LOG(ERR, "%s", g_szErrorString);
    }
    return 1;
}

static int spifns_sequence_setvar(const char *szName, const char *szValue) {
    LOG(DEBUG, "(%s, %s)", szName, szValue);
    if (szName==0)
        return 1;
    if (szValue==0)
        return 1;
    long nValue=strtol(szValue,0,0);
    for (unsigned int i=0; i<(sizeof(g_pVarList)/sizeof(*g_pVarList)); i++) {
        if (stricmp(szName,g_pVarList[i].szName)==0) {
            switch (i) {
            case VARLIST_SPISPORT:{
                if (!spifns_sequence_setvar_spiport(nValue)) {
                    const char szError[]="Couldn't find SPI port";
                    memcpy(g_szErrorString,szError,sizeof(szError));
                    return 1;
                }
                                  }break;
            case VARLIST_SPICLOCK:
                if (nValue <= 0)
                    return 1; //ERROR!
                if (spi_set_clock((unsigned long)nValue) < 0) {
                    const char szError[]="Couldn't set SPI clock";
                    memcpy(g_szErrorString,szError,sizeof(szError));
                    return 1;
                }
                break;
            case VARLIST_SPIMAXCLOCK:
                if (nValue <= 0) {
                    const char szError[]="SPIMAXCLOCK value should be positive integer";
                    memcpy(g_szErrorString,szError,sizeof(szError));
                    return 1;
                }
                spi_set_max_clock((unsigned long)nValue);
                break;

            case VARLIST_FTDI_BASE_CLOCK:
                LOG(WARN, "Setting FTDI_BASE_CLOCK is deprecated, use SPIMAXCLOCK instead");
                if (nValue <= 0) {
                    const char szError[]="FTDI_BASE_CLOCK value should be positive integer";
                    memcpy(g_szErrorString,szError,sizeof(szError));
                    return 1;
                }
                spi_set_max_clock((unsigned long)nValue / 2000);
                break;
            case VARLIST_FTDI_LOG_LEVEL:
                {
                    char *val, *cp, *tok;
                    uint32_t lvl;

                    val = strdup(szValue);
                    if (val == NULL)
                        return 1;

                    cp = val;
                    lvl = 0;
                    while ((tok = strtok(cp, ",")) != NULL) {
                        cp = NULL;
                        switch (toupper(tok[0])) {
                        case 'Q':
                            lvl = (lvl & ~LOG_LEVEL_MASK) | LOG_LEVEL_QUIET;
                            break;
                        case 'E':
                            lvl = (lvl & ~LOG_LEVEL_MASK) | LOG_LEVEL_ERR;
                            break;
                        case 'W':
                            lvl = (lvl & ~LOG_LEVEL_MASK) | LOG_LEVEL_WARN;
                            break;
                        case 'I':
                            lvl = (lvl & ~LOG_LEVEL_MASK) | LOG_LEVEL_INFO;
                            break;
                        case 'D':
                            if (toupper(tok[1]) == 'E') /* DEBUG */
                                lvl = (lvl & ~LOG_LEVEL_MASK) | LOG_LEVEL_DEBUG;
                            else    /* DUMP */
                                lvl |= LOG_FLAGS_DUMP;
                            break;
                        default:
                            free(val);
                            return 1;
                        }
                    }

                    free(val);
                    log_set_options(lvl);
                }
                break;
            case VARLIST_FTDI_LOG_FILE:
                if (!stricmp(szValue, "stdout")) {
                    log_set_dest(stdout);
                } else if (!stricmp(szValue, "stderr")) {
                    log_set_dest(stderr);
                } else {
                    if (log_set_file(szValue) < 0) {
                        const char szError[]="Couldn't open log file";
                        memcpy(g_szErrorString,szError,sizeof(szError));
                        return 1;
                    }
                }
                break;
            }
        }
    }
    return 0;
}

static int spifns_sequence_read(unsigned short nAddress, unsigned short nLength, unsigned short *pnOutput) {
    uint8_t outbuf[] = {
        3,                          /* Command: read */
        (uint8_t)(nAddress >> 8),   /* Address high byte */
        (uint8_t)(nAddress & 0xff), /* Address low byte */
    };
    uint8_t inbuf1[2];

    LOG(DEBUG, "(0x%02x, %d, %p)", nAddress, nLength, pnOutput);

#define _ERR_RETURN(n, s) do { \
        SET_ERROR((n), (s)); \
        goto error; \
    } while (0)

    if (!spi_isopen())
        _ERR_RETURN(SPIERR_NO_LPT_PORT_SELECTED, "No FTDI device selected");

#ifdef ENABLE_LEDS
    spi_led(SPI_LED_READ);
#endif

    if (spi_xfer_begin(0) < 0)
        _ERR_RETURN(SPIERR_READ_FAILED, "Unable to begin transfer");

    if (spi_xfer(SPI_XFER_WRITE, 8, outbuf, 3) < 0)
        _ERR_RETURN(SPIERR_READ_FAILED, "Unable to start read");

    if (spi_xfer(SPI_XFER_READ, 8, inbuf1, 2) < 0)
        _ERR_RETURN(SPIERR_READ_FAILED,
                "Unable to start read (getting control data)");

    if (inbuf1[0] != 3 || inbuf1[1] != (nAddress >> 8)) {
        _ERR_RETURN(SPIERR_READ_FAILED,
                "Unable to start read (invalid control data)");
        LOG(ERR, "Control data: 0x%02x 0x%02x", inbuf1[0], inbuf1[1]);
    }

    if (spi_xfer(SPI_XFER_READ, 16, pnOutput, nLength) < 0) {
        _ERR_RETURN(SPIERR_READ_FAILED, "Unable to read (reading buffer)");
    }

    if (spi_xfer_end() < 0)
        _ERR_RETURN(SPIERR_READ_FAILED, "Unable to end transfer");

    DUMP(pnOutput, nLength << 1, "read16(addr=0x%04x, len16=%d)",
            nAddress, nLength);

    return 0;

#undef _ERR_RETURN

error:
    if (g_nError != SPIERR_NO_ERROR) {
        g_nErrorAddress=nAddress;
        LOG(ERR, "%s", g_szErrorString);
    }
    return 1;
}

DLLEXPORT int spifns_sequence(SPISEQ *pSequence, unsigned int nCount) {
    int nRetval=0;

    LOG(DEBUG, "(%p, %d)", pSequence, nCount);

    while (nCount--) {
        LOG(DEBUG, "command %d", pSequence->nType);
        switch (pSequence->nType) {
        case SPISEQ::TYPE_READ:{
            if (spifns_sequence_read(pSequence->rw.nAddress,pSequence->rw.nLength,pSequence->rw.pnData)==1)
                nRetval=1;
                               }break;
        case SPISEQ::TYPE_WRITE:{
            if (spifns_sequence_write(pSequence->rw.nAddress,pSequence->rw.nLength,pSequence->rw.pnData)==1)
                nRetval=1;
                                }break;
        case SPISEQ::TYPE_SETVAR:{
            if (spifns_sequence_setvar(pSequence->setvar.szName,pSequence->setvar.szValue)==1)
                nRetval=1;
                                 }break;
        default:
            LOG(WARN, "Sequence command not implemented: %d", pSequence->nType);
            g_nError = SPIFNS_ERROR_INVALID_PARAMETER;
            snprintf(g_szErrorString, sizeof(g_szErrorString),
                    "sequence command %d not implemented", pSequence->nType);
            nRetval = 1;

        }
        pSequence++;
    }
    return nRetval;
}

DLLEXPORT int spifns_bluecore_xap_stopped() {
    /* Read chip version */
    uint8_t outbuf[] = {
        3,      /* Command: read */
        GBL_CHIP_VERSION_GEN1_ADDR >> 8,   /* Address high byte */
        GBL_CHIP_VERSION_GEN1_ADDR & 0xff,   /* Address low byte */
    };
    uint8_t inbuf[2];
    int status;

    LOG(DEBUG, "");

    status = spi_xfer_begin(1);
    if (status < 0)
        return SPIFNS_XAP_NO_REPLY;
    if (spi_xfer(SPI_XFER_WRITE, 8, outbuf, 3) < 0)
        return SPIFNS_XAP_NO_REPLY;
    if (spi_xfer(SPI_XFER_READ, 8, inbuf, 2) < 0)
        return SPIFNS_XAP_NO_REPLY;
    if (spi_xfer_end() < 0)
        return SPIFNS_XAP_NO_REPLY;
    DUMP(inbuf, 2, "read8(addr=0x%04x, len=%d)", GBL_CHIP_VERSION_GEN1_ADDR, 3);

    if (status == SPI_CPU_RUNNING) {
        LOG(DEBUG, "CPU is running");
        return SPIFNS_XAP_RUNNING;
    }

    if (inbuf[0] != 3 || inbuf[1] != (GBL_CHIP_VERSION_GEN1_ADDR >> 8)) {
        /* No chip present or not responding correctly, no way to find out. */
        LOG(ERR, "No reply from XAP");
        return SPIFNS_XAP_NO_REPLY;
    }

    LOG(DEBUG, "CPU is stopped");
    return SPIFNS_XAP_STOPPED;
}

/* This is a limited implementation of CSR SPI API 1.4. It supports only 1
 * stream and does not support all of the features. */

DLLEXPORT int spifns_stream_init(spifns_stream_t *p_stream)
{
    LOG(DEBUG, "(%p)", p_stream);
    int rc;
    
    rc = spifns_init();
    if (rc == 0)
        *p_stream = STREAM;
    
    return rc;
}

DLLEXPORT void spifns_stream_close(spifns_stream_t stream)
{
    LOG(DEBUG, "(%d)", stream);
    if (SPIFNS_STREAMS_EQUAL(stream, STREAM))
        spifns_close();
}

DLLEXPORT unsigned int spifns_count_streams(void)
{
    LOG(DEBUG, "");
    return NSTREAMS;
}

DLLEXPORT int spifns_stream_sequence(spifns_stream_t stream, SPISEQ_1_4 *pSequence, int nCount)
{
    int nRetval=0;

    LOG(DEBUG, "(%d, %p, %d)", stream, pSequence, nCount);

    while (nCount--) {
        LOG(DEBUG, "command %d", pSequence->nType);
        switch (pSequence->nType) {
        case SPISEQ_1_4::TYPE_READ:
            if (spifns_sequence_read(pSequence->rw.nAddress,pSequence->rw.nLength,pSequence->rw.pnData)==1)
                nRetval=1;
            break;
        case SPISEQ_1_4::TYPE_WRITE:
            if (spifns_sequence_write(pSequence->rw.nAddress,pSequence->rw.nLength,pSequence->rw.pnData)==1)
                nRetval=1;
            break;
        case SPISEQ_1_4::TYPE_SETVAR:
            if (spifns_sequence_setvar(pSequence->setvar.szName,pSequence->setvar.szValue)==1)
                nRetval=1;
            break;
        default:
            LOG(WARN, "Sequence command not implemented: %d", pSequence->nType);
            g_nError = SPIFNS_ERROR_INVALID_PARAMETER;
            snprintf(g_szErrorString, sizeof(g_szErrorString),
                    "sequence command %d not implemented", pSequence->nType);
            nRetval = 1;
        }
        pSequence++;
    }
    return nRetval;
}

DLLEXPORT const char* spifns_stream_command(spifns_stream_t stream, const char *command)
{
    LOG(DEBUG, "(%d, %s)", stream, command);
    return spifns_command(command);
}

DLLEXPORT const char* spifns_stream_getvar(spifns_stream_t stream, const char *var)
{
    LOG(DEBUG, "(%d, %s)", stream, var);
    return spifns_getvar(var);
}

DLLEXPORT void spifns_stream_chip_select(spifns_stream_t stream, int which)
{
    LOG(DEBUG, "(%d, %d)", stream, which);
    spifns_chip_select(which);
}

DLLEXPORT int spifns_stream_bluecore_xap_stopped(spifns_stream_t stream)
{
    LOG(DEBUG, "(%d)", stream);
    return spifns_bluecore_xap_stopped();
}

/* returns the last error code, and if a pointer is passed in, the problematic
 * address.*/
/* get_last_error and clear_last_error both deal with the error that occurred
 * in the current thread */
DLLEXPORT int spifns_get_last_error32(uint32_t *addr, const char ** buf)
{
    unsigned short saddr;
    int rc;

    LOG(DEBUG, "(%p, %p)", addr, buf);

    rc = spifns_get_last_error(&saddr, buf);
    if (addr)
        *addr = saddr;
    return rc;
}

DLLEXPORT void spifns_stream_set_debug_callback(spifns_stream_t stream, spifns_debug_callback fn, void *pvcontext)
{
    LOG(DEBUG, "(%d, %p, %p)", stream, fn, pvcontext);
    spifns_set_debug_callback(fn);
}

DLLEXPORT int spifns_stream_get_device_id(spifns_stream_t stream, char *buf, size_t length)
{
    LOG(DEBUG, "(%d, %p, %u)", stream, buf, length);
    snprintf(buf, length, "csr-spi-ftdi-%d", STREAM);
    return 0;
}

DLLEXPORT int spifns_stream_lock(spifns_stream_t stream, uint32_t timeout)
{
    LOG(DEBUG, "(%d, %u)", stream, timeout);
    return 0;
}

DLLEXPORT void spifns_stream_unlock(spifns_stream_t stream)
{
    LOG(DEBUG, "(%d)", stream);
}
