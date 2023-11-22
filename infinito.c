/*
El programa que hay que presentar constará de un único fichero fuente de nombre infinito.c. La correcta compilación de dicho programa,
producirá un fichero ejecutable, cuyo nombre será obligatoriamente infinito. Respetad las mayúsculas/minúsculas de los nombres, si las hubiere.

La ejecución del programa creará una serie de procesos que se transmitirán señales entre ellos.
En concreto, el árbol de procesos que hay que crear es el siguiente:

        <- H1
         H2 -> N2
        /
PADRE(P)
        \
         H3 -> N3
        <- H4

Para comprender mejor el funcionamiento de la práctica, cambiemos la posición de los procesos en el dibujo de forma que formen una figura semejante al símbolo de infinito:

    P -> H2 -> N2 -> H1 -> P-> H3 -> N3 -> H4 -> P

Las relaciones paterno-filiales son las indicadas por las líneas sólidas.
Durante veinticinco segundos, deben estar circulando señales entre los procesos en el sentido que indican las flechas rojas de la figura,
de modo que cuando un proceso recibe una señal, envía otra al siguiente proceso.
El proceso padre enviará la primera señal para que se ponga en marcha el mecanismo.
El tipo de señal o señales usadas se deja libre.
El proceso padre es el que controlará estos veinticinco segundos (no uséis sleep).

Transcurridos los venticinco segundos, el proceso padre dejará de enviar señales, arbitrará un mecanismo para que los procesos mueran (o se maten),
esperarán a que hayan muerto y escribirá por la salida estándar "La señal ha dado x vueltas.", donde x será el número de vueltas que ha dado la señal
a los procesos del dibujo.

Los procesos, una vez creados, deben permanecer bloqueados, sin consumir CPU mientras esperan a recibir la señal y enviar esa u otra al siguiente proceso.

Si se requiere sincronizar alguna acción, se pueden usar señales, pero la espera se debe realizar sin consumo de CPU.

Restricciones

    Se deberán usar llamadas al sistema siempre que sea posible, a no ser que se especifique lo contrario.
    No está permitido usar la función de biblioteca system, salvo indicación explícita en el enunciado de la práctica.
    Para generar el árbol de procesos, no se usarán llamadas al sistema de la familia exec.
    No se puede suponer que los PIDs de los procesos de una ristra van a aparecer consecutivos. Puestos en plan exquisito, ni siquiera podemos suponer que estarán ordenados de menor a mayor (puede ocurrir que se agoten los PIDs y retome la cuenta partiendo de cero).
    No está permitido el uso de ficheros, tuberías u otro mecanismo externo para transmitir información entre los procesos.



SIGNALS:
    -SIGKILL: envia una señal al proceso para que se mate -9 (predeterminada)
    -SIGTERM: envia una señal al proceso para que se mate = kill (pid)
    -SIGSTOP: envia una señal al proceso para que se pare
    -SIGUSR1: usar para trabajar como user (señal a mover)
    -SIGUSR2: usar para trabajar como user (señal a mover)
    -SIGCHLD: envia una señal al proceso padre cuando el hijo muere (por defecto se ignora)
    -SIGACTION: permite cambiar el comportamiento de las señales

sigset_t: variable que almacena un conjunto de señales
    -sigemptyset: inicializa el conjunto de señales
    -sigfillset: añade todas las señales al conjunto
    -sigaddset: añade una señal al conjunto
    -sigdelset: elimina una señal del conjunto
    -sigismember: comprueba si una señal pertenece al conjunto

sigprocmask: permite bloquear señales (señales pendientes)
    -SIG_BLOCK: bloquea las señales del conjunto
    -SIG_UNBLOCK: desbloquea las señales del conjunto
    -SIG_SETMASK: establece el conjunto de señales <-- USAR ESTE

sigsuspend: espera a que llegue una señal (señales pendientes)
    -sigprocmask: bloquea las señales del conjunto
    -pause: espera a que llegue una señal
    -sigprocmask: desbloquea las señales del conjunto

sigsetmask: espera a que llegue una señal (señales pendientes)
    -sigprocmask: bloquea las señales del conjunto
    -sigprocmask: desbloquea las señales del conjunto

¡¡¡¡¡CONTROL DE ERRORES A TODO!!!!
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>

static pid_t pidsHijos[4];
static pid_t pidsNietos[4];

int main()
{
    puts("ARBOL DE PROCESOS");

    int i, vueltas = 0;
    

    printf("Soy el padre %d\n", getpid());

    for (i = 1; i <= 4; i++)
    {
        pidsHijos[i] = fork(); // Crea un proceso hijo
        switch (pidsHijos[i])
        {
        case -1:

            perror("Error en el fork del proceso padre");
            exit(-1);
            break;

        case 0:

            if (i == 2 || i == 3)
            {
                pidsNietos[i] = fork();
                switch (pidsNietos[i])
                {
                case -1:

                    perror("Error en el fork del proceso hijo");
                    exit(-1);
                    break;

                case 0:

                    if (i == 2)
                    {
                        // Programa Nieto 2
                        accionN2();
                    }
                    else if (i == 3)
                    {
                        // Programa Nieto 3
                        accionN3();
                    }
                    break;

                default:

                    if (i == 2)
                    {
                        // Programa Hijo 2
                        accionH2();
                        wait(NULL);
                    }
                    else if (i == 3)
                    {
                        // Programa Hijo 3
                        accionH3();
                        wait(NULL);
                    }
                    wait(NULL);
                    break;
                }
            }
            else if (i == 1)
            {
                // Programa Hijo 1
                accionH1();
                wait(NULL);
            }
            else if (i == 4)
            {
                // Programa Hijo 1
                accionH4();
                wait(NULL);
            }
            exit(0);
            break;

        default:

            // Programa Padre
            accionP();
            wait(NULL);
            break;
        }
    }

    int t_inicio = time(NULL); // Tiempo de inicio
    while (time(NULL) - t_inicio < 25)
    {

        vueltas++;
    }

    printf("La señal ha dado %d vueltas.\n", vueltas);
}
void accionP()
{
    int i;
    // Programa Padre
    for (i = 1; i <= 4; i++)
    {
        printf ("PIDS Hijos: %d\n", pidsHijos[i]);
    }
    
}
void accionH1()
{
    // Programa Hijo 1
    printf("Soy el hijo %d %d\n", 1, getpid());
}
void accionH2()
{
    int i;
    // Programa Hijo 2
    printf("Soy el hijo %d %d\n", 2, getpid());
    for (i = 1; i <= 4; i++)
    {
        printf("PIDS Nietos: %d\n", pidsNietos[i]);
    }

}
void accionH3()
{
    int i;
    // Programa Hijo 3
    printf("Soy el hijo %d %d\n", 3, getpid());
    for (i = 1; i <= 4; i++)
    {
        printf("PIDS Nietos: %d\n", pidsNietos[i]);
    }
}
void accionH4()
{
    // Programa Hijo 4
    printf("Soy el hijo %d %d\n", 4, getpid());
}
void accionN2()
{
    // Programa Nieto 2
    printf("Soy el nieto %d %d de %d\n", 2, getpid(), getppid());
}
void accionN3()
{
    // Programa Nieto 3
    printf("Soy el nieto %d %d de %d\n", 3, getpid(), getppid());
}
