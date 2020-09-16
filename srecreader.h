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
#ifndef _SRECREADER_H_
#define _SRECREADER_H_

#include <stdint.h>
#include <stdio.h>
#include <srec.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct _srec_reader_t_
{
	FILE*		        fil;			
	srec_result_t       record;
    int (*meta_fn)(struct _srec_reader_t_*);    /* meta-record callback */
    int (*store_fn)(struct _srec_reader_t_*);   /* store payload callbacl */
    int (*term_fn)(struct _srec_reader_t_*);    /* termination / entry-point callabck */
    char*               line_buffer;
    int                 max_line_len;
} srec_reader_t;

typedef int  (*srec_callback_fn_t)(srec_reader_t*);
typedef void (*srec_entry_pt_t)(void);

extern void srec_reader_init (  
                                srec_reader_t*      reader, 
                                FILE*               fil, 
                                srec_callback_fn_t  meta_fn, 
                                srec_callback_fn_t  store_fn, 
                                srec_callback_fn_t  term_fn,
                                char*               line_buffer, 
                                int                 max_line_len 
                             );
extern void srec_reader_read ( srec_reader_t* reader );

#ifdef __cplusplus
}
#endif

#endif

