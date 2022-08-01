#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define CANTIDAD_PALABRAS 30

//funcion dada en comunidades para acceder a un archivo
//La usamos con un path relativo, "palabras.txt" *tiene que* estar
//en la misma carpeta que "main.c"
void getWordInLine(char *fileName, int lineNumber, char *p);

//Esta funcion simplemente limpia la consola, y detecta automaticamente el
//sistema operativo para cambiar el llamado al sistema que lo hace (multiplataforma)
void limpiarPantalla();

//Esta funcion limpia el buffer luego de cada ingreso
//Ya que queda a la espera de que presiones enter
//Hace las veces de fflush, en cierta forma
//Y además, puede usarse como "Presione enter para continuar"
void bufferEnter();

//Las secciones del programa estan moduladas para un armado más sencillo
void menuPrincipal();
void nuevaPartida();
int jugada(int actual, int total, char todosIntentos[][7][6], char todosWordles[][1][6]);
void wordleRandomizer(char wordle[]);
void mensajeNroJugada(int actual, int total);
void mostrarJugada(char todosIntentos[][7][6], char todosWordles[][1][6], int nroJugada, int puntaje);

//Estas funciones setean el color del que va a salir el output. Útil para el diagrama y utilidades varias
void verde();
void rojo();
void amarillo();
void blanco();
void negro();
void colorReset();


int main(){
    menuPrincipal();
    printf("Saliste del juego. Presione enter para terminar el programa...");
    bufferEnter();
    return 0;
}

void menuPrincipal(){

    int i = 0, n, invalido = 0;

    printf("---WORDLE---\nAUS - Taller de programacion I \nBaiocchi Teo, Ceron Santiago");
    printf("\n\n");
    printf("1) Jugar una partida\n");
    printf("2) Reglas del juego\n");
    printf("3) Salir\n");
    char ingreso[6];

    while(i != (-1)){
        if(invalido == 1){
            printf("El numero ingresado no fue valido. Intente nuevamente... \n");
        }
        printf("Ingrese un numero para seleccionar: ");
        scanf("%d", &n);
        bufferEnter();
        switch(n){
            case 1:
                invalido = 0;
                nuevaPartida();
                limpiarPantalla();
                menuPrincipal();
                break;
            case 2:
                invalido = 0;
                printf("Aca irian las reglas. Hay que acordarse de escribirlas. \n");
                printf("Presione Enter para continuar");
                bufferEnter();
                limpiarPantalla();
                menuPrincipal();
            case 3:
                return;
                break;
            default:
                limpiarPantalla();
                invalido = 1;
                menuPrincipal();
                break;
        }
    }
}

void nuevaPartida(){
    int i, j, k, j2, salir = 0, n, max = 0, min = 10001, imax = 0, imin = 0, p = 0;
    int todosPuntajes[8] = {-1, -1, -1, -1, -1, -1, -1, -1};
    float sum=0, prom=0;
    char todosIntentos[9][7][6]; //Uno extra para el terminador, por las dudas
    char todosWordles[9][1][6]; //todos los wordle de cada jugada

    do{
        limpiarPantalla();
        printf("Ingrese cuantas palabras para adivinar en esta partida (No mayor a 8): ");
        scanf("%d", &j);
        bufferEnter();} while(j<1 || j>8);

    j2 = j;


    for(i = 0; i <= j; i++)
    {
        if(i != 0  && i != j)
        {
            printf("Terminaste tu partida %i de %i, para terminar el juego ahora ingrese 0\nPara continuar, ingrese cualquier otra tecla: ", i, j);
            scanf("%d", &n);
            bufferEnter();
            if(n == 0)
            {
                j2 = i; //Si se corta la partida antes de tiempo, en J2 se guarda cuantas jugadas se jugaron efectivamente
                salir = 1;
            }
        }



        if(i == j || salir == 1) //Cuando la partida termina, ya sea por que termino o porque fue cortada
        {
            printf("Presione enter para continuar.\n");
            bufferEnter();
            limpiarPantalla();
            printf("Fin del juego...\n");

            //puntaje maximo
            for(k=0; k<8; k++){
                if(todosPuntajes[k] > max && todosPuntajes[k] != -1){
                    max = todosPuntajes[k];
                    imax = k + 1;
                }
            }


            //puntaje minimo
            for(k=0; k<8; k++){
                if(todosPuntajes[k] < min && todosPuntajes[k] != -1){
                    min = todosPuntajes[k];
                    imin = k + 1;
                }
            }

            //promedio
            for(k=0; k<8; k++){
                if(todosPuntajes[k] != 0 && todosPuntajes[k] != -1){
                    sum = sum + todosPuntajes[k];
                    p++;
                }
            }
            prom=sum/p;

            printf("Tu puntaje maximo fue de %d puntos y lo obtuviste en la partida %d\n", max, imax);
            printf("Tu puntaje minimo fue de %d puntos y lo obtuviste en la partida %d\n", min, imin);
            printf("El promedio de los puntajes en que lograste una victoria fue de: %.2f puntos\n", prom);
            printf("Si deseas revisar el mapa o puntaje de una jugada especifica de tus %i jugadas, ingresala. \nCaso contrario, ingresa 0 para volver al menu: ", j2);
            scanf("%d", &n);
            bufferEnter();

            //Sale del juego
            if(n == 0){
                break;
            }

            n = n-1;
            if(n<j2 && n>=0){
                mostrarJugada(todosIntentos, todosWordles, n, todosPuntajes[n]);
            }
        } else {
            todosPuntajes[i] = jugada(i, j, todosIntentos, todosWordles);
        }
    }
}

int jugada(int actual, int total, char palabras[][7][6], char todosWordles[][1][6]){

    char wordle[6];
    char intento[6];
    int i, j, k, l, comprobante = 0, puntaje = 5000, derrota=0;

    //array de verificacion de "estado de la letra" para asignar puntaje
    //5 espacios, uno por cada letra a adivinar
    // valor "0", no fue encontrada. "1", fue adivinada pero en el lugar incorrecto, "2", letra ya validad en su posicion
    int punt[5] = {0, 0, 0, 0, 0};

    limpiarPantalla();
    mensajeNroJugada(actual+1, total);
    wordleRandomizer(wordle);

    for(i = 0; i < 6; i++){
        todosWordles[actual][0][i] = wordle[i];
    }

    for(i = 0; i < 6; i++)
    {
        if(i>0)
        {
            //Restar 500 puntos cada vez que se hace un nuevo intento, excepto en el primer intento
            puntaje = puntaje - 500;
        }
        printf("\nTu puntaje es de %d puntos\n", puntaje);

        comprobante = 0;

        while(comprobante != 1)
        {

                char aux[10]; //para testear el largo
                printf("Ingrese su intento Nro %i: ", i+1);
                scanf("%s", aux);
                bufferEnter();

                if(strlen(aux) > 5)
                {
                    printf("Tu ingreso fue demasiado largo. Recorda que son palabras de 5 caracteres.\n");

                } else {
                    if (strlen(aux) < 5)
                    {
                    printf("Tu ingreso fue demasiado corto. Recorda que son palabras de 5 caracteres.\n");

                    } else {
                        strcpy(intento, aux);
                        comprobante = 1;
                }
            }
        }


        j = 0;  //Normaliza la palabra a mayuscula
        while(intento[j])
        {
            intento[j] = toupper(intento[j]);
            j++;
        }

        //guardar los intentos para mostrarlos al final de la sesion
        for(k=0; k<5; k++)
        {
            palabras[actual][i][k]=intento[k];
        }
        palabras[actual][i][6] = '\0';


        //Si la palabra y el ingreso son iguales, ganaste!
        //Break para salir del for de los 5 intentos de ingresar palabra y cortar las otras verificaciones
        if (strcmp(intento, wordle) == 0)
        {
            if(i == 0)
            {
                puntaje = puntaje + 5000;
            } else
            {
                puntaje = puntaje + 2000;
            }
            verde();
            printf("Adivinaste la palabra!\n");
            printf("Tu puntaje final es de %d puntos.\n", puntaje);
            colorReset();
            break;
        } else
        {
            derrota++;
        }

        //Aca el ingreso ya es valido y guardado en intento
        //Se revisa en las 5 letras del intento si esta bien puesta,
        //o si esta en cualquier otro lado de la palabra (con un for anidado)
        for(j = 0; j < 5; j++)
        {
            comprobante = 0;
            if(wordle[j] == intento[j])
            { //letra correcta en lugar correcto
                verde();
                printf("%c", intento[j]);
                if(punt[j] != 2)
                {  //controlo que la letra no haya sumado puntos todavia
                puntaje = puntaje + 100;
                punt[j] = 2;
                }

            } else {
                for(k = 0; k < 5 ; k++)
                {   //letra correcta en lugar incorrecto
                    if(wordle[k] == intento[j])
                    {
                        amarillo();
                        printf("%c", intento[j]);

                        if(punt[k] == 0)
                        {
                            puntaje = puntaje + 50;
                            punt[k] = 1;
                        }
                        k = 6; //Para cortar antes este for si se diere el caso
                    } else comprobante++;
                }

                if(comprobante == 5)
                { //la letra no esta en la palabra
                    negro();
                    printf("%c", intento[j]);
                }
            }
        }
        colorReset();
        if(derrota == 6){
            puntaje=0;
            printf("\n\nPerdiste! Te quedaste sin intentos para adivinar la palabra\n");
            printf("Tu puntaje final es: %d puntos\n", puntaje);
            printf("El Wordle era: ");
            verde();
            printf("%s\n", wordle);
            colorReset();
        }
    }

    return puntaje;
}




void mostrarJugada(char todosIntentos[][7][6], char todosWordles[][1][6], int nroJugada, int puntaje){
    limpiarPantalla();
    printf("Estadisticas de tu jugada Nro %i\n-----", nroJugada);
    printf("Tu puntaje en esta ronda fue de %i\n", puntaje);
    printf("Tu Wordle era: "); verde(); printf("%s\n\n", todosWordles[nroJugada][0]); colorReset();
    bufferEnter();
    bufferEnter();
}




void wordleRandomizer(char wordle[]){

    //Se randomiza la semilla
    srand(time(NULL));

    //n = entero entre 0 y 30, la cantidad de palabras definida en el archivo dado
    int n = (rand() % CANTIDAD_PALABRAS);

    //Multiplataforma, por la cuestion de la barra en el path
  #if defined(linux) || defined(unix) || defined(APPLE)
    getWordInLine("palabras.txt", n, wordle);
  #endif

  #if defined(_WIN32) || defined(_WIN64)
    getWordInLine("palabras.txt", n, wordle);
  #endif

  wordle[5] = '\0';
}

void getWordInLine(char *fileName, int lineNumber, char *p) {
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(fileName, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    int i=1;
    while ((read = getline(&line, &len, fp)) != -1) {
        if (i==lineNumber) {
           printf("%s", line);
           strcpy(p, line);
           return;
        }
        i++;
    }
}


void mensajeNroJugada(int actual, int total){
    printf("Esta es tu jugada %i de %i \nRecorda que...\nVerde = Lugar correcto, Amarillo = Lugar incorrecto, Gris = Letra incorrecta\nCada letra en el wordle es unica y no va a repetirse \n-----\n", actual, total);
}

void verde(){
    printf("\033[0;32m");
}

void rojo(){
    //Setea el color a rojo
    printf("\033[0;31m");
}

void amarillo(){
    printf("\033[0;33m");
}

void blanco(){
    printf("\033[0;37m");
}
void negro(){
    printf("\033[1;30m");
}

void colorReset(){
    //vuelve al color default
    printf("\033[0m");
}


void limpiarPantalla(){
  #if defined(linux) || defined(unix) || defined(APPLE)
    system("clear");
  #endif

  #if defined(_WIN32) || defined(_WIN64)
    system("cls");
  #endif
 }


void bufferEnter(){
  int c;
  while ((c = getchar()) != '\n' && c != EOF){}
}
