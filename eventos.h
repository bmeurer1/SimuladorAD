extern int total_ID;


//Lista duplamente encadeada para compor nossa lista de eventos
struct Node{
	int 	ID;
	double 	tempo_de_chegada;
	char 	tipo_de_evento;
	struct 	Node *proximo;
};

extern void le_fila(struct Node *node);

double gera_chegada(double taxa){

	//Retorna o tempo até a próxima chegada com Poisson
	double t = 1.0 - (double)rand()/(double)RAND_MAX;
	return -log(t) / taxa;
}

//Gera um novo evento e popula com as informações pertinentes
struct Node* cria_evento(int ID, char tipo_de_evento, double tempo_de_chegada){

	struct Node *novo = (struct Node*) malloc(sizeof(struct Node));

	novo->ID 				= ID;
	novo->tempo_de_chegada 	= tempo_de_chegada;
	novo->tipo_de_evento	= tipo_de_evento;
	novo->proximo 			= NULL;

	return novo;
}

//Insere um evento na lista indicada
void insere_evento(struct Node **inicio, int ID, char tipo_de_evento, double tempo_de_chegada){

	//Cria um novo evento
	struct Node *novo_evento = cria_evento(ID, tipo_de_evento, tempo_de_chegada);

	struct Node *temp;

	temp = *inicio;

	//Se a fila estiver vazia, inserir o evento no início
	if(*inicio == NULL){

		*inicio 			  = novo_evento;
		novo_evento->proximo  = NULL;
		
		return;

	//Se o evento atual acontecer antes do primeiro da fila, colocá-lo na frente
	}else if(novo_evento->tempo_de_chegada < (*inicio)->tempo_de_chegada){

		novo_evento->proximo = *inicio;
		*inicio 			 = novo_evento;
	
	//Se houver apenas um outro elemento na fila, inserir o atual na segunda posição
	}else if((*inicio)->proximo == NULL){

		(*inicio)->proximo 	  = novo_evento;

	//Caso contrário, buscar na fila em qual posição o  novo evento deve entrar
	}else{

		struct Node *temp 	  = *inicio;

		while(temp->proximo != NULL){

			//Se encontrar um evento na lista com um tempo de chegada maior, 
			//colocar o novo evento antes dele
			if((temp->proximo)->tempo_de_chegada > novo_evento->tempo_de_chegada){
				
				novo_evento->proximo = temp->proximo;
					   temp->proximo = novo_evento;

				return;

			}
			
			temp = temp->proximo;
			
		}

		//Caso contrário, insere no final
		temp->proximo 		  = novo_evento;
		novo_evento->proximo  = NULL;
	}
}


//Retira o primeiro elemento da lista desejada
void pop_evento(struct Node **inicio){

	//Se não houver nenhum elemento na fila, não fazer nada
	if(*inicio == NULL){	
		return ;
	}

	//Se houver apenas um elemento na fila, retirá-lo
	if((*inicio)->proximo == NULL){
		free(*inicio);
		*inicio = NULL;
		return;
	}

	//Caso contrário, libera o primeiro e aponta para o segunda da fila
	struct Node *temp = *inicio;
	free(temp);
	 *inicio 	      = (*inicio)->proximo;

	return ;

}