# Projeto Blinky Básico para STM32 💡

Este é um projeto **minimalista** gerado pelo **STM32CubeIDE**, que demonstra a funcionalidade mais fundamental: piscar um LED em uma placa de desenvolvimento STM32, como a Nucleo.

O objetivo é inicializar o microcontrolador, configurar um único pino GPIO como saída e, em um loop infinito, alternar seu estado para fazer um LED piscar.

## Hardware Necessário

* **Placa de Desenvolvimento STM32:** Qualquer placa da família Nucleo que possua um LED de usuário (ex: NUCLEO-F446RE, NUCLEO-L476RG, etc.).
* **Cabo USB:** Para alimentar e programar a placa através do ST-LINK integrado.



## Funcionalidade do Código

O programa executa as seguintes ações:

1.  **Inicialização do Sistema:**
    * Inicializa a biblioteca HAL (Hardware Abstraction Layer).
    * Configura o clock do sistema para usar o oscilador interno de alta velocidade (HSI).

2.  **Inicialização dos Periféricos:**
    * **GPIO:** Apenas o pino `LD2` (geralmente conectado ao LED verde da placa) é configurado como uma saída digital.

3.  **Loop Principal (`while(1)`):**
    * O coração do programa reside no loop infinito.
    * A função `HAL_GPIO_TogglePin(GPIOA, LD2_Pin);` inverte o estado do LED. Se estava aceso, apaga; se estava apagado, acende.
    * A função `HAL_Delay(1000);` cria uma pausa de 1000 milissegundos (1 segundo).
    * O resultado é um **LED que pisca a cada segundo**.
