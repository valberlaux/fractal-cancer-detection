#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include <string.h>

#define HI(num) (((num) & 0x0000FF00) >> 8)
#define LO(num) ((num) & 0x000000FF)

#define TAMANHO 1024

#define LIMIAR_INICIAL 30
#define LIMIAR_INCREMENTO 5
#define LIMIAR_POCENTAGEM_ACEITAVEL 0.3f

typedef struct _PGMData {
    int row;
    int col;
    int max_gray;
    int **matrix;
} PGMData;


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

    int px_mais_claro = pixel_mais_claro(data->matrix,1024);

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
int findLimiar(PGMData *data, int xc, int yc, int raio)
{
	int limiar = LIMIAR_INICIAL,pixelValue;
	int xi,yi;
	int pixelsBrancos=0,pixelsTotal=0;
	float porcentagem;

	yc = TAMANHO - yc;

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

		if(porcentagem > LIMIAR_POCENTAGEM_ACEITAVEL)
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
	int limiar = LIMIAR_INICIAL,pixelValue;
	int xi,yi;
	int pixelsBrancos=0,pixelsTotal=0;
	float porcentagem;

	yc = TAMANHO - yc;

	for(xi = (-1)*raio; xi <= raio; xi++)
	{
		yi = round(sqrt(raio*raio - xi*xi));

		data->matrix[yi*-1 + yc][xi+xc] = data->matrix[yi*-1 + yc][xi+xc] ? 0 : 255;
		data->matrix[yi + yc][xi+xc] = data->matrix[yi + yc][xi+xc] ? 0 : 255;
	}
}

//cálculo da dimensão fractal
//http://www.wahl.org/fe/HTML_version/link/FE4W/c4.htm#box

int caixaPertence(int **caixa, int tamanho){
    int i, j;
    for (i = 0; i < tamanho; i++) {
        for (j = 0; j < tamanho; j++) {
            if (caixa[i][j] != 0){
                
                if (caixa[i][j] != 255) {
                    printf("Opa. Algum bit não está binarizado nessa matriz! [PIXEL [%d][%d]\n", i, j);
                }
                
                return 1; // há um pixel branco na caixa
            }
        }
    }
    return 0; // todos pretos
}

int proximoTamanhoDeCaixa(int tamanhoAtual){
    /*
     Não está funcionando.
     Se o raio original fosse 35, o lado mediria 70px,
     cada caixa mediria 7px. Assim, o próximo tamanho fica 3,
     mas 70 não dá pra dividir em caixas de 3px.
     */
    return (int)tamanhoAtual/2;
}

int calculaDimensaoFractal(PGMData *data, int x, int y, int raio){
    
    int quantidadeCaixas, caixasPertencentes = 0;
    /*
     Define o tamanho da caixa
     Na primeira iteração, região é dividida 10x10 caixas.
     Depois, o tamanho das caixas é calculado pela fç proximoTamanhoDeCaixa()
     */
    
    while (raio%5 != 0)
        raio++;
    
    //determina o tamanho da primeira caixa. As próximas serão definidas por uma função
    int tamanhoCaixa = raio*0.2;
    
    while (tamanhoCaixa > 0) {
        
        int i, j;
        for (i = 0; i < raio*2/tamanhoCaixa; i++) {
            for (j = 0; j < raio*2/tamanhoCaixa; j++) {
                caixasPertencentes += caixaPertence(&data->matrix[x - raio + j*tamanhoCaixa, y - raio + i*tamanhoCaixa], tamanhoCaixa);
            }
        }
        
        quantidadeCaixas = (raio*2)/tamanhoCaixa * (raio*2)/tamanhoCaixa;
        
        // adicionar a uma lista a tupla {QTDE CAIXAS PREENCHIDAS; PROPORÇÃO} pra depois calcular as combinações com o log
        
        tamanhoCaixa = proximoTamanhoDeCaixa(tamanhoCaixa);
    }
    
    return 0;
}


int main(int argc, const char * argv[])
{
	PGMData matrix;
	int limiar,xc,yc,raio;

	readPGM(argv[1], &matrix);

	equalizar_imagem(&matrix);

	xc = atoi(argv[3]);
	yc = atoi(argv[4]);
	raio = atoi(argv[5]);

	teste1(&matrix,xc,yc,raio);

	limiar = findLimiar(&matrix,xc,yc,raio);
	binarizarPGM(&matrix, limiar);

	teste2(&matrix,xc,yc,raio);

	writePGM(argv[2], &matrix);
}
