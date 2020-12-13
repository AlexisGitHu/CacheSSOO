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
// 12.- Actuamos en base a la comparacion si es false. Sin cargar los datos porque tenemos que pensar si queremos almacenar del revés los datos o imprimirlos al reves
// 13.- Mejor imprimir los datos al revés que guardarlos al revés, porque en el caso de que necesitemos acceder a esos datos, estarían al revés. Asi que por simplicidad guardaremos los datos del derecho y los imprimiremos del revés
// 14.- Ahora nos falta tratar con el texto leído y la simplicidad del programa 
// Con respecto a la simplicidad, si usamos la librería <string.h> y usamos strtol (su tercer argumento es la base de la que pasamos un string a un long (y como queremos un unsigned int no hay problema, pero si que lo habría si fuesemos de unsigned int a long))
//Además como hemos incluido <string.h> podemos hacer uso de strcat para el texto. Sin embargo para hacer strcat debe ser un string, pero estaríamos evaluando un char en vez de string (RAM[i] --> char), pero si hacemos strncat y hacemos: strncat(texto,&RAM[i], 1) sería correcto porque &RAM[i] lo evaluaría como String y luego solo copia 1 caracter 
// 15.- Hacemos el sleep también
// 16.- Imprimimos el texto leído tras todos los aciertos. Además de imprimir el mensaje final
// 17.- Simplificamos más el programa añadiendo los operadores con los operandos en hexadecimal en vez de en binario en vez de 0b0000000111 pues 0x7 etc
// 18.- Terminamos de añadir todas las constantes

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//Macros que actuan como booleanos
#define TRUE 1
#define FALSE 0
#define DATOS_POR_BLOQUE 8 //Porque la linea tiene como valores: 0,1,2,3,4,5,6,7
#define MAX_CAPACIDAD_RAM 1024
#define MAX_LENGTH 5
#define INCREMENTO 5
#define NUM_VALORES_QUE_PUEDE_TOMAR_LA_LINEA 4 //El 0, 1, 2, 3

typedef struct {
	unsigned int ETQ;
	unsigned int Datos[DATOS_POR_BLOQUE];
} T_LINEA_CACHE;

typedef struct {
	unsigned int direccion;
	unsigned int palabra;
	unsigned int linea;
	unsigned int etiqueta;
	unsigned int bloque;
} T_DIRECCION_SEPARACION;

int indiceBucle = 0;

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
	direccionRepartida.palabra = direccion & 0x7; //La palabra tiene 3 bits significativos
	direccionRepartida.linea = (direccion >> 3) & 0x3; //Rotamos los 3 bits de la palabra y la linea tiene 2 bits significativos
	direccionRepartida.etiqueta = (direccion >> 5) & 0x1F; //Rotamos los 3 bits de la palabra y los 2 bits de la linea y la etiqueta tiene 5 bits significativos
	direccionRepartida.bloque = (direccion >> 3) & 0x7F; //Rotamos los 3 bits de la palabra y el bloque tiene 7 bits significativos
	
	return direccionRepartida;
}

char* leeLineaDeFichero(FILE* fichero)
{
	int maxLength = MAX_LENGTH;

	char* linea = (char*)malloc(sizeof(char) * maxLength);
	int contador = 0;
	int incremento = INCREMENTO;
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

int compararEtiquetaConCache(unsigned int ETQ, unsigned int etiqueta)
{
	int comparacion = FALSE;

	if (ETQ == etiqueta)
	{
		comparacion = TRUE;
	}

	return comparacion;
}

T_LINEA_CACHE cargarLosDatos(char *RAM, T_LINEA_CACHE CACHEsym, unsigned int bloque)
{
	char aux = '\0';
	
	for(indiceBucle = 0; indiceBucle < DATOS_POR_BLOQUE; indiceBucle++)
	{
		CACHEsym.Datos[indiceBucle] = RAM[DATOS_POR_BLOQUE*bloque + indiceBucle];
	}
	
	return CACHEsym;
}

void imprimirEtqYDatos(T_LINEA_CACHE *CACHEsym)
{
	int i = 0;
	
	for (indiceBucle = 0; indiceBucle < NUM_VALORES_QUE_PUEDE_TOMAR_LA_LINEA; indiceBucle++)
	{
		printf("ETQ:%X\tDatos ", CACHEsym[indiceBucle].ETQ);
		for (i = DATOS_POR_BLOQUE-1; i >= 0; i--)
		{
			printf("%02X ", CACHEsym[indiceBucle].Datos[i]);
		}
		printf("\n");
	}
}

void mostrarMensajeFinal(int tiempoGlobal, int numFallos, int numAccesosALaCache, char* texto)
{
	double mediaTiempoAcceso = 0;

	if(numAccesosALaCache == 0)
	{
		printf("El numero de accesos a la cache han sido 0...\n");
	}
	else
	{
		mediaTiempoAcceso = (double)tiempoGlobal/(double)numAccesosALaCache; //Hacemos el casting porque si no, la division de int/int = int
	}
	
	printf("\n-----------------------------\n");
	printf("Tiempo global: %d\nNumero de accesos a la cache: %d\nNumero de accesos fallidos a la cache: %d\nTiempo medio de acceso a la cache: %f\nTexto: %s\n",tiempoGlobal, numAccesosALaCache, numFallos, mediaTiempoAcceso, texto);
	printf("-----------------------------\n");
}

void buclePrincipal(FILE* fichero2, T_LINEA_CACHE *CACHEsym, unsigned char* RAM)
{
	char* direccion = (unsigned char*)malloc(sizeof(unsigned char)*5);
	direccion = '\0';

	int tiempoGlobal = 0;
	int numFallos = 0;
	int contadorAccesosALaCache = 0;
	
	int comparacion = FALSE;
	
	unsigned int aux = 0x0;
	
	T_DIRECCION_SEPARACION direccionRepartida;
	char texto[100];

	while (!feof(fichero2)) //Realizamos este bucle hasta que no sea end of file
	{
		tiempoGlobal++;
		contadorAccesosALaCache++; //Actualizamos el contador de intentos de accesos a la cache		
		
		direccion = leeLineaDeFichero(fichero2); //Leemos la direccion de memoria desde el fichero
		
		aux = strtol(direccion,NULL,16); //Traducimos la direccion

		printf("\n");
		
		direccionRepartida = calcularElRestoDeCamposDeDireccion(aux); //Calculamos el resto de campos en base a la direccion obtenida		

		comparacion = compararEtiquetaConCache(CACHEsym[direccionRepartida.linea].ETQ, direccionRepartida.etiqueta); //Usamos una variable como booleano para saber si la etiqueta está cargada
		
		if (comparacion == FALSE)
		{
			numFallos++;
			
			printf("T: %d, Fallo de CACHE %d, ADDR %04X ETQ %X linea %02X palabra %02X bloque %02X\n", tiempoGlobal, numFallos, direccionRepartida.direccion, direccionRepartida.etiqueta, direccionRepartida.linea, direccionRepartida.palabra, direccionRepartida.bloque);

			tiempoGlobal += 10;

			printf("Cargando el bloque %02X en la linea %02X\n", direccionRepartida.bloque, direccionRepartida.linea);
			
			CACHEsym[direccionRepartida.linea] = cargarLosDatos(RAM, CACHEsym[direccionRepartida.linea], direccionRepartida.bloque);

			CACHEsym[direccionRepartida.linea].ETQ = direccionRepartida.etiqueta; //Llamar a una funcion para q cada indice de dato corresponda
		}
		
		printf("T: %d, Acierto de CACHE, ADDR %04X ETQ %X linea %02X palabra %02X DATO %02X\n", tiempoGlobal, direccionRepartida.direccion, direccionRepartida.etiqueta, direccionRepartida.linea, direccionRepartida.palabra, CACHEsym[direccionRepartida.linea].Datos[direccionRepartida.palabra]);

		imprimirEtqYDatos(CACHEsym); //Imprimimos la etiqueta con sus datos correspondientes		
		
		strncat(texto, &RAM[direccionRepartida.bloque*8 +direccionRepartida.palabra],1);

		sleep(2);
	}
	free(direccion);
	
	mostrarMensajeFinal(tiempoGlobal, numFallos, contadorAccesosALaCache, texto);
}

int main(int argc, char* argv[])
{
	//Declaramos variables
	FILE* fichero1;
	FILE* fichero2;
	T_LINEA_CACHE* CACHEsym = (T_LINEA_CACHE*)malloc(sizeof(T_LINEA_CACHE) * NUM_VALORES_QUE_PUEDE_TOMAR_LA_LINEA);
	unsigned char* RAM = (unsigned char*)malloc(sizeof(unsigned char)*(MAX_CAPACIDAD_RAM+1));

	//Inicializamos los datos
	for (indiceBucle = 0; indiceBucle < NUM_VALORES_QUE_PUEDE_TOMAR_LA_LINEA; indiceBucle++)
	{
		CACHEsym[indiceBucle].ETQ = 0xFF;

		for (int j = 0; j < DATOS_POR_BLOQUE; j++)
		{
			CACHEsym[indiceBucle].Datos[j] = 0x0;
		}
	}

	//Abrimos comprobamos la vaidez de esos ficheros
	fichero1 = fopen("RAM.bin", "rb");//Lo abrimos en modo lectura binaria
	comprobarValidezFichero(fichero1);

	fread(RAM, sizeof(char), MAX_CAPACIDAD_RAM, fichero1);
	fclose(fichero1);//Cerramos el fichero para asegurarnos de que no se puede "ensuciar" el fichero
	
	RAM[MAX_CAPACIDAD_RAM+1] = '\0'; //Añadimos el '\0' del final para no rellenar con basura

	fichero2 = fopen("accesos_memoria.txt", "r"); //Lo abrimos en modo lectura para posteriormente cambiar de hexadecimal a binario
	comprobarValidezFichero(fichero2);

	//Hacemos el bucle principal del programa
	buclePrincipal(fichero2, CACHEsym, RAM);
	fclose(fichero2);
	
	//Liberamos memoria
	free(CACHEsym);
	free(RAM);
	return 0;
}