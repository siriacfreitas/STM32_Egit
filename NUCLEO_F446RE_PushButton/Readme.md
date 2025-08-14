# Projeto de Controle de LED com Botão para STM32 🕹️

Este projeto demonstra uma das interações mais básicas em sistemas embarcados: ler o estado de um botão (entrada digital) para controlar um LED (saída digital). O programa utiliza uma abordagem de *polling* para verificar continuamente o estado do botão do usuário.

## Hardware Necessário

* **Placa de Desenvolvimento STM32:** Qualquer placa da família Nucleo com um LED de usuário (`LD2`) e um botão (`B1`), como a NUCLEO-F446RE.
* **Cabo USB:** Para alimentar e programar a placa através do ST-LINK integrado.



## Funcionalidade do Código

O objetivo é simples: o LED `LD2` (verde) acende **apenas enquanto** o botão de usuário `B1` (azul) estiver pressionado.

### Como Funciona

O programa opera em um loop infinito (`while(1)`) que constantemente executa os seguintes passos:

1.  **Leitura do Botão:** A função `HAL_GPIO_ReadPin(GPIOC, B1_Pin)` lê o estado do pino conectado ao botão.
    * O pino do botão (`B1_Pin`) está configurado com um resistor de **pull-up interno**. Isso significa que, quando o botão está **solto**, o pino lê um nível lógico **ALTO** (`GPIO_PIN_SET`).
    * Quando o botão é **pressionado**, ele conecta o pino ao terra, fazendo com que a leitura seja um nível lógico **BAIXO** (`GPIO_PIN_RESET`).

2.  **Lógica de Controle:** Uma estrutura `if-else` decide o que fazer:
    * `if(!HAL_GPIO_ReadPin(...))`: A condição é verdadeira somente quando a leitura do pino é `GPIO_PIN_RESET` (botão pressionado).
    * Nesse caso, `HAL_GPIO_WritePin(..., GPIO_PIN_SET);` é executado, **acendendo o LED**.
    * Caso contrário (botão solto), o bloco `else` é executado, e `HAL_GPIO_WritePin(..., GPIO_PIN_RESET);` **apaga o LED**.

---

## Periféricos Configurados

* **GPIO:**
    * **Pino `B1_Pin` (PC13):** Configurado como **entrada** com resistor de pull-up interno ativado.
    * **Pino `LD2_Pin` (PA5):** Configurado como **saída** do tipo push-pull.

