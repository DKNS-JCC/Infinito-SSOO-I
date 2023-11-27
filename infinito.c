#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

void alarma(int sig);
void sigusrHandler1(int sig);
void sigusrHandler2(int sig);

int pidp, pidh1, pidh2, pidh3, pidh4, pidn2, pidn3, mediavuelta,infinitos;

int main()
{
    struct sigaction salarma;
    salarma.sa_handler = &alarma;

    struct sigaction susr1;
    susr1.sa_handler = &sigusrHandler1;
    susr1.sa_flags = SA_RESTART;

    struct sigaction susr2;
    susr2.sa_handler = &sigusrHandler2;
    susr2.sa_flags = SA_RESTART;

    sigset_t mask;
    sigfillset(&mask);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    sigset_t maskusr1;
    sigfillset(&maskusr1);
    sigdelset(&maskusr1, SIGUSR1);
    sigdelset(&maskusr1, SIGALRM);
    sigdelset(&maskusr1, SIGINT);

    sigset_t maskusr2;
    sigfillset(&maskusr2);
    sigdelset(&maskusr2, SIGUSR1);
    sigdelset(&maskusr2, SIGUSR2);
    sigdelset(&maskusr2, SIGINT);

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
    alarm(5);
    pidp = getpid();
    printf("P: %d\n", pidp);
    pidh1 = fork();

    switch (pidh1)
    {
    case -1:
        perror("Error en el fork h1");
        exit(-1);

    case 0:
        printf("H1 creado: %d\n", getpid());
        pidh1 = getpid();
        while (1)
        {
            sigsuspend(&maskusr1);
            
            kill(pidp, SIGUSR1);
            
        }
        break;
    default:
        pidh4 = fork();
        switch (pidh4)
        {
        case -1:
            perror("Error en el fork h4");
            exit(-1);
        case 0:
            printf("H4 creado: %d\n", getpid());
            pidh4 = getpid();
            while (1)
            {
                sigsuspend(&maskusr1);
                kill(pidp, SIGUSR1);
                
            }
            break;
        default:
            pidh2 = fork();
            switch (pidh2)
            {
            case -1:
                perror("Error en el fork h2");
                exit(-1);
            case 0:
                printf("H2 creado: %d\n", getpid());
                pidn2 = fork();
                pidh2 = getpid();
                switch (pidn2)
                {
                case -1:
                    perror("Error en el fork n2");
                    exit(-1);
                case 0:
                    printf("N2 creado: %d\n", getpid());
                    pidn2 = getpid();
                    while (1)
                    {
                        sigsuspend(&maskusr1);
                        kill(pidh1, SIGUSR1);
                    }
                    break;
                default:
                    while (1)
                    {
                        sigsuspend(&maskusr2);
                        kill(pidn2, SIGUSR1);
                    }
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
                    printf("H3 creado: %d\n", getpid());
                    pidn3 = fork();
                    pidh3 = getpid();
                    switch (pidn3)
                    {
                    case -1:
                        perror("Error en el fork n3");
                        exit(-1);
                    case 0:
                        printf("N3 creado: %d \n", getpid());
                        pidn3 = getpid();
                        while (1)
                        {
                            sigsuspend(&maskusr1);
                            kill(pidh4, SIGUSR1);
                        }
                        break;
                    default:
                        while (1)
                        {
                            sigsuspend(&maskusr2);
                            kill(pidn3, SIGUSR1);
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
        kill(pidh3, SIGUSR1);
        while (1)
        {
            if ((mediavuelta%2)==0)
            {   
                sigsuspend(&maskusr1);
                mediavuelta++;
                kill(pidh2, SIGUSR1);
                
            }
            else 
            {
                sigsuspend(&maskusr1);
                mediavuelta++;
                kill(pidh3, SIGUSR1);
            }
        }
    }
}

void alarma(int sig)
{
    if (getpid()==pidp)
    {
        printf("Ha dado %d vueltas\n", mediavuelta/2);
        kill (pidh2, SIGUSR2);
        kill (pidh3, SIGUSR2);
        printf ("PASS\n");
    }
    else if (getpid()==pidh1)
    {
        printf("H1: %d\n", getpid());
    }
    else if (getpid()==pidh2)
    {
        printf("H2: %d\n", getpid());
    }
    else if (getpid()==pidh3)
    {
        printf("H3: %d\n", getpid());
    }
    else if (getpid()==pidh4)
    {
        printf("H4: %d\n", getpid());
    }
    else if (getpid()==pidn2)
    {
        printf("N2: %d\n", getpid());
    }
    else if (getpid()==pidn3)
    {
        printf("N3: %d\n", getpid());
    }
    else
    {
        printf("Error en la alarma\n");
    }
}

void sigusrHandler1(int sig){
    printf("Infinitos: %d\n", infinitos);
    infinitos++;
}

void sigusrHandler2(int sig){
    if (getpid()==pidh2)
    {
        kill (pidn2, SIGKILL);
        puts ("N2 muerto");
    }
    else if (getpid()==pidh3)
    {
        kill (pidn3, SIGKILL);
        puts ("N3 muerto");
    }
    else
    {
        printf("Error kill nietos\n");
    }
}