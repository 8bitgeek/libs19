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
#include <srec.h>
#include <string.h>
#include <caribou/lib/stdio.h>

static uint8_t  nibble         (uint8_t c);
static uint8_t 	ascii_hex_translate(uint8_t length, char* input, srec_result_t* output);
static uint8_t 	make_checksum  (uint8_t length, char* input);
static uint8_t  fetch_checksum (uint8_t length, char* input);
static uint8_t 	ascii_hex_8    (char* input);
static uint16_t ascii_hex_16   (char* input);
static uint32_t ascii_hex_24   (char* input);
static uint32_t ascii_hex_32   (char* input);

/**
 * @brief Parse the srecord format input ASCII text string
 * @param input ASCII text buffer containing one line of S record data
 * @param output A pointer to an srec output structure.
 * @return Output Status.
 */
srecord_t srec_parse(char* input, srec_result_t* output)
{
	memset(output,0,sizeof(srec_result_t));
	output->record = Invalid;
	if ( input )
	{
		if ( strlen(input) >= 10 && *input++ == 'S' ) // could be an S record?
		{
			uint8_t srec_type = *input++;
			uint8_t length = ascii_hex_8(input); 
			uint8_t calc_chk = make_checksum(length,input);
			uint8_t actual_chk = fetch_checksum(length,input);
			if ( calc_chk == actual_chk )
			{
				input += 2; // skip count
			
				switch(srec_type)
				{
					case '0':	
						output->record = S0;
						break;
					case '1':	
						output->address = ascii_hex_16(input);
						ascii_hex_translate(length-3,&input[4],output);
						output->record = S1;
						break;
					case '2':	
						output->address = ascii_hex_24(input);
						ascii_hex_translate(length-5,&input[6],output);
						output->record = S2;
						break;
					case '3':
						output->address = ascii_hex_32(input);
						ascii_hex_translate(length-5,&input[8],output);
						output->record = S3;
						break;
					case '4':
						output->record = Invalid;
						break;
					case '5':
						output->record = S5;
						break;
					case '6':
						output->record = S6;
						break;
					case '7':	
						output->address = ascii_hex_32(input);
						output->record = S7;
						break;
					case '8':	
						output->address = ascii_hex_24(input);
						output->record = S8;
						break;
					case '9':	
						output->address = ascii_hex_16(input);
						output->record = S9;
						break;
				}
			}
		}
	}
	return output->record;
}

/**
 * Calculate the checksum.
 * @param length The length byte extracted from the S record.
 * @param input The ASCII input record starting after the length byte.
 * @return the 1's compliment checksum.
 */
static uint8_t make_checksum(uint8_t length, char* input)
{
	uint8_t sum=0;
	for(uint8_t n=0; n < length; n++)
	{
		sum+=ascii_hex_8(input);
		input += 2;
	}
	return ~sum;
}


static uint8_t nibble(uint8_t c)
{
	if ( c >= 'A' && c <= 'F' )
		c = (c-'A')+10;
	else if ( c >= 'a' && c <= 'f' )
		c = (c-'a')+10;
	else if ( c >= '0' && c <= '9' )
		c = (c-'0');
	else
		c = 0;

	return c;
}

static uint8_t ascii_hex_8(char* input)
{
	uint8_t rc;
	rc=(nibble(*input++)<<4);
	rc|=nibble(*input);
	return rc;
}

static uint16_t ascii_hex_16(char* input)
{
	uint16_t rc;
	rc=(ascii_hex_8(input)<<8);
	input+=2;
	rc|=ascii_hex_8(input);
	return rc;
}

static uint32_t ascii_hex_24(char* input)
{
	uint32_t rc;
	rc=(ascii_hex_8(input)<<16);
	input+=2;
	rc|=(ascii_hex_8(input)<<8);
	input+=2;
	rc|=ascii_hex_8(input);
	return rc;
}

static uint32_t ascii_hex_32(char* input)
{
	uint32_t rc;
	rc=(ascii_hex_16(input)<<16);
	input+=4;
	rc|=ascii_hex_16(input);
	return rc;
}

/**
 * @brief Translate ASCII hex input buffer to binary output buffer.
 * @return length
 */
static uint8_t ascii_hex_translate(uint8_t length, char* input, srec_result_t* output)
{
	for(int n=0; n < length; n++)
	{
		uint8_t ch = ascii_hex_8(input);
		input++;
		input++;
		output->data[n]=ch;
	}
	return ( output->length = length );
}

/**
 * @brief get the binary checksum given the ASCII input buffer and length
 * @param length The length byte from the S record.
 * @param input The input ASCII hex text imediately following the length byte.
 * @return The checkcum contained in the S record ASCII text, converted to binary.
 */
static uint8_t fetch_checksum(uint8_t length, char* input)
{
	char* pchecksum = &input[length*2];
	uint8_t rc=ascii_hex_8(pchecksum);
	return rc;
}

