#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <locale.h>
/* ======================= FunÃ§Ãµes comeÃ§am na linha 102 ======================= */
/* ======================= Config DLL ======================= */
static HMODULE g_hDll = NULL;

/* ConvenÃ§Ã£o de chamada (Windows): __stdcall */
#ifndef CALLCONV
#  define CALLCONV WINAPI
#endif

/* ======================= Assinaturas da DLL ======================= */
typedef int (CALLCONV *AbreConexaoImpressora_t)(int, const char *, const char *, int);
typedef int (CALLCONV *FechaConexaoImpressora_t)(void);
typedef int (CALLCONV *ImpressaoTexto_t)(const char *, int, int, int);
typedef int (CALLCONV *Corte_t)(int);
typedef int (CALLCONV *ImpressaoQRCode_t)(const char *, int, int);
typedef int (CALLCONV *ImpressaoCodigoBarras_t)(int, const char *, int, int, int);
typedef int (CALLCONV *AvancaPapel_t)(int);
typedef int (CALLCONV *AbreGavetaElgin_t)(int, int, int);
typedef int (CALLCONV *AbreGaveta_t)(int, int, int);
typedef int (CALLCONV *SinalSonoro_t)(int, int, int);
typedef int (CALLCONV *ImprimeXMLSAT_t)(const char *, int);
typedef int (CALLCONV *ImprimeXMLCancelamentoSAT_t)(const char *, const char *, int);
typedef int (CALLCONV *InicializaImpressora_t)(void);

/* ======================= Ponteiros ======================= */
static AbreConexaoImpressora_t        AbreConexaoImpressora        = NULL;
static FechaConexaoImpressora_t       FechaConexaoImpressora       = NULL;
static ImpressaoTexto_t               ImpressaoTexto               = NULL;
static Corte_t                        Corte                        = NULL;
static ImpressaoQRCode_t              ImpressaoQRCode              = NULL;
static ImpressaoCodigoBarras_t        ImpressaoCodigoBarras        = NULL;
static AvancaPapel_t                  AvancaPapel                  = NULL;
static AbreGavetaElgin_t              AbreGavetaElgin              = NULL;
static AbreGaveta_t                   AbreGaveta                   = NULL;
static SinalSonoro_t                  SinalSonoro                  = NULL;
static ImprimeXMLSAT_t                ImprimeXMLSAT                = NULL;
static ImprimeXMLCancelamentoSAT_t    ImprimeXMLCancelamentoSAT    = NULL;
static InicializaImpressora_t         InicializaImpressora         = NULL;

/* ======================= ConfiguraÃ§Ã£o ======================= */
static int   g_tipo      = 1;
static char  g_modelo[64] = "i9";
static char  g_conexao[128] = "USB";
static int   g_parametro = 0;
static int   g_conectada = 0;

/* ======================= Utilidades ======================= */
#define LOAD_FN(h, name)                                                         \
    do {                                                                         \
        name = (name##_t)GetProcAddress((HMODULE)(h), #name);                    \
        if (!(name)) {                                                           \
            fprintf(stderr, "Falha ao resolver sÃ­mbolo %s (erro=%lu)\n",         \
                    #name, GetLastError());                                      \
            return 0;                                                            \
        }                                                                        \
    } while (0)

static void flush_entrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

/* ======================= FunÃ§Ãµes para manipular a DLL ======================= */
static int carregarFuncoes(void)
{
    g_hDll = LoadLibraryA("E1_Impressora01.dll");
    if (!g_hDll) {
        fprintf(stderr, "Erro ao carregar E1_Impressora01.dll (erro=%lu)\n", GetLastError());
        return 0;
    }

    LOAD_FN(g_hDll, AbreConexaoImpressora);
    LOAD_FN(g_hDll, FechaConexaoImpressora);
    LOAD_FN(g_hDll, ImpressaoTexto);
    LOAD_FN(g_hDll, Corte);
    LOAD_FN(g_hDll, ImpressaoQRCode);
    LOAD_FN(g_hDll, ImpressaoCodigoBarras);
    LOAD_FN(g_hDll, AvancaPapel);
    LOAD_FN(g_hDll, AbreGavetaElgin);
    LOAD_FN(g_hDll, AbreGaveta);
    LOAD_FN(g_hDll, SinalSonoro);
    LOAD_FN(g_hDll, ImprimeXMLSAT);
    LOAD_FN(g_hDll, ImprimeXMLCancelamentoSAT);
    LOAD_FN(g_hDll, InicializaImpressora);

    return 1;
}

static void liberarBiblioteca(void)
{
    if (g_hDll) {
        FreeLibrary(g_hDll);
        g_hDll = NULL;
    }
}

/* ======================= Funcoes ======================= */
static void configurarConexao(void)
{
    flush_entrada();

    /* ======================= Seleção do modelo ======================= */
    printf("\nModelos suportados:\n");
    printf("i7 | i7 Plus | i8 | i9 | ix | Fitpos\n");
    printf("BK-T681 | MP-4200 | MP-4200 HS | MK | MP-2800\n");

    printf("\nDigite o modelo da impressora: ");
    fgets(g_modelo, sizeof(g_modelo), stdin);
    g_modelo[strcspn(g_modelo, "\n")] = 0;

    if (strlen(g_modelo) == 0) {
        printf("Modelo invalido!\n");
        return;
    }

 

    /* ======================= Tipo da conexao ======================= */
    printf("\nSelecione o tipo:\n");
    printf("1 - USB\n");
    printf("2 - RS232\n");
    printf("3 - TCP/IP\n");
    printf("4 - Bluetooth\n");
    printf("5 - Impressoras acopladas Android\n");
    printf("Digite a opção: ");

    if (scanf("%d", &g_tipo) != 1 || g_tipo < 1 || g_tipo > 5) {
        printf("Tipo invalido!\n");
        flush_entrada();
        return;
    }

    /* ======================= Parametro ======================= */
    if (g_tipo == 1 || g_tipo == 2 || g_tipo == 5) {
        g_parametro = 0;
        printf("Parametro automatico = 0\n");
    } else {
        printf("Digite o parametro (porta, IP, canal, etc): ");
        if (scanf("%d", &g_parametro) != 1) {
            printf("Parametro invalido!\n");
            flush_entrada();
            return;
        }
    }

    printf("\nConfiguracao concluida!\n");
    printf("Modelo   : %s\n", g_modelo);
    printf("Conexao  : %s\n", g_conexao);
    printf("Tipo     : %d\n", g_tipo);
    printf("Parametro: %d\n", g_parametro);
}

static void abrirConexao(void)
{
    // Aqui a funcoo valida as informacoes, caso o 'resultado' seja = 0, significa que nao houve divergencia na configuracao e a conexao esta¡ correta.
    // Caso tenha algum problema na variavel 'resultado', ele fecha a conexÃ£o com a impressora pela variavel g_conectada=0.
    int resultado;
    resultado = AbreConexaoImpressora(g_tipo,g_modelo,g_conexao,g_parametro);
	if(resultado==0){g_conectada=1;printf("Conectado com sucesso...\n");
    InicializaImpressora(); SinalSonoro(4, 2, 5);
    flush_entrada();
    return;
}
	else{printf("Falha na conexao...\n");g_conectada=0;}
    return;
}

void fecharConexao()
{
    // funcaÃ§Ã£o para fechar conexÃ£o com impressora delimitada pela variÃ¡vel 'g_conectada'.
    if(g_conectada){
    FechaConexaoImpressora();
    g_conectada = 0;
    printf("conexao fechada!");
}
    else{
         printf("Sem conexao ativa");
         }
}

static void imprimirTexto(void)
{
     // Entrada de dados para texto, o usuÃ¡rio tem atÃ© 250 para escrever qualquer dado do tipo string, delimitado pelo FGETS e Sizeof.
    if(!g_conectada){printf("Impressora nao conectada!\n");return;}
     flush_entrada();
    char texto[250];
    int pos, estilo, tamanho;
	printf("Digite a mensagem a ser impressa, max. 250 caracteres:\n");
    fgets(texto,sizeof(texto),stdin);
    texto[strcspn(texto, "\n")]=0;
    // O usuÃ¡rio tem autonomia para escolher a formtaÃ§Ã£o, pos(posiÃ§Ã£o do texto na impressÃ£o),estilo(estilo do texto na impressÃ£o) e tamanho(seleciona o tamanho do texto).
    printf("0-Esquerda\n1-Centro\n2-Direira\nPosicao da impressao:\n");
    scanf("%d",&pos);
    printf("0-Fonte A\n1-Fonte B\n2-Sublinhado\n4-Modo reverso\n8-Negrito\nSelecione o estilo da impressao:\n");
    scanf("%d",&estilo);
	printf("Tamanho do texto(0 a 112):\n");
	scanf("%d",&tamanho);
	ImpressaoTexto(texto,pos,estilo,tamanho);
	AvancaPapel(3);
	Corte(1);
}

static void imprimirQRCode(void)
{
    // Entrada de dados com formataÃ§Ã£o do tipo QRCode, usuÃ¡rio digita os dados do tipo string, e a funÃ§Ã£o converte para QRCode.
    if(!g_conectada){printf("Impressora nao conectada!\n");return;}
     flush_entrada();
    char texto[250];
    int tamanho,correcao;
    flush_entrada();
    printf("Digite a mensagem a ser impressa, max. 250 caracteres:\n");
    fgets(texto,sizeof(texto),stdin);
    texto[strcspn(texto, "\n")]=0;
    // Aqui podemos configurar o tamanho e nivel de correÃ§Ã£o da funÃ§Ã£o ImpressaoQRCode(variavel tipo string,valor entre 1 a 6 para tamanho,valor de 1 a 4 para correÃ§Ã£o)
    ImpressaoQRCode(texto,6,4);
	AvancaPapel(3);
	Corte(1);
}

static void imprimirCodigoBarras(void)
{
    // Aqui podemos imprimir um valor fixo de cÃ³digo de barras, sem a entrada de dados. A sintaxe Ã© tipo do cÃ³digo de barras,
    // dados a serem impresso, altura, largura e HRI que define a posiÃ§Ã£o da impressÃ£o do conteÃºdo do cÃ³digo.
    if(!g_conectada){printf("Impressora nao conectada!\n");return;}
    ImpressaoCodigoBarras(8, "{A012345678912", 100, 2, 3);
    AvancaPapel(3);
	Corte(1);
}

static void imprimirXMLSAT(void)
{
    if(!g_conectada){printf("Impressora nao conectada!\n");return;}
    ImprimeXMLSAT("path=./XMLSAT.xml", 1);
    // TODO: ler o arquivo ./XMLSAT.xml e enviar via ImprimeXMLSAT
    // incluir AvancaPapel e Corte no final
    AvancaPapel(3);
	Corte(1);
}

static void imprimirXMLCancelamentoSAT(void)
{
    if(!g_conectada){printf("Impressora nao conectada!\n");return;}
    ImprimeXMLCancelamentoSAT("path=./CANC_SAT.xml", "Q5DLkpdRijIRGY6YSSNsTWK1TztHL1vD0V1Jc4spo/CEUqICEb9SFy82ym8EhBRZ"
        "jbh3btsZhF+sjHqEMR159i4agru9x6KsepK/q0E2e5xlU5cv3m1woYfgHyOkWDNc"
        "SdMsS6bBh2Bpq6s89yJ9Q6qh/J8YHi306ce9Tqb/drKvN2XdE5noRSS32TAWuaQE"
        "Vd7u+TrvXlOQsE3fHR1D5f1saUwQLPSdIv01NF6Ny7jZwjCwv1uNDgGZONJdlTJ6"
        "p0ccqnZvuE70aHOI09elpjEO6Cd+orI7XHHrFCwhFhAcbalc+ZfO5b/+vkyAHS6C"
        "YVFCDtYR9Hi5qgdk31v23w==", 1 );
    
    // TODO: ler o arquivo ./CANC_SAT.xml e chamar ImprimeXMLCancelamentoSAT
    // incluir AvancaPapel e Corte no final
    
	/*usar assinatura abaixo:
        "Q5DLkpdRijIRGY6YSSNsTWK1TztHL1vD0V1Jc4spo/CEUqICEb9SFy82ym8EhBRZ"
        "jbh3btsZhF+sjHqEMR159i4agru9x6KsepK/q0E2e5xlU5cv3m1woYfgHyOkWDNc"
        "SdMsS6bBh2Bpq6s89yJ9Q6qh/J8YHi306ce9Tqb/drKvN2XdE5noRSS32TAWuaQE"
        "Vd7u+TrvXlOQsE3fHR1D5f1saUwQLPSdIv01NF6Ny7jZwjCwv1uNDgGZONJdlTJ6"
        "p0ccqnZvuE70aHOI09elpjEO6Cd+orI7XHHrFCwhFhAcbalc+ZfO5b/+vkyAHS6C"
        "YVFCDtYR9Hi5qgdk31v23w==";
        */
    AvancaPapel(3);
	Corte(1);
}

static void abrirGavetaElginOpc(void)
{
    if(!g_conectada){printf("Impressora nao conectada!\n");return;}
    // Abre trava da gaveta da impressora elgin
    AbreGavetaElgin(1, 50, 50);
}

static void abrirGavetaOpc(void)
{
    if(!g_conectada){printf("Impressora nao conectada!\n");return;}
    // Abre gaveta com parametros personalizados, utilizado para impressoras de outras marcas. Sintaxe (valor inteiro de 0 a 1 com base no pino da impressora,
    // valor inteiro de 1 a 255 tempo de inicializaÃ§Ã£o, valor inteiro de 1 a 255 tempo de desativaÃ§Ã£o)
  
    AbreGaveta(1, 5, 10);
}

static void emitirSinalSonoro(void)
{
    if(!g_conectada){printf("Impressora nao conectada!\n");return;}
    // Emite um sinal sonoro afim de testes de conexÃ£o e funcionamento
    SinalSonoro(4, 2, 5);
}

static void exibirMenu(void)
{
    // Menu base com informaÃ§Ãµes das funÃ§Ãµes para selecionar
    printf("\n************************** MENU IMPRESSORA **************************\n\n");
    printf("1  -  Configurar Conexao\n2  -  Abrir Conexao\n3  -  Impressao Texto\n4  -  Impressao QRCode\n5  -  Impressao Cod Barras\n");
	printf("6  -  Impressao XML SAT\n7  -  Impressao XML Canc SAT\n8  -  Abrir Gaveta Elgin\n9  -  Abrir Gaveta\n10 -  Sinal Sonoro\n0  -  Fechar Conexao e Sair\n");	
}	

/* ======================= FunÃ§Ã£o principal ======================= */
int main(void)
{
    if (!carregarFuncoes()) {
        return 1;
    }

    int opcao = -1;
    while (opcao!=0) {    
	// La�o de repeti��o com menu(switch case) para intera��o com as fun��es da impressora Elgin, o la�o se encerra com o 'case 0' para encerrar conex�o e programa!
        //exibirMenu();
		scanf("%d",&opcao);
		flush_entrada();
		switch(opcao){
			case 1: configurarConexao(); break;
			case 2: abrirConexao(); break;
			case 3: imprimirTexto(); break;
				case 4: imprimirQRCode(); break;
				case 5: imprimirCodigoBarras(); break;
				case 6: imprimirXMLSAT(); break;
					case 7: imprimirXMLCancelamentoSAT(); break;
					case 8: abrirGavetaElginOpc(); break;
					case 9: abrirGavetaOpc(); break;
						case 10: emitirSinalSonoro(); break;
						case 0: fecharConexao(); break;
							default:
								printf("Op��o invalida\n"); break;    
                
    	}
       system("cls");
    }
}

