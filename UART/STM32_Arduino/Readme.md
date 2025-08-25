# Comunicação Serial (UART) entre Arduino e STM32

Este projeto demonstra uma comunicação serial bidirecional entre uma placa Arduino (usando o framework Arduino) e um microcontrolador STM32 (usando a HAL Library com FreeRTOS). A comunicação permite que mensagens enviadas do monitor serial do PC para o Arduino sejam repassadas ao STM32, e mensagens enviadas do STM32 sejam exibidas no monitor serial do Arduino. Da mesma forma, mensagens do PC para o STM32 são repassadas ao Arduino.

## Visão Geral da Funcionalidade

O sistema funciona como uma ponte de comunicação dupla:

1.  **PC (via Arduino) → STM32:** Você digita uma mensagem no Monitor Serial da IDE do Arduino. O Arduino lê essa mensagem e a envia, via sua porta `Serial2`, para a porta UART do STM32.
2.  **STM32 → PC (via Arduino):** O STM32 recebe a mensagem do Arduino, a processa e pode enviar uma resposta de volta para o Arduino, que a exibirá no Monitor Serial.
3.  **PC (via ST-Link) → Arduino:** Você digita uma mensagem em um terminal conectado à porta COM virtual do STM32. O STM32 lê essa mensagem e a envia para o Arduino através de sua porta UART.

---

## Código do Arduino (`.ino`)

Este código é responsável por gerenciar a comunicação entre o PC (através do Monitor Serial da IDE) e o STM32 (através de uma porta serial de hardware, `Serial2`).

### `void setup()` - Configuração Inicial

```cpp
void setup() {
  Serial.begin(9600);    // Inicia a comunicação com o PC (Monitor Serial)
  Serial2.begin(9600);   // Inicia a comunicação com o STM32
  Serial.println("Monitor disponivel");
}
```

-   `Serial.begin(9600)`: Inicializa a comunicação serial principal (`Serial`) que se conecta ao computador via USB. A velocidade de comunicação é definida como **9600 bits por segundo (baud rate)**. É através desta porta que interagimos com o PC.
-   `Serial2.begin(9600)`: Inicializa uma segunda porta serial de hardware (`Serial2`). Em placas como o Arduino Mega, `Serial2` corresponde aos pinos físicos `RX2` (17) e `TX2` (16). **É esta porta que deve ser conectada ao STM32.** A velocidade é a mesma para garantir que ambos os dispositivos "falem" no mesmo ritmo.

### `void loop()` - Lógica Principal

O `loop()` é executado repetidamente e funciona como um "agente de trânsito" para as mensagens.

```cpp
void loop() {
  // 1. Verifica se o STM32 enviou dados
  if (Serial2.available()) {
    String msg = Serial2.readStringUntil('\n'); // Lê a linha inteira
    msg.trim(); // Remove espaços/caracteres invisíveis
    if (msg.length() > 0) {
      Serial.print("Recebido do STM32: ");
      Serial.println(msg); // Exibe no monitor do PC
    }
  }

  // 2. Verifica se o usuário digitou algo no Monitor Serial
  if (Serial.available()) {
    String msg = Serial.readStringUntil('\n');
    msg.trim();
    if (msg.length() > 0) {
      Serial.print("Mensagem enviada para o STM: ");
      Serial.println(msg); // Mostra no monitor local o que foi enviado
      Serial2.println(msg); // Envia a mensagem para o STM32
    }
  }
  delay(100); // Pequena pausa para estabilidade
}
```

#### Análise Detalhada do `loop()`

-   `if (Serial2.available())`: Esta linha verifica se há algum byte esperando para ser lido na porta serial que vem do STM32. A função retorna um número maior que zero se houver dados.
-   `String msg = Serial2.readStringUntil('\n')`: Esta é uma função de alto nível da biblioteca Arduino. Ela lê todos os caracteres da porta serial e os armazena em um objeto `String` até encontrar o caractere de nova linha (`\n`), que é o caractere que a função `println` do outro dispositivo geralmente envia no final.
-   `msg.trim()`: Remove quaisquer espaços em branco, tabulações ou caracteres de nova linha do início e do fim da `String`. Isso garante que a mensagem esteja "limpa" antes de ser processada.
-   `if (msg.length() > 0)`: Garante que não processemos mensagens vazias, que podem ocorrer se apenas um caractere de nova linha for recebido.
-   `Serial.println(msg)` vs `Serial2.println(msg)`: O primeiro (`Serial`) envia a mensagem para o PC. O segundo (`Serial2`) envia a mensagem para o STM32.
-   `delay(100)`: Introduz uma pausa de 100 milissegundos. Em um código simples como este, ajuda a evitar que o loop rode rápido demais, o que poderia, em alguns casos, causar instabilidade ou sobrecarregar o buffer serial. Em sistemas mais complexos (como o do STM32), os delays são gerenciados de forma mais eficiente pelo RTOS.

---

## Código do STM32 (`main.c`)

Este código usa a **HAL (Hardware Abstraction Layer)**, que fornece uma camada de abstração para o hardware, e o **FreeRTOS**, um sistema operacional de tempo real que permite a execução de múltiplas tarefas de forma concorrente.

### `main()` - Ponto de Entrada e Configuração do RTOS

A função `main` é o ponto de partida. Suas responsabilidades são:

1.  **Inicializar o hardware:** Chama `HAL_Init()`, `SystemClock_Config()`, e as funções `MX_..._Init()` geradas pelo CubeMX para configurar GPIOs e as duas UARTs (`huart2` para o PC e `huart4` para o Arduino).
2.  **Inicializar o Kernel do RTOS:** `osKernelInitialize()` prepara o sistema operacional para ser executado.
3.  **Criar as Tarefas (Threads):**
    ```c
    ArduinoRxHandle = osThreadNew(Task_ArduinoRx, NULL, &ArduinoRx_attributes);
    PCRxHandle = osThreadNew(Task_PCRx, NULL, &PCRx_attributes);
    ```
    -   `osThreadNew` é uma função do FreeRTOS que cria e registra uma nova tarefa.
    -   `Task_ArduinoRx` e `Task_PCRx` são as funções que contêm a lógica de cada tarefa.
    -   O RTOS irá gerenciar a execução dessas duas tarefas, alternando entre elas para dar a ilusão de que estão rodando ao mesmo tempo.
4.  **Iniciar o Agendador (Scheduler):** `osKernelStart()` entrega o controle do processador ao FreeRTOS, que começa a executar as tarefas. O código após esta linha nunca será executado.

### `void Task_ArduinoRx(void *argument)` - Tarefa de Escuta do Arduino

Esta tarefa tem uma única responsabilidade: esperar e processar mensagens vindas do Arduino pela `huart4`.

```c
void Task_ArduinoRx(void *argument)
{
  uint8_t buffer_arduino[64];
  uint8_t byte;
  uint8_t i;

  for(;;) // Loop infinito da tarefa
  {
    memset(buffer_arduino, 0, sizeof(buffer_arduino));
    i = 0;

    // Loop de recepção byte a byte
    while (i < sizeof(buffer_arduino) - 1)
    {
      if (HAL_UART_Receive(&huart4, &byte, 1, portMAX_DELAY) == HAL_OK)
      {
        if (byte == '\r') break;
        buffer_arduino[i++] = byte;
      }
    }

    // Retransmite a mensagem para o PC para depuração
    HAL_UART_Transmit(&huart2, (uint8_t *)"Recebido do Arduino: ", 22, HAL_MAX_DELAY);
    HAL_UART_Transmit(&huart2, buffer_arduino, i, HAL_MAX_DELAY);
    HAL_UART_Transmit(&huart2, (uint8_t *)"\r\n", 2, portMAX_DELAY);
    osDelay(1000);
  }
}
```

#### Análise Detalhada da Tarefa `Task_ArduinoRx`

-   `uint8_t buffer_arduino[64]`: Diferente do Arduino que usa o objeto `String` dinâmico, aqui usamos um array de bytes (`uint8_t`) de tamanho fixo (um buffer) para armazenar a mensagem. Esta é uma abordagem de mais baixo nível e mais comum em C para sistemas embarcados.
-   `for(;;)`: É um loop infinito, padrão para o corpo de uma tarefa de RTOS. A tarefa nunca deve "terminar".
-   `memset(buffer_arduino, 0, sizeof(buffer_arduino))`: Antes de cada nova leitura, esta função preenche o buffer com zeros para garantir que não haja lixo de mensagens anteriores.
-   `HAL_UART_Receive(&huart4, &byte, 1, portMAX_DELAY)`: Esta é a função mais importante aqui.
    -   `&huart4`: O manipulador da UART que queremos ouvir.
    -   `&byte`: O endereço da variável onde o byte recebido será armazenado.
    -   `1`: O número de bytes que queremos receber nesta chamada (apenas um).
    -   `portMAX_DELAY`: Esta constante do FreeRTOS instrui a tarefa a **bloquear** (ficar "dormindo") indefinidamente até que um byte chegue. Isso é extremamente eficiente, pois a tarefa não consome CPU enquanto espera. O RTOS pode usar esse tempo para executar outras tarefas.
-   `if (byte == '\r') break;`: O código do Arduino usa `println`, que termina com `\r\n`. Aqui, estamos tratando `\r` (carriage return) como o fim da mensagem.
-   `HAL_UART_Transmit(...)`: Envia dados pela UART. Neste caso, envia o prefixo "Recebido do Arduino: " e a mensagem contida no buffer para a `huart2` (conectada ao PC), para que possamos depurar o que o STM32 está recebendo.
-   `osDelay(1000)`: Pausa a **tarefa atual** por 1000 milissegundos (1 segundo). Durante esta pausa, o FreeRTOS com certeza executará outras tarefas.

### `void Task_PCRx(void *argument)` - Tarefa de Escuta do PC

Esta tarefa é quase idêntica à anterior, mas opera nos canais opostos: escuta o PC (`huart2`) e envia para o Arduino (`huart4`).

```c
void Task_PCRx(void *argument)
{
  // ... inicialização do buffer_pc ...
  for(;;)
  {
    // ... memset e loop de recepção ...
    while (i < sizeof(buffer_pc) - 1)
    {
        // Bloqueia esperando dados do PC na huart2
        if (HAL_UART_Receive(&huart2, &byte, 1, portMAX_DELAY) == HAL_OK)
        {
            if (byte == '\r') break;
            buffer_pc[i++] = byte;
        }
    }
    // ... Envia confirmação de volta para o PC ...

    // Envia a mensagem recebida para o Arduino via huart4
    HAL_UART_Transmit(&huart4, buffer_pc, i, portMAX_DELAY);
    HAL_UART_Transmit(&huart4, (uint8_t *)"\r\n", 2, portMAX_DELAY);

    osDelay(1000);
  }
}
```

A lógica é a mesma, mas a fonte dos dados é `huart2` e o destino é `huart4`. A confirmação enviada de volta para o PC é útil para saber que o STM32 recebeu a mensagem corretamente antes de repassá-la.

---

## Como Conectar e Usar

### Conexões Físicas

-   **Conecte as portas seriais de forma cruzada (essencial):**
    -   Pino **TX** (Transmissor) do STM32 (`TX` da `UART4`) → Pino **RX** (Receptor) do Arduino (`RX2`).
    -   Pino **RX** (Receptor) do STM32 (`RX` da `UART4`) → Pino **TX** (Transmissor) do Arduino (`TX2`).
-   **Conecte os terras (GND) para referência de tensão comum:**
    -   Pino `GND` do STM32 → Pino `GND` do Arduino.

### Passos para Execução

1.  **Carregue os Códigos:** Compile e envie cada código para sua respectiva placa.
2.  **Conecte ao PC:**
    -   Conecte o Arduino ao PC com um cabo USB.
    -   Conecte o STM32 ao PC com o cabo do ST-Link.
3.  **Abra os Terminais:**
    -   Na IDE do Arduino, abra o **Monitor Serial** (verifique a porta COM correta). A velocidade deve ser **9600 baud**.
    -   Abra um segundo programa de terminal (como PuTTY, Tera Term ou o terminal do STM32CubeIDE) e conecte-se à porta COM virtual do ST-Link, também a **9600 baud**.
4.  **Teste a Comunicação:**
    -   Digite algo no Monitor Serial do Arduino e pressione Enter. A mensagem deve aparecer no terminal do STM32.
    -   Digite algo no terminal do STM32 e pressione Enter. A mensagem deve aparecer no Monitor Serial do Arduino.
