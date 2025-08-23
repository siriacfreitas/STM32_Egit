# Projeto UART: Transmissão Periódica e Recepção por Interrupção 📡🔄

Este projeto demonstra uma arquitetura comum em sistemas embarcados, onde o microcontrolador executa duas tarefas de comunicação serial em paralelo:

1.  **Transmissão Periódica (Tarefa de Fundo):** O STM32 envia um contador que incrementa a cada 5 segundos. Esta é uma tarefa síncrona, controlada pelo loop principal.
2.  **Recepção por Interrupção (Tarefa Assíncrona):** O STM32 fica constantemente "ouvindo" por dados vindos do computador. Ao receber dados, ele interrompe sua tarefa principal para processar a informação imediatamente e enviar uma resposta.

O objetivo é mostrar como usar interrupções para criar um sistema responsivo que não fica "travado" esperando por dados, ao mesmo tempo que executa uma rotina regular.

-----

## Hardware Necessário

  * **Placa de Desenvolvimento STM32 Nucleo:** A `USART2` da placa é conectada ao ST-LINK, que cria uma Porta COM Virtual no computador.
  * **Cabo USB:** Para alimentação, programação e comunicação.
  * **Computador:** Com um software de terminal serial (ex: PuTTY, Tera Term, etc.).

-----

## Funcionalidade

O programa tem dois comportamentos independentes:

### Transmissão (Saída ➡)

A cada **5 segundos**, a placa envia uma mensagem para o computador com o valor de um contador, formatado com 4 dígitos. A saída será parecida com:

```
X = 0001
X = 0002
X = 0003
...
```

### Recepção (Entrada ⬅)

O microcontrolador está sempre pronto para receber dados. Sempre que você envia **2 caracteres** do seu terminal para a placa, ela irá **imediatamente** responder com uma mensagem de confirmação, ecoando o primeiro caractere que você enviou. Por exemplo, se você digitar `OK` e enviar, a placa responderá:

```
RECEBIDO: O
```

Essa resposta é imediata e pode acontecer a qualquer momento, mesmo durante a espera de 5 segundos da tarefa de transmissão, pois é tratada por uma **interrupção**.

-----

## Como Usar e Testar

1.  **Carregue o código** na sua placa Nucleo.
2.  **Abra um Terminal Serial** (como PuTTY ou Tera Term).
3.  **Configure a Conexão** com os seguintes parâmetros:
      * **Porta:** A porta COM virtual da sua placa (verifique no Gerenciador de Dispositivos).
      * **Baud Rate (Velocidade):** `115200`
      * **Configuração:** 8N1 (8 bits de dados, sem paridade, 1 bit de parada).
4.  **Conecte** ao terminal.

### Testando a Funcionalidade

  * **Observe a Transmissão:** Primeiro, apenas observe o terminal. Você verá as mensagens `X = ...` aparecendo a cada 5 segundos.
  * **Interaja com a Recepção:** A qualquer momento, digite **um caracteres** no seu terminal (ex: `a`, `1`, `Z`) e aperte **enter**. Assim que o segundo caractere for enviado, você verá a resposta `RECEBIDO: ...` aparecer instantaneamente no terminal, sem esperar o ciclo de 5 segundos terminar.

-----

## Análise Detalhada do Código

Este código cria um sistema que executa duas tarefas de comunicação de forma concorrente: uma tarefa principal, lenta e periódica, e uma tarefa secundária, rápida e reativa.

### 1\. Variáveis Globais e Buffers

```c
#include <stdio.h> // Adicionado para usar a função sprintf

uint16_t X_soma = 0;
uint8_t buffer_rx[2];
uint8_t buffer_tx[32];
```

  * **`#include <stdio.h>`**: A biblioteca padrão de Entrada/Saída de C é incluída para termos acesso à função `sprintf`, que é excelente para formatar texto e números em uma string.
  * **`X_soma`**: Um contador simples que será incrementado e enviado periodicamente.
  * **`buffer_tx[32]`**: Um buffer de 32 bytes que serve como "rascunho". A função `sprintf` montará a string de saída (`"X = 0001\r\n"`) nesta área da memória antes de enviá-la pela UART.
  * **`buffer_rx[2]`**: Um buffer de 2 bytes para armazenar os dados que chegam do computador. A interrupção será acionada assim que exatamente 2 bytes forem recebidos.

### 2\. A Função `main()` - Ponto de Entrada e Configuração

A função `main()` é responsável por inicializar o sistema e executar a tarefa principal.

#### Bloco de Inicialização:

```c
HAL_Init();
SystemClock_Config(); // Usa o PLL para maior performance
MX_GPIO_Init();
MX_USART2_UART_Init();

// Inicia recepção UART2 com interrupção
HAL_UART_Receive_IT(&huart2, buffer_rx, sizeof(buffer_rx));
```

  * **`HAL_Init()`, `SystemClock_Config()`, `MX...Init()`**: Funções padrão que inicializam a biblioteca HAL, configuram os clocks do sistema (usando o PLL para maior performance) e os periféricos GPIO e USART2.
  * **`HAL_UART_Receive_IT(...)`**: Esta é uma chamada crucial e **não-bloqueante**. Ela "arma" a recepção por interrupção na UART2, instruindo o hardware a começar a ouvir por dados e gerar uma interrupção quando 2 bytes forem recebidos. O programa continua imediatamente para o `while(1)`.

#### O Loop Principal - Tarefa Periódica:

```c
while (1)
{
    X_soma++;
    sprintf((char *)buffer_tx, "X = %04d\r\n", X_soma);
    HAL_UART_Transmit(&huart2, buffer_tx, strlen((char *)buffer_tx), HAL_MAX_DELAY);
    HAL_Delay(5000);
}
```

Este loop executa a tarefa de fundo:

  * **`sprintf(...)`**: Formata uma string e a armazena no `buffer_tx`. O formato `"%04d"` garante que o número tenha sempre 4 dígitos.
  * **`HAL_UART_Transmit(...)`**: Envia os dados de forma **bloqueante**. O processador fica "preso" nesta linha até que todos os bytes sejam enviados.
  * **`HAL_Delay(5000)`**: Outra função **bloqueante** que pausa a execução por 5 segundos. Durante esta pausa, as interrupções continuam ativas e podem ocorrer a qualquer momento.

### 3\. A Rotina de Callback - O Coração da Reatividade

```c
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        // Envia confirmação de recepção
        HAL_UART_Transmit(&huart2, (uint8_t *)"RECEBIDO: ", 10, HAL_MAX_DELAY);
        HAL_UART_Transmit(&huart2, buffer_rx, 1, HAL_MAX_DELAY); // Envia só o 1º byte
        HAL_UART_Transmit(&huart2, (uint8_t *)"\r\n", 2, HAL_MAX_DELAY);

        // Limpa o buffer de recepção (boa prática)
        memset(buffer_rx, 0, sizeof(buffer_rx));

        // Reinicia recepção por interrupção (essencial!)
        HAL_UART_Receive_IT(&huart2, buffer_rx, 2);
    }
}
```

  * **O que é?** `HAL_UART_RxCpltCallback` é uma função de *callback*. A biblioteca HAL a chama automaticamente sempre que uma recepção por interrupção é concluída. O hardware da UART gera a interrupção, o processador pausa o `while(1)` (mesmo se estiver no meio do `HAL_Delay`) e executa esta função.
  * **Lógica Interna:**
    1.  O código envia a mensagem de confirmação, incluindo o primeiro byte que foi recebido.
    2.  `memset(...)`: Zera o `buffer_rx`, limpando os dados antigos.

    3.  **`HAL_UART_Receive_IT(...)`**: Esta é a linha **mais importante** do callback. Após processar os dados recebidos, ela **re-arma a interrupção**, dizendo ao hardware para começar a ouvir novamente pelos próximos 2 bytes. Sem esta linha, o sistema só receberia dados uma única vez.
