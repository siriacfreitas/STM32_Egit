# Projeto ADC com Cálculo em Ponto Flutuante (Float) 🎛️

Este projeto demonstra como utilizar o conversor analógico-digital (ADC) de um microcontrolador STM32 para ler uma tensão, converter o valor para um número inteiro e, em seguida, usar matemática de ponto flutuante (`float`) para calcular a tensão real em Volts.

Como o código não envia dados para fora (via UART, por exemplo), a visualização das variáveis `adc` (valor digital) e `adc_f` (tensão em float) é feita exclusivamente através do **modo Debug** do STM32CubeIDE.

## Hardware Necessário

* **Placa de Desenvolvimento STM32:** Qualquer placa da família Nucleo.
* **Cabo USB:** Para alimentar, programar e depurar a placa.
* **Potenciômetro:** (ex: 10kΩ) para fornecer uma tensão analógica variável.

### Montagem

Conecte o potenciômetro da seguinte forma para fornecer uma tensão variável entre 0V e 3.3V ao pino `PA0` (ADC1_IN0):
* **Pino central do potenciômetro:** Conecte ao pino **`PA0`** da placa Nucleo.
* **Um pino lateral:** Conecte ao **`3V3`** da placa.
* **O outro pino lateral:** Conecte ao **`GND`** da placa.

<div align="center">
<img src="https://github.com/user-attachments/assets/cfd491c7-fa5d-4b6b-9579-6e3c83fe4360" width="600px" />
</div>


## Funcionalidade do Código

O programa executa um loop contínuo que realiza as seguintes tarefas a cada 500ms:

1.  **Inicia a Conversão:** O ADC, configurado para **10 bits de resolução**, é ativado.
2.  **Lê o Valor Digital:** O programa espera a conversão terminar e lê o resultado. Este é um número inteiro entre **0** (para 0V) e **1023** (para 3.3V). O valor é armazenado na variável `uint16_t adc`.
3.  **Calcula a Tensão (Ponto Flutuante):** O valor digital é convertido para a tensão correspondente usando a seguinte operação de ponto flutuante:
    ```c
    adc_f = adc * 0.00322580;
    ```
Este "número mágico" é o fator de conversão, calculado como:

Fator = VREF / (2^N - 1)
= 3.3 / (2^10 - 1)
= 3.3 / 1023
≈ 0.0032258

O resultado é armazenado na variável `float adc_f`.

4. **Para a Conversão:** O ADC é desligado para economizar energia até o próximo ciclo.

---

## Como Habilitar o Suporte a `float` no `printf`

Embora este código não use a função `printf`, é uma etapa crucial em projetos que precisam imprimir valores `float` em um terminal serial. Para economizar memória, o STM32CubeIDE, por padrão, usa uma versão do `printf` que não suporta números de ponto flutuante.

Para habilitar o suporte completo:

1.  Clique com o botão direito no seu projeto na aba "Project Explorer" e vá em **Properties**.
2.  Navegue até **C/C++ Build** -> **Settings**.
3.  Vá para a aba **Tool Settings** -> **MCU GCC Linker** -> **Miscellaneous**.
4.  No campo "Other flags", adicione a seguinte flag:
    ```
    -u _printf_float
    ```
5.  Clique em "Apply and Close" e recompile o projeto.

---

## Visualizando os Resultados com o Debugger 🐞

Como não há saída de dados, o debugger é a nossa janela para o microcontrolador.

1.  **Conecte a placa** ao computador.
2.  Clique no ícone de **Debug** (o inseto verde 🐞) na barra de ferramentas para compilar e carregar o código no modo de depuração. O IDE mudará para a perspectiva "Debug".
3.  O programa estará pausado no início da função `main()`. Vá para o menu **Window** -> **Show View** -> **Live Expressions**.
4.  Na janela "Expressions" que aparecer, clique em **"Add new expression"** e digite `adc`. Pressione Enter.
5.  Clique novamente em **"Add new expression"** e digite `adc_f`. Pressione Enter.
6.  Clique no ícone **Resume** (parecido com um play/pause) ou pressione **F8** para deixar o código rodar livremente.

Agora, você verá os valores das variáveis `adc` e `adc_f` sendo atualizados na janela "Expressions" a cada 500ms. Gire o potenciômetro e observe como os valores mudam em tempo real!

<div align="center">
<img src="https://github.com/user-attachments/assets/ac95ab29-7061-44b0-b153-62ae62eb2d83" width="600px" />
</div>
