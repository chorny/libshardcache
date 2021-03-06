#include "shardcache.h"

#define SHC_ESCAPE_BUFFER_SIZE_MAX (1<<16)

unsigned int shardcache_loglevel = 0;

int shardcache_log_initialized = 0;

unsigned long shardcache_byte_escape(char ch,
                             char esc,
                             char *buffer,
                             unsigned long len,
                             char **dest,
                             unsigned long *newlen)
{
    char *newbuf;
    unsigned long buflen;
    unsigned long i;
    unsigned long cnt;
    int escape;
    char *p;
    unsigned long off;

    if(len == 0)
        return 0;

    newbuf = (char *)malloc(len);
    if(!newbuf)
        return 0;
    buflen = len;
    p = buffer;
    off = 0;
    cnt = 0;
    for(i=0;i<len;i++)
    {
        escape = 0;
        if(*p == ch)
            cnt ++;

        if(*p == ch || *p == esc)
            escape = 1;

        if(escape)
        {
            buflen++;
            newbuf = (char *)realloc(newbuf, buflen+1);
            memcpy(newbuf+off, &esc, 1);
            off++;
        }
        memcpy(newbuf+off, p, 1);
        p++;
        off++;
    }
    *dest = newbuf;
    *newlen = buflen;
    return cnt;
}

char *shardcache_hex_escape(char *buf, int len, int limit, int include_prefix)
{
    int i;
    static __thread char str[SHC_ESCAPE_BUFFER_SIZE_MAX+6];

    int olen = (limit > 0 && limit < len) ? limit : len;

    if (olen > SHC_ESCAPE_BUFFER_SIZE_MAX/2)
        olen = SHC_ESCAPE_BUFFER_SIZE_MAX/2;

    char *p = str;
    if (include_prefix) {
        strcpy(str, "0x");
        p += 2;
    }
    for (i = 0; i < olen; i++) {
        sprintf(p, "%02x", (unsigned char)buf[i]);
        p+=2;
    }
    if (olen < len)
        strcat(str, "...");
    return str;
}

void shardcache_log_init(char *ident, int loglevel)
{
    shardcache_loglevel = loglevel;
    openlog(ident, LOG_CONS|LOG_PERROR, LOG_LOCAL0);
    setlogmask(LOG_UPTO(loglevel));
    shardcache_log_initialized = 1;
}

unsigned int shardcache_log_level()
{
    return shardcache_loglevel;
}

void shardcache_log_message(int prio, int dbglevel, const char *fmt, ...)
{
    char *newfmt = NULL;
    const char *prefix = NULL;

    switch (prio) {
        case LOG_ERR:
            prefix = "[ERROR]: ";
            break;
        case LOG_WARNING:
            prefix = "[WARNING]: ";
            break;
        case LOG_NOTICE:
            prefix = "[NOTICE]: ";
            break;
        case LOG_INFO:
            prefix = "[INFO]: ";
            break;
        case LOG_DEBUG:
            switch (dbglevel) {
                case 1:
                    prefix = "[DBG]: ";
                    break;
                case 2:
                    prefix = "[DBG2]: ";
                    break;
                case 3:
                    prefix = "[DBG3]: ";
                    break;
                case 4:
                    prefix = "[DBG4]: ";
                    break;
                case 5:
                    prefix = "[DBG5]: ";
                    break;
                default:
                    prefix = "[DBGX]: ";
                    break;
            }
            break;
        default:
            prefix = "[UNKNOWN]: ";
            break;
    }

    // ensure the user passed a valid 'fmt' pointer before proceeding
    if (prefix && fmt) { 
        newfmt = (char *)calloc(1, strlen(fmt)+strlen(prefix)+1);
        if (newfmt) { // safety belts in case we are out of memory
            sprintf(newfmt, "%s%s", prefix, fmt);
            va_list arg;
            va_start(arg, fmt);
            vsyslog(prio, newfmt, arg);
            va_end(arg);
            free(newfmt);
        }
    }
}

// vim: tabstop=4 shiftwidth=4 expandtab:
/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
