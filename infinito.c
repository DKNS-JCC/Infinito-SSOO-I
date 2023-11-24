#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

void alarma(int sig);
int sigusrHandler(int sig);

pid_t pidp, pidh1, pidh2, pidh3, pidh4, pidn2, pidn3, mediavuelta;

int main()
{
    struct sigaction salarma;
    salarma.sa_handler = &alarma;
    sigemptyset(&salarma.sa_mask);
    salarma.sa_flags = 0;

    struct sigaction susr1;
    susr1.sa_handler = &sigusrHandler;
    sigemptyset(&susr1.sa_mask);
    susr1.sa_flags = 0;

    pidp = getpid();
    printf("P: %d\n", pidp);
    pidh1 = fork();

    switch (pidh1)
    {
    case -1:
        perror("Error en el fork h1");
        exit(-1);

    case 0:
        printf("H1: %d\n", getpid());
        pidh1 = getpid();
        break;
    default:
        pidh4 = fork();
        switch (pidh4)
        {
        case -1:
            perror("Error en el fork h4");
            exit(-1);
        case 0:
            printf("H4: %d\n", getpid());
            pidh4 = getpid();
            break;
        default:
            pidh2 = fork();
            switch (pidh2)
            {
            case -1:
                perror("Error en el fork h2");
                exit(-1);
            case 0:
                printf("H2: %d\n", getpid());
                pidn2 = fork();
                pidh2 = getpid();
                switch (pidn2)
                {
                case -1:
                    perror("Error en el fork n2");
                    exit(-1);
                case 0:
                    printf("N2: %d\n", getpid());
                    pidn2 = getpid();
                    break;
                default:
                    break;
                }
                break;
            default:
                pidh3 = fork();
                switch (pidh3)
                {
                case -1:
                    perror("Error en el fork h3");
                    exit(-1);
                case 0:
                    printf("H3: %d\n", getpid());
                    pidn3 = fork();
                    pidh3 = getpid();
                    switch (pidn3)
                    {
                    case -1:
                        perror("Error en el fork n3");
                        exit(-1);
                    case 0:
                        printf("N3: %d\n", getpid());
                        pidn3 = getpid();
                        break;
                    default:
                        break;
                    }
                    break;
                default:
                    break;
                }
                break;
            }
        }
    }
    if (sigaction(SIGALRM, &salarma, NULL) == -1)
    {
        perror("Error en sigaction alarma");
        exit(-1);
    }
    alarm(3);
    while (1)
    {
        // if (sigusrHandler(SIGUSR1) != 0)
        // {
        //     printf ("error al enviar señal a H%d", mediavuelta % 2 == 0 ? 3 : 2); //CALCULAR H3 O H2
        //     exit(-1);
        // }
    }
}

// PROGRAMA PARA MANEJAR LA SEÑAL SIGUSR1
int sigusrHandler(int sig)
{
    int error = 0;
    if (pidp == getpid())
    {
        if (pidp == getpid())
        {
            // ENVIAR SEÑAL A H3
            if (mediavuelta % 2 == 0) //CASO H3
            {
                if (kill (pidh3, sig) == -1)
                {
                    perror("Fallo al enviar señal de P a H3");
                    error = 3;
                }
                else
                {
                    puts("Señal enviada de P a H3");
                    mediavuelta++;
                }
            }
            // ENVIAR SEÑAL A H2
            else
            {
                if (kill (pidh2, sig) == -1)
                {
                    perror("Fallo al enviar señal de P a H2");
                    error = 2;
                }
                else
                {
                    puts("Señal enviada de P a H2");
                    mediavuelta++;
                }
            }
        }
    }
    if (pidh1 == getpid())
    {
        
    }
    return error;
}

// PROGRAMA PARA MATAR A TODOS LOS PROCESOS AL FINALIZAR EL TIEMPO
void alarma(int time)
{
    if (pidp == getpid())
    {
        puts("Soy el padre");
        wait(NULL);
        wait(NULL);
        wait(NULL);
        // ESPERA A QUE TODOS LOS HIJOS MUERAN MENOS H4
        if (kill(pidh4, SIGKILL) == -1)
        {
            perror("Error en kill h4");
            exit(-1);
        }
        else
        {
            puts("H4 muerto");
            wait(NULL);
        }
        printf("La señal ha dado:");
        exit(0);
    }
    else if (pidh1 == getpid())
    {
        puts("Soy el hijo 1");
    }
    else if (pidh2 == getpid())
    {
        puts("Soy el hijo 2");
        if (kill(pidn2, SIGKILL) == -1)
        {
            perror("Error en kill n2");
            exit(-1);
        }
        else
        {
            puts("N2 muerto");
            wait(NULL);
        }
    }
    else if (pidh3 == getpid())
    {
        puts("Soy el hijo 3");
        if (kill(pidn3, SIGKILL) == -1)
        {
            perror("Error en kill n3");
            exit(-1);
        }
        else
        {
            puts("N3 muerto");
            wait(NULL);
        }
        // Como h3 es el ultimo hijo en crearse, mata a todos los procesos y asi mismo.
        if (kill(pidh1, SIGKILL) == -1)
        {
            perror("Error en kill h1");
            exit(-1);
        }
        else
        {
            puts("H1 muerto");
        }
        if (kill(pidh2, SIGKILL) == -1)
        {
            perror("Error en kill h2");
            exit(-1);
        }
        else
        {
            puts("H2 muerto");
        }
        if (raise(SIGKILL) == -1)
        {
            perror("Error en raise");
            exit(-1);
        }
        else
        {
            puts("H3 muerto");
        }
    }
    else if (pidh4 == getpid())
    {
        puts("Soy el hijo 4");
    }
    else if (pidn2 == getpid())
    {
        puts("Soy el nieto 2");
    }
    else if (pidn3 == getpid())
    {
        puts("Soy el nieto 3");
    }
}