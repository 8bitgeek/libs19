/****************************************************************************
 * Copyright (c) 2020 Mike Sharkey <mike@pikeaero.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 * DEALINGS IN THE SOFTWARE.
 ****************************************************************************/
#include <srecreader.h>

#if defined(__BARE_METAL__)
    #include <board.h>
    #include <stddef.h>
    static void*  memset(void *s, int c, size_t n);
    static size_t strlen(const char * str);
#elif defined(_CARIBOU_RTOS_)
    #include <board.h>
    #include <caribou/lib/string.h>
#else 
    #include <string.h>
#endif



static int srec_readline( SREC_FILE* fil, char *input, int len );

extern void srec_reader_init (  srec_reader_t*      reader, 
                                SREC_FILE*          fil, 
                                srec_callback_fn_t  meta_fn, 
                                srec_callback_fn_t  store_fn, 
                                srec_callback_fn_t  term_fn,
                                srec_callback_fn_t  fault_fn,
                                char*               line_buffer, 
                                int                 max_line_len,
                                void*               arg )
{
    memset(reader,0,sizeof(srec_reader_t));
    reader->fil          = fil;
    reader->meta_fn      = meta_fn;
    reader->store_fn     = store_fn;
    reader->term_fn      = term_fn;
    reader->fault_fn     = fault_fn;
    reader->line_buffer  = line_buffer;
    reader->max_line_len = max_line_len;
    reader->arg          = arg;
}

extern int srec_reader_read_once ( srec_reader_t* reader )
{
    int rc;
    if ( (rc=srec_readline( reader->fil, reader->line_buffer, reader->max_line_len )) > 0 )
    {
        switch( srec_parse( reader->line_buffer, &reader->record ) )
        {
            case S0:
                if ( reader->meta_fn != NULL )
                    reader->meta_fn( reader );
                break;
            case S1:
            case S2:
            case S3:
                if ( reader->store_fn != NULL)
                    reader->store_fn( reader );
                break;
            case S4: 
                break;
            case S5:
            case S6:
                break;
            case S7:
            case S8:
            case S9:
                if ( reader->term_fn != NULL )
                    reader->term_fn( reader );
                break;
            case InvalidType:
            case InvalidCRC: 
                if ( reader->fault_fn != NULL )
                    reader->fault_fn( reader );
                break;  
        }
    }
    return rc;
}

extern void srec_reader_read( srec_reader_t* reader )
{
    while ( srec_reader_read_once ( reader ) > 0 );
}

static int srec_readline( SREC_FILE* fil, char *input, int len )
{
    if ( srec_fgets( input, len, fil ) != NULL )
    {
        return strlen(input);
    }
    return -1;
}

#if defined(__BARE_METAL__)

    static void *memset(void *s, int c, size_t n)
    {
        unsigned char* p=s;
        while(n--) *p++ = c;
        return s;
    }

    static size_t strlen(const char * str)
    {
        register size_t n=0;
        if ( str )
        {
            for( register char* p=(char*)str; *p++; n++ );
        }
        return n;
    }

#endif
