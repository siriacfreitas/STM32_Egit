# Leitor de Cartão SD com STM32

Este projeto demonstra como usar um microcontrolador STM32 para ler o conteúdo de um arquivo de texto (`teste.txt`) armazenado em um cartão SD e enviar esses dados para um computador através da comunicação serial (UART). 🖥️➡️💾

O projeto utiliza a biblioteca **FatFs** para gerenciar o sistema de arquivos do cartão SD e a biblioteca **HAL (Hardware Abstraction Layer)** da ST para configurar e controlar os periféricos do microcontrolador.

## Funcionalidade Principal

O programa executa uma sequência simples de operações ao ser inicializado:

1.  **Monta o Sistema de Arquivos:** Tenta "montar" (ou seja, acessar) o sistema de arquivos FAT do cartão SD. A comunicação com o cartão SD é geralmente feita pelo protocolo **SPI**.
2.  **Abre o Arquivo:** Se a montagem for bem-sucedida, ele tenta abrir um arquivo chamado `teste.txt`. Se o arquivo não existir, ele será criado.
3.  **Lê o Conteúdo:** O programa lê até 30 bytes do arquivo e armazena os dados em um buffer (um array de caracteres).
4.  **Fecha o Arquivo:** Após a leitura, o arquivo é fechado para liberar os recursos.
5.  **Envia via UART:** Finalmente, o conteúdo lido do arquivo é transmitido para o computador através da porta serial. Se ocorrer um erro ao montar o cartão SD, uma mensagem de erro será enviada.

Após essa sequência, o microcontrolador entra em um loop infinito, não realizando mais nenhuma ação.

## Estrutura do Código

Aqui estão as partes mais importantes do código e o que elas fazem:

### Variáveis Globais e Funções de Suporte

```c
/* Includes */
#include "main.h"
#include "fatfs.h" // Essencial para o sistema de arquivos
#include "string.h"
#include "stdio.h"

/* Variáveis Globais */
SPI_HandleTypeDef hspi1; // Handle para a comunicação SPI com o cartão SD
UART_HandleTypeDef huart2; // Handle para a comunicação serial com o PC
char rxdados[30]; // Buffer para armazenar os dados lidos do arquivo

/* Função de Envio UART */
void send_uart(const char *msg) {
    HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
}
```

  - **Includes:** `fatfs.h` é a biblioteca chave para manipular o cartão SD. `string.h` e `stdio.h` fornecem funções úteis para manipulação de texto.
  - **Handles (`hspi1`, `huart2`):** São estruturas que guardam todas as configurações dos periféricos SPI e UART, respectivamente.
  - **`rxdados`:** É um array de 30 caracteres que servirá como um "depósito" temporário para os dados lidos do arquivo `teste.txt`.
  - **`send_uart()`:** Uma função auxiliar criada para simplificar o envio de mensagens de texto pela porta serial (`huart2`).

-----

### Função `main()` - O Coração do Programa ❤️

A função `main` é onde toda a lógica de inicialização e operação acontece.

```c
int main(void)
{
    /* ... Inicialização do MCU e Periféricos ... */
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART2_UART_Init();
    MX_SPI1_Init();
    MX_FATFS_Init();

    /* Variáveis locais para o FatFs */
    FATFS meuFATFS;    // Estrutura que representa o sistema de arquivos
    FIL meuArquivo;    // Estrutura que representa o arquivo aberto
    UINT testeByte;    // Variável para armazenar o número de bytes lidos

    // Tenta montar o cartão SD
    if (f_mount(&meuFATFS, USERPath, 1) == FR_OK) {
        char caminho[] = "teste.txt"; // Nome do arquivo a ser lido

        // Abre o arquivo (cria se não existir)
        f_open(&meuArquivo, caminho, FA_READ | FA_OPEN_ALWAYS);
        // Lê até 30 bytes do arquivo para o buffer 'rxdados'
        f_read(&meuArquivo, rxdados, 30, &testeByte);
        // Fecha o arquivo
        f_close(&meuArquivo);

    } else {
        // Se a montagem falhar, prepara uma mensagem de erro
        strcpy(rxdados, "Erro: Falha no f_mount.\n");
    }
    
    // Envia o conteúdo do buffer 'rxdados' pela serial
    send_uart(rxdados);

    /* Loop infinito */
    while (1)
    {
        // O programa fica aqui para sempre após a execução inicial
    }
}
```

  - **Inicialização (`MX_..._Init()`):** Funções geradas pelo STM32CubeMX que configuram os periféricos essenciais (GPIOs, UART, SPI) e o sistema de arquivos (FATFS).
  - **Lógica do FatFs:**
      - `f_mount`: É o primeiro e mais importante passo. Ele tenta inicializar e reconhecer o cartão SD. Se isso falhar, nada mais funcionará.
      - `f_open`: Abre o arquivo `teste.txt` em modo de leitura (`FA_READ`). A opção `FA_OPEN_ALWAYS` garante que, se o arquivo não existir, ele será criado vazio.
      - `f_read`: Copia os dados do arquivo para a nossa variável `rxdados`.
      - `f_close`: Libera o arquivo, garantindo que os dados não sejam corrompidos.
  - **Comunicação:** A função `send_uart(rxdados)` é chamada para transmitir o resultado (o conteúdo do arquivo ou a mensagem de erro) para o seu computador.

-----

### Funções de Configuração de Periféricos

  - **`MX_SPI1_Init()`:** Configura a interface **SPI1** para se comunicar com o cartão SD. Define parâmetros como modo (Master), velocidade e formato dos dados.
  - **`MX_USART2_UART_Init()`:** Configura a porta **USART2** para a comunicação serial. Define a velocidade (`BaudRate` de 115200), formato dos dados e modo (transmissão e recepção).
  - **`MX_GPIO_Init()`:** Configura os pinos de entrada e saída. Uma parte importante aqui é a configuração do pino **Chip Select (CS)** para o SPI, que é usado para selecionar o cartão SD como o dispositivo ativo na linha de comunicação.

## Como Usar

1.  **Hardware:** Conecte um módulo de cartão SD aos pinos SPI1 do seu STM32. Conecte também o STM32 ao seu computador via um conversor USB-Serial (geralmente integrado em placas como a Nucleo através do ST-Link).
2.  **Cartão SD:** Formate um cartão SD com o sistema de arquivos **FAT32**. Crie um arquivo chamado `teste.txt` na raiz do cartão e escreva alguma mensagem dentro dele.
3.  **Software:** Compile e carregue este código na sua placa STM32.
4.  **Monitor Serial:** Abra um programa de monitor serial (como o da IDE do Arduino, PuTTY ou Tera Term), conecte-o à porta COM correspondente ao seu STM32 e configure a velocidade para **115200 baud**.
5.  **Resultado:** Ao resetar a placa, a mensagem contida no arquivo `teste.txt` deverá aparecer no seu monitor serial. Se houver um problema com o cartão ou a conexão, você verá a mensagem de erro.
