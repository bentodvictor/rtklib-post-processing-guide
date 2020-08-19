/*---------------------------------------------------------------------#
############       Universidade Federal de Santa Maria      ############
############    Instituto Nacional de Pesquisas Espaciais   ############
############               Centro de Tecnlogias             ############
############             Engenharia de Computação           ############
############              Victor Dallagnol Bento            ############
#---------------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <dirent.h>

#define BasesInput "base_stations.txt"	/*    Variável para o arquivo de texto que contém informações sobre as bases   */
#define NumberOfCorretions 4	/*  IGS, Klobuchar, SUPIM, DUAL FREQ	*/
#define TotalBases 100	/*    Numero total de estações bases   */
/******* Vetor de estruturas para armazenar os dados das estações base *******/
typedef struct {
	char name[5];
	double x_;
	double y_;
	double z_;
} Stations;
Stations base_data[TotalBases];

/*******    Variáveis Globais   *******/
char **names;	/* Vetor para salvar nomes dos arquivos .pos */
char **rinex;
int namePosLengh;	/* Numero de caracteres que contém no nome do arquivo .pos */
int amoountPosFiles;	/* Quantidade arquivos de posicionamentos (.pos) gerados pelo RTKPost */
double **Results;		/* Resultado do erro médio para cada correção */
double **to_print;

//	ALTERAR DATA JULIANA
char PathRinex[] = "/YOUR_PATH/SIMULATIONS/344/RINEX/";
char PathDir1[] = "/YOUR_PATH/SIMULATIONS/344/IGS/";	/*    Caminho para leitura dos arquivos IGS */
char PathDir2[] = "/YOUR_PATH/344/Klobuchar/";	/*    Caminho para leitura dos arquivos KLOBUCHAR */
char PathDir3[] = "/YOUR_PATH/344/SUPIM/";	/*    Caminho para leitura dos arquivos SUPIM */
char PathDir4[] = "/YOUR_PATH/344/DualFreq/";
int Correction;		/*	0: IGS, 
						1: Klobuchar
						2: SUPIM	
						3: DUAL FREQ	*/
int aloc = 0;   /* Variável para alocar matriz de resultados somente uma vez */
			
/*******	Funções		*******/
void saveBase(char *PathDir);
void readDir(char *PathDir);
void findCoordenatesDistSave(FILE* fp, char *PathDir, int Correction);
void correctionsPoints();




/****************************
			MAIN			*
****************************/
int main(){
	
	FILE *fp;
	int counterMain = 0;
	int counterGeneral = 0;
	saveBase(PathRinex);
	
LoopDir:
	    
		if (counterMain == 0)
		{
			readDir(PathDir1);		// Direorio da data da simulação só precisa ser lido uma vez independente da correção pois são os mesmos arquivos
			Correction = 0;
			findCoordenatesDistSave(fp, PathDir1, Correction);
			counterMain++;
		}
		if (counterMain == 1)
		{
			Correction = 1;
			findCoordenatesDistSave(fp, PathDir2, Correction);
			counterMain++;
		}
		if (counterMain == 2)
		{
			Correction = 2;
			findCoordenatesDistSave(fp, PathDir3, Correction);
			counterMain = 3;
		}
		if(counterMain == 3)
		{
			Correction = 3;
			findCoordenatesDistSave(fp, PathDir4, Correction);
			counterMain = 0;
		}
			
end:
	free (names);
	free (Results);
    	free (rinex);
    
	return 0;
}





/****************************************************************************
			SALVA COORDENADAS DAS ESTAÇÕES BASES DE OBSERVAÇÕES EM MATRIZ	*
****************************************************************************/
void saveBase(char* PathDir)
{
    FILE* fp;
	DIR *dir;   /* Ponteiro para diretório */
    struct dirent *lsdir;   /* Ponteiro para estrutura presente em DIR */
    dir = opendir(PathDir);     /* Abre diretório */
    char PathRinex [62];
	char aux[160];
	int temp;
	
    /* contadores */
    int i = 0;  /* Contador para n colunas */
    int i_2 = 0;    /* Contador para n arquivos */
    int nFiles = 0;
    int colunas = 0;    /* Inicia em 2 para incluir o 0 e o '\0' */

    /* Leitura do diretório para salvar quantidade de arquivos e quantidade de letras dos nomes (colunas) */
    while ((lsdir = readdir(dir)) != NULL )
    {
        while(lsdir->d_name[i] != '\0')
        {
            colunas++;
            i++;
        }
        nFiles++;
    }
    nFiles -= 2;	/* Quantidade precisa ser decrescida de 2 pois os primeiros 2 arquivos são '." e ".."	*/
    i = 0;
    closedir(dir);

    // Aloca as arquivos da matriz 
    rinex = (char **) calloc(nFiles, sizeof(char *));
    
    // Aloca as colunas da matriz 
    for (int i_ = 0; i_ < nFiles; i_++ )
        rinex[i_] = (char*) calloc(colunas, sizeof(char));

    /* Salva o nome de cada diretório no vetor criado */
    dir = opendir(PathDir);
    while ((lsdir = readdir(dir)) != NULL )
    {
        if(i == 0 && lsdir->d_name[i] == '.')
        {}
        else
        {
            while(i < colunas)
            {
                rinex[i_2][i] = lsdir->d_name[i];
                ++i;
            }
            i_2++;
            i = 0;
        }
    }
    closedir(dir);
        
    
    int countRinex = 0;
    while(countRinex < TotalBases)
    {
        strcpy(PathRinex, PathDir);
		strcat(PathRinex, rinex[countRinex]);
        
        char getNameRinex[5];
        char words[160];
        int count_2spaces = 1;	/* Contador de espaços no texto */
        int find = 0;
        
        strncpy(getNameRinex, rinex[countRinex], 4);		/* Copia os primeiros 5 caracteres do nome da estação para variável getName */
		getNameRinex[4] = '\0'; 
		
		strcpy(base_data[countRinex].name, getNameRinex);
        
        fp = fopen(PathRinex, "r");	// Abre arquivo para leitura 
        
        fgets(words, sizeof(words), fp);	// Copia primeira linha do arquivo
        
        while(!feof(fp))      // Enquanto não chegar ao final do arquivo 
		{
			for (int i_ = 0; i_ < strlen(words); ++i_)		// Laço de repeticao para percorrer linha 
			{
				if(words[i_] == 'X' && words[i_+1] == 'Y' && words[i_+2] == 'Z')		// procura pela palavra ratio 
				{
					find = 1;
					break;
				}
			}
			if(!find)	// Se nao encontrar palavra ratio na linha, le proxima linha 
            {	
                fgets(words, sizeof(words), fp);
            }
			else
				break;
		}
		
		i = 0;
		test_i:
			if(words[i] < 45){
				++i;
				goto test_i;
			}else{
				if(count_2spaces == 1)		// Latitude 
				{	
					temp = 0;
					strcpy(aux, "");
					
					while(words[i] == 46 || words[i] == 45 || (words[i] >= 48 && words[i] <= 57))
					{
						aux[temp] = words[i];
						temp++;
						i++;
					}
					aux[temp] = '\0';
					base_data[countRinex].x_ = atof(aux);
					count_2spaces = 2;
					goto test_i;
				}
				
				if(count_2spaces == 2)
				{		// Longitude 
					temp = 0;
					strcpy(aux, "");
						while(words[i] == 46 || words[i] == 45 || (words[i] >= 48 && words[i] <= 57))
					{
						aux[temp] = words[i];
						temp++;
						i++;
					}
					aux[temp] = '\0';
					base_data[countRinex].y_ = atof(aux);
					count_2spaces = 3;
					goto test_i;
				}
				
				if(count_2spaces == 3)		// altura 
				{	
					temp = 0;
					strcpy(aux, "");
					while(words[i] == 46 || words[i] == 45 || (words[i] >= 48 && words[i] <= 57))
					{
						aux[temp] = words[i];
						temp++;
						i++;
					}
					aux[temp] = '\0';
					base_data[countRinex].z_ = atof(aux);
					count_2spaces = 1;
				}
			}
	fclose(fp);   		
    countRinex++;
	}
    
}





/********************************************************************
			FUNÇÃO PARA LEITURA DO DIRETÓRIO COM OS ARQUIVOS .POS	*
********************************************************************/
void readDir(char *PathDir)
{
    DIR *dir;   /* Ponteiro para diretório */
    struct dirent *lsdir;   /* Ponteiro para estrutura presente em DIR */
    dir = opendir(PathDir);     /* Abre diretório */
    
    /* contadores */
    int i = 0;  /* Contador para n colunas */
    int i_2 = 0;    /* Contador para n arquivos */
    int nFiles = 0;
    int colunas = 0;    /* Inicia em 2 para incluir o 0 e o '\0' */

    /* Leitura do diretório para salvar quantidade de arquivos e quantidade de letras dos nomes (colunas) */
    while ((lsdir = readdir(dir)) != NULL )
    {
        while(lsdir->d_name[i] != '\0')
        {
            colunas++;
            i++;
        }
        nFiles++;
    }
    nFiles -= 2;	/* Quantidade precisa ser decrescida de 2 pois os primeiros 2 arquivos são '." e ".."	*/
    i = 0;
    namePosLengh = colunas;		/*	Atribui nummero de colunar e linhas para variáves globais	*/
    amoountPosFiles = nFiles;
    closedir(dir);

    // Aloca as arquivos da matriz 
    names = (char **) calloc(nFiles, sizeof(char *));
    
    // Aloca as colunas da matriz 
    for (int i_ = 0; i_ < nFiles; i_++ )
        names[i_] = (char*) calloc(colunas, sizeof(char));

    /* Salva o nome de cada diretório no vetor criado (names) */
    dir = opendir(PathDir);
    while ((lsdir = readdir(dir)) != NULL )
    {
        if(i == 0 && lsdir->d_name[i] == '.')
        {}
        else
        {
            while(i < colunas)
            {
                names[i_2][i] = lsdir->d_name[i];
                ++i;
            }
            i_2++;
            i = 0;
        }
    }
    closedir(dir);

}





/****************************************************************
				Lê arquivo de saída e salva distancias médias	*
****************************************************************/
void findCoordenatesDistSave(FILE* fp, char *PathDir, int Correction)
{
	char words[160];
	char aux[160];
	int i, c, temp;
	int count_2spaces = 1;	/* Contador de espaços no texto */
	int counterBoss = 0;
	char getName[5];	/* Variável para pegar 5 primeiros caracteres do nome da base na matriz "names" */
	char pathPos[75];	/* Caminho para fazer a leitura do arquivos .pos */
	int numSamples;		/* Número de amostras no arquivos .pos */
	long int linha_to_print = 0;
	
//	printf("\n\n===================================================================================\n");
//	printf("\t%s\n", PathDir);
///	printf("......................\n");
//	if (Correction == 0)
//		printf("\tIGS\n");
//	if (Correction == 1)
//		printf("\tKLOBUCHAR\n");
//	if (Correction == 2)
//		printf("\tSUPIM\n");
//	if (Correction == 3)
//		printf("\tDUAL FREQ\n");
//	printf("===================================================================================\n\n");
	
	if(aloc == 0){
		/* Matriz resultados é alocado com a quantidade de arquivos .POS (linhas) e de correções ionosféricas (colunas)	*/ 
		Results = (double **) calloc(amoountPosFiles, sizeof(double *));

		to_print = (double **) calloc((amoountPosFiles * 12), sizeof(double *));    
		
		for (int i_ = 0; i_ < amoountPosFiles; i_++ ) 
		    Results[i_] = (double*) calloc(NumberOfCorretions, sizeof(double));		// Aloca colunas para cada correção de cada resultado dos arquivos .POS
		
		for (int c_ = 0; c_ < (amoountPosFiles*12); c_++ ) 
		    to_print[c_] = (double*) calloc(NumberOfCorretions, sizeof(double));		// Aloca colunas para cada correção de cada resultado dos arquivos .POS
		
		aloc = 1;
    }
        
	while(counterBoss < amoountPosFiles)	// Contador para todos os arquivos .pos (Para o diretório)
	{
		
		double x_base, y_base, z_base;
		double x, y, z, d, sum_d;
		int find = 0;
		int find_hour = 0;
		int hour = 0;

		numSamples = 0;
		sum_d = 0;
		strncpy(getName, names[counterBoss], 4);		/* Copia os primeiros 5 caracteres do nome da estação para variável getName */
		getName[4] = '\0';
		c = 0;
		
		while(c < TotalBases)
		{
			if((strcmp(getName, base_data[c].name)) == 0)
			{
				x_base = base_data[c].x_;
				y_base = base_data[c].y_;
				z_base = base_data[c].z_;
				c = TotalBases;
			}
			else
				c++;
		}
		
		strcpy(getName, "");
		strcpy(pathPos, "");
		strcpy(pathPos, PathDir);
		strcat(pathPos, names[counterBoss]);

		
		/*******	Busca no arquivo de saída: ratio	*******/
		fp = fopen(pathPos, "r");	// Abre arquivo para leitura 
		fgets(words, sizeof(words), fp);	// Copia primeira linha do arquivo
		
		while(!feof(fp))      // Enquanto não chegar ao final do arquivo 
		{
			for (i = 0; i < strlen(words); ++i)		// Laço de repeticao para percorrer linha 
			{
				if(words[i] == 'r' && words[i+1] == 'a' && words[i+2] == 't' && words[i+3] == 'i' && words[i+4] == 'o')		// procura pela palavra ratio 
				{
					find = 1;
					break;
				}
			}
			if(!find)	// Se nao encontrar palavra ratio na linha, le proxima linha 
				fgets(words, sizeof(words), fp);
			else
				break;
		}
		
		/*******	Busca por valores de latitute, longitude e altura em um arquivo .pos  de 2h em 2h   *******/
	test_endFile:
		while(!feof(fp)){
			i = 0;
			temp = 0;
			numSamples++;
			fgets(words, sizeof(words), fp);	// Linha dos dados
			find_hour = 0;
			i = 11;	//pula até primeiro caractere da hora
			
				if(words[i] == '0' && words[i+1] == '0' && words[i+3] == '0')// && words[i+4] == '0')	// 00:00
				{
					linha_to_print = (12*counterBoss);
					goto pass;
				}
				if(words[i] == '0' && words[i+1] == '2' && words[i+3] == '0')// && words[i+4] == '0')	// 02:00
				{
					linha_to_print = (12*counterBoss) + 1;
					goto pass;
				}
				if(words[i] == '0' && words[i+1] == '4' && words[i+3] == '0')// && words[i+4] == '0')	// 04:00
				{
					linha_to_print = (12*counterBoss) + 2;
					goto pass;
				}
				if(words[i] == '0' && words[i+1] == '6' && words[i+3] == '0')// && words[i+4] == '0')		//06:00
				{
					linha_to_print = (12*counterBoss) + 3;
					goto pass;
				}
				if(words[i] == '0' && words[i+1] == '8' && words[i+3] == '0')/// && words[i+4] == '0')	// 08:00
				{

					linha_to_print = (12*counterBoss) + 4;
					goto pass;
				}
				if(words[i] == '1' && words[i+1] == '0' && words[i+3] == '0')// && words[i+4] == '0')	// 10:00
				{
					linha_to_print = (12*counterBoss) + 5;
					goto pass;
				}
				if(words[i] == '1' && words[i+1] == '2' && words[i+3] == '0')// && words[i+4] == '0')	//12:00
				{
					linha_to_print = (12*counterBoss) + 6;
					goto pass;
				}
				if(words[i] == '1' && words[i+1] == '4' && words[i+3] == '0')// && words[i+4] == '0')	//14:00
				{
					linha_to_print = (12*counterBoss)+7;
					goto pass;
				}
				if(words[i] == '1' && words[i+1] == '6' && words[i+3] == '0')// && words[i+4] == '0')	//16:00
				{
					linha_to_print = (12*counterBoss)+8;
					goto pass;
				}
				if(words[i] == '1' && words[i+1] == '8' && words[i+3] == '0')// && words[i+4] == '0')	//18:00
				{
					linha_to_print = (12*counterBoss)+9;
					goto pass;
				}
				if(words[i] == '2' && words[i+1] == '0' && words[i+3] == '0')// && words[i+4] == '0')	// 20:00
				{
					linha_to_print = (12*counterBoss)+10;
					goto pass;
				}
				if(words[i] == '2' && words[i+1] == '2'  && words[i+3] == '0')// && words[i+4] == '0')	//22:00
				{
					linha_to_print = (12*counterBoss)+11;
					goto pass;
				}
				if(!feof(fp))
					goto test_endFile;
				if(feof(fp))
					goto end_WhilePos;


		pass:		
			i = 23;
			test_i:
				if(words[i] < 45){
					++i;
					goto test_i;
				}else{
					if(count_2spaces == 1)		// Latitude 
					{	
						temp = 0;
						strcpy(aux, "");
						
						while(words[i] == 46 || words[i] == 45 || (words[i] >= 48 && words[i] <= 57))
						{
							aux[temp] = words[i];
							temp++;
							i++;
						}
						aux[temp] = '\0';
						x = atof(aux);
						count_2spaces = 2;
						goto test_i;
					}
					
					if(count_2spaces == 2)
					{		// Longitude 
						temp = 0;
						strcpy(aux, "");

						while(words[i] == 46 || words[i] == 45 || (words[i] >= 48 && words[i] <= 57))
						{
							aux[temp] = words[i];
							temp++;
							i++;
						}
						aux[temp] = '\0';
						y = atof(aux);
						count_2spaces = 3;
						goto test_i;
					}
					
					if(count_2spaces == 3)		// altura 
					{	
						temp = 0;
						strcpy(aux, "");
						while(words[i] == 46 || words[i] == 45 || (words[i] >= 48 && words[i] <= 57))
						{
							aux[temp] = words[i];
							temp++;
							i++;
						}
						aux[temp] = '\0';
						z = atof(aux);
						count_2spaces = 1;
					}
					d = sqrt(pow((x_base - x),2)+pow((y_base - y),2)+pow((z_base - z),2));
					to_print[linha_to_print][Correction] = d;
					sum_d += d;

					goto test_endFile;
				}
		}
	end_WhilePos:
		Results[counterBoss][Correction] = sum_d / (float)(numSamples);
		fclose(fp);
		counterBoss++;
	}
}
