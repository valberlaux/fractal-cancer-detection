#include <stdlib.h>
#include <stdio.h>
#include <float.h>

#define HI(num) (((num) & 0x0000FF00) >> 8)
#define LO(num) ((num) & 0x000000FF)

typedef struct _PGMData {
    int row;
    int col;
    int max_gray;
    int **matrix;
} PGMData;


int binarizacao(int pixel)
{

   if ((255 - pixel) < pixel)
        return 255;
    else return 0;
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
    } else {
        for (i = 0; i < data->row; ++i)
            for (j = 0; j < data->col; ++j) {
              //  lo = LO(data->matrix[i][j]);
              lo = data->matrix[i][j];
                 //lo = (lo - 65 < 0 ? 0: lo -95);
               // lo = binarizacao(lo);

                fputc(lo, pgmFile);
            }
    }

    fclose(pgmFile);
    deallocate_dynamic_matrix(data->matrix, data->row);
}


int main()
{
    FILE *arq, *saida;

    PGMData matrix;
    readPGM("/home/raiza/mdb012.pgm", &matrix);

    equalizar_imagem(&matrix);
 writePGM("/home/raiza/B005.pgm", &matrix);
   // writePGM("/home/raiza/benigno/B005.pgm", &matrix);




}
