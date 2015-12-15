#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

#define HI(num) (((num) & 0x0000FF00) >> 8)
#define LO(num) ((num) & 0x000000FF)

#define TAMANHO 1024

#define LIMIAR_INICIAL 1
#define LIMIAR_INCREMENTO 1
//#define LIMIAR_POCENTAGEM_ACEITAVEL 0.35f

typedef struct _PGMData {
    int row;
    int col;
    int max_gray;
    int **matrix;
} PGMData;

typedef struct _Tripla {
    int totalCaixas;
    int caixasPreenchidas;
    int proporcao;
    struct _Tripla *proxima;
} Tripla;


int binarizacao(int pixelValue, int limiar)
{
   if (pixelValue > limiar)
        return 255;
    else 
		return 0;
}

int pixel_mais_claro(int **matrix, int dim)
{
    int p = 0, i, j;
    for(i = 0; i < dim; i++)
        for(j = 0; j< dim; j++)
            if(matrix[i][j] > p)
                  p = matrix[i][j];

    return p;
}

void equalizar_imagem(PGMData *data)
{
    int histograma_original[data->max_gray], histograma_final[data->max_gray];
    int i, j;

    //Inicializando histogramas
    for(i = 0; i < data->max_gray; i++)
    {
           histograma_original[i] = 0;
           histograma_final[i] = 0;
    }

    //Calculando o histograma da imagem
    for(i = 0; i < data->col; i++)
        for(j = 0; j < data->row; j++)
            histograma_original[LO(data->matrix[i][j])] += 1;


    //Equalizando
    float p = (float)data->max_gray / ((float)data->col*(float)data->row);

    float n = 0.0f;
    float aux = 0.0f;

    for(i = 0; i < data->max_gray; i++)
    {
        for(j = 0; j < i; j++)
            aux += histograma_original[j];

        n = p * aux;

        aux = 0.0f;

        histograma_final[i] = n;//(n - 100 < 0? n: n - 100);
    }

    //Atualizando a imagem
    for(i = 0; i < data->col; i++)
    {
        for(j = 0; j < data->row; j++)
        {
            data->matrix[i][j] = histograma_final[LO(data->matrix[i][j])];
        }
    }
}

int **allocate_dynamic_matrix(int row, int col)
{
    int **ret_val;
    int i;

    ret_val = (int **)malloc(sizeof(int *) * row);
    if (ret_val == NULL) {
        perror("Falha de alocacao de memoria");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < row; ++i) {
        ret_val[i] = (int *)malloc(sizeof(int) * col);
        if (ret_val[i] == NULL) {
            perror("Falha de alocacao de memoria");
            exit(EXIT_FAILURE);
        }
    }

    return ret_val;
}

void deallocate_dynamic_matrix(int **matrix, int row)
{
    int i;

    for (i = 0; i < row; ++i)
        free(matrix[i]);
    free(matrix);
}

void SkipComments(FILE *fp)
{
    int ch;
    char line[100];

    while ((ch = fgetc(fp)) != EOF && isspace(ch))
        ;
    if (ch == '#') {
        fgets(line, sizeof(line), fp);
        SkipComments(fp);
    } else
        fseek(fp, -1, SEEK_CUR);
}

/*for reading:*/
PGMData* readPGM(const char *file_name, PGMData *data)
{
    FILE *pgmFile;
    char version[3];
    int i, j;
    int lo, hi;

    pgmFile = fopen(file_name, "rb");
    if (pgmFile == NULL) {
        perror("cannot open file to read");
        exit(EXIT_FAILURE);
    }

    fgets(version, sizeof(version), pgmFile);
    if (strcmp(version, "P5")) {
        fprintf(stderr, "Wrong file type!\n");
        exit(EXIT_FAILURE);
    }

    SkipComments(pgmFile);
    fscanf(pgmFile, "%d", &data->col);
    SkipComments(pgmFile);
    fscanf(pgmFile, "%d", &data->row);
    SkipComments(pgmFile);
    fscanf(pgmFile, "%d", &data->max_gray);
    fgetc(pgmFile);

    data->matrix = allocate_dynamic_matrix(data->row, data->col);
    if (data->max_gray > 255)
        for (i = 0; i < data->row; ++i)
            for (j = 0; j < data->col; ++j) {
                hi = fgetc(pgmFile);
                lo = fgetc(pgmFile);
                data->matrix[i][j] = (hi << 8) + lo;
            }
    else
        for (i = 0; i < data->row; ++i)
            for (j = 0; j < data->col; ++j) {
                lo = fgetc(pgmFile);
                data->matrix[i][j] = lo;
            }

    fclose(pgmFile);
    return data;

}

/*and for writing*/

void writePGM(const char *filename, const PGMData *data)
{
    FILE *pgmFile;
    int i, j;
    int hi, lo;

//    int px_mais_claro = pixel_mais_claro(data->matrix,data->row);

    pgmFile = fopen(filename, "wb");
    if (pgmFile == NULL) {
        perror("cannot open file to write");
        exit(EXIT_FAILURE);
    }

    fprintf(pgmFile, "P5 ");
    fprintf(pgmFile, "%d %d ", data->col, data->row);
    fprintf(pgmFile, "%d ", data->max_gray);

    if (data->max_gray > 255) {
        for (i = 0; i < data->row; ++i) {
            for (j = 0; j < data->col; ++j) {
                hi = HI(data->matrix[i][j]);
                lo = LO(data->matrix[i][j]);
                fputc(hi, pgmFile);
                fputc(lo, pgmFile);
            }
        }
    }
    else
    {
        for (i = 0; i < data->row; ++i)
            for (j = 0; j < data->col; ++j)
            {
            	//lo = LO(data->matrix[i][j]);
            	lo = data->matrix[i][j];
             	//lo = (lo - 65 < 0 ? 0: lo -95);
            	//lo = binarizacao(lo,lo);

                fputc(lo, pgmFile);
            }
    }

    fclose(pgmFile);
    deallocate_dynamic_matrix(data->matrix, data->row);
}


// binariza todos os pixels da imagem de acordo com o dado limiar
void binarizarPGM(PGMData *data, int limiar)
{
	int i,j,lo;

	for(i = 0; i < data->col; i++)
	{
		 for(j = 0; j < data->row; j++)
	     {
			 lo = data->matrix[i][j];
	    	 data->matrix[i][j] = binarizacao(lo,limiar);
	     }
	}
}

// encontra e retorna o valor de limiar aceito para a imagem, dentro dos parametros definidos
// xc, yc = coordenadas do centro do tumor, tal com estao no arquivo fonte; raio = raio tal qual esta no arquivo
int acharLimiar(PGMData *data, int xc, int yc, int raio, float f)
{
	int limiar = LIMIAR_INICIAL,pixelValue;
	int xi,yi;
	int pixelsBrancos=0,pixelsTotal=0;
	float porcentagem;

//	yc = TAMANHO - yc;
	if (f >= 0){
	while(1)
	{
		for(xi = (-1)*raio; xi <= raio; xi++)
		{
			yi = round(sqrt(raio*raio - xi*xi));

			pixelValue = data->matrix[yi*-1 + yc][xi+xc];
			if(binarizacao(pixelValue,limiar)>0)
			{
				pixelsBrancos++;
			}

			pixelValue = data->matrix[yi + yc][xi+xc];
			if(binarizacao(pixelValue,limiar)>0)
			{
				pixelsBrancos++;
			}

			pixelsTotal+=2;
		}

		porcentagem = (float)pixelsBrancos/(float)pixelsTotal;

		if(porcentagem > f)
		{
			limiar+=LIMIAR_INCREMENTO;
			pixelsBrancos = 0;
			pixelsTotal = 0;
		}
		else
		{
			break;
		}
	}
	}
	return limiar;
}

void teste1(PGMData *data, int xc, int yc, int raio)
{
	int limiar = LIMIAR_INICIAL,pixelValue;
	int xi,yi;
	int pixelsBrancos=0,pixelsTotal=0;
	float porcentagem;

	yc = TAMANHO - yc;

	data->matrix[yc][xc] = 0;

	int i,j;
	for(i = -7; i < 7; i++)
	{
		for(j = -7; j < 7; j++)
		{
			data->matrix[yc + i][xc + j] = 0;
		}
	}
	for(i = -3; i < 3; i++)
	{
		for(j = -3; j < 3; j++)
		{
			data->matrix[yc + i][xc + j] = 255;
		}
	}
}

void teste2(PGMData *data, int xc, int yc, int raio)
{
	int xi,yi;

	yc = TAMANHO - yc;

	for(xi = (-1)*raio; xi <= raio; xi++)
	{
		yi = round(sqrt(raio*raio - xi*xi));

		data->matrix[yi*-1 + yc][xi+xc] = data->matrix[yi*-1 + yc][xi+xc] ? 0 : 255;
		data->matrix[yi + yc][xi+xc] = data->matrix[yi + yc][xi+xc] ? 0 : 255;
	}
}

void teste3(PGMData *data, int xc, int yc, int raio)
{
	int xi,yi;

//	yc = TAMANHO - yc;

	for(xi = (-1)*raio; xi <= raio; xi++)
	{
		yi = round(sqrt(raio*raio - xi*xi));

		data->matrix[yi*-1 + yc][xi+xc] = data->matrix[yi*-1 + yc][xi+xc] ? 0 : 255;
		data->matrix[yi + yc][xi+xc] = data->matrix[yi + yc][xi+xc] ? 0 : 255;
	}
}

void limpar1(PGMData *data, float f)
{
	if (f >= 0){
	int xi,yi;

//	yc = TAMANHO - yc;

	int pixelMaisClaro = pixel_mais_claro(data->matrix,data->row);

	int i,j,dim=data->row;

	int limiar = pixelMaisClaro * f;

	for(i = 0; i < dim; i++)
	{
		for(j = 0; j < dim; j++)
		{
			if (data->matrix[j][i] < limiar)
			{
				data->matrix[j][i] = 0;
			}
		}
	}
	}
}

void limpar2(PGMData *data, float f)
{
	if (f >= 0){
	int xi,yi;

//	yc = TAMANHO - yc;

	int pixelPivo = data->matrix[data->row/2][data->row/2];

	int i,j,dim=data->row;

	int limiar = pixelPivo * f;

	for(i = 0; i < dim; i++)
	{
		for(j = 0; j < dim; j++)
		{
			if (data->matrix[j][i] < limiar)
			{
				data->matrix[j][i] = 0;
			}
		}
	}
	}
}

//cálculo da dimensão fractal
//http://www.wahl.org/fe/HTML_version/link/FE4W/c4.htm#box

void imprimeMatriz(int **matriz, int x, int y, int tamanho) // @TODO valberlaux método utilitário para testes. Excluir ao final
{
    printf("Params: %d, %d, %d\n Matriz: \n", x, y, tamanho);
    int i, j;
    for (i = x; i < x + tamanho; i++) {
        printf("\n");
        for (j = y; j < y + tamanho; j++) {
            printf("%d ", matriz[i][j]);
        }
    }
    printf("Fim\n\n");
}

int caixaPertence(int **caixa, int x, int y, int tamanho){
    int i, j;
    for (i = x; i < x + tamanho; i++) {
        for (j = y; j < y + tamanho; j++) {
            
            if (caixa[i][j] != 0){
                
                if (caixa[i][j] != 255) {
                    printf("Opa. Algum bit não está binarizado nessa matriz! [PIXEL [%d][%d], com valor %d\n", i, j, caixa[i][j]);
                    exit(1);
                }
                
                return 1; // há um pixel branco na caixa
            }
        }
    }
    return 0; // todos pretos
}

long double calculaResultado(Tripla *resultados, int n_iteracoes)
{
    int i, j, contador = 0;
    long double dimensao = 0;
    
    for (i = 1; i < n_iteracoes; i++) {
        for (j = i-1; j >= 0; j--) {
            dimensao += ((log10(resultados[i].caixasPreenchidas) - log10(resultados[j].caixasPreenchidas))/(log10(resultados[i].proporcao) - log10(resultados[j].proporcao)));
            
            contador++;
        }
    }
    
    return dimensao/contador; //retorna a média de todas as dimensões
}

long double calculaDimensaoFractal(PGMData *data, int x, int y, int raio){
    
//    y = TAMANHO - y;
    
    Tripla resultados[100];
    
    int quantidadeCaixas, caixasPertencentes = 0, k = 0;
    
    /*
     Define o tamanho da caixa
     Na primeira iteração, região é dividida 10x10 caixas.
     Depois, os lados são dividos pela metade a cada iteração
     */
    float tamanhoCaixa = raio*0.2; //tamanho da primeira caixa
    
    while (tamanhoCaixa >= 1) {
        
        int i, j;
        for (i = 0; i < ((raio*2)/tamanhoCaixa); i++) {
            for (j = 0; j < ((raio*2)/tamanhoCaixa); j++) {
                
                caixasPertencentes += caixaPertence(data->matrix,
                                                    y - raio + (int)floor(i * tamanhoCaixa),
                                                    x - raio + (int)floor(j * tamanhoCaixa),
                                                    (int)ceil(tamanhoCaixa));
            }
        }
        
        resultados[k].caixasPreenchidas = caixasPertencentes;
        resultados[k].proporcao = (int)pow(2,k);
        
        tamanhoCaixa = tamanhoCaixa/2;
        caixasPertencentes = 0;
        k++;
    }
    
    return calculaResultado(resultados, k);
}

void grava_resultados(const char* nome_instancia, long double df)
{
    FILE* saida = fopen("saida.txt", "a");
    if (saida == NULL){
        printf("Erro ao gravar dados de saída\n");
        exit(1);
    }
        
    fprintf(saida, "%s\t%LF\n", nome_instancia, df);
    fclose(saida);
}

void cropRegiaoDeInteresse(PGMData *data, int xc, int yc, int raio)
{
	int xi,yi,i,j,dim=2*raio + 10;
	int ** cropedMatriz;

	yc = TAMANHO - yc;

	cropedMatriz = allocate_dynamic_matrix(dim, dim);

	int coordenadaInicialX = xc - raio - 5;
	int coordenadaInicialY = yc - raio - 5;

	for(i = 0; i < dim; i++)
	{
		for(j = 0; j < dim; j++)
		{
			cropedMatriz[j][i] = data->matrix[coordenadaInicialY+j][coordenadaInicialX+i];
		}
	}

	deallocate_dynamic_matrix(data->matrix, data->row);

	data->matrix = cropedMatriz;
	data->col = data->row = dim;
//	data->max_gray = pixel_mais_claro(data->matrix,dim);
}


int main(int argc, const char * argv[])
{
	PGMData matrix;
	int limiar,xc,yc,raio;
	float f1,f2,f3;

	readPGM(argv[1], &matrix);

	xc = atoi(argv[3]);
	yc = atoi(argv[4]);
	raio = atoi(argv[5]);

	f1 = atof(argv[6]);
	f2 = atof(argv[7]);
	f3 = atof(argv[8]);

//	printf("%f %f %f\n", f1,f2,f3);

	cropRegiaoDeInteresse(&matrix,xc,yc,raio);
	equalizar_imagem(&matrix);

	limpar1(&matrix, f2);
	limpar2(&matrix, f3);

//	teste1(&matrix,xc,yc,raio);

	limiar = acharLimiar(&matrix,matrix.row/2,matrix.row/2,raio, f1);
	binarizarPGM(&matrix, limiar);

//	teste2(&matrix,xc,yc,raio);
//	teste3(&matrix,matrix.row/2,matrix.row/2,raio);

	long double dimensao;
	dimensao = calculaDimensaoFractal(&matrix, matrix.row/2, matrix.row/2, raio);
	printf("Dimensão:%LF\n\n", dimensao);
	grava_resultados(argv[2], dimensao);
	
//	long double dimensao = calculaDimensaoFractal(&matrix, xc, yc, raio); // Válber usando. Ainda não está pronto

	writePGM(argv[2], &matrix);
}
