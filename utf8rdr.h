/*
	UTF8RDR: lector de secuencias UTF-8
	Copyright (c) 2021-2022 Oscar Elias
	
	This file is part of UTF8RDR.
	UTF8RDR is free software: you can redistribute it and/or modify it
	under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	
	UTF8RDR is distributed in the hope that it will be useful, but
	WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
	General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with UTF8RDR. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef _UTF8RDR_H_
#define _UTF8RDR_H_

#define REPLACEMENT_CHARACTER 0xFFFD

#define MIN_UTF8_SEQUENCE_LEN 2

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <stdint.h>

#ifndef _UTFRDR_DATATYPES_
#define _UTFRDR_DATATYPES_

typedef uint8_t UTF8CH;
typedef uint16_t UTF16CH;
typedef uint32_t UTF32CH;
typedef uint32_t UTFCODEPOINT;

#endif

void * UTF8StrToUTF16Str(UTF8CH * sequence, size_t len);
void * UTF8StrToUTF32Str(UTF8CH * sequence, size_t len);
int EncodeToUTF16Ch(UTFCODEPOINT code, UTF32CH *utf16ch);
int DecodeUTF8Ch(uint8_t *data, UTFCODEPOINT *code, int datasize);

#endif
