/*
desenvolver um mini analisador sintático para a seguinte gramática:
<expressao_simples> ::= <fator> { “+”  <fator> }
<fator> ::= IDENTIFICADOR | NUMERO_INTEIRO

A gramática está na notação BNF estendida, os <não-terminais> da gramática
são nomes entre parênteses angulares < e > e os símbolos terminais
estão em NEGRITO ou entre aspas (Ex: “+”).

*/
#include <stdio.h>
#include <ctype.h>  // isdigit
#include <stdlib.h> // free, calloc
#include <string.h> // strcmp

typedef enum
{
    ERRO,
    ADICAO,
    NUMERO_INTEIRO,
    IDENTIFICADOR,
    WHILE,
    EOS
} TAtomo;

char *strMsgAtomo[] = {"ERRO LEXICO", "ADICAO", "NUMERO_INTEIRO", "IDENTIFICADOR", "WHILE", "EOS"};

typedef struct
{
    TAtomo atomo;
    int linha;
    int atributo_numero;
    char atributo_identificador[15];
} TInfoAtomo;

// variaveis globais
char *cadeia;
int linha = 1;
TAtomo lookahead;
TInfoAtomo info_atomo_atual;

// declaracao de funcoes para analisador lexico
void reconhece_numero(TInfoAtomo *InfoAtomo);
void reconhece_identificador(TInfoAtomo *InfoAtomo);

void leArquivo(char *nomeArquivo);
TInfoAtomo obter_atomo();

// declaracao das funcoes para analisador sintatico
void consome(TAtomo atomo);
void expressao_simples(); // simbolo inicial da gramatica
void fator();

int main(void)
{
    char *iniBuffer;
    leArquivo("entrada.txt");
    iniBuffer = cadeia; // para poder desalocar depois o buffer lido
    printf("\nanalise sintatica da palavra: %s\n", iniBuffer);
    // Antes de começar a análise sintática, temos que inicializar a variável lookahead.
    info_atomo_atual = obter_atomo();
    lookahead = info_atomo_atual.atomo;

    expressao_simples();
    // testa se chegou ao final do arquivo (buffer)
    consome(EOS);
    printf("\nAnalise Sintatica finalizada com SUCESSO.\n");
    // libera o buffer alocado no inicio do programa
    free(iniBuffer);
    return 0;
}
// FUNCOES DO ANALISADOR SINTATICO
// toda vez que quiser atualizar lookahead deve ser chamada a funcao consome()
// essa funcao que fara a chamada da funcao obter_atomo() do analisador lexico
void consome(TAtomo atomo)
{
    if (lookahead == ERRO)
        printf("Erro lexico\n");

    if (lookahead == atomo)
    {
        info_atomo_atual = obter_atomo();
        lookahead = info_atomo_atual.atomo;
    }
    else
    {
        printf("erro sintatico #%d: esperado [%s] encontrado [%s]\n", info_atomo_atual.linha, strMsgAtomo[atomo], strMsgAtomo[lookahead]);
        exit(1);
    }
}
// <expressao_simples> ::= <fator> { “+”  <fator> }
void expressao_simples()
{
    fator();
    while (lookahead == ADICAO)
    {
        consome(ADICAO);
        fator();
    }
}
//<fator> ::= IDENTIFICADOR | NUMERO_INTEIRO
void fator()
{
    if (lookahead == IDENTIFICADOR)
        consome(IDENTIFICADOR);
    else
        consome(NUMERO_INTEIRO);
}

// FUNCOES DO ANALISADOR LEXICO
TInfoAtomo obter_atomo()
{
    TInfoAtomo InfoAtomo;
    InfoAtomo.atomo = ERRO;
    // descarta delimitadores
    while (*cadeia == 0 || *cadeia == ' ' || *cadeia == '\n' || *cadeia == '\r')
    {
        if (*cadeia == 0)
        {
            InfoAtomo.atomo = EOS; // finaliza o programa
            InfoAtomo.linha = linha;
            return InfoAtomo;
        }
        if (*cadeia == '\n')
            linha++;
        cadeia++;
    }

    InfoAtomo.linha = linha;
    if (isdigit(*cadeia))
        reconhece_numero(&InfoAtomo);
    else if (islower(*cadeia))
        reconhece_identificador(&InfoAtomo); // reconhece ID e WHILE
    else if (*cadeia == '+')
    {
        cadeia++;
        InfoAtomo.atomo = ADICAO;
    }
    return InfoAtomo;
}
// funcao reconhece identificdor
// cada rotulo representa um estado na minha funcao
void reconhece_identificador(TInfoAtomo *InfoAtomo)
{
    int i = 0;
q0:
    if (islower(*cadeia))
    {
        InfoAtomo->atributo_identificador[i++] = *cadeia;
        // consome o simbolo
        cadeia++;
        // vai para q1
        goto q1;
    }
    // retorna erro caso venha um simbolo desconhecido
    return;
q1:
    if (isalpha(*cadeia))
    {
        InfoAtomo->atributo_identificador[i++] = *cadeia;
        // consome o simbolo
        cadeia++;
        // vai para q1
        goto q1;
    }
    if (isdigit(*cadeia))
        // retorna erro caso venha um simbolo desconhecido
        return;

    // copia o lexema do IDENTIFICADOR
    InfoAtomo->atributo_identificador[i] = 0;
    // testa se o lexema eh igual a while
    if (strcmp(InfoAtomo->atributo_identificador, "while") == 0)
        InfoAtomo->atomo = WHILE;
    else
        InfoAtomo->atomo = IDENTIFICADOR; // reconhece IDENTIFICADOR

    return;
}

// funcao reconhece numeros inteiros
// cada rotulo representa um estado na minha funcao
void reconhece_numero(TInfoAtomo *InfoAtomo)
{
    char strNum[20];
    int i = 0;
q0:
    // if(*cadeia == '0'|| *cadeia == '1'...)
    if (isdigit(*cadeia))
    {
        strNum[i++] = *cadeia;
        // consome o simbolo
        cadeia++;
        // vai para q1
        goto q1;
    }
    // retorna erro caso venha um simbolo desconhecido
    return;
q1:
    if (isdigit(*cadeia))
    {
        strNum[i++] = *cadeia;
        // consome o simbolo
        cadeia++;
        // vai para q1
        goto q1;
    }
    if (isalpha(*cadeia))
        // retorna erro caso venha um simbolo desconhecido
        return;

    InfoAtomo->atomo = NUMERO_INTEIRO; // reconhece numero inteiro
    // converte a lexema que gerou o atomo para inteiro
    strNum[i] = 0;
    InfoAtomo->atributo_numero = atoi(strNum);

    return;
}

// rotina le um arquivo e armazena no buffer cadeia
void leArquivo(char *nomeArquivo)
{
    int tamArquivo;
    // abre o arquivo
    FILE *fp;
    fp = fopen(nomeArquivo, "r");
    if (!fp)
    {
        printf("falha na abertura de arquivo.\n");
        exit(1);
    }

    // move ponteiro para o final
    fseek(fp, 0, SEEK_END);
    tamArquivo = ftell(fp); // pega o numero de bytes deslocas
    fseek(fp, 0, SEEK_SET); // move o ponteiro para o inicio
    // aloca um buffer com a quantide de bytes
    cadeia = (char *)calloc(tamArquivo + 1, sizeof(char));

    // le todos os caracteres do arquivo
    fread(cadeia, 1, tamArquivo, fp);
    // printf("%s",cadeia);
    fclose(fp);
}
