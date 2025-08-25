# Controle de LED via I2C: Arduino Uno (Mestre) e STM32 (Escravo)

Este projeto demonstra uma comunicação I2C básica entre uma placa Arduino Uno, atuando como mestre (Master), e uma placa de desenvolvimento STM32, atuando como escrava (Slave). O Arduino envia comandos para ligar e desligar um LED conectado ao STM32.

## Visão Geral do Projeto

-   **Arduino Uno (Mestre):** Envia periodicamente os caracteres `'1'` (para ligar) e `'0'` (para desligar) através do barramento I2C.
-   **STM32 (Escravo):** Fica em modo de escuta na rede I2C. Ao receber um comando do mestre, ele interpreta o caractere e aciona um pino de GPIO para controlar um LED.

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

Utilize o conversor de nível lógico para conectar as linhas I2C (SDA e SCL).

| Arduino Uno (5V) | Conversor (Lado HV) | Conversor (Lado LV) | STM32 (3.3V)      |
| :--------------- | :------------------ | :------------------ | :---------------- |
| **GND** | GND                 | GND                 | **GND** |
| **5V** | HV                  | -                   | -                 |
| -                | -                   | LV                  | **3V3** |
| **A4 (SDA)** | HV1 (SDA)           | LV1 (SDA)           | **PB9 (I2C1_SDA)**|
| **A5 (SCL)** | HV2 (SCL)           | LV2 (SCL)           | **PB8 (I2C1_SCL)**|


### Observação sobre Resistores de Pull-up
O protocolo I2C requer que as linhas de dados (SDA) e clock (SCL) tenham resistores de pull-up.
* **Pull-ups Internos (STM32):** A maneira mais fácil é habilitar os resistores de pull-up internos do próprio STM32. No STM32CubeIDE, ao configurar os pinos I2C (`PB9` e `PB8`), vá até as configurações de GPIO e certifique-se de que o modo "Pull-up" está selecionado.
* **Pull-ups Externos:** Se a comunicação for instável ou se os pull-ups internos não forem suficientes, você pode adicionar resistores externos (geralmente entre 4.7kΩ e 10kΩ) conectando as linhas SDA e SCL à sua respectiva fonte de tensão (5V no lado do Arduino e 3.3V no lado do STM32, no conversor de nível lógico).

---

## Configuração e Uso

### 1. Código do STM32 (Escravo)

1.  Abra o projeto no STM32CubeIDE. O arquivo `main.c` fornecido já contém toda a lógica.
2.  **Endereço I2C:** No arquivo `main.c`, dentro da função `MX_I2C1_Init()`, o endereço do escravo é definido por:
    ```c
    hi2c1.Init.OwnAddress1 = 84;
    ```
    O valor `84` (decimal) corresponde ao endereço de 7 bits `0x2A` (hexadecimal) já deslocado. A biblioteca HAL do STM32 espera este formato. O endereço que o mestre (Arduino) deve usar é `0x2A`.
3.  **Pino do LED:** O código está configurado para usar o pino `PA5`, que em muitas placas Nucleo já está conectado ao LED do usuário.
4.  Compile o projeto e grave o firmware na placa STM32.

### 2. Código do Arduino Uno (Mestre)

1.  Abra o código do mestre na Arduino IDE. O código fornecido para o ESP32 funciona sem modificações no Arduino Uno, pois ambos usam a mesma biblioteca `Wire.h`.
2.  **Endereço do Escravo:** Verifique se o endereço no código corresponde ao do STM32.
    ```cpp
    const byte ENDERECO_ESCRAVO = 0x2A; // Deve corresponder ao endereço do STM32
    ```
3.  Selecione "Arduino Uno" e a porta COM correspondente no menu `Ferramentas`.
4.  Carregue o código para o Arduino.

### 3. Execução

1.  Com ambos os dispositivos programados e as conexões feitas, energize as placas.
2.  Abra o **Monitor Serial** na Arduino IDE (Baud Rate: `115200`).
3.  Você verá as mensagens "Comando '1' (LIGAR) enviado." e "Comando '0' (DESLIGAR) enviado." a cada 2 segundos.
4.  Simultaneamente, o LED conectado ao STM32 deverá acender e apagar, seguindo os comandos do Arduino.

---

## Explicação dos Códigos

### Arduino Uno (Mestre - `Arduino/Wire.h`)

-   `Wire.begin()`: Inicia o barramento I2C, configurando o Arduino como mestre.
-   `Wire.beginTransmission(ENDERECO_ESCRAVO)`: Inicia a comunicação com o dispositivo escravo no endereço especificado.
-   `Wire.write('1')`: Coloca o byte (caractere '1') no buffer para ser enviado.
-   `Wire.endTransmission()`: Envia os dados do buffer para o escravo e finaliza a transmissão.
-   O `loop()` simplesmente alterna entre enviar `'1'` e `'0'` com um intervalo de 2 segundos.

### STM32 (Escravo - `STM32 HAL`)

-   `MX_I2C1_Init()`: Função gerada pelo CubeMX/IDE que configura o periférico I2C1 em modo escravo com o endereço definido.
-   `MX_GPIO_Init()`: Configura o pino `PA5` como saída para controlar o LED. É nesta etapa que os pull-ups dos pinos I2C também são configurados.
-   `HAL_I2C_Slave_Receive(&hi2c1, i2c_rx_buffer, 1, HAL_MAX_DELAY)`: Esta é a função principal. Ela coloca o STM32 em modo de escuta, aguardando receber dados do mestre. O programa fica bloqueado nesta linha até que `1` byte seja recebido.
-   Após receber o byte, o código verifica se o caractere é `'1'` ou `'0'` e usa `HAL_GPIO_WritePin()` para ligar (`GPIO_PIN_SET`) ou desligar (`GPIO_PIN_RESET`) o LED.
