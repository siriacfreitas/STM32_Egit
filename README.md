# STM32_Egit

📘 **Códigos de teste e aprendizado utilizando o microcontrolador STM32 Nucleo-F446RE.**
Este repositório contém exemplos progressivos para iniciantes e estudantes que desejam aprender programação embarcada com STM32 utilizando a biblioteca HAL e o ambiente STM32CubeIDE.

---

## 🧠 Objetivo

Este repositório foi criado com fins didáticos para:

* Explorar os recursos básicos do STM32 (GPIO, interrupções, delays, timers etc.);
* Entender o uso da HAL (Hardware Abstraction Layer);
* Servir como base para projetos maiores e aplicações reais.

---

## 🛠️ Requisitos

* **Placa**: STM32 Nucleo-F446RE (ou compatível)
* **IDE**: [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html)
* **Programador**: ST-LINK (já integrado na placa)
* **Cabos**: Micro USB para alimentação e programação

---

## 📂 Organização dos Códigos

Cada pasta ou arquivo representa um exemplo prático. Alguns exemplos presentes:

| Exemplo                  | Descrição                                                             |
| ------------------------ | --------------------------------------------------------------------- |
| `01_blink_led`           | Piscar LED onboard com `HAL_Delay()`                                  |
| `02_botao_led`           | Acender LED ao pressionar botão (leitura de entrada digital)          |
| `03_toggle_com_debounce` | Alternar LED com botão usando `HAL_GetTick()` para debounce por tempo |
| `04_interrupcao_exti`    | Usar interrupção por botão para alternar um segundo LED               |

---

## ⚙️ Conceitos Abordados

* Manipulação de GPIOs (entrada e saída)
* Controle de tempo com `HAL_Delay()` e `HAL_GetTick()`
* Interrupções externas com `EXTI`
* Estrutura de um projeto gerado pelo STM32CubeIDE
* Organização de código embarcado com boas práticas

---

## 📌 Placa Utilizada

XXXXX

* **Nucleo-F446RE**

  * MCU: STM32F446RE (ARM Cortex-M4, 180 MHz)
  * Interface ST-LINK/V2-1
  * LED LD2 (PA5)
  * Botão USER (PC13)

---

## 📄 Licença

Todos os exemplos seguem a licença padrão da STMicroelectronics (AS-IS). Utilize livremente para fins educacionais e não comerciais.


Desenvolvido como parte de atividades práticas e estudos sobre sistemas embarcados com STM32.
Estes projetos foram desenvolvidos com base no curso de Sistemas Embarcados com STM32 do Professor Gabriel Vigiano.


---
