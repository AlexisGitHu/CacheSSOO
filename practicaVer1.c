//Lógica para el programa

// 1.- Vamos a abrir los ficheros en los distintos modos r de lectura, rb lectura de un fichero binario
// 2.- Comprobar si están creados esos ficheros
// 3.- Si están creados, leerlos directamente si es posible. El fichero .bin podremos leerlo directamente, con uso del fread lo leemos y después cerramos el fichero
// 4.- Como vamos a leer direcciones, de una en una, usaremos un struct para almacenar las distintas partes que contiene la direccion de memoria (palabra, linea ....)
// 5.- Para poder calcular la palabra, linea, etiqueta y bloque necesitaremos hacer uso de los operadores: (>> , &)
// 6.- A la hora de calcular necesitaremos ver qué cantidad de bits rotar y cuáles son los significativos
// 7.- Una vez inicializado el array de CACHEsym y leído el fichero .bin, ahora tenemos que entrar en el bucle principal
// 8.- En el bucle principal:
// 8.1.- Necesitaremos pasarnos lo leído de la variable RAM, el fichero de accesos_memoria y CACHEsym para poder ir actualizando esta variable
// 9.- Leeremos las direcciones de memoria una a una (hasta un '\n' o un feof) del fichero accesos_memoria.txt
// 10.- Calculamos linea, palabra etc....
// 11.- Comparamos las distintas etiquetas

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//Macros que actuan como booleanos
#define TRUE 1
#define FALSE 0

typedef struct {
	unsigned int ETQ;
	unsigned int Datos[8];
} T_LINEA_CACHE;

typedef struct {
	unsigned int direccion;
	unsigned int palabra;
	unsigned int linea;
	unsigned int etiqueta;
	unsigned int bloque;
} T_DIRECCION_SEPARACION;

void comprobarValidezFichero(FILE* fichero)
{
	if (fichero == NULL)
	{
		printf("No se ha seleccionado un fichero valido\n");
		exit(-1);
	}
}

T_DIRECCION_SEPARACION calcularElRestoDeCamposDeDireccion(unsigned int direccion)
{
	T_DIRECCION_SEPARACION direccionRepartida;

	direccionRepartida.direccion = direccion;
	direccionRepartida.palabra = direccion & 0b0000000111; //La palabra tiene 3 bits significativos
	direccionRepartida.linea = (direccion >> 3) & 0b0000011; //Rotamos los 3 bits de la palabra y la linea tiene 2 bits significativos
	direccionRepartida.etiqueta = (direccion >> 5) & 0b11111; //Rotamos los 3 bits de la palabra y los 2 bits de la linea y la etiqueta tiene 5 bits significativos
	direccionRepartida.bloque = (direccion >> 3) & 0b1111111; //Rotamos los 3 bits de la palabra y el bloque tiene 7 bits significativos
	
	return direccionRepartida;
}

char* leeLineaDeFichero(FILE* fichero)
{
	int maxLength = 5;

	char* linea = (char*)malloc(sizeof(char) * maxLength);
	int contador = 0;
	int incremento = 5;
	char c = getc(fichero);

	while ((c != '\n') && (!feof(fichero)))
	{
		linea[contador] = c;

		contador++;
		if (contador >= maxLength)
		{
			maxLength += incremento;
			linea = (char*)realloc(linea, sizeof(char) * maxLength);
		}
		c = getc(fichero);
	}

	linea[contador] = '\0';

	return linea;
}

unsigned int traducir(char hexaChar)
{
	unsigned int traduccion = 0x0;
	switch (hexaChar)
	{
		case '0':
			break;
		case '1':
			traduccion += 1;
			break;
		case '2':
			traduccion += 2;
			break;
		case '3':
			traduccion += 3;
			break;
		case '4':
			traduccion += 4;
			break;
		case '5':
			traduccion += 5;
			break;
		case '6':
			traduccion += 6;
			break;
		case '7':
			traduccion += 7;
			break;
		case '8':
			traduccion += 8;
			break;
		case '9':
			traduccion += 9;
			break;
		case 'A':
			traduccion += 10;
			break;
		case 'B':
			traduccion += 11;
			break;
		case 'C':
			traduccion += 12;
			break;
		case 'D':
			traduccion += 13;
			break;	
		case 'E':
			traduccion += 14;
			break;
		case 'F':
			traduccion += 15;
			break;
	}
	return traduccion;
}

unsigned int traducirDeHexaCharAHexa(char* direccion)
{
	int i = 0;
	unsigned int traduccion = 0;
	
	for(i = 0; i < 4; i++)
	{
		traduccion += (traducir(direccion[i]))*(pow(16,3-i));
	}
	
	return traduccion;
}

int compararEtiquetaConCache(unsigned int ETQ, unsigned int etiqueta)
{
	int comparacion = FALSE;

	if (ETQ == etiqueta)
	{
		comparacion = TRUE;
	}

	return comparacion;
}

void buclePrincipal(FILE* fichero2, T_LINEA_CACHE *CACHEsym, unsigned char* RAM)
{
	char* direccion = (char*)malloc(sizeof(char)*5);
	direccion = '\0';
	
	int tiempoGlobal = 0;
	int contadorAccesosALaCache = 0;
	int comparacion = FALSE;
	T_DIRECCION_SEPARACION direccionRepartida;
	unsigned int aux = 0x0;
	
	while (!feof(fichero2)) //Realizamos este bucle hasta que no sea end of file
	{
		tiempoGlobal++;
		contadorAccesosALaCache++;
		direccion = leeLineaDeFichero(fichero2); //Leemos la direccion de memoria desde el fichero
		
		aux = traducirDeHexaCharAHexa(direccion); //Traducimos la direccion
		direccionRepartida = calcularElRestoDeCamposDeDireccion(aux);
		
		comparacion = compararEtiquetaConCache(CACHEsym[direccionRepartida.linea].ETQ, direccionRepartida.etiqueta);
	}
}

int main(int argc, char* argv[])
{
	//Declaramos variables
	FILE* fichero1;
	FILE* fichero2;
	T_LINEA_CACHE* CACHEsym = (T_LINEA_CACHE*)malloc(sizeof(T_LINEA_CACHE) * 4);
	unsigned char* RAM = (unsigned char*)malloc(sizeof(unsigned char)*(1025));
	int indiceBucle = 0;
	//Inicializamos los datos
	for (indiceBucle = 0; indiceBucle < 4; indiceBucle++)
	{
		CACHEsym[indiceBucle].ETQ = 0xFF;

		for (int j = 0; j < 8; j++)
		{
			CACHEsym[indiceBucle].Datos[j] = 0x0;
		}
	}

	//Abrimos comprobamos la vaidez de esos ficheros
	fichero1 = fopen("RAM.bin", "rb");//Lo abrimos en modo lectura binaria
	comprobarValidezFichero(fichero1);

	fread(RAM, sizeof(char), 1024, fichero1);
	fclose(fichero1);//Cerramos el fichero para asegurarnos de que no se puede "ensuciar" el fichero
	
	RAM[1025] = '\0'; //Añadimos el '\0' del final para no rellenar con basura

	fichero2 = fopen("accesos_memoria.txt", "r"); //Lo abrimos en modo lectura para posteriormente cambiar de hexadecimal a binario
	comprobarValidezFichero(fichero2);

	//Hacemos el bucle principal del programa
	buclePrincipal(fichero2, CACHEsym, RAM);
	fclose(fichero2);
	
	free(CACHEsym);
	free(RAM);
	return 0;
}