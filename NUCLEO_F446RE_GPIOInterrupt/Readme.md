# Projeto Multitarefa: Blinky e Interrupção em STM32 🚦⚡

Este projeto demonstra a coexistência de duas tarefas em um microcontrolador STM32, um conceito fundamental em sistemas embarcados:

1.  **Tarefa Síncrona (Blinky):** Um LED pisca em um intervalo fixo, usando a função `HAL_Delay()`. Esta é uma tarefa "bloqueante".
2.  **Tarefa Assíncrona (Interrupção):** Um segundo LED tem seu estado invertido instantaneamente toda vez que um botão é pressionado, usando uma interrupção de hardware (EXTI).

Isso mostra como o microcontrolador pode executar uma rotina principal enquanto responde imediatamente a eventos externos, sem precisar verificar o estado do botão constantemente (*polling*).

-----

## Hardware Necessário

  * **Placa de Desenvolvimento STM32:** Qualquer placa da família Nucleo (ex: NUCLEO-F446RE).
  * **Cabo USB:** Para alimentar e programar a placa.
  * **LED Externo:** Um LED de qualquer cor.
  * **Resistor:** Um resistor para limitar a corrente do LED (ex: **220Ω** ou **330Ω**).
  * **Protoboard e Jumpers:** Para montar o circuito do LED externo.

### Montagem do Circuito Externo

Conecte o LED externo ao pino `LD_EXT_Pin` (neste exemplo, ele está no mesmo port do `LD2`, o `GPIOA`).

  * Pino de saída (ex: `PA8` ou outro pino do `GPIOA`) → Resistor → Anodo do LED (perna mais longa)
  * Catodo do LED (perna mais curta) → `GND` da placa

-----

## Funcionalidade

O projeto tem dois comportamentos que ocorrem em paralelo:

### Comportamento 1: LED Piscando (Tarefa de Fundo)

O LED onboard `LD2` (verde) pisca continuamente, ligando e desligando a cada **1 segundo**. Isso é controlado pelo código dentro do loop `while(1)` na função `main()`.

```c
while (1)
{
  HAL_GPIO_TogglePin(GPIOA, LD2_Pin);
  HAL_Delay(1000);
}
```

### Comportamento 2: Controle por Interrupção (Tarefa Assíncrona)

O **LED externo** (`LD_EXT_Pin`) muda de estado (de aceso para apagado, ou vice-versa) toda vez que o botão do usuário `B1` é pressionado. Isso ocorre imediatamente, pois é tratado por uma rotina de interrupção, que "interrompe" a tarefa principal (inclusive o `HAL_Delay()`) para executar sua lógica e depois retorna de onde parou.

-----

## Configuração no STM32CubeIDE

Para que este projeto funcione, os pinos precisam ser configurados corretamente no STM32CubeIDE.

1.  **Pinos dos LEDs:** Configure `LD2_Pin` e `LD_EXT_Pin` como `GPIO_Output`.

2.  **Pino do Botão (B1):** Esta é a parte mais importante. Para o pino do botão (`B1_Pin`, geralmente em **PC13**):

      * Defina seu modo como **`GPIO_EXTI13`**. A sigla **EXTI** significa "External Interrupt". Isso informa ao MCU que este pino pode gerar interrupções.
      * Configure o resistor interno como **Pull-up**.

3.  **Ativar a Interrupção (NVIC):**

      * Vá para a aba `System Core` → `GPIO` → `NVIC`.
      * Marque a caixa de seleção para habilitar a linha de interrupção `EXTI line[15:10] interrupts`.

-----

## Implementação da Interrupção

A lógica que executa quando a interrupção ocorre está na função de callback `HAL_GPIO_EXTI_Callback`.

**🚨 Importante:** Para que a biblioteca HAL encontre e use sua implementação, esta função deve ser escrita **fora da função `main()`**, no espaço de código do usuário (a seção `/* USER CODE BEGIN 0 */` é um ótimo lugar para isso).

```c
/* USER CODE BEGIN 0 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	// Verifica se a interrupção foi causada pelo pino B1
	if(GPIO_Pin == B1_Pin){
		// Inverte o estado do LED externo
		HAL_GPIO_TogglePin(GPIOA, LD_EXT_Pin);
	}
}
/* USER CODE END 0 */
```
