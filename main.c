// Testar Matriz até 100000 x 100000
// TF - Ti
// tf.tv_sec*1000 + tf.tv_nsec/1000000

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>


int16_t   **matrixA;
int16_t   **matrixB;
int16_t   **matrixC;
u_int16_t *sizeMatrix;

// Estrutura para auxiliar na multiplicação de matriz onde passamos a linha que ira fazer a multiplicação
// Inicio e fim
typedef struct helpIndicesThread
{
    unsigned start;
    unsigned end;
}helpThreads;



// Função de multiplicar a matriz
void *multi_thread(void *param);


void printMatrix(int16_t **matrix, u_int16_t sizeMatrix){
    for(int i=0;i<sizeMatrix;i++){
        printf("\t");
        for(int j=0;j<sizeMatrix;j++){
            printf("%" PRIi16 "  ", matrix[i][j]);
        }
        printf("\n");   
    }
}

int main(int argc, char *argv[]) {
    FILE *archive;
    FILE *newArchive;
    clock_t start, end;
    helpThreads *indiceHelper;
    pthread_t *thread_identifier; /* the thread identifier */
    int qtd_threads;

    if(argc != 3){
        printf("\nA entrada deve ser: ./programa nome-do-arquivo N\n");
        printf("Onde N deve ser o numero de threads que deseja utilizar na aplicação\n");
        return 1;
    }
    
    char *nameOpenFile = argv[1];
    qtd_threads = atoi(argv[2]);
    archive = fopen(nameOpenFile,"rb");


    if (archive==NULL){
        printf("Não conseguiu abrir o arquivo\n");
        return(1);
    }

    printf("Arquivo aberto com sucesso\n");
    printf("--------------------------------\n\n");

    sizeMatrix = malloc(sizeof(u_int16_t));

    fread(sizeMatrix, sizeof(u_int16_t), 1, archive);

    matrixA = malloc (*(sizeMatrix) * sizeof(int16_t*));
    matrixB = malloc (*(sizeMatrix) * sizeof(int16_t*));
    matrixC = malloc (*(sizeMatrix) * sizeof(int16_t*));

    for(int i = 0; i < *(sizeMatrix); i++){
        matrixA[i] = malloc (*(sizeMatrix) * sizeof (int16_t));
        matrixB[i] = malloc (*(sizeMatrix) * sizeof (int16_t)) ; 
        matrixC[i] = malloc (*(sizeMatrix) * sizeof (int16_t)) ;               
    }

    for(int i = 0; i < *(sizeMatrix); i++){
        for(int j = 0; j < *(sizeMatrix); j++){
            fread(&matrixA[i][j], sizeof(int16_t), 1, archive);
        }
    }   

    for(int i = 0; i < *(sizeMatrix); i++){
        for(int j = 0; j < *(sizeMatrix); j++){
            fread(&matrixB[i][j], sizeof(int16_t), 1, archive);
        }
    }

    fclose(archive);

    thread_identifier = malloc(qtd_threads * sizeof(pthread_t));
    indiceHelper = malloc(qtd_threads * sizeof(helpThreads));
    
    //Inicio do tempo
    start = clock();
    for(int i=0; i < qtd_threads; i++){ 
        // Tratando o indice das linhas da matrizes similar aos do vetores.
        indiceHelper[i].start =  (unsigned)i * ((*sizeMatrix) / qtd_threads);

        if(i == qtd_threads-1){
            indiceHelper[i].end = (unsigned) (*(sizeMatrix)-1);
        }else{
            indiceHelper[i].end = (unsigned) indiceHelper[i].start + ((*(sizeMatrix))/qtd_threads)-1;
        }

        pthread_create(&thread_identifier[i], NULL,  multi_thread, &indiceHelper[i]);
    }

    for(int i=0; i<qtd_threads; i++){
		pthread_join(thread_identifier[i],NULL);
    }
    end = clock();  //tempo final

    printf ("Tempo gasto %f Milisegundos\n",((double)( end - start ) / ((double)CLOCKS_PER_SEC ))); 
    
    newArchive = fopen("M.out", "wb");
    fwrite(&sizeMatrix, sizeof(u_int16_t), 1, newArchive);

    for (int i = 0; i < (*(sizeMatrix)); i++) {
        for(int j = 0 ; j < (*(sizeMatrix)); j++){
            fwrite(&matrixC[i][j], sizeof(int16_t), 1, newArchive);
        }
    }

    // Desalocando recursos utilizados
    
    for(int i=0; i<(*(sizeMatrix)); i++){
        free(matrixA[i]);
        free(matrixB[i]);
        free(matrixC[i]);   
    }
    
    free(matrixA);
    free(matrixB);
    free(matrixC);  
    free(sizeMatrix);
    free(indiceHelper);
    free(thread_identifier);

    return 0;
}

void *multi_thread(void *param){
    //Realizar a multiplicação de matrizes.
    unsigned helper=0;
    helpThreads indicesMatrix = (*((helpThreads *)param));

    for(unsigned i = indicesMatrix.start; i <= indicesMatrix.end ; i++){
        for(unsigned j = 0; j < *(sizeMatrix); j++){

            matrixC[i][j] = 0;
            for (int k = 0; k < *(sizeMatrix); k++){
                helper +=  matrixA[i][k] * matrixB[k][j];
            }
            matrixC[i][j] = helper;
            helper = 0;
        }
    }
    pthread_exit(NULL);
}

