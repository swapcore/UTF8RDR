
# UTF8RDR: Lector de secuencias UTF-8

En 1967 un conjunto de caracteres utilizados en
telegrafía más otros caracteres adicionales formaron
ASCII. Más tarde, en 1967 este se actualizo y se
renombro como US-ASCII. US-ASCII utiliza 7 bits para
representar 128 caracteres definidos previamente por
ASCII. Un octavo bit se usaba como bit de paridad
para detectar errores de transmisión.

Por el momento no era requerido otro conjunto de
caracteres, sin embargo, debido al desarrollo
tecnológico y la expansión de Internet, empresas
crearon sus conjuntos de caracteres utilizando el bit
de paridad de US-ASCII, conocidos popularmente como
“ASCII extendido”.

Estos conjuntos de caracteres, a pesar de que contenían
caracteres de otras lenguas, fueron ineficientes debido
a incompatibilidades de codificación entre dispositivos
además de que algunos conjuntos de caracteres ni siquiera
incluían US-ASCII. Estos problemas fueron comunes hasta
el año 1991 cuando se desarrolló un estándar conocido
como UNICODE. Este nuevo estándar de conjunto de caracteres
se alzó rápidamente remplazando gradualmente los
conjuntos de caracteres existentes.

Los caracteres contenidos en UNICODE son representados
por valores numéricos llamados puntos de código. Estos
puntos están divididos entre el plano básico multilingüe
y el plano suplementario. Y estos se dividen en bloques
que pueden corresponder con un grupo de caracteres de una
lengua, jerarquía o característica que los relacione entre
sí. Esta organización facilita la consulta de los
caracteres y la adicción de nuevos dialectos, lenguas y
símbolos para su conservación, uso y estudio.

UNICODE es mantenido por el consorcio UNICODE, una organización
sin ánimos de lucro conformada por empresas como Adobe
Systems, Apple, Google, IBM, Microsoft, Oracle Corporation
y Yahoo! entre otros.

## ¿Por qué usar este software?

Para que un dispositivo pueda almacenar y procesar los códigos
UNICODE de manera eficiente se diseñaron los sistemas de
codificación UTF-16, UTF-32 e UTF-8. A continuación se
explicaran cada uno de estos:

* UTF-16: Sistema de codificación que utiliza 16 bits para almacenar códigos UNICODE desde 0 a 65535 (`0x0` a `0xFFFF`). Para códigos más allá de este rango se almacenan en pares subrogados de 16 bits.
* UTF-32: Sistema de codificación que utiliza 32 bits para almacenar códigos UNICODE desde 0 a 1114111 (`0x0` a `0x10FFFF`). Este sistema almacena los códigos sin cambios y abarca todos los bloques UNICODE definidos.
* UTF-8: Sistema de codificación de longitud variable; puede utilizar 1, 2, 3 o 4 bytes para representar los códigos UNICODE. Al igual que UTF-32 abarca todos los bloques UNICODE definidos.

Generalmente los sistemas operativos utilizan UTF-16 e UTF-32 para
codificar caracteres de forma nativa mientras que UTF-8 es usado
para enviar y recibir datos a través de Internet.
Este envió y recibo de datos supone un problema a la hora de
decodificarlos ya que cada sistema operativo contiene sus funciones
de decodificación y esto impide que una aplicación sea portable. 
Para solucionar este problema diseñe este software. Espero que sea
una alternativa simple y portátil para leer secuencias de datos UTF-8.

## ¿Cómo funciona?

UTF8RDR está conformada por cuatro funciones, mismas que pueden ser
utilizadas de manera individual para casos muy específicos. Este software
está escrito en lenguaje C en su versión C99 y utiliza la librería
`<stdint.h>`. Además de que procesa las secuencias 
UTF-8 con el endianess que maneja el microprocesador anfitrión.
Cualidades que hacen que este software sea multiplataforma y portable. 

A continuación explicare a detalle las funciones mencionadas.

### DecodeUTF8Ch()

Esta función es el corazón de este software. Recibe un buffer de datos
que contiene un carácter UTF-8 y lo decodifica en un punto de código
UNICODE a través de un puntero a una variable del tipo
`UTFCODEPOINT`. La función retorna el número de bytes
leídos desde el buffer. Este valor es usado por las funciones
`UTF8StrToUTF16Str()` y `UTF8StrToUTF32Str()`. 

En caso de un error de codificación los datos se rechazan y se retorna
0. El tratamiento de este error dependerá de la función que la invoque
aunque UNICODE recomienda sustituir el carácter por un carácter de
remplazo (`0xFFFD`) o detener el procesamiento en caso
de que el buffer de datos pertenezca a una cadena UTF-8. En mi
opinión no recomiendo intentar reparar estos datos ya que pueden
producir comportamientos indefinidos o mostrar caracteres erróneos
al usuario. 

### EncodeToUTF16Ch()`

Esta función recibe un punto de código UNICODE y retorna un carácter
UTF-16 a través de un puntero a una variable del tipo
`UTF32CH`. Debido a que UTF-16 puede utilizar 16 o 32 bits
para codificar caracteres la variable que recibe el carácter UTF-16
tiene una longitud de 32 bits, útil en caso de que el carácter sea
representado en dos pares subrogados en dos unidades de 16 bits.

La función comprueba si el código UNICODE representa un carácter real.
Retorna el número de unidades de 16 bits utilizados.  En caso de error retorna 0.

### UTF8StrToUTF16Str() y UTF8StrToUTF32Str()

Estas funciones se comportan de manera similar. Reciben un buffer a una
cadena UTF-8 y retornan un buffer a una cadena UTF-16 y UTF-32
respectivamente. En caso de un error de codificación detienen el
procesamiento de la cadena UTF-8 y retornan `NULL`.

Las cadenas resultantes que devuelven estas funciones son formadas de
acuerdo al orden de bytes que maneja el microprocesador anfitrión y
se utiliza la función `malloc()` para reservar espacio en memoria para
las cadenas . No se realiza conversiones de formato UNICODE.

## ¿Cómo usar este software?

Solo incluya este repositorio a su proyecto y enlácelo de la
siguiente manera:

```C
#include "utf8rdr.h"
```

## Condiciones de uso

Tenga presente que este software se publica bajo los términos de
la licencia GNU General Public Licence Versión 3. Consulte el archivo
LICENCE de este repositorio o diríjase a https://www.gnu.org/licenses
para más información acerca de las lineamientos de uso del este software.
