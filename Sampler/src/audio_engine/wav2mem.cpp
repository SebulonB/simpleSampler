
// Convert a set of WAV audio Files to C data arrays for the Teensy3 Audio Library
// Copyright 2014, Paul Stoffregen (paul@pjrc.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation Files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// 2021/05
// Edited: fill array with samples. Could be used for SD-Card -> EXMEM
// Sebastian Brunnauer
//
//

#include <Arduino.h>
#include <SD.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <string.h>


#define DEBUG_WAV2MEM

#ifdef DEBUG_WAV2MEM
  char str_w2m[100];
#endif

uint32_t padding(uint32_t length, uint32_t block);

uint8_t  read_uint8(File *in);
int16_t  read_int16(File *in);
uint32_t read_uint32(File *in);

uint8_t  ulaw_encode(int16_t audio);
unsigned int*   print_byte(unsigned int *out, uint8_t b);

void     die(const char *format, ...) __attribute__ ((format (printf, 1, 2)));

unsigned int bcount;
unsigned int total_length=0;


int wav2m(File *in, unsigned int *out, int pcm_mode=1)
{

  if( out == NULL){return -1;}

  uint32_t header[4];
  int16_t format, channels, bits;
  uint32_t rate;
  uint32_t i, length, padlength=0, arraylen;
  uint32_t chunkSize;
  int32_t audio=0;

  // read the WAV File's header
  for (i=0; i < 4; i++) {
      header[i] = read_uint32(in);
  }
  while (header[3] != 0x20746D66) {
      // skip past unknown sections until "fmt "
      chunkSize = read_uint32(in);
      for (i=0; i < chunkSize; i++) {
          read_uint8(in);
      }
      header[3] = read_uint32(in);
  }
  chunkSize = read_uint32(in);

  // read the audio format parameters
  format = read_int16(in);
  channels = read_int16(in);
  rate = read_uint32(in);
  read_uint32(in); // ignore byterate
  read_int16(in);  // ignore blockalign
  bits = read_int16(in);

#ifdef DEBUG_WAV2MEM
  sprintf(str_w2m, "File: %s format: %d, channels: %d, rate: %lu, bits %d\n", in->name(), format, channels, rate, bits);
  Serial.print(str_w2m);
#endif

	if (format != 1){
		die("file %s is compressed, only uncompressed supported\n", in->name());
        return (-1);
	}
	if (rate != 44100 && rate != 22050 && rate != 11025 /*&& rate != 8000*/ ){
		die("sample rate %lu in %s is unsupported\n"
		  "Only 44100, 22050, 11025 work", rate, in->name());
		return (-1);  
	}
	if (channels != 1 && channels != 2){
		die("file %s has %d channels, but only 1 & 2 are supported\n", in->name(), channels);
		return (-1);
	}
	if (bits != 16){
		die("file %s has %d bit format, but only 16 is supported\n", in->name(), bits);
        return (-1);
	}
	// skip past any extra data on the WAVE header (hopefully it doesn't matter?)
	for (chunkSize -= 16; chunkSize > 0; chunkSize--) {
		read_uint8(in);
	}

	// read the data header, skip non-audio data
	while (1) {
		header[0] = read_uint32(in);
		length = read_uint32(in);
		if (header[0] == 0x61746164) break; // beginning of actual audio data
		// skip over non-audio data
		for (i=0; i < length; i++) {
			read_uint8(in);
		}
	}


	// the length must be a multiple of the data size
	if (channels == 2) {
		if (length % 4) die("file %s data length is not a multiple of 4\n", in->name());
		length = length / 4;
	}
	if (channels == 1) {
		if (length % 1) die("file %s data length is not a multiple of 2\n", in->name());
		length = length / 2;
	}
	if (length > 0xFFFFFF) die("file %s data length is too long\n", in->name());
	bcount = 0;

	// AudioPlayMemory requires padding to 2.9 ms boundary (128 samples @ 44100)
	if (rate == 44100) {
		padlength = padding(length, 128);
		format = 1;
	} else if (rate == 22050) {
		padlength = padding(length, 64);
		format = 2;
	} else if (rate == 11025) {
		padlength = padding(length, 32);
		format = 3;
	}
	if (pcm_mode) {
		arraylen = ((length + padlength) * 2 + 3) / 4 + 1;
		format |= 0x80;
	} else {
		arraylen = (length + padlength + 3) / 4 + 1;
	}
	total_length += arraylen;  


    unsigned int * out_s = out;
	*out = length | (format << 24);
	out++;

#ifdef DEBUG_WAV2MEM
    sprintf(str_w2m,  "AudioSample%s[%lu]\n", in->name(), arraylen);
    Serial.print(str_w2m);
#endif

	//finally, read the audio data
	while (length > 0) {
		if (channels == 1) {
			audio = read_int16(in);
		} else {
			audio = read_int16(in);
			audio += read_int16(in);
			audio /= 2;
		}
		if (pcm_mode) {
			out=print_byte(out, audio);
			out=print_byte(out, audio >> 8);
		} else {
			out=print_byte(out, ulaw_encode(audio));
		}
		length--;
	}
	while (padlength > 0) {
		out=print_byte(out, 0);
		padlength--;
	}
	while (bcount > 0) {
		out=print_byte(out, 0);
	}


   //fill rest with 0
   while((out - out_s) < (int)arraylen){
	*out = 0;
	out++;	  
   }

   return (out - out_s);
}


uint8_t read_uint8(File *in)
{
	int c1;

	c1 = in->read();
	if (c1 == EOF) die("error, end of data while reading: %s\n" , in->name());
	c1 &= 255;
	return c1;
}

int16_t read_int16(File *in)
{
	int c1, c2;

	c1 = in->read();
	if (c1 == EOF) die("error, end of data while reading: %s\n" , in->name());
	c2 = in->read();
	if (c2 == EOF) die("error, end of data while reading: %s\n" , in->name());
	c1 &= 255;
	c2 &= 255;
	return (c2 << 8) | c1;
}

uint32_t read_uint32(File *in)
{
	int c1, c2, c3, c4;

	c1 = in->read();
	if (c1 == EOF) die("error, end of data while reading: %s\n" , in->name());
	c2 = in->read();
	if (c2 == EOF) die("error, end of data while reading: %s\n" , in->name());
	c3 = in->read();
	if (c3 == EOF) die("error, end of data while reading: %s\n" , in->name());
	c4 = in->read();
	if (c4 == EOF) die("error, end of data while reading: %s\n" , in->name());
	c1 &= 255;
	c2 &= 255;
	c3 &= 255;
	c4 &= 255;
	return (c4 << 24) | (c3 << 16) | (c2 << 8) | c1;
}


// compute the extra padding needed
uint32_t padding(uint32_t length, uint32_t block)
{
	uint32_t extra;

	extra = length % block;
	if (extra == 0) return 0;
	return block - extra;
}

uint8_t ulaw_encode(int16_t audio)
{
	uint32_t mag, neg;

	// http://en.wikipedia.org/wiki/G.711
	if (audio >= 0) {
		mag = audio;
		neg = 0;
	} else {
		mag = audio * -1;
		neg = 0x80;
	}
	mag += 128;
	if (mag > 0x7FFF) mag = 0x7FFF;
	if (mag >= 0x4000) return neg | 0x70 | ((mag >> 10) & 0x0F);  // 01wx yz00 0000 0000
	if (mag >= 0x2000) return neg | 0x60 | ((mag >> 9) & 0x0F);   // 001w xyz0 0000 0000
	if (mag >= 0x1000) return neg | 0x50 | ((mag >> 8) & 0x0F);   // 0001 wxyz 0000 0000
	if (mag >= 0x0800) return neg | 0x40 | ((mag >> 7) & 0x0F);   // 0000 1wxy z000 0000
	if (mag >= 0x0400) return neg | 0x30 | ((mag >> 6) & 0x0F);   // 0000 01wx yz00 0000
	if (mag >= 0x0200) return neg | 0x20 | ((mag >> 5) & 0x0F);   // 0000 001w xyz0 0000
	if (mag >= 0x0100) return neg | 0x10 | ((mag >> 4) & 0x0F);   // 0000 0001 wxyz 0000
	                   return neg | 0x00 | ((mag >> 3) & 0x0F);   // 0000 0000 1wxy z000
}


unsigned int*   print_byte(unsigned int *out, uint8_t b)
{
	static uint32_t buf32=0;

	buf32 |= (b << (8 * bcount++));
	if (bcount >= 4) {
		*out = buf32;

// #ifdef DEBUG_WAV2MEM
// 		sprintf(str_w2m, "%08p, 0x%08lX\n", out, *out);
// 		Serial.print(str_w2m);
// #endif			

	   buf32 = 0;
	   bcount = 0;
       
	   //!bug: out++ has no effekt
	   return  (out + 1);
	}
    
	return  (out);	
}




void die(const char *format, ...)
{
#ifdef DEBUG_WAV2MEM
  char str_w2m[100];
	va_list args;
	va_start(args, format);
	sprintf(str_w2m, format, args);
  Serial.print(str_w2m);
#endif
}





