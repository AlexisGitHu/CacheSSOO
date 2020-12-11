//Lógica para el programa

// 1.- Vamos a abrir los ficheros en los distintos modos r de lectura, rb lectura de un fichero binario
// 2.- Comprobar si están creados esos ficheros
// 3.- Si están creados, leerlos directamente si es posible. El fichero .bin podremos leerlo directamente, con uso del fread lo leemos y después cerramos el fichero
// 4.- Como vamos a leer direcciones, de una en una, usaremos un struct para almacenar las distintas partes que contiene la direccion de memoria (palabra, linea ....)
// 5.- Para poder calcular la palabra, linea, etiqueta y bloque necesitaremos hacer uso de los operadores: (>> , &)
// 6.- A la hora de calcular necesitaremos ver qué cantidad de bits rotar y cuáles son los significativos

#include <stdio.h>
#include <stdlib.h>

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

int main(int argc, char* argv[])
{
	//Declaramos variables
	FILE* fichero1;
	unsigned char* RAM = (unsigned char*)malloc(sizeof(unsigned char)*1025);
	int indiceBucle = 0;

	//Abrimos comprobamos la vaidez de esos ficheros
	fichero1 = fopen("RAM.bin", "rb");//Lo abrimos en modo lectura binaria
	comprobarValidezFichero(fichero1);

	fread(RAM, sizeof(char), 1024, fichero1);
	fclose(fichero1);//Cerramos el fichero para asegurarnos de que no se puede "ensuciar" el fichero
	
	RAM[1025] = '\0'; //Añadimos el '\0' del final para no rellenar con basura
	
	free(RAM); //Liberamos memoria
	return 0;
}