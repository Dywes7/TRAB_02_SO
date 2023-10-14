#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#define TROCA_CONTEXTO 1

typedef struct {

    int id_processo;
    int tempo_burst;
    int execucao_restante;
    int numero_execucoes;
    int tempo_esperando;
    int tempo_espera;
    int tempo_retorno;
    int cont_tempo_retorno;
    int tempo_consumido;

} Processo;

// MARCADOR DE TEMPO GLOBAL PARA CADA CICLO
int temporizador = 0;
int primeira_execucao = 0;
int processos_concluidos = 0;
int preempcoes = 0;
float vazao = 0;

void executa_processo (Processo *process, int quantum) {

    // ADICIONA AO TEMPO DE ESPERA A TROCA DE CONTEXTO
    if (primeira_execucao) {

        process->tempo_esperando += TROCA_CONTEXTO;

    }

    process->tempo_espera = process->tempo_esperando;       // GUARDANDO VALOR DE TEMPO DE ESPERA MEDIDO
    process->cont_tempo_retorno += process->tempo_espera;   // AUMENTA O TEMPO DE ESPERA EM CADA TEMPO DE RETORNO
    process->tempo_esperando = 0;   // INICIALIZA TEMPO DE ESPERANDO DO PROCESSO
    process->numero_execucoes++;    // INCREMENTA O NUMERO DE EXECUCOES DO PROCESSO

    // RENOVA TEMPO DE EXECUÇÃO RESTANTE PARA O VALOR DO BURST, JÁ QUE ESTAVA ZERADO (FOI CONCLUÍDO)
    if (process->execucao_restante == 0) {

        process->execucao_restante = process->tempo_burst;

    }

    if (process->execucao_restante <= quantum) {

        process->tempo_consumido = process->execucao_restante;  // TEMPO GASTO DA CPU = TEMPO RESTANTE DO PROCESSO
        process->cont_tempo_retorno += process->tempo_consumido;    // ADICIONA AO TEMPO DE RETORNO O TEMPO CONSUMIDO DE CPU
        process->tempo_retorno = process->cont_tempo_retorno;   // VARIAVEL TEMPO DE RETORNO = TEMPO DE RETORNO MEDIDO
        process->execucao_restante = 0;       // EXECUTOU TUDO QUE PRECISAVA
        process->cont_tempo_retorno = 0;
        processos_concluidos++; // AUMENTA UM PROCESSO CONCLUIDO        

    } else {    

        process->execucao_restante = process->execucao_restante - quantum;  // CONSOME PERÍODO DO QUANTUM, MAIS AINDA RESTA EXECUÇÃO
        // temporizador += quantum;            // AUMENTA O TEMPO EM 1 QUANTUM
        process->tempo_consumido = quantum;     // TEMPO CONSUMIDO = TEMPO DE QUANTUM
        process->cont_tempo_retorno += quantum;  // ADICIONA AO TEMPO DE RETORNO O VALOR DE QUANTUM CONSUMIDO
        preempcoes++;       // AUMENTA O NUMERO DE PREEMPÇOES

    }
    
}

void escalonador (Processo process[], int num_procesos, int quantum) {

    // SUPONDO EXECUÇAO INICIAL A PARTIR DO PROCESSO COM O MENOR ID
    for (int i = 0; i < num_procesos; i++) {

        // CHAMA FUNCAO EXECUTA PROCESSO
        executa_processo(&process[i], quantum);
        
        // CONTAGEM DE TEMPO GLOBAL
        temporizador += process[i].tempo_consumido + TROCA_CONTEXTO;

        // AUMENTA A CONTAGEM DE ESPERA DOS OUTROS PROCESSOS
        for (int j = 0; j < num_procesos; j++) {

            if (j == i) {
                // PULA A ADIÇAO DO TEMPO DO PROCESSO NELE MESMO
                continue;

            } else {
                // ADICIONA TEMPO CONSUMIDO NO TEMPO DE ESPERA DOS OUTROS PROCESSOS
                process[j].tempo_esperando += (process[i].tempo_consumido + TROCA_CONTEXTO);


            }
        }
    }  
}

int main() {

    setlocale(LC_ALL, "Portuguese");

    int qtd_processos;
    float media_tempo_retorno = 0;
    float media_tempo_espera = 0;
    int quantum;
    int num_ciclos;

    printf("Digite o número de processos: ");
    scanf("%d", &qtd_processos);

    Processo processos[qtd_processos];

    for (int i = 0; i < qtd_processos; i++) {

        printf("Digite o tempo de burst para o processo %d: ", i + 1);
        scanf("%d", &processos[i].tempo_burst);

        processos[i].id_processo = i;
        processos[i].tempo_espera = 0;
        processos[i].numero_execucoes = 0;
        processos[i].execucao_restante = processos[i].tempo_burst;   // TEMPO DE EXECUÇÃO INICIAL = TEMPO BURST
        processos[i].tempo_esperando = 0;
        processos[i].tempo_consumido = 0;
        processos[i].tempo_retorno = 0;
        processos[i].cont_tempo_retorno = 0;

    }


    printf("Digite o valor do quantum: ");
    scanf("%d", &quantum);

    printf("Quantos ciclos de execução você deseja realizar? ");
    scanf("%d", &num_ciclos);

    escalonador(processos, qtd_processos, quantum);
    primeira_execucao = 1;

    for (int j = 0; j < num_ciclos - 1; j++) {

        escalonador(processos, qtd_processos, quantum);

    }


    vazao = (float) processos_concluidos / (float) temporizador;

    for (int i = 0; i < qtd_processos; i++) {

        media_tempo_espera += processos[i].tempo_espera;
        media_tempo_retorno += processos[i].tempo_retorno;

    }

    printf("\nPROCESSOS CONCLUÍDOS AO FINAL: %d\n", processos_concluidos);
    printf("NUMERO DE PREEMPÇÕES: %d\n", preempcoes);
    printf("VAZAO: %.5f -> %d PROCESSOS CONCLUIDOS A CADA %d UNIDADES DE TEMPO\n", vazao, processos_concluidos, temporizador);
    printf("VALOR MÉDIO DE ESPERA: %.2f (Unidades de Tempo)\n", media_tempo_espera / (float) qtd_processos);
    printf("VALOR MÉDIO DE RETORNO: %.2f (Unidades de Tempo)\n\n", media_tempo_retorno / (float) qtd_processos);

    return 0;

}