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

#include "utf8rdr.h"

// decodificar caracter UTF-8 
int DecodeUTF8Ch(uint8_t *data, UTFCODEPOINT *code, int blocksize)
{	
	// verificación de parametros
	if(!(data&&code)) {
		return 0;
	}
	
	if(blocksize!=4) {
		return 0;
	}
	
	// codificación UTF-8 de 1 byte
	if(!(data[0]&0x80)) {
		*code=data[0];
		return 1;
	}
	// codificación UTF-8 de 2 bytes
	else if((data[0]&0xE0)==0xC0&&
		(data[1]&0xC0)==0x80)
	{
		*code = 
			(data[0]&0x1F)<<6|
			(data[1]&0x3F)<<0;
		return 2;
	}
	// codificación UTF-8 de 3 bytes
	else if((data[0]&0xF0)==0xE0&&
		(data[1]&0xC0)==0x80&&
		(data[2]&0xC0)==0x80)
	{
		*code =
			(data[0]&0x0F)<<12|
			(data[1]&0x3F)<< 6|
			(data[2]&0x3F)<< 0;
		
		/* verificar si el codigo decodificado
		 * corresponde a un caracter real */
		if(*code==0xFFFE||*code==0xFFFF) 
			*code=REPLACEMENT_CHARACTER;
		if(*code>=0xFDD0&&*code<=0xFDEF) 
			*code=REPLACEMENT_CHARACTER;
		return 3;
	}
	// codificación UTF-8 de 4 bytes
	else if((data[0]&0xF8)==0xF0&&
		(data[1]&0xC0)==0x80&&
		(data[2]&0xC0)==0x80&&
		(data[3]&0xC0)==0x80)
	{
		*code =
			(data[0]&0x07)<<18|
			(data[1]&0x3F)<<12|
			(data[2]&0x3F)<< 6|
			(data[3]&0x3F)<< 0;
		return 4;
	}
	
	return 0;
}

// codificar punto de codigo a UTF-16
int EncodeToUTF16Ch(UTFCODEPOINT code, UTF32CH *utf16ch)
{
	UTF32CH u;
	UTF16CH w1,w2;
	
	if(!utf16ch) {
		return 0;
	}

	if(code<=0xFFFF) {
		/* El codigo UNICODE esta dentro del plano basico
		 * multilingue */
		*utf16ch=code&0xFFFF;
		
		/* Verificar si el codigo decodificado
		 * corresponde a un caracter real */
		if(*utf16ch==0xFFFE||*utf16ch==0xFFFF) 
			*utf16ch=REPLACEMENT_CHARACTER;
		if(*utf16ch>=0xFDD0&&*utf16ch<=0xFDEF) 
			*utf16ch=REPLACEMENT_CHARACTER;
		
		// una unidad de 16 bits utilizado
		return 1;
	}
	
	if(code>0x10FFFF) {
		// punto de codigo fuera del rango UNICODE
		return 0;
	}
	
	// definir caracter UTF-16 con pares subrogados
	u  = code-0x10000;
	w1 = 0xDC00 + ((u>> 0)&0x3FF);  // par subrogado bajo
	w2 = 0xD800 + ((u>>10)&0x3FF);  // par subrogado alto
	
	// crear caracter UTF-16
	*utf16ch=w1<<16|w2;
	
	// dos unidades de 16 bits utilizados
	return 2;
}

// convertir secuencia UTF-8 a UTF-16
void * UTF8StrToUTF16Str(UTF8CH * sequence, size_t len)
{
	UTFCODEPOINT code;
	UTF16CH * utf16str;
	UTF16CH * temp;
	UTF32CH c;
	size_t remdata;
	size_t strindex=0;
	UTF8CH data[4];
	UTF8CH *offset;
	int bytesread=0;
	int codeunits;
	
	if(!sequence||len<MIN_UTF8_SEQUENCE_LEN) {
		return NULL;
	}
	
	/* Asumir que el numero de caracteres en el texto
	 * resultante de la secuencia UTF-8 es igual a su
	 * tamaño en bytes */
	if(!(utf16str=calloc(len+1,sizeof(UTF16CH)))) {
		return NULL;
	}
	
	remdata=len-1;
	offset=sequence;
	
	/* Leer secuencia UTF-8 en busca de puntos 
	 * de codigos UNICODE */
	while(offset<(sequence+len)) {
		// copiar 4 bytes de la secuencia
		memcpy(data,offset,(remdata>=4)?4:remdata);
		
		// obtener punto de codigo UNICODE
		if(!(bytesread=DecodeUTF8Ch(data,&code,4))) {
			// secuencia no valida
			free(utf16str);
			return NULL;
		}
		
		// obtener caracter UTF-16
		codeunits=EncodeToUTF16Ch(code,&c);
		
		// insertar caracter a la cadena UTF-16
		if(codeunits==1) {
			/* Caracter del plano basico multilingue.
			 * Utiliza una unidad de 16 bits */
			utf16str[strindex++]=c;
		}
		else if(codeunits==2) {
			/* caracter mas alla del plano basico
			 * multilingue. Utiliza dos unidades de
			 * 16 bits en dos pares subrogados */
			utf16str[strindex++]=c&0xFFFF;
			utf16str[strindex++]=c>>16;
		}
		
		// avanzar al siguiente caracter
		offset  += bytesread;
		remdata -= bytesread;
	}
	
	// truncar cadena
	if(!(temp=realloc(utf16str,
		strindex*sizeof(UTF16CH)))) {
			free(utf16str);
			return NULL;
	}
	utf16str=temp;
	utf16str[strindex-1]=0;
	
	return utf16str;
}

// convertir secuencia UTF-8 a UTF-32
void * UTF8StrToUTF32Str(UTF8CH * sequence, size_t len)
{
	UTFCODEPOINT code;
	UTF32CH * utf32str;
	UTF32CH * temp;
	size_t remdata;
	size_t strindex=0;
	UTF8CH data[4];
	UTF8CH *offset;
	int bytesread=0;
	
	if(!sequence||len<MIN_UTF8_SEQUENCE_LEN) {
		return NULL;
	}
	
	/* Asumir que el numero de caracteres en el texto
	 * resultante de la secuencia UTF-8 es igual a su
	 * tamaño en bytes */
	if(!(utf32str=calloc(len+1,sizeof(UTF32CH)))) {
		return NULL;
	}
	
	remdata=len-1;
	offset=sequence;
	
	/* Leer secuencia UTF-8 en busca de puntos 
	 * de codigos UNICODE */
	while(offset<(sequence+len)) {
		// copiar 4 bytes de la secuencia
		memcpy(data,offset,(remdata>=4)?4:remdata);
		
		// obtener punto de codigo UNICODE
		if(!(bytesread=DecodeUTF8Ch(data,&code,4))) {
			// secuencia no valida
			free(utf32str);
			return NULL;
		}

		// insertar caracter a la cadena UTF-32
		utf32str[strindex++]=code;
		
		// avanzar al siguiente caracter
		offset  += bytesread;
		remdata -= bytesread;
	}
	
	// truncar cadena
	if(!(temp=realloc(utf32str,
		strindex*sizeof(UTF32CH)))) {
			free(utf32str);
			return NULL;
	}
	utf32str=temp;
	utf32str[strindex-1]=0;
	
	return utf32str;
}
