# Projeto UART "Hello World" para STM32 📡

Este é um projeto básico que demonstra como enviar dados de um microcontrolador STM32 para um computador através da comunicação serial **UART** (Universal Asynchronous Receiver-Transmitter). O programa envia repetidamente a string "STM32" a cada segundo.

É o equivalente a um "Hello, World\!" para a comunicação serial em sistemas embarcados.

## Hardware Necessário

  * **Placa de Desenvolvimento STM32 Nucleo:** A maioria das placas Nucleo conecta a `USART2` ao programador/depurador ST-LINK embarcado, que cria uma **Porta COM Virtual** no seu computador. Isso elimina a necessidade de um conversor USB-Serial externo.
  * **Cabo USB:** Para alimentar, programar a placa e para a comunicação serial.
  * **Computador:** Com um software de terminal serial instalado (ex: PuTTY, Tera Term, Monitor Serial da IDE do Arduino, etc.).

## Funcionalidade do Código

O programa executa uma sequência simples de inicialização e entra em um loop infinito.

1.  **Inicialização:**

      * O clock do sistema é configurado para usar o oscilador interno (HSI).
      * O periférico `USART2` é inicializado com os seguintes parâmetros:
          * **Baud Rate (Velocidade):** 115200 bps
          * **Bits de dados:** 8
          * **Paridade:** Nenhuma
          * **Bits de parada:** 1
          * (Configuração padrão conhecida como **115200 8N1**)

2.  **Loop Infinito (`while(1)`):**

      * A função `HAL_UART_Transmit()` é chamada para enviar a string `"STM32\r\n"` através da UART.
          * `\r\n` são caracteres de controle que significam "retorno de carro" e "nova linha", fazendo com que o texto pule para a linha seguinte no terminal.
      * A função `HAL_Delay(1000)` cria uma pausa de 1 segundo.
      * O processo se repete, enviando a mensagem a cada segundo.

-----

## Como Visualizar a Saída

Para ver a mensagem "STM32" sendo impressa, siga estes passos:

1.  **Compile e Carregue o Código:** Use o STM32CubeIDE para carregar o programa na sua placa Nucleo.

2.  **Identifique a Porta COM:**

      * Com a placa conectada ao PC, abra o **Gerenciador de Dispositivos** do Windows.
      * Procure na seção **"Portas (COM e LPT)"**.
      * Você deverá ver um dispositivo chamado "STMicroelectronics STLink Virtual COM Port (COMx)". **Anote o número da porta COM** (ex: COM3, COM4, etc.).

3.  **Abra um Terminal Serial:** Inicie o seu programa de terminal preferido (neste exemplo, usaremos o PuTTY).

4.  **Configure a Conexão:**

      * **Serial line:** A porta que você anotou (ex: `COM4`).
      * **Speed:** `115200`.
      * Certifique-se de que o tipo de conexão é **"Serial"**.

5.  **Inicie a Conexão:** Clique em "Open".

6.  **Resultado:** Uma janela de terminal preta se abrirá. Você verá a mensagem **"STM32"** aparecendo em uma nova linha a cada segundo. 🎉

## Análise da Função Principal

A linha mais importante do código é:

```c
HAL_UART_Transmit(&huart2, (uint8_t*)"STM32\r\n", 7, 100);
```

  * `&huart2`: É o ponteiro para a estrutura de controle da UART2 que foi inicializada.
  * `(uint8_t*)"STM32\r\n"`: É o ponteiro para os dados que queremos enviar. O `(uint8_t*)` é um *cast* para garantir que o tipo de dado seja o esperado pela função (um array de bytes).
  * `7`: É o número de bytes a serem enviados ('S', 'T', 'M', '3', '2', '\\r', '\\n').
  * `100`: É o valor do *timeout* em milissegundos. Se a transmissão não for possível dentro de 100ms, a função retornará um erro.