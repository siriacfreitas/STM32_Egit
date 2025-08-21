# Projeto: Debounce de Botão com HAL\_GetTick() ⏱️⚡

Este projeto demonstra uma técnica essencial em programação de microcontroladores: como lidar com a leitura de botões de forma não-bloqueante usando o temporizador do sistema (`HAL_GetTick()`).

A técnica implementada serve para tratar o efeito de **debounce** (filtrar ruídos mecânicos do botão) e para criar uma ação contínua e controlada enquanto um botão é mantido pressionado, sem usar a função `HAL_Delay()`.

## Hardware Necessário

  * **Placa de Desenvolvimento STM32:** Qualquer placa da família Nucleo com um LED de usuário (`LD2`) e um botão (`B1`).
  * **Cabo USB:** Para alimentar e programar a placa.

-----

## Funcionalidade

O programa não faz nada até que o botão do usuário (`B1`) seja pressionado. Ao **pressionar e segurar o botão `B1`**, o LED `LD2` começará a piscar em altíssima velocidade. Para o olho humano, o LED pode parecer estar aceso continuamente, mas com um brilho mais fraco.

### Lógica do Código

A mágica acontece dentro do `while(1)`, onde o estado do botão é verificado continuamente.

```c
if(!HAL_GPIO_ReadPin(GPIOC, B1_Pin)){ // Se o botão estiver pressionado...
    tempo = HAL_GetTick(); // Pega o tempo atual em milissegundos

    // Verifica se já se passaram mais de 10ms desde a última ação
    if(tempo - ultimo_tempo > 10){
        HAL_GPIO_TogglePin(GPIOA, LD2_Pin); // Pisca o LED
        conta++; // Incrementa um contador para debug
    }

    ultimo_tempo = tempo; // Atualiza o tempo da última verificação
}
```

  * **`HAL_GetTick()`:** Esta função retorna o número de milissegundos desde que o microcontrolador foi ligado. Funciona como um cronômetro.
  * **`if(tempo - ultimo_tempo > 10)`:** Esta é a condição principal. Ela garante que a lógica de piscar o LED só seja executada se já tiverem passado mais de **10 milissegundos** desde a última vez que este bloco foi acessado. Isso cria um intervalo mínimo entre as ações, limitando a frequência com que o LED pisca.
  * **`conta++`:** É uma variável simples que incrementa rapidamente. Ela não tem efeito no programa, mas é muito útil para ser observada no modo **Debug** do STM32CubeIDE para confirmar que o código está sendo executado conforme o esperado.

-----

## O que é "Debounce"?

Mecanicamente, quando um botão é pressionado, seus contatos metálicos vibram por alguns instantes, gerando múltiplos sinais de "pressionado" e "solto" em uma fração de segundo. Isso é chamado de **bouncing**. O código, ao esperar um intervalo de 10ms, ignora essas leituras falsas e garante que apenas um evento seja processado de forma estável.

-----

## Configuração do Botão (Pull-up Interno)

Nesta versão, o pino do botão (`B1_Pin`) foi configurado com a opção `GPIO_PULLUP`.

Isso ativa o **resistor de pull-up interno** do próprio microcontrolador. Essa é uma prática recomendada, pois torna o projeto mais robusto e autossuficiente, não dependendo de componentes externos para garantir que o pino tenha um estado lógico **ALTO** quando o botão está solto.

-----

## Como Usar

1.  **Pré-requisitos:** Tenha o [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html) instalado.
2.  **Importar o Projeto:** Abra o STM32CubeIDE e importe o projeto.
3.  **Compilar:** Clique no ícone de "Build" (martelo) 🔨.
4.  **Carregar:** Clique no ícone de "Run" (play verde) ▶️ para carregar o firmware.
5.  **Testar:** Pressione e segure o botão azul (`B1`). Você verá o LED verde (`LD2`) piscar muito rápido, parecendo estar aceso com menos brilho. Solte o botão e o LED se apagará.
6.  **(Opcional) Debugar:**
      * Execute o programa no modo **Debug** (ícone de inseto) 🐞.
      * Vá para a aba `Window` \> `Show View` \> `Expressions`.
      * Adicione a variável `conta` à janela 'Live Expressions'.
      * Observe como seu valor aumenta rapidamente enquanto o botão está pressionado.