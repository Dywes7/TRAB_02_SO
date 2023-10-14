#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <locale.h>

#define REPETICOES 100
#define NUM_FILOSOFOS 5

// DECLARANDO SEMAFOROS
sem_t garfos[NUM_FILOSOFOS];
sem_t mutex;
int execucoes = 0;

void *filosofo(void *arg) {

    setlocale(LC_ALL, "Portuguese");

    // FILOSOFO ID
    int id = *((int *)arg);
    // IDENTIFICAÇÃO DOS GARFOS
    int garfo_esquerda = id;
    int garfo_direita = (id + 1) % NUM_FILOSOFOS;

    for (int i = 0; i < REPETICOES; i++) {

        // FILOSOFO PENSANDO...
        printf("Filósofo %d está pensando\n", id);
        // TEMPO PENSANDO
        // sleep(rand() % 3);

        // PEGAR GARFOS (Acessar região crítica)
        sem_wait(&mutex);    // Tenta acessar região crítica e DECREMENTA MUTEX (Bloqueia acesso a região crítica)
        sem_wait(&garfos[garfo_esquerda]);  // PEGANDO GARFO DA ESQUERDA (caso livre)
        sem_wait(&garfos[garfo_direita]);   // PEGANDO GARFO DA DIREITA (caso livre)
        sem_post(&mutex);   // APÓS PEGAR GARFOS, LIBERAR REGIÃO CRÍTICA (Incrementa Mutex)

        // FILOSOFO COMENDO...
        printf("Filósofo %d está comendo\n", id);
        execucoes++;
        // sleep(rand() % 3);

        // DEVOLVE OS GARFOS
        sem_post(&garfos[garfo_esquerda]);
        sem_post(&garfos[garfo_direita]);

    }
}

int main() {

    setlocale(LC_ALL, "Portuguese");

    // REPRESENTAR IDENTIFICADORES DE THREADS (Filósofos)
    pthread_t filosofos[NUM_FILOSOFOS];

    // ENUMERAS AS THREADS
    int ids[NUM_FILOSOFOS];

    // INICIALIZA O SEMAFORO COM 1 (TORNA DISPONÍVEL PARA ACESSO)
    sem_init(&mutex, 0, 1);

    // INICIALIZAR "SEMAFORO" PARA ACESSO AOS GARFOS COM "1" (Livre)
    for (int i = 0; i < NUM_FILOSOFOS; i++) {

        sem_init(&garfos[i], 0, 1);

    }

    for (int i = 0; i < NUM_FILOSOFOS; i++) {

        // INICILIZA IDS DAS THREADS (Filosofos) PARA SER MANDADO COMO PARAMETRO
        ids[i] = i;
         // CRIAR THREADS (ENDEREÇO QUE GUARDA IDENTIFICADOR DA THREAD, NULL, FUNÇÃO À EXECUTAR, PARÂMETROS DA FUNCÃO)
        pthread_create(&filosofos[i], NULL, filosofo, &ids[i]);

    }

     // Esperar que todas as threads sejam encerradas antes que a funçao principal termine sua execução
    for (int i = 0; i < NUM_FILOSOFOS; i++) {

        pthread_join(filosofos[i], NULL);

    }

    printf("\nTOTAL de Execuções: %d\n", execucoes);

    return 0;

}
