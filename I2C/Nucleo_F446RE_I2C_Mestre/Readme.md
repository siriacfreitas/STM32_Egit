# Controle de LED via I2C: STM32 (Mestre) e Arduino Uno (Escravo)

Este projeto demonstra uma comunicação I2C básica entre uma placa de desenvolvimento STM32, atuando como mestre (Master), e uma placa Arduino Uno, atuando como escrava (Slave). O STM32 envia comandos para ligar e desligar o LED embutido do Arduino.

## Visão Geral do Projeto

-   **STM32 (Mestre):** Envia periodicamente os caracteres `'1'` (para ligar) e `'0'` (para desligar) através do barramento I2C.
-   **Arduino Uno (Escravo):** Fica em modo de escuta na rede I2C. Ao receber um comando do mestre, ele interpreta o caractere e aciona o LED embutido (pino 13).

---

## Hardware Necessário

* 1x Placa de desenvolvimento Arduino Uno
* 1x Placa de desenvolvimento STM32 (ex: Nucleo, Blue Pill)
* 1x **Conversor de Nível Lógico Bidirecional (Essencial!)**
* Protoboard e Jumpers

> ### ⚠️ **Aviso Importante sobre Níveis de Tensão**
> O Arduino Uno opera com níveis lógicos de **5V**, enquanto muitas placas STM32 (especialmente as mais modernas como as da linha Nucleo) operam com **3.3V**. Para garantir uma comunicação I2C estável e segura, **é obrigatório o uso de um conversor de nível lógico bidirecional** entre os dois dispositivos. Conectar diretamente os pinos I2C pode danificar a sua placa STM32.

---

## Montagem e Conexões

Utilize o conversor de nível lógico para conectar as linhas I2C (SDA e SCL). A montagem física é a mesma, mas os papéis de mestre e escravo estão invertidos.

| STM32 (Mestre, 3.3V) | Conversor (Lado LV) | Conversor (Lado HV) | Arduino Uno (Escravo, 5V) |
| :------------------- | :------------------ | :------------------ | :------------------------ |
| **GND** | GND                 | GND                 | **GND** |
| **3V3** | LV                  | -                   | -                         |
| -                    | -                   | HV                  | **5V** |
| **PB9 (I2C1_SDA)** | LV1 (SDA)           | HV1 (SDA)           | **A4 (SDA)** |
| **PB8 (I2C1_SCL)** | LV2 (SCL)           | HV2 (SCL)           | **A5 (SCL)** |

* **Conexão do LED:** Este projeto utiliza o LED embutido do Arduino Uno, conectado internamente ao pino 13. Nenhuma conexão de LED externa é necessária.

### Observação sobre Resistores de Pull-up
O protocolo I2C requer que as linhas de dados (SDA) e clock (SCL) tenham resistores de pull-up.
* **Pull-ups Internos (STM32):** A maneira mais fácil é habilitar os resistores de pull-up internos do próprio STM32 (o mestre). No STM32CubeIDE, ao configurar os pinos I2C (`PB8` e `PB9`), vá até as configurações de GPIO e certifique-se de que o modo "Pull-up" está selecionado.
* **Pull-ups Externos:** Se a comunicação for instável, você pode adicionar resistores externos (geralmente entre 4.7kΩ e 10kΩ) conectando as linhas SDA e SCL à sua respectiva fonte de tensão (3.3V no lado do STM32 e 5V no lado do Arduino, no conversor de nível lógico).

---

## Configuração e Uso

### 1. Código do Arduino Uno (Escravo)

1.  Abra o código do escravo na Arduino IDE.
2.  **Endereço do Escravo:** Verifique se o endereço no código está correto. Este é o endereço que o Arduino usará para se identificar na rede.
    ```cpp
    const byte ENDERECO_ESCRAVO = 0x2A;
    ```
3.  Selecione "Arduino Uno" e a porta COM correspondente no menu `Ferramentas`.
4.  Carregue o código para o Arduino.

### 2. Código do STM32 (Mestre)

1.  Abra o projeto no STM32CubeIDE. O arquivo `main.c` fornecido já contém a lógica do mestre.
2.  **Endereço do Escravo:** No código, o endereço do escravo é definido e deslocado para a esquerda (`<< 1`), pois a biblioteca HAL espera um endereço de 8 bits (7 bits do endereço + 1 bit de leitura/escrita).
    ```c
    #define I2C_SLAVE_ADDRESS 0x2A << 1
    ```
3.  Compile o projeto e grave o firmware na placa STM32.

### 3. Execução

1.  Com ambos os dispositivos programados e as conexões feitas, energize as placas.
2.  Abra o **Monitor Serial** na Arduino IDE (Baud Rate: `9600`).
3.  Você verá as mensagens "Comando recebido: 1" e "Comando recebido: 0" sendo impressas a cada 2 segundos.
4.  Simultaneamente, o LED embutido (L) do Arduino Uno deverá acender e apagar, seguindo os comandos enviados pelo STM32.

---

## Explicação dos Códigos

### STM32 (Mestre - `STM32 HAL`)

-   `MX_I2C1_Init()`: Configura o periférico I2C1 em modo mestre.
-   `HAL_I2C_Master_Transmit(&hi2c1, I2C_SLAVE_ADDRESS, &cmd, 1, HAL_MAX_DELAY)`: Esta é a função principal. Ela envia os dados para o barramento I2C.
    -   `&hi2c1`: Ponteiro para a configuração do I2C.
    -   `I2C_SLAVE_ADDRESS`: Endereço do escravo para o qual os dados serão enviados.
    -   `&cmd`: Ponteiro para o dado a ser enviado.
    -   `1`: Número de bytes a serem enviados.
    -   `HAL_MAX_DELAY`: Timeout da operação.
-   O `while(1)` simplesmente alterna entre enviar `'1'` e `'0'` com um intervalo de 2 segundos usando `HAL_Delay()`.

### Arduino Uno (Escravo - `Arduino/Wire.h`)

-   `pinMode(LED_PIN, OUTPUT)`: Configura o pino do LED embutido como saída.
-   `Wire.begin(ENDERECO_ESCRAVO)`: Inicia o barramento I2C com um endereço específico, configurando o Arduino como escravo.
-   `Wire.onReceive(receiveEvent)`: Registra uma função de callback (`receiveEvent`). Esta função será chamada automaticamente por uma interrupção sempre que o mestre enviar dados para este endereço de escravo.
-   `receiveEvent(int howMany)`: Dentro desta função, `Wire.available()` verifica se há dados para ler e `Wire.read()` lê o byte recebido. O código então interpreta o comando e usa `digitalWrite()` para controlar o LED.
