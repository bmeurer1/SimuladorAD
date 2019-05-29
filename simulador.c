#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "eventos.h"

int total_ID = 0;

//Lista os eventos da fila em ordem cronológica
void le_fila(struct Node *node){

	printf("FILA\n");
	while(node != NULL){
		printf("(%d|", node->ID);
		printf("%c|", node->tipo_de_evento);
		printf("%f) ", node->tempo_de_chegada);
		node = node->proximo;
	}
	printf("\nEND FILA\n");

}


int main(int argc, char const *argv[])
{

	if(argc != 3){
		printf("Favor utilizar o seguinte formato: %s <Utilização> <Discplina de Atendimento (FCFS ou LCFS)>\n", argv[0]);
		return 0;
	}

	double utilizacao = atof(argv[1]);

	const char *disciplina = argv[2];

	if(strcmp(disciplina, "FCFS") != 0 && strcmp(disciplina, "LCFS") != 0){
		printf("Discplina '%s' não reconhecida. Utilizar 'FCFS' ou 'LCFS'\n", disciplina);
		return 0;
	}

	srand(time(NULL));
	struct Node *inicio 			= NULL;
	struct Node *fim 				= NULL;
	struct Node *fila_de_espera		= NULL;
	double tempo_total_de_simulacao = 0.0;

	double taxa_de_servico   = 1.0;
	double taxa_de_chegadas  = (utilizacao/taxa_de_servico);

	int rodadas 			 = 10000;
	int rodadas_transientes  = 1000;
	int amostras 			 = 1000;

	int servidor_ocioso 	 = 1;
	int clientes_servidos 	 = 0;
	int num_clientes_na_fila = 0;

	double tempo_medio_na_fila 				 = 0.0;
	double tempo_medio_na_fila_todas_rodadas = 0.0;
	double tempo_soma_medias				 = 0.0;
	double tempo_soma_medias_quadrado		 = 0.0;

	double area_clientes_na_fila			 = 0.0;
	double area_soma_medias				 	 = 0.0;
	double area_soma_medias_quadrado		 = 0.0;
	double area_media_na_fila_todas_rodadas  = 0.0;
	

//FASE TRANSIENTE
//--------------------------------------------------------------------------------

	//Insere o primeiro evento na fila para dar a partida
	insere_evento(&inicio, ++total_ID, 'C', tempo_total_de_simulacao + gera_chegada(taxa_de_chegadas));

	for(int i = 0; i < rodadas_transientes; i++){
		
		clientes_servidos = 0.0;
		
		while(1){

			//Se o evento for uma chegada
			if(inicio->tipo_de_evento == 'C'){

				//Avança o tempo de simulação até a chegada do evento
				tempo_total_de_simulacao = inicio->tempo_de_chegada;

				//Copia informações do evento de chegada para criar um evento de saída
				int temp_ID = inicio->ID;

				pop_evento(&inicio);

				//Se o servidor estiver ocioso, tratar o evento imediatamente
				if(servidor_ocioso){
					
					//Retira o evento de chegada da fila, insere o evento de saída e cria o próximo evento de chegada
					insere_evento(&inicio, temp_ID, 'S', tempo_total_de_simulacao + gera_chegada(taxa_de_servico));

					//Evento está sendo tratado, servidor não está mais ocioso
					servidor_ocioso = 0;

				//Se o servidor estiver ocupado, colocar o evento na fila de espera
				}else{

					//Aumenta quantidade de clientes na fila de espera
					num_clientes_na_fila++;
					insere_evento(&fila_de_espera, temp_ID, 'C', tempo_total_de_simulacao);
				}

				//Cria o próximo evento de chegada
				insere_evento(&inicio, ++total_ID, 'C', tempo_total_de_simulacao + gera_chegada(taxa_de_chegadas));

			}else{

				//Avança o tempo até a saída do evento
				tempo_total_de_simulacao = inicio->tempo_de_chegada;

				//Retira o evento da fila
				pop_evento(&inicio);

				//Se houver algum evento na fila de espera, tratá-lo imediatamente
				if(fila_de_espera != NULL){

					//Diminui quantidade de clientes na fila
					num_clientes_na_fila--;

					//Copia informações do evento para criar um evento de saída
					int temp_ID = fila_de_espera->ID;

					//Retira o evento da fila de espera e insere o evento de saída na lista de eventos
					pop_evento(&fila_de_espera);
					insere_evento(&inicio, temp_ID, 'S', tempo_total_de_simulacao + gera_chegada(taxa_de_servico));

				//Caso contrário, marcar o servidor como ocioso
				}else{
					servidor_ocioso = 1;
				}

				//Se já serviu clientes suficientes, sai do loop
				if(++clientes_servidos == amostras){
					break;
				}

			}

		// 	printf("%lf\n", tempo_total_de_simulacao);
		// 	le_fila(inicio);
		// le_fila(fila_de_espera);
		// printf("0---------------------------------\n");
		}

		// le_fila(inicio);
		// le_fila(fila_de_espera);
		// printf("0---------------------------------\n");
		// printf("%d\n", i);
	}

//--------------------------------------------------------------------------------
	//FASE PRICIPAL
//--------------------------------------------------------------------------------


	printf("TERMINOU FASE TRANSIENTE\n");
	//return 0;
	for(int i = 0; i < rodadas; i++){
		
		area_clientes_na_fila = 0.0;
		tempo_medio_na_fila   = 0.0;
		clientes_servidos 	  = 0.0;
		
		while(1){

			//Guarda o instante de tempo em que houve a última alteração da
			//quantidade de pessoas na fila para calculo de número de pessoas
			//na fila
			double momento_alteracao_na_fila = tempo_total_de_simulacao;

			//Se o evento for uma chegada
			if(inicio->tipo_de_evento == 'C'){

				//Avança o tempo de simulação até a chegada do evento
				tempo_total_de_simulacao = inicio->tempo_de_chegada;

				//Copia informações do evento de chegada para criar um evento de saída
				int temp_ID = inicio->ID;

				pop_evento(&inicio);

				//Se o servidor estiver ocioso, tratar o evento imediatamente
				if(servidor_ocioso){
					
					//Insere o evento de saída
					insere_evento(&inicio, temp_ID, 'S', tempo_total_de_simulacao + gera_chegada(taxa_de_servico));

					//Evento está sendo tratado, servidor não está mais ocioso
					servidor_ocioso = 0;

				//Se o servidor estiver ocupado, colocar o evento na fila de espera
				}else{

					//Calcula área de quantidade de clientes na fila para calcular média depois
					area_clientes_na_fila += (tempo_total_de_simulacao - momento_alteracao_na_fila) * num_clientes_na_fila;

					//Aumenta quantidade de clientes na fila
					num_clientes_na_fila++;

					insere_evento(&fila_de_espera, temp_ID, 'C', tempo_total_de_simulacao);
				}

				//Cria o próximo evento de chegada
				insere_evento(&inicio, ++total_ID, 'C', tempo_total_de_simulacao + gera_chegada(taxa_de_chegadas));

			}else{

				momento_alteracao_na_fila = tempo_total_de_simulacao;

				//Avança o tempo até a saída do evento
				tempo_total_de_simulacao = inicio->tempo_de_chegada;

				pop_evento(&inicio);

				//Se houver algum evento na fila de espera, tratá-lo imediatamente
				if(fila_de_espera != NULL){

					//Copia informações do evento para criar um evento de saída
					int temp_ID = fila_de_espera->ID;

					//Coleta dados de tempo de espera na fila do cliente
					double espera = tempo_total_de_simulacao - fila_de_espera->tempo_de_chegada;
					tempo_medio_na_fila += (espera > 0)? espera : 0;

					//Calcula área de quantidade de clientes na fila para calcular média depois
					area_clientes_na_fila += (tempo_total_de_simulacao - momento_alteracao_na_fila) * num_clientes_na_fila;

					//Diminui quantidade de clientes na fila
					num_clientes_na_fila--;

					//Retira evento da fila e coloca um evento de saída na lista de eventos
					pop_evento(&fila_de_espera);
					insere_evento(&inicio, temp_ID, 'S', tempo_total_de_simulacao + gera_chegada(taxa_de_servico));

				//Caso contrário, marca o servidor como ocioso
				}else{
					servidor_ocioso = 1;
				}

				//Se já coletou amostras suficientes, sai do loop
				if(++clientes_servidos == amostras){
					break;
				}

				//printf("%d ", clientes_servidos);
			}

		// 	printf("%lf\n", tempo_total_de_simulacao);
		// 	le_fila(inicio);
		// le_fila(fila_de_espera);
		// printf("0---------------------------------\n");
		}

		// le_fila(inicio);
		// le_fila(fila_de_espera);
		// printf("0---------------------------------\n");
		//printf("%d\n", i);


		area_media_na_fila_todas_rodadas += area_clientes_na_fila;

		tempo_medio_na_fila  /= amostras;
		area_clientes_na_fila/= amostras;
		//printf("FILA %lf\n", tempo_medio_na_fila);
		tempo_medio_na_fila_todas_rodadas += tempo_medio_na_fila;

		tempo_soma_medias_quadrado 	+= tempo_medio_na_fila * tempo_medio_na_fila;
		tempo_soma_medias 		 	+= tempo_medio_na_fila;

		area_soma_medias_quadrado 	+= area_clientes_na_fila * area_clientes_na_fila;
		area_soma_medias 			+= area_clientes_na_fila;

	}

	double tempo_variancia  = (tempo_soma_medias_quadrado)/(rodadas-1) - (tempo_soma_medias * tempo_soma_medias)/(rodadas*(rodadas-1));
	double area_variancia	=  (area_soma_medias_quadrado)/(rodadas-1) - (area_soma_medias 	* area_soma_medias)/(rodadas*(rodadas-1));

	printf("Média:          %lf\n", tempo_medio_na_fila_todas_rodadas/rodadas);
	printf("Variância:      %lf\n", tempo_variancia);
	printf("Desvio padrão: 	%lf\n", sqrt(tempo_variancia));
	printf("Média na fila: 	%lf\n", area_media_na_fila_todas_rodadas/tempo_total_de_simulacao);
	printf("Var na fila: 	%lf\n", area_variancia);
	printf("Desvio na fila: %lf\n", sqrt(area_variancia));

	return 0;
}