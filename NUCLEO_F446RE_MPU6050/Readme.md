# Leitor de Sensor MPU6050 com STM32 🛰️

Este projeto demonstra a leitura de dados de um sensor de movimento **MPU6050** (acelerômetro, giroscópio e temperatura) utilizando um microcontrolador da família STM32. A comunicação com o sensor é feita via **I2C**, e os dados coletados são enviados para um computador através da comunicação **UART** para visualização em um terminal serial.

## ✨ Funcionalidades

  * **Verificação do Barramento I2C:** Ao iniciar, o código escaneia o barramento I2C para garantir que o MPU6050 está conectado e respondendo corretamente.
  * **Configuração do Sensor:**
      * Define o alcance do **acelerômetro** para **±2g**.
      * Define o alcance do **giroscópio** para **±250 graus por segundo (°/s)**.
  * **Leitura Contínua de Dados:** Em um loop infinito, o programa lê e processa os dados do sensor a cada 2 segundos.
  * **Envio via UART:** Os dados lidos são formatados e enviados para o monitor serial, incluindo:
      * **Aceleração** nos eixos X, Y e Z, convertida para a unidade de aceleração da gravidade (g).
      * **Temperatura** do sensor em graus Celsius (°C).
      * **Velocidade angular** nos eixos X, Y e Z, convertida para centi-graus por segundo (cdps).

-----

## 🛠️ Hardware Necessário

  * Uma placa de desenvolvimento STM32 (ex: Nucleo, Blue Pill).
  * Módulo com sensor MPU6050.
  * Jumpers para conectar o sensor à placa.
  * Um cabo USB para conectar a placa STM32 ao computador.

### Conexões (Pinagem)

Conecte o MPU6050 à sua placa STM32 usando os pinos I2C. A pinagem pode variar, mas para uma configuração padrão, seria:

| MPU6050 | STM32                         | Descrição          |
| :------ | :---------------------------- | :----------------- |
| **VCC** | 5V ou 3.3V                    | Alimentação        |
| **GND** | GND                           | Terra              |
| **SCL** | `PB6` ou pino SCL do I2C1     | Linha de Clock I2C |
| **SDA** | `PB7` ou pino SDA do I2C1     | Linha de Dados I2C |

-----

## 🚀 Como Usar

1.  **Montagem:** Conecte o hardware conforme a tabela de pinagem acima.
2.  **Compilação:** Compile o projeto utilizando o STM32CubeIDE ou sua toolchain de preferência.
3.  **Gravação:** Grave o firmware na placa STM32.
4.  **Monitoramento:** Abra um monitor serial (ex: PuTTY, Tera Term, ou o terminal integrado do VS Code) e configure-o para a porta COM correspondente à sua placa com as seguintes configurações:
      * **Baud Rate:** 115200
      * **Data Bits:** 8
      * **Parity:** None
      * **Stop Bits:** 1
5.  **Visualização:** Reinicie a placa. Você deverá ver os dados do sensor sendo impressos no terminal a cada 2 segundos.

### Exemplo de Saída no Terminal

```
----------------------------------------
g: 0.012 -0.004 1.015
Temp: 28.45 C
cdps:    -51     23    -26
----------------------------------------
----------------------------------------
g: 0.011 -0.005 1.014
Temp: 28.48 C
cdps:    -50     24    -27
----------------------------------------
```

-----

## 👨‍💻 Análise Detalhada do Código

O código-fonte é modular, dividido em três arquivos principais que separam as responsabilidades do projeto.

### 1\. Driver do Sensor (`mpu6050.c`) 🧠

Este arquivo contém a lógica principal para se comunicar com o sensor MPU6050. Ele "traduz" ações de alto nível (como "ler aceleração") em comandos de baixo nível (escrever/ler em registradores I2C).

#### API do Acelerômetro 🎢

  * `MPU6050_SetAccelRange_2g`

      * **O que faz:** Configura o acelerômetro para operar na sua faixa de menor sensibilidade: **±2g**.
      * **Como funciona:** Escreve o valor `0x00` no registrador `MPU6050_REG_ACCEL_CONFIG` do sensor. De acordo com o *datasheet* do MPU6050, isso seleciona a faixa de ±2g.

  * `MPU6050_ReadAccelRaw`

      * **O que faz:** Lê os valores "crus" (raw) do acelerômetro para os três eixos (X, Y, Z).
      * **Como funciona:** Pede ao sensor para ler **6 bytes** de memória sequencialmente. O sensor retorna os valores de cada eixo como dois bytes separados (High Byte e Low Byte), que são combinados em um único número de 16 bits através de operações de deslocamento de bits (`<< 8`) e OU (`|`).

  * `MPU6050_AccelRawTo_g_2g`

      * **O que faz:** Converte os valores crus em uma unidade física: **g** (aceleração da gravidade).
      * **Como funciona:** Divide o valor cru por `16384.0`. Este número é o **fator de sensibilidade** para a faixa de ±2g, obtido do *datasheet* do sensor.

#### API da Temperatura 🌡️

  * `MPU6050_ReadTemp_centiC`
      * **O que faz:** Lê a temperatura e a converte para **centésimos de grau Celsius** (ex: `2534` =\> `25.34 °C`).
      * **Como funciona:** É uma implementação inteligente que evita matemática de ponto flutuante. Ela aplica a fórmula de conversão do *datasheet* (`Temp(°C) = (Valor_Cru / 340) + 36.53`) usando apenas operações com números inteiros, o que é muito mais eficiente em microcontroladores.

#### API do Giroscópio 🔄

  * As funções do giroscópio (`Set...Range`, `Read...Raw`, `...RawTo...`) seguem **exatamente a mesma lógica** do acelerômetro, mas com valores diferentes:
      * O registrador de configuração é o `MPU6050_REG_GYRO_CONFIG`.
      * O fator de sensibilidade para a faixa de **±250 graus por segundo (dps)** é **131 LSB/(°/s)**.
      * A conversão resulta em **centi-graus por segundo**.

### 2\. Verificação do I2C e Sensor (`i2c_scanner.c`) 🔍

Este arquivo contém funções de ajuda para garantir que a comunicação e o sensor estão funcionando corretamente antes de iniciar a leitura dos dados.

  * `I2C_ScanBus`

      * **O que faz:** Verifica se há um dispositivo respondendo no endereço I2C padrão do MPU6050 (`0x68`). É um "ping" para ver se o sensor está conectado e funcional.
      * **Como funciona:** Usa a função `HAL_I2C_IsDeviceReady`, que envia o endereço do dispositivo e espera por um sinal de confirmação (ACK).

  * `MPU6050_Check`

      * **O que faz:** Realiza duas verificações cruciais específicas do MPU6050.
      * **Como funciona:**
        1.  **Lê o registrador `WHO_AM_I`**: Este registrador deve retornar `0x68`, confirmando que o dispositivo é de fato um MPU6050.
        2.  **"Acorda" o sensor**: Escreve `0x00` no registrador de gerenciamento de energia (`PWR_MGMT_1`) para tirar o MPU6050 do modo de baixo consumo em que ele pode iniciar.

### 3\. Utilitários de UART (`uart.c`) ✉️

Este arquivo é uma biblioteca de conveniência (*wrapper*) que torna o envio de dados pela UART muito mais simples e legível.

  * `UART_SendStr` e `UART_SendStrln`

      * **O que faz:** Funções simples para enviar uma string. A versão `ln` (`line`) adiciona uma quebra de linha (`\r\n`) automaticamente.

  * `UART_Printf` e `UART_Printfln`

      * **O que faz:** Permite enviar texto formatado, como a função `printf` da linguagem C, usando especificadores como `%d` e `%s`.
      * **Como funciona:** Usa `vsnprintf` para formatar a string em um buffer de memória e depois a envia pela UART.

  * `UART_SendFloat3`

      * **O que faz:** Envia um número de ponto flutuante com 3 casas decimais.
      * **Por que existe?** Em sistemas embarcados, habilitar o suporte completo a `%f` no `printf` pode aumentar significativamente o tamanho do programa. Esta função é uma alternativa leve e eficiente que evita esse problema, separando a parte inteira da fracionária e imprimindo-as com matemática de inteiros.

-----

## 📄 Licença

Este software é licenciado sob os termos que podem ser encontrados no arquivo LICENSE no diretório raiz do componente de software. Se nenhum arquivo LICENSE vier com este software, ele será fornecido **AS-IS**.

**Copyright (c) 2025 STMicroelectronics. Todos os direitos reservados.**