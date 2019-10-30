#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include "timer.h"



typedef struct intervalo {
    double comprimento;
    double largura;
} intervalo;


typedef struct pilha_no {
	struct pilha_no * prox;
	intervalo * inter;
} no;


no *pilhaCriartopo() {
    no *topo = (no*) malloc(sizeof(no));
    topo->prox = NULL;
    topo->inter = NULL;
    return topo;
}


intervalo *pop(no *topo) {

  if(topo == NULL){
    printf("Erro na funcao \n");
    exit(-1);
  }

	intervalo * inter = NULL;
	no * noTopo;

	noTopo = topo->prox;
	if(noTopo == NULL) {

	} else {
    topo->prox = noTopo->prox;
    inter = noTopo->inter;
  }

	free(noTopo);
	return inter;
}


void push(no *topo, intervalo *inter) {

  if(topo == NULL){
    printf("Erro na funcao\n");
    exit(-1);
  }

  no *noNovo, *noTopo;

	noNovo = (no*) malloc(sizeof(no));
	noNovo->inter = inter;
	noTopo = topo->prox;

	noNovo->prox = noTopo;
	topo->prox = noNovo;
}


void pilhaDestruir(no *topo) {
	intervalo *inter = pop(topo);

	while(inter != NULL) {
		free(inter);
		inter = pop(topo);
	}

	free(topo);
}


intervalo *criarintervalo(double comprimento, double largura) {

    intervalo *inter;

    inter = (intervalo*)malloc(sizeof(intervalo));
    inter->comprimento = comprimento;
    inter->largura = largura;

    return inter;
}

pthread_mutex_t mutex_particoes;
pthread_mutex_t mutex_pilha;
pthread_mutex_t mutex_area;

no * pilha_intervalos;
int particoes_restantes = 0;
double erro;
double area_total = 0.0;



double function(double x)
{
	return 1 + x;
}


void *Integrarintervalos(void * arg) {

	intervalo *inter;


	while(particoes_restantes > 0) {

		pthread_mutex_lock(&mutex_pilha);
		inter = pop(pilha_intervalos);
		pthread_mutex_unlock(&mutex_pilha);


		if(inter == NULL)
		  continue;


    double ponto_medio = (inter->comprimento + inter->largura)/2;
		double area1 = function(ponto_medio) * (inter->largura - inter->comprimento);
		double area2 = function((inter->comprimento + ponto_medio) / 2) * (ponto_medio - inter->comprimento) +  function((ponto_medio + inter->largura) / 2) * (inter->largura - ponto_medio);

		double erro_local = fabs(area2 - area1);


		if(erro_local <= erro) {
		  pthread_mutex_lock(&mutex_area);
		  area_total += area1;
		  pthread_mutex_unlock(&mutex_area);

			pthread_mutex_lock(&mutex_particoes);
			particoes_restantes--;
			pthread_mutex_unlock(&mutex_particoes);

			continue;
		}


  	intervalo * inter1 = criarintervalo(inter->comprimento, ponto_medio);
    intervalo * inter2 = criarintervalo(ponto_medio, inter->largura);

    pthread_mutex_lock(&mutex_pilha);
    push(pilha_intervalos, inter1);
    push(pilha_intervalos, inter2);
    pthread_mutex_unlock(&mutex_pilha);

    pthread_mutex_lock(&mutex_particoes);
    particoes_restantes++;
    pthread_mutex_unlock(&mutex_particoes);
	}

	pthread_exit(NULL);
}



int isZero(char *str) {
  if(str[0] != '0' && str[0] != '+' && str[0] != '-')
    return 0;
  int i = 1;
  int has_dot = 0;
  while(str[i] != '\0') {
    if(str[i] == '.')
      has_dot++;
    else if(str[i] != '0')
      return 0;
    i++;
  }
  if(has_dot > 1)
    return 0;
  else
    return 1;
}

int main(int argc, char *argv[]) {

	double comprimento      = atof(argv[1]);
	double largura      = atof(argv[2]);
	erro          = atof(argv[3]);
	int numthreads = atoi(argv[4]);

  if((argc != 5) || (comprimento == 0 && !(isZero(argv[1]))) || (largura == 0 && !(isZero(argv[2]))) || (erro == 0 && !(isZero(argv[3]))) || numthreads <= 0){
    printf("Erro\n", argv[0]);
    return -1;
  }

	pilha_intervalos = pilhaCriartopo();


	intervalo *intervalo_inicial = criarintervalo(comprimento, largura);
	push(pilha_intervalos, intervalo_inicial);
	particoes_restantes++;


  double comeco, fim;
	GET_TIME(comeco);

	pthread_t tid_sistema[numthreads];

	for(int i = 0; i < numthreads; i++) {
		pthread_create(&tid_sistema[i], NULL, Integrarintervalos, NULL);
	}

	for(int i = 0; i < numthreads; i++) {
	    pthread_join(tid_sistema[i], NULL);
	}

	GET_TIME(fim);

	printf("Area Total = %lf\n", area_total);
	printf("Tempo de execução = %lf\n", fim-comeco);

	pilhaDestruir(pilha_intervalos);

	return 0;
}
