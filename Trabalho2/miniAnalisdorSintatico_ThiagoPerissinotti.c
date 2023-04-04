// THIAGO PERISSINOTTI
// TIA : 41903447

// Codigo indentado com ALT + SHIFT + F do VS Code
#include <stdio.h>
#include <ctype.h>  // isdigit
#include <stdlib.h> // free, calloc
#include <string.h> // strcmp

// Enum de possiveis tipos de atomos
typedef enum
{
    ERRO,
    NUMERO,
    EXPONENCIAL,
    IDENTIFICADOR,
    WHILE,
    EOS,
    ABRE_PAR,
    ADICAO,
    AND,
    BEGIN,
    BOOLEAN,
    CARACTERE,
    CHAR,
    COMENTARIO,
    DI,
    DIVISAO,
    DO,
    ELSE,
    END,
    FALSE,
    FECHA_PAR,
    IF,
    IG,
    MA,
    MAI,
    ME,
    MEI,
    MOD,
    MULTIPLICACAO,
    NOT,
    NUMBER,
    OR,
    PONTO,
    PONTO_VIRGULA,
    PROGRAM,
    READ,
    SET,
    SUBTRACAO,
    THEN,
    TO,
    TRUE,
    VIRGULA,
    WRITE
} TAtomo;

char *strMsgAtomo[] = {"ERRO", "NUMERO", "EXPONENCIAL", "IDENTIFICADOR", "WHILE", "EOS", "ABRE_PAR", "ADICAO", "AND", "BEGIN", "BOOLEAN", "CARACTERE", "CHAR", "COMENTARIO", "DI", "DIVISAO", "DO", "ELSE", "END", "FALSE", "FECHA_PAR", "IF", "IG", "MA", "MAI", "ME", "MEI", "MOD", "MULTIPLICACAO", "NOT", "NUMBER", "OR", "PONTO", "PONTO_VIRGULA", "PROGRAM", "READ", "SET", "SUBTRACAO", "THEN", "TO", "TRUE", "VIRGULA", "WRITE"};

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
void reconhece_comentario_varias_linhas(TInfoAtomo *InfoAtomo);
void reconhece_comentario_uma_linha(TInfoAtomo *InfoAtomo);
void reconhece_caracter(TInfoAtomo *InfoAtomo);

void leArquivo(char *nomeArquivo);
TInfoAtomo obter_atomo();

// declaracao das funcoes para analisador sintatico
void consome(TAtomo atomo);
void programa();// simbolo inicial da gramatica
void bloco();
void declaracao_de_variaveis();
void tipo();
void variaveis();
void comando_composto();
void comando();
void comando_atribuicao();
void comando_condicional();
void comando_repeticao();
void comando_entrada();
void comando_saida();
void expressao();
void expressao_simples();
void operador_simples();
void termo();
void operador_termo();
void fator();

int main(void)
{
    char *iniBuffer;
    leArquivo("entrada.txt");
    iniBuffer = cadeia; // para poder desalocar depois o buffer lido
    printf("\nanalise sintatica da palavra: %s\n", iniBuffer);
    // Antes de começar a análise sintática, temos que inicializar a variável lookahead.
    info_atomo_atual = obter_atomo();
    while (info_atomo_atual.atomo == COMENTARIO)
        {
            info_atomo_atual = obter_atomo();
        }
    lookahead = info_atomo_atual.atomo;

    programa();
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
        while (info_atomo_atual.atomo == COMENTARIO)
        {
            info_atomo_atual = obter_atomo();
        }
        
        lookahead = info_atomo_atual.atomo;
    }
    else
    {
        printf("Erro sintatico na linha %d: esperado [%s] encontrado [%s]\n", info_atomo_atual.linha, strMsgAtomo[atomo], strMsgAtomo[lookahead]);
        exit(1);
    }
}

// <programa>::= PROGRAM IDENTIFICADOR “;” <bloco> “.”
void programa()
{
    consome(PROGRAM);
    consome(IDENTIFICADOR);
    consome(PONTO_VIRGULA);
    bloco();
    consome(PONTO);
}

// <bloco>::= <declaracao_de_variaveis> <comando_composto>
void bloco()
{
    declaracao_de_variaveis();
    comando_composto();
}

// <declaracao_de_variaveis> ::= {<tipo> <variaveis> “;”}
void declaracao_de_variaveis()
{
    while (lookahead == NUMBER || lookahead == CHAR || lookahead == BOOLEAN)
    {
        tipo();
        variaveis();
        consome(PONTO_VIRGULA);
    }
}

// <tipo> ::= NUMBER | CHAR | BOOLEAN
void tipo()
{
    if (lookahead == NUMBER)
    {
        consome(NUMBER);
    }else if (lookahead == CHAR)
    {
        consome(CHAR);
    }else
        consome(BOOLEAN);
}

// <variaveis> ::= IDENTIFICADOR {“,” IDENTIFICADOR }
void variaveis()
{
    consome(IDENTIFICADOR);
    while (lookahead == VIRGULA)
    {
        consome(VIRGULA);
        consome(IDENTIFICADOR);
    }
}

// <comando_composto> ::= BEGIN <comando> {“;”<comando>} END
void comando_composto()
{
    consome(BEGIN);
    comando();
    while (lookahead == PONTO_VIRGULA)
    {
        consome(PONTO_VIRGULA);
        comando();
    }
    consome(END);
}

// <comando> ::= <comando_atribuicao> |
//              <comando_condicional> |
//                <comando_repeticao> |
//                  <comando_entrada> |
//                    <comando_saida> |
//                 <comando_composto>
void comando()
{
    if (lookahead == SET)
    {
        comando_atribuicao();
    }else if (lookahead == IF)
    {
        comando_condicional();
    }else if (lookahead == WHILE)
    {
        comando_repeticao();
    }else if (lookahead == READ)
    {
        comando_entrada();
    }else if (lookahead == WRITE)
    {
        comando_saida();
    }else
        comando_composto();
}

// <comando_atribuicao> ::= SET IDENTIFICADOR TO <expressão>
void comando_atribuicao()
{
    consome(SET);
    consome(IDENTIFICADOR);
    consome(TO);
    expressao();
}

// <comando_condicional> ::=    IF “(” <expressao> “)” THEN
//                                  <comando> [ELSE <comando>]
void comando_condicional()
{
    consome(IF);
    consome(ABRE_PAR);
    expressao();
    consome(FECHA_PAR);
    consome(THEN);
    comando();
    if (lookahead == ELSE)
    {
        consome(ELSE);
        comando();
    }
}

// <comando_repeticao> ::= WHILE “(” <expressao> “)” DO <comando>
void comando_repeticao()
{
    consome(WHILE);
    consome(ABRE_PAR);
    expressao();
    consome(FECHA_PAR);
    consome(DO);
    comando();
}

// <comando_entrada> ::= READ “(“ <variaveis> “)”
void comando_entrada()
{
    consome(READ);
    consome(ABRE_PAR);
    variaveis();
    consome(FECHA_PAR);
}

// <comando_saida> ::= WRITE “(“ <expressao> { “,” <expressao> } “)”
void comando_saida()
{
    consome(WRITE);
    consome(ABRE_PAR);
    expressao();
    while (lookahead == VIRGULA)
    {
        consome(VIRGULA);
        expressao();
    }
    consome(FECHA_PAR);
}

// <expressao> ::= <expressao_simples> [OP_RELACIONAL <expressao_simples> ]
void expressao()
{
    expressao_simples();
    if (lookahead == ME)
    {
        consome(ME);
        expressao_simples();
    }else if (lookahead == MEI)
    {
        consome(MEI);
        expressao_simples();
    }else if (lookahead == IG)
    {
        consome(IG);
        expressao_simples();
    }else if (lookahead == DI)
    {
        consome(DI);
        expressao_simples();
    }else if (lookahead == MA)
    {
        consome(MA);
        expressao_simples();
    }else if (lookahead == MAI)
    {
        consome(MAI);
        expressao_simples();
    }
}

// <expressao_simples> ::= [“+” | “−”] <termo> { <operador_simples> <termo> }
void expressao_simples()
{
    if (lookahead == ADICAO)
    {
        consome(ADICAO);
    }else if (lookahead == SUBTRACAO)
    {
        consome(SUBTRACAO);
    }
    termo();
    
    while (lookahead == ADICAO || lookahead == SUBTRACAO || lookahead == OR)
    {
        operador_simples();
        termo();
    }
}

// <operador_simples> ::= “+” | “−” | OR
void operador_simples()
{
    if (lookahead == ADICAO)
    {
        consome(ADICAO);
    }else if (lookahead == SUBTRACAO)
    {
        consome(SUBTRACAO);
    }else
        consome(OR);    
}

// <termo> ::= <fator> { <operador_termo> <fator> }
void termo()
{
    fator();
    while (lookahead == MULTIPLICACAO || lookahead == DIVISAO || lookahead == MOD || lookahead == AND)
    {
        operador_termo();
        fator();
    }
}

// <operador_termo> ::= “*” | “/” | MOD | AND
void operador_termo()
{
    if (lookahead == MULTIPLICACAO)
    {
        consome(MULTIPLICACAO);
    }else if (lookahead == DIVISAO)
    {
        consome(DIVISAO);
    }else if (lookahead == MOD)
    {
        consome(MOD);
    }else
        consome(AND);
}

// <fator> ::= IDENTIFICADOR |
//                    NUMERO |
//                 CARACTERE |
//                      TRUE |
//                     FALSE |
//               NOT <fator> |
//              “(” <expressao> “)”
void fator()
{
    if (lookahead == IDENTIFICADOR)
    {
        consome(IDENTIFICADOR);
    }else if (lookahead == NUMERO)
    {
        consome(NUMERO);
    }else if (lookahead == CARACTERE)
    {
        consome(CARACTERE);
    }else if (lookahead == TRUE)
    {
        consome(TRUE);
    }else if (lookahead == FALSE)
    {
        consome(FALSE);
    }else if (lookahead == NOT)
    {
        consome(NOT);
        fator();
    }else
    {
        consome(ABRE_PAR);
        expressao();
        consome(FECHA_PAR);
    }
}

// FUNCOES DO ANALISADOR LEXICO
TInfoAtomo obter_atomo()
{
    TInfoAtomo InfoAtomo;
    InfoAtomo.atomo = ERRO;
    // descarta delimitadores
    while (isspace(*cadeia) || *cadeia == 0)
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
    else if (isalpha(*cadeia))
    {
        reconhece_identificador(&InfoAtomo);
    }
    else if (*cadeia == '{')
    {
        reconhece_comentario_varias_linhas(&InfoAtomo);
    }
    else if (*cadeia == '(')
    {
        InfoAtomo.atomo = ABRE_PAR;
        cadeia++;
    }
    else if (*cadeia == ')')
    {
        InfoAtomo.atomo = FECHA_PAR;
        cadeia++;
    }
    else if (*cadeia == '.')
    {
        InfoAtomo.atomo = PONTO;
        cadeia++;
    }
    else if (*cadeia == ';')
    {
        InfoAtomo.atomo = PONTO_VIRGULA;
        cadeia++;
    }
    else if (*cadeia == ',')
    {
        InfoAtomo.atomo = VIRGULA;
        cadeia++;
    }
    else if (*cadeia == '<')
    {
        cadeia++;
        if (*cadeia != '=')
        {
            InfoAtomo.atomo = ME;
        }
        else
        {
            InfoAtomo.atomo = MEI;
            cadeia++;
        }
    }
    else if (*cadeia == '=')
    {
        InfoAtomo.atomo = IG;
        cadeia++;
    }
    else if (*cadeia == '>')
    {
        cadeia++;
        if (*cadeia != '=')
        {
            InfoAtomo.atomo = MA;
        }
        else
        {
            InfoAtomo.atomo = MAI;
            cadeia++;
        }
    }
    else if (*cadeia == '-')
    {
        InfoAtomo.atomo = SUBTRACAO;
        cadeia++;
    }
    else if (*cadeia == '+')
    {
        InfoAtomo.atomo = ADICAO;
        cadeia++;
    }
    else if (*cadeia == '/')
    {
        cadeia++;
        if (*cadeia == '/')
            reconhece_comentario_uma_linha(&InfoAtomo);
        else if (*cadeia == '=')
        {
            cadeia++;
            InfoAtomo.atomo = DI;
        }
        else
        {
            InfoAtomo.atomo = DIVISAO;
        }
    }
    else if (*cadeia == '*')
    {
        InfoAtomo.atomo = MULTIPLICACAO;
        cadeia++;
    }
    else if (*cadeia == '\'')
    {
        reconhece_caracter(&InfoAtomo);
    }
    return InfoAtomo;
}

// funcao reconhece identificdor
// cada rotulo representa um estado na minha funcao
void reconhece_identificador(TInfoAtomo *InfoAtomo)
{
    int i = 0;
    InfoAtomo->atributo_identificador[i] = tolower(*cadeia);
    cadeia++;
q0:
    if ((isalnum(*cadeia) || *cadeia == '_') && i < 13)
    {
        i++;
        InfoAtomo->atributo_identificador[i] = tolower(*cadeia);
        cadeia++;
        goto q0;
    }
    if (isspace(*cadeia) || *cadeia == ';' || *cadeia == ',' || *cadeia == '.' || *cadeia == ')' || *cadeia == '(')
    {
        i++;
        InfoAtomo->atributo_identificador[i] = 0;
        // procura se e uma palavra reservada
        if (strcmp(InfoAtomo->atributo_identificador, "and") == 0)
        {
            InfoAtomo->atomo = AND;
        }
        else if (strcmp(InfoAtomo->atributo_identificador, "begin") == 0)
        {
            InfoAtomo->atomo = BEGIN;
        }
        else if (strcmp(InfoAtomo->atributo_identificador, "boolean") == 0)
        {
            InfoAtomo->atomo = BOOLEAN;
        }
        else if (strcmp(InfoAtomo->atributo_identificador, "char") == 0)
        {
            InfoAtomo->atomo = CHAR;
        }
        else if (strcmp(InfoAtomo->atributo_identificador, "do") == 0)
        {
            InfoAtomo->atomo = DO;
        }
        else if (strcmp(InfoAtomo->atributo_identificador, "else") == 0)
        {
            InfoAtomo->atomo = ELSE;
        }
        else if (strcmp(InfoAtomo->atributo_identificador, "end") == 0)
        {
            InfoAtomo->atomo = END;
        }
        else if (strcmp(InfoAtomo->atributo_identificador, "false") == 0)
        {
            InfoAtomo->atomo = FALSE;
        }
        else if (strcmp(InfoAtomo->atributo_identificador, "if") == 0)
        {
            InfoAtomo->atomo = IF;
        }
        else if (strcmp(InfoAtomo->atributo_identificador, "mod") == 0)
        {
            InfoAtomo->atomo = MOD;
        }
        else if (strcmp(InfoAtomo->atributo_identificador, "not") == 0)
        {
            InfoAtomo->atomo = NOT;
        }
        else if (strcmp(InfoAtomo->atributo_identificador, "number") == 0)
        {
            InfoAtomo->atomo = NUMBER;
        }
        else if (strcmp(InfoAtomo->atributo_identificador, "or") == 0)
        {
            InfoAtomo->atomo = OR;
        }
        else if (strcmp(InfoAtomo->atributo_identificador, "program") == 0)
        {
            InfoAtomo->atomo = PROGRAM;
        }
        else if (strcmp(InfoAtomo->atributo_identificador, "read") == 0)
        {
            InfoAtomo->atomo = READ;
        }
        else if (strcmp(InfoAtomo->atributo_identificador, "set") == 0)
        {
            InfoAtomo->atomo = SET;
        }
        else if (strcmp(InfoAtomo->atributo_identificador, "then") == 0)
        {
            InfoAtomo->atomo = THEN;
        }
        else if (strcmp(InfoAtomo->atributo_identificador, "to") == 0)
        {
            InfoAtomo->atomo = TO;
        }
        else if (strcmp(InfoAtomo->atributo_identificador, "true") == 0)
        {
            InfoAtomo->atomo = TRUE;
        }
        else if (strcmp(InfoAtomo->atributo_identificador, "while") == 0)
        {
            InfoAtomo->atomo = WHILE;
        }
        else if (strcmp(InfoAtomo->atributo_identificador, "write") == 0)
        {
            InfoAtomo->atomo = WRITE;
        }
        else // determina que nao e reservada e retorna IDENTIFICADOR
            InfoAtomo->atomo = IDENTIFICADOR;
    }
    return;
}

// reconhece comentario multi-linhas
void reconhece_comentario_varias_linhas(TInfoAtomo *InfoAtomo)
{
    cadeia++;
q0:
    if (*cadeia == '-')
    {
        cadeia++;
        goto q1;
    }
    return;
q1:
    if (*cadeia == '-')
    {
        cadeia++;
        goto q2;
    }
    if (*cadeia == '\n')
    {
        linha++;
    }
    else if (*cadeia == 0)
    {
        return;
    }
    cadeia++;
    goto q1;

q2:
    if (*cadeia == '}')
    {
        cadeia++;
        goto q3;
    }
    cadeia++;
    goto q1;
q3:
    InfoAtomo->atomo = COMENTARIO;
}

// reconhece comentario comum
void reconhece_comentario_uma_linha(TInfoAtomo *InfoAtomo)
{
q0:
    if (*cadeia == '\n')
    {
        linha++;
        cadeia++;
        goto q1;
    }
    cadeia++;
    goto q0;
q1:
    InfoAtomo->atomo = COMENTARIO;
}

// reconhece numero ou notacao
void reconhece_numero(TInfoAtomo *InfoAtomo)
{
    int i = 0;
    InfoAtomo->atributo_identificador[i] = *cadeia;
    i++;
    cadeia++;

q0:
    if (isdigit(*cadeia))
    {
        InfoAtomo->atributo_identificador[i] = *cadeia;
        cadeia++;
        i++;
        goto q0;
    }
    else if (*cadeia == 'e')
    {
        InfoAtomo->atributo_identificador[i] = *cadeia;
        cadeia++;
        i++;
        goto q1;
    }
    else if (isalpha(*cadeia))
        return;

    InfoAtomo->atributo_identificador[i] = 0;
    InfoAtomo->atomo = NUMERO;
    return;

q1:
    if (*cadeia == '+' || *cadeia == '-')
    {
        InfoAtomo->atributo_identificador[i] = *cadeia;
        cadeia++;
        i++;
        goto q2;
    }
    if (isdigit(*cadeia))
    {
        InfoAtomo->atributo_identificador[i] = *cadeia;
        cadeia++;
        i++;
        goto q3;
    }
    return;
q2:
    if (isdigit(*cadeia))
    {
        InfoAtomo->atributo_identificador[i] = *cadeia;
        cadeia++;
        i++;
        goto q3;
    }
    return;
q3:
    if (isdigit(*cadeia))
    {
        InfoAtomo->atributo_identificador[i] = *cadeia;
        cadeia++;
        i++;
        goto q3;
    }
    else if (isalpha(*cadeia))
        return;
    InfoAtomo->atributo_identificador[i] = 0;
    InfoAtomo->atomo = EXPONENCIAL;
    return;
}
// reconhece strings de caracteres
void reconhece_caracter(TInfoAtomo *InfoAtomo)
{
    int i = 0;
    cadeia++;
q0:
    if (*cadeia == '\'')
    {
        InfoAtomo->atributo_identificador[i] = 0;
        cadeia++;
        InfoAtomo->atomo = CARACTERE;
        return;
    }
    else
    {
        InfoAtomo->atributo_identificador[i] = *cadeia;
        i++;
        cadeia++;
        goto q0;
    }
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
