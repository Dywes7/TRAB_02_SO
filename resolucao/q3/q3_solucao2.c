/* ***************SOLU��O 2 Acesso justo as threads ************* */ 

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <locale.h>

#define ESCRITORES 2
#define LEITORES 5

sem_t mutex;
sem_t sem_escrita;

int leitores_atuais = 0;

void *leitor (void *ids) {

    setlocale(LC_ALL, "Portuguese");

    int id = *((int *)ids);

    while (1) {

        // GARANTINDO ACESSO EXCLUSIVO A VARIAVEL COMPARTILHADA ENTRE OS LEITORES
        sem_wait(&mutex);
        leitores_atuais++;  // UM LEITOR ENTRANDO NA REGI�O CR�TICA

        // SE FOR O PRIMEIRO LEITOR A ENTRAR NA REGI�O CR�TICA
        if (leitores_atuais == 1) {
            // BLOQUEANDO POSSIBLIDADE DE ESCRITA
            sem_wait(&sem_escrita);

        }
        // PERMITIR QUE OUTROS LEITORES (THREADS) ACESSEM A VARI�VEL COMPARTILHADA
        sem_post(&mutex);

        printf("Leitor %d -> LENDO NA REGI�O CR�TICA\n", id);

        // BLOQUEANDO ACESSO A VARIAVEL COMPARTILHADA PARA ATUALIZ�-LA
        sem_wait(&mutex);
        leitores_atuais--; // UM LEITOR SAINDO DA REGI�O CR�TICA

        // SE O LEITOR FOR O �LTIMO A SAIR DA REGI�O CR�TICA, ENT�O LIBERAR ESCRITA
        if (leitores_atuais == 0) {

            sem_post(&sem_escrita);

        }

        // PERMITIR QUE OUTROS LEITORES (THREADS) ACESSEM A VARI�VEL COMPARTILHADA
        sem_post(&mutex);

        // SIMULANDO LEITURA
        sleep(1);

    }

}


void *escritor (void *ids) {

    setlocale(LC_ALL, "Portuguese");

    int id = *((int *)ids);

    while (1) {

        // TENTANDO ACESSAR REGI�O CR�TICA
        sem_wait(&sem_escrita);
        printf("Escritor %d -> ESCREVENDO NA REGI�O CR�TICA\n", id);

        sem_post(&sem_escrita);
        // printf("Escritor %d -> SAINDO DA REGI�O CR�TICA\n", id);

       // SIMULANDO ESCRITA 
       sleep(1);

    }
    

}

int main () {

    pthread_t escri[ESCRITORES];
    pthread_t leit[LEITORES];

    int ids_escri[ESCRITORES];
    int ids_leit[LEITORES];

    // INICIALIZANDO SEMAFOROS, Liberando acesso a regi�o cr�tica
    sem_init(&mutex, 0, 1);
    sem_init(&sem_escrita, 0, 1);

    for (int i = 0; i < ESCRITORES; i++) {

        // INICILIZA IDS DAS THREADS PARA SER MANDADO COMO PARAMETRO
        ids_escri[i] = i;

        // CRIAR THREADS (ENDERE�O QUE GUARDA IDENTIFICADOR DA THREAD, NULL, FUN��O � EXECUTAR, PAR�METROS DA FUNC�O )
        pthread_create(&escri[i], NULL, escritor, &ids_escri[i]);

    }

    for (int i = 0; i < LEITORES; i++) {

        // INICILIZA IDS DAS THREADS PARA SER MANDADO COMO PARAMETRO
        ids_leit[i] = i;

        // CRIAR THREADS (ENDERE�O QUE GUARDA IDENTIFICADOR DA THREAD, NULL, FUN��O � EXECUTAR, PAR�METROS DA FUNC�O )
        pthread_create(&leit[i], NULL, leitor, &ids_leit[i]);

    }

    // AGUARDA FIM DAS THREADS ANTES DE ENCERRAR FUN��O PRINCIPAL
    for (int i = 0; i < ESCRITORES; i++) {

        pthread_join(escri[i], NULL);

    }

    // AGUARDA FIM DAS THREADS ANTES DE ENCERRAR FUN��O PRINCIPAL
    for (int i = 0; i < LEITORES; i++) {

        pthread_join(leit[i], NULL);

    }

    return 0;
}