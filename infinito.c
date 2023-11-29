#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

// PROTOTIPO MANEJADORES
void alarma(int sig);
void sigusrHandler1(int sig);
void sigusrHandler2(int sig);

// VARIABLE GLOBAL
typedef struct
{
    pid_t pidp;
    pid_t pidh1;
    pid_t pidh2;
    pid_t pidh3;
    pid_t pidh4;
    pid_t pidn2;
    pid_t pidn3;
    int mediavuelta;
} info;

info pids;

int main()
{
    // CREACION DE MANEJADORES Y CONFIGURACION
    struct sigaction salarma;
    salarma.sa_handler = &alarma;

    struct sigaction susr1;
    susr1.sa_handler = &sigusrHandler1;
    susr1.sa_flags = SA_RESTART; // NO LETAL

    struct sigaction susr2;
    susr2.sa_handler = &sigusrHandler2;
    susr2.sa_flags = SA_RESTART; // NO LETAL

    // MASCARA DE SEÑALES
    sigset_t mask;
    sigfillset(&mask);
    if (sigprocmask(SIG_SETMASK, &mask, NULL) == -1) // MASCARA DEFAULT
    {
        perror("Error en sigprocmask");
        exit(-1);
    }

    // MASKS DE SUSPEND PARA SIGUSR1 (INFINITO)
    sigset_t maskusr1;
    sigfillset(&maskusr1);
    sigdelset(&maskusr1, SIGUSR1);
    sigdelset(&maskusr1, SIGALRM);
    sigdelset(&maskusr1, SIGINT);

    // MASKS DE SUSPEND PARA SIGUSR2 (KILL FINAL)
    sigset_t maskusr2;
    sigfillset(&maskusr2);
    sigdelset(&maskusr2, SIGUSR1);
    sigdelset(&maskusr2, SIGUSR2);
    sigdelset(&maskusr2, SIGINT);

    // ASIGNACION DE MANEJADORES
    if (sigaction(SIGALRM, &salarma, NULL) == -1)
    {
        perror("Error en sigaction alarma");
        exit(-1);
    }

    if (sigaction(SIGUSR1, &susr1, NULL) == -1)
    {
        perror("Error en sigaction usr1");
        exit(-1);
    }

    if (sigaction(SIGUSR2, &susr2, NULL) == -1)
    {
        perror("Error en sigaction usr2");
        exit(-1);
    }

    puts("     _..-----.._            _..-----.._");
    puts("  .-'           '-.      .-'           '-.");
    puts(" /                 '. .'                  \\");
    puts("|                    ><                    |");
    puts(" \\                 .' '._                 /");
    puts("   '-._         _.-'     '- _         _.-'");
    puts("       ''-----''             ''------'       ");

    // CREACION DE PROCESOS
    pids.pidp = getpid();
    printf("P: %lu\n", pids.pidp);
    pids.pidh1 = fork(); // CREACION DE H1

    switch (pids.pidh1)
    {
    case -1:

        perror("Error en el fork h1");
        kill(pids.pidp, SIGKILL);
        exit(-1);
        break;

    case 0:

        printf("  |_H1: %lu\n", getpid());
        pids.pidh1 = getpid();

        while (1)
        {
            sigsuspend(&maskusr1);
            // MANDAR SEÑAL DE H1 A P
            kill(pids.pidp, SIGUSR1);
        }

        break;

    default:

        pids.pidh4 = fork(); // CREACION DE H4
        switch (pids.pidh4)
        {
        case -1:

            perror("Error en el fork h4");
            exit(-1);

        case 0:

            printf("  |_H4: %lu\n", getpid());
            pids.pidh4 = getpid();

            while (1)
            {
                sigsuspend(&maskusr1);
                // MANDAR SEÑAL DE H4 A P
                kill(pids.pidp, SIGUSR1);
            }
            break;

        default:

            pids.pidh2 = fork(); // CREACION DE H2
            switch (pids.pidh2)
            {
            case -1:

                perror("Error en el fork h2");
                exit(-1);

            case 0:

                printf("  |_H2: %lu\n", getpid());
                pids.pidn2 = fork(); // CREACION DE N2
                pids.pidh2 = getpid();
                switch (pids.pidn2)
                {
                case -1:
                    perror("Error en el fork n2");
                    exit(-1);
                case 0:
                    printf("      |_N2: %lu\n", getpid());
                    pids.pidn2 = getpid();
                    while (1)
                    {
                        sigsuspend(&maskusr1);
                        // MANDAR SEÑAL DE N2 A H1
                        kill(pids.pidh1, SIGUSR1);
                    }
                    break;
                default:
                    while (1)
                    {
                        sigsuspend(&maskusr2);
                        // MANDAR SEÑAL DE H2 A N2
                        kill(pids.pidn2, SIGUSR1);
                    }
                    break;
                }
                break;

            default:

                pids.pidh3 = fork(); // CREACION DE H3
                switch (pids.pidh3)
                {
                case -1:

                    perror("Error en el fork h3");
                    exit(-1);

                case 0:

                    printf("  |_H3: %lu\n", getpid());
                    pids.pidn3 = fork(); // CREACION DE N3
                    pids.pidh3 = getpid();
                    switch (pids.pidn3)
                    {
                    case -1:

                        perror("Error en el fork n3");
                        exit(-1);
                    case 0:

                        printf("      |_N3: %lu \n", getpid());
                        pids.pidn3 = getpid();

                        while (1)
                        {
                            sigsuspend(&maskusr1);
                            // MANDAR SEÑAL DE N3 A H4
                            kill(pids.pidh4, SIGUSR1);
                        }
                        break;

                    default:

                        while (1)
                        {
                            sigsuspend(&maskusr2);
                            // MANDAR SEÑAL DE H3 A N3
                            kill(pids.pidn3, SIGUSR1);
                        }
                        break;
                    }
                    break;
                default:

                    break;
                }
                break;
            }
        }

        // SET DE ALARMA
        alarm(25);

        kill(pids.pidh2, SIGUSR1);
        while (1)
        {
            // MANDAR SEÑAL A UN LADO U OTRO DEPENDIENDO DE LA VUELTA
            if ((pids.mediavuelta % 2) == 0)
            {
                sigsuspend(&maskusr1);
                pids.mediavuelta++;
                kill(pids.pidh3, SIGUSR1);
            }
            else
            {
                sigsuspend(&maskusr1);
                pids.mediavuelta++;
                kill(pids.pidh2, SIGUSR1);
            }
        }
    }
}

// MANEJADOR DE SIGALRM
void alarma(int sig)
{
    if (sig == SIGALRM)
    {

        if (getpid() == pids.pidp)
        {
            sigset_t maskp;
            sigfillset(&maskp);
            sigdelset(&maskp, SIGUSR2);

            // ESPERA A QUE N2 Y N3 MUERAN PARA MATAR A HIJOS
            kill(pids.pidh2, SIGUSR2);
            sigsuspend(&maskp);

            kill(pids.pidh3, SIGUSR2);
            sigsuspend(&maskp);

            // MATAR A LOS HIJOS
            kill(pids.pidh1, SIGTERM); // MATAR A H1
            kill(pids.pidh1, SIGKILL); // Si no muere a la primera, matar a la segunda
            kill(pids.pidh4, SIGTERM);
            kill(pids.pidh4, SIGKILL);
            kill(pids.pidh2, SIGTERM);
            kill(pids.pidh2, SIGKILL);
            kill(pids.pidh3, SIGTERM);
            kill(pids.pidh3, SIGKILL);

            // ESPERA A QUE LOS HIJOS MUERAN PARA ACABAR
            wait(NULL);
            wait(NULL);
            wait(NULL);
            wait(NULL);

            // IMPRIMIR RESULTADO
            printf("\n=== La senal ha dado |%d| vueltas ===\n", pids.mediavuelta / 2);

            // MATAR A P
            exit(0);
        }
    }
}

// MANEJADORES DE SIGUSR1
void sigusrHandler1(int sig)
{
    if (sig == SIGUSR1)
    {
    }
    else
    {
        puts("ERROR EN SIGUSR1");
    }
}

// MANEJADORES DE SIGUSR2 (MATAR A N2 Y N3)
void sigusrHandler2(int sig)
{
    if ((getpid() == pids.pidh2 || getpid() == pids.pidh3) && sig == SIGUSR2)
    {

        if (getpid() == pids.pidh2)
        {
            kill(pids.pidn2, SIGKILL);
            wait(NULL);
            kill(getppid(), SIGUSR2);
        }

        if (getpid() == pids.pidh3)
        {
            kill(pids.pidn3, SIGKILL);
            wait(NULL);
            kill(getppid(), SIGUSR2);
        }
    }
}