# Entendendo o ADC: Traduzindo o Mundo Real para o Microcontrolador 📈

## 1. O que é um ADC?

**ADC** é a sigla para **Analog-to-Digital Converter**, ou **Conversor Analógico-Digital**. Ele é um dos periféricos mais importantes de um microcontrolador, atuando como uma ponte entre o mundo analógico (o mundo real) e o mundo digital (o microcontrolador).

Pense no ADC como um **tradutor universal**:

  * O **mundo real** é cheio de sinais analógicos: grandezas contínuas que podem ter infinitos valores dentro de uma faixa, como a temperatura de uma sala, a intensidade da luz, o volume de um som ou a posição de um botão giratório (potenciômetro).
  * O **microcontrolador** só entende o mundo digital: números discretos, compostos por zeros e uns.

O trabalho do ADC é "olhar" para uma tensão analógica em um determinado instante e convertê-la para um número digital que o microcontrolador possa entender e processar.

## 2. Por que Precisamos de um ADC?

Se você quer que seu projeto interaja com o ambiente de forma mais rica do que apenas ligar e desligar coisas, você precisará de um ADC.

**Exemplos de uso:**

  * **Ler um Potenciômetro:** Para controlar o brilho de um LED ou a velocidade de um motor.
  * **Medir Temperatura:** Usando sensores como o LM35 ou um termistor.
  * **Medir Luminosidade:** Com um sensor LDR (Resistor Dependente de Luz).
  * **Medir Nível de Bateria:** Lendo a tensão que a bateria fornece.
  * **Processar Áudio:** Capturando o sinal de um microfone.

Sem um ADC, o microcontrolador só consegue saber se um pino está em `HIGH` ou `LOW` (GPIO), mas não consegue saber os "infinitos" valores intermediários.

## 3. Características Chave de um ADC

### 3.1. Resolução (em bits)

A resolução define a **precisão** do ADC. Ela é medida em bits e determina em quantos "degraus" ou "passos" o ADC pode dividir a tensão que está lendo.

A fórmula é simples: um ADC de **N** bits pode representar **2^N** níveis diferentes.

  * **ADC de 8 bits:** 2^8 = 256 níveis (valores de 0 a 255).
  * **ADC de 10 bits:** 2^10 = 1024 níveis (valores de 0 a 1023).
  * **ADC de 12 bits (comum em STM32):** 2^12 = 4096 níveis (valores de 0 a 4095).

**Analogia:** Pense em uma régua. Uma régua marcada apenas com centímetros (baixa resolução) é menos precisa do que uma régua marcada com milímetros (alta resolução). Da mesma forma, um ADC de 12 bits é mais preciso que um de 10 bits.

### 3.2. Tensão de Referência (VREF)

A tensão de referência é o **valor máximo da escala** de medição do ADC. O conversor medirá qualquer tensão entre **0V** (GND) e **VREF**.

  * O valor digital **0** corresponderá a 0V.
  * O valor digital **máximo** (ex: 4095 para 12 bits) corresponderá a VREF.

Em muitas placas STM32, a VREF é conectada à própria tensão de alimentação, que geralmente é **3.3V**.

### 3.3. A Conversão: Do Digital de Volta para a Tensão

O ADC lhe dará um número. Para que esse número tenha significado, você precisa convertê-lo de volta para uma tensão. A fórmula é:

```
Tensão = (Valor_Lido_ADC / (2^Resolução - 1)) * VREF
```

**Exemplo prático (STM32 com ADC de 12 bits e VREF = 3.3V):**
Se o ADC leu o valor digital **2048**:

```
Tensão = (2048 / 4095) * 3.3V ≈ 1.65V
```

## 4. O Passo a Passo no STM32CubeIDE (HAL)

Para usar o ADC em um projeto STM32, você geralmente segue estes passos:

1.  **Habilitar o Clock do ADC:** Assim como qualquer periférico, o ADC precisa ter seu clock ativado.
2.  **Configurar o Pino GPIO:** O pino que você usará para a leitura (ex: `PA0`) deve ser configurado no modo **Analog Mode**. Isso o desconecta das funções digitais e o conecta ao circuito do ADC.
3.  **Configurar o Periférico ADC:** Definir a resolução, modo de conversão (única, contínua), alinhamento dos dados, etc.
4.  **Calibrar o ADC:** Antes do primeiro uso, é uma ótima prática executar uma rotina de calibração para aumentar a precisão das leituras. A biblioteca HAL oferece uma função para isso.
5.  **Iniciar a Conversão e Ler o Valor:**
      * `HAL_ADC_Start()`: Inicia o processo de conversão.
      * `HAL_ADC_PollForConversion()`: Espera a conversão terminar.
      * `HAL_ADC_GetValue()`: Pega o valor digital resultante.

### Exemplo de Código: Lendo um Potenciômetro

```c
// main.c
// Supondo que o ADC1 já foi configurado pelo CubeIDE
// e um potenciômetro está conectado ao pino PA0.

uint32_t adc_valor;
float tensao;

// ... dentro do while(1) do main() ...
while (1)
{
  // 1. Inicia a conversão do ADC
  HAL_ADC_Start(&hadc1);

  // 2. Espera a conversão ser concluída (com um timeout)
  HAL_ADC_PollForConversion(&hadc1, 100);

  // 3. Pega o valor digital (0-4095)
  adc_valor = HAL_ADC_GetValue(&hadc1);

  // 4. (Opcional) Converte o valor para tensão
  tensao = (adc_valor / 4095.0) * 3.3;

  // 5. Para a conversão para economizar energia
  HAL_ADC_Stop(&hadc1);
  
  HAL_Delay(500); // Espera meio segundo antes da próxima leitura
}
```

## 5. Boas Práticas

  * **Sempre Calibre:** A calibração remove pequenos desvios (offsets) do circuito interno, melhorando muito a precisão.
  * **Fonte de Tensão Estável:** O ADC é tão preciso quanto sua tensão de referência. Se a sua VREF (geralmente a alimentação de 3.3V) for ruidosa, suas leituras também serão.
  * **Consulte o Datasheet:** O datasheet é a fonte de verdade para saber quais pinos podem ser usados pelo ADC, os tempos de conversão e outras características avançadas.
  * **Para Leituras Rápidas:** Para aplicações que precisam de muitas leituras por segundo (como áudio), use o ADC com **DMA (Direct Memory Access)**. Isso permite que o ADC salve os resultados diretamente na memória sem sobrecarregar o processador.
