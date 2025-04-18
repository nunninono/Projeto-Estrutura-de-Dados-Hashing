/* Arquivo com acesso direto via hashing */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TAM 57

typedef struct carro {
	char placa[8];
	char marca[15];
	char modelo[15];
	char cor[15];
	int status;  // 1 - ativo ou 0 - removido
} CARRO;

typedef struct noTabela {
	char placa[8];
	int posicao;
	struct noTabela* prox;
	struct noTabela* ant;
} No;

FILE* prepararArquivo (char nome[]) {
	FILE* arq = fopen(nome, "r+b");
	if (arq == NULL) {
		arq = fopen(nome, "w+b");
		if (arq == NULL) {
			printf("Erro ao tentar abrir o arquivo!\n\n");
			return NULL;
		}
	}
	return arq;
}

void liberarArquivo (FILE* arq) {
	FILE* novoArq = fopen("aux.dat", "w+b");  
    if (novoArq == NULL) {
        printf("Erro ao criar o novo arquivo!!\n\n");
        return;
    }

    CARRO carro;
    rewind(arq);

    while (fread(&carro, sizeof(CARRO), 1, arq) == 1) {  // Lê todos os registros do arquivo original
        if (carro.status == 1) { 
            fwrite(&carro, sizeof(CARRO), 1, novoArq);  // Copia para o novo arquivo
        }
    }
    fclose(arq); 
    fclose(novoArq);  
    remove("carros.dat");
    rename("aux.dat", "carros.dat");
    printf("Arquivo limpo com sucesso.\nRegistros removidos logicamente.\n");
}

int hashing (char placa[]) {
	int soma = 0;
	int tamanho = strlen(placa);
	for (int i = 0; i < tamanho; i++) {
		soma += (placa[i] << (i % 8));
	}
	return abs(soma) % TAM;
}

No* criarNo (char placa[], int posicao) {
	No* novoNo = (No*) malloc (sizeof(No));
	strcpy(novoNo->placa, placa);
	novoNo->posicao = posicao;
	novoNo->prox = NULL;
	novoNo->ant = NULL;
	return novoNo;
}

void inserirOrdenado(No** lista, No* novoNo) {
    No* aux = *lista;
    if (aux == NULL) {  // 0 elementos
        novoNo->prox = aux;  
        *lista = novoNo;
        return;
    }
	else if (strcmp(novoNo->placa, aux->placa) < 0) {  // Menor que o primeiro termo
		novoNo->prox = aux;
		aux->ant = novoNo;
		*lista = novoNo;
		return;
	}
	else {  // meio ou fim (nao tem ponteiro no fim)
		while (aux->prox != NULL && strcmp(novoNo->placa, aux->prox->placa) > 0) {
			aux = aux->prox;  
		}
		novoNo->prox = aux->prox;  
		if (aux->prox != NULL) {
			aux->prox->ant = novoNo;  
		}
		aux->prox = novoNo;  
		novoNo->ant = aux;  
	}
}

void criarIndice (FILE* arq, No* tabelaHashing[]) {
    CARRO carro;
    int posicao = 0;
    rewind(arq);  // Vai para o inicio do arquivo

    while (fread(&carro, sizeof(CARRO), 1, arq) == 1) {  // Enquanto os registros forem lidos corretamente
        if (carro.status == 1) {  // Verifica se o carro esta ativo logicamente
			No* novoNo = criarNo(carro.placa, posicao);
            int chave = hashing(carro.placa);
			inserirOrdenado(&tabelaHashing[chave], novoNo);
		}	
        posicao++;
    }
}

void desalocarIndice (No* tabelaHashing[]) {
	for (int i = 0; i < TAM; i++) {  // for que passa por todos os espaços do vetor
        No* aux = tabelaHashing[i];
        while (aux != NULL) {
            No* aux2 = aux;
            aux = aux->prox;  
            free(aux2); 
        }
    }
    printf("Tabela foi hashing desalocada!\n\n");
}

void exibirOpcoes () {
	printf("_______-OPCOES-_______\n");
	printf("1 - Cadastrar um carro \n");
	printf("2 - Consultar carro \n");
	printf("3 - Alterar dados do carro \n");
	printf("4 - Remover carro \n");
	printf("5 - Exibir carros cadastrados \n");
	printf("0 - Encerrar programa \n");
	printf("Informe a opcao: ");
}

int buscar (No* tabelaHashing[], char placa[]) {
    int indice = hashing(placa);
    for (No* aux = tabelaHashing[indice]; aux != NULL; aux = aux->prox) {
        if (strcmp(aux->placa, placa) == 0) {
            return aux->posicao;
        }
    }
    return -1;
}

void inserirTabelaHash (No* tabelaHashing[], char placa[], int pos) {
	int indice = hashing(placa);  
    No* novoNo = criarNo(placa, pos); 
	inserirOrdenado(&tabelaHashing[indice], novoNo);
}

void removerTabelaHash (No* tabelaHashing[], char placa[], int posTabela) {
	No* aux = tabelaHashing[posTabela];  // Pega a lista encadeada correspondente à posição da tabela
    No* aux2 = NULL;

    // Busca pela placa na lista encadeada
    while (strcmp(aux->placa, placa) != 0) {
        aux2 = aux; 
        aux = aux->prox; 
    }

    // Se o nó a ser removido for o primeiro da lista
    if (aux2 == NULL) {
		tabelaHashing[posTabela] = aux->prox;  // O primeiro nó passa a ser o próximo
		if (aux->prox != NULL) {
			aux->prox->ant = NULL;  // Se houver um próximo nó, o ponteiro "ant" é ajustado
		}
	}
	else {
        // Se o nó a ser removido não for o primeiro
        aux2->prox = aux->prox;  // O ponteiro "prox" do nó anterior aponta para o próximo nó
        if (aux->prox != NULL) {	// Caso nao seja o ultimo nó a ser removido
            aux->prox->ant = aux2;  
        }
    }
    free(aux);
    printf("Carro de placa %s foi removida da tabela.\n", placa);
}

void cadastrar (FILE* arq, No* tabelaHashing[]) {
	CARRO novoCarro;
    char placa[8];
    int posicao;

    printf("Insira a placa do carro: ");
    scanf("%s", placa); 

    int jaCadastrado = buscar(tabelaHashing, placa);
    if (jaCadastrado != -1) {
        printf("Carro com placa %s já está cadastrado!!\n", placa);
        return;
    }

    strcpy(novoCarro.placa, placa); 
    printf("Insira a marca do carro: ");
    scanf("%s", novoCarro.marca);
    printf("Insira o modelo do carro: ");
    scanf("%s", novoCarro.modelo);
    printf("Insira a cor do carro: ");
    scanf("%s", novoCarro.cor);
    novoCarro.status = 1; 

	// move o ponteiro para o fim, dai o ftell le o tamanho de todo o arquivo
    fseek(arq, 0, SEEK_END); 
    posicao = ftell(arq) / sizeof(CARRO);  // ftell le o tamanho de todo o arquivo
	fwrite(&novoCarro, sizeof(CARRO), 1, arq);
    inserirTabelaHash(tabelaHashing, placa, posicao);
    printf("Carro cadastrado com sucesso!\n\n");
}

void consultar (FILE* arq, No* tabelaHashing[]) {
	char placa[8];  
    int posicao;   
	CARRO carro;

    printf("Informe a placa do carro a ser consultado: ");
    scanf("%s", placa);

    posicao = buscar(tabelaHashing, placa);  
    if (posicao == -1) {
        printf("Carro com placa %s nao encontrado!!\n\n", placa);
        return;  
    }

    fseek(arq, posicao * sizeof(CARRO), SEEK_SET);  // Vai até a posição do carro no arquivo
    fread(&carro, sizeof(CARRO), 1, arq);  // Lê o registro do carro

    if (carro.status == 1) {  
        printf("\n================================\n");
        printf("     CARRO ENCONTRADO\n");
        printf("Placa do carro: %s\n", carro.placa);
        printf("Marca do carro: %s\n", carro.marca);
        printf("Modelo do carro: %s\n", carro.modelo);
        printf("Cor do carro: %s\n", carro.cor);
        printf("================================\n\n");
    } 
	else {
        printf("Carro com placa %s foi removido!!\n", placa);
    }
}

void alterar (FILE* arq, No* tabelaHashing[]) {
	char placa[8];  
    int posicao;    
    CARRO carro;  
	int opcao;  

    printf("Informe a placa do carro a ser alterado: ");
    scanf("%s", placa);

    posicao = buscar(tabelaHashing, placa);  
    if (posicao == -1) {
        printf("Carro com placa %s nao encontrado!!\n\n", placa);
        return;  
    }

    fseek(arq, posicao * sizeof(CARRO), SEEK_SET);  // Vai até a posição do carro no arquivo
    fread(&carro, sizeof(CARRO), 1, arq);  // Lê o registro do carro

	printf("\n================================\n");
	printf("     CARRO ENCONTRADO\n");
	printf("Placa do carro: %s\n", carro.placa);
	printf("Marca do carro: %s\n", carro.marca);
	printf("Modelo do carro: %s\n", carro.modelo);
	printf("Cor do carro: %s\n", carro.cor);
	printf("================================\n\n");

	char novaMarca[15];
	char novoModelo[15];
	char novaCor[15];
    printf("Escolha o dado a ser alterado:\n");
    printf("1. Marca do carro.\n");
    printf("2. Modelo do carro.\n");
    printf("3. Cor do carro.\n");
	printf("4. Sair sem alterar.\n");
    printf("Digite a opcao: ");
    scanf("%d", &opcao);
	printf("\n");

    switch (opcao) {
        case 1:
            printf("Digite a nova marca do carro: ");
            scanf("%s", novaMarca);
			strcpy(carro.marca, novaMarca);
            break;

        case 2:
            printf("Digite o novo modelo do carro: ");
            scanf("%s", novoModelo);
			strcpy(carro.modelo, novoModelo);
            break;

        case 3:
            printf("Digite a nova cor: ");
            scanf("%s", novaCor);
			strcpy(carro.cor, novaCor);
            break;

        case 4:
            printf("Nada foi alterado!!.\n");
            return;
			break;

        default:
            printf("Opção invalida. Escolha outra!!\n");
            return;
			break;
    }

    fseek(arq, posicao * sizeof(CARRO), SEEK_SET);  // Vai até a posição do carro no arquivo
    fwrite(&carro, sizeof(CARRO), 1, arq);  // Grava o novo registro com os dados alterados
    printf("\nAlteracao realizada!!\n\n");
}

void remover (FILE* arq, No* tabelaHashing[]) {
	char placa[8];
    int posicao;
	CARRO carro;

    printf("Insira a placa do carro a ser removido: ");
    scanf("%s", placa);

    posicao = buscar(tabelaHashing, placa);
    if (posicao == -1) {
        printf("Essa placa nao esta no cadastro!!\n");
        return;
    }

    fseek(arq, posicao * sizeof(CARRO), SEEK_SET);  // Move o ponteiro para a posição do carro no arquivo
    fread(&carro, sizeof(CARRO), 1, arq);  // le os dados do carro 
    carro.status = 0;
    fseek(arq, posicao * sizeof(CARRO), SEEK_SET);  // Volta para a posição do carro
    fwrite(&carro, sizeof(CARRO), 1, arq);  // reescreve os dados com status = 0

    int indice = hashing(placa);  
    removerTabelaHash(tabelaHashing, placa, indice);  
    printf("Carro de placa %s foi removido!!\n\n", placa);
}

void exibirCadastro (FILE* arq) {
	CARRO carro;
	int count = 1;
    rewind(arq);  // Vai para o início do arquivo

	printf("Carros cadastrados (ATIVOS):\n");
    while (fread(&carro, sizeof(CARRO), 1, arq) == 1) {  // Lê o arquivo registro por registro
        if (carro.status == 1) {  // Verifica se o carro está ativo
			printf("\n================================\n");
			printf("      CARRO %d\n", count);
			printf("Placa do carro %d: %s\n", count, carro.placa);
			printf("Marca do carro %d: %s\n", count, carro.marca);
			printf("Modelo do carro %d: %s\n", count, carro.modelo);
			printf("Cor do carro %d: %s\n", count, carro.cor);
			printf("================================\n");
			count++;
        }
		printf("\n");
    }
}

int main() {
	char nomeArq[] = "carros.dat";
	int escolha;
	FILE* cadastro;
	No* tabelaHashing[TAM];
    for (int i = 0; i < TAM; i++) {
        tabelaHashing[i] = NULL;
    }
	cadastro = prepararArquivo(nomeArq);
	if (cadastro == NULL)
		printf("Erro na abertura do arquivo. Programa encerrado \n");
	else {
		criarIndice(cadastro, tabelaHashing);

		do {
			exibirOpcoes();
			scanf("%d",&escolha); 
			printf("\n");
			switch (escolha) {
                
                case 1: 
                    cadastrar(cadastro, tabelaHashing);
                    break;

                case 2: 
                    consultar(cadastro, tabelaHashing);
                    break;
                
                case 3: 
                    alterar(cadastro, tabelaHashing);
                    break;
                
                case 4: 
                    remover(cadastro, tabelaHashing);
                    break;
                
                case 5: 
                    exibirCadastro(cadastro);
                    break;
                
                case 0: 
                    liberarArquivo(cadastro);
                    desalocarIndice(tabelaHashing);
                    break;
                
                default: printf("Opcao invalida. Escolha outra!\n");
            }
		} while (escolha != 0);
	}
	return 0;
}