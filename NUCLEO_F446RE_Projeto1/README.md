# 🧠 Meu Primeiro Projeto com STM32CubeIDE: Contador com Debugger

Bem-vindo ao seu primeiro projeto com STM32!  
Este guia foi criado para ser um ponto de partida prático e foca em duas habilidades essenciais:

- ✅ Manipular variáveis em C  
- 🐞 Usar o debugger para observar o que acontece dentro do microcontrolador em tempo real  

## 🎯 Objetivo

Criar uma variável chamada `teste`, fazê-la contar de 0 a 10 e depois reiniciar.

---

## 📁 Estrutura do Código

Todo o nosso trabalho será feito no arquivo:
Core/Src/main.c

⚠️ IMPORTANTE: Nunca escreva código fora das seções USER CODE BEGIN/END, ou o CubeMX pode apagar tudo!
---

### 1️. Declarando a Variável `teste`

Abra o arquivo `main.c` e vá até a seção de variáveis privadas:


/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

int teste = 0;

/* USER CODE END PV */


Declarar a variável dentro da seção USER CODE BEGIN PV garante que ela será global dentro do main.c e manterá seu valor entre os ciclos do loop. 

### 12. Criando a Lógica do Contador
Agora vá até o loop principal while(1) e insira o seguinte código dentro da seção indicada:


/* USER CODE BEGIN WHILE */

  while (1)
  {
  
    HAL_Delay(1000);  // Espera 1 segundo

    teste++;          // Incrementa a variável

    if (teste > 10)   // Se passar de 10
    {
      teste = 0;      // Reinicia para 0
    }
  }
  
/* USER CODE END WHILE */


## 🐞 Usando o Debugger para "Ver" a Variável `teste`

Agora vamos ver a variável `teste` mudando em tempo real dentro do microcontrolador, usando o debugger do STM32CubeIDE.

---

### 🧱 1. Compile o Projeto

Antes de rodar ou debugar, você precisa compilar o projeto:

- Clique no ícone de **martelo** na barra superior: 🔨  
  *(Esse botão compila o projeto por completo.)*

- Ou vá em:  
  `Project > Build Project`

> Se a compilação estiver correta, o console exibirá:  
> ✅ **Build Finished**

---

### 🐛 2. Inicie o Debugger

Depois de compilar, clique no **ícone do inseto verde** na barra superior: 🐞  
Ou vá em:  
`Run > Debug`

Na **primeira vez que você for rodar o debugger neste projeto**, o STM32CubeIDE abrirá a janela de configuração de debug.
---

### 🔍 3. Escaneie a Placa (Apenas na Primeira Vez)

Na aba `Debugger`, você precisa verificar se o STM32CubeIDE reconheceu sua placa. Para isso:

- Verifique se o modo está em `ST-LINK (ST-LINK GDB server)`
- Em `ST-LINK S/N`, clique no botão **Scan**
- Aguarde até o número de série da sua placa aparecer automaticamente

> Esse passo só precisa ser feito **na primeira vez** que você roda o debug ou se trocar de placa.

Depois disso, clique em **"Debug"** no canto inferior direito da janela para iniciar a sessão de depuração.

---

### 👁️ 4. Visualize a Variável em Tempo Real

- Na aba `Live Expressions` (canto superior direito), clique em `Add new expression`
- Digite `teste` e pressione **Enter**

Agora o STM32CubeIDE vai mostrar o valor de `teste` **atualizando a cada segundo**.

---

### ▶️ 5. Rode o Programa

Com o debugger pronto:

- Pressione `F8` ou clique no botão verde de "play"

Você verá o valor de `teste` mudando na aba `Live Expressions`:  
`0 → 1 → 2 → ... → 10 → 0 → 1 ...`

---

### 🧠 Por que isso é importante?

Com o debugger, você pode:

- Observar o comportamento do sistema em tempo real  
- Verificar erros sem precisar usar `printf()` ou LEDs  
- Testar lógicas de controle com precisão

---

Parabéns! 🎉  
Você concluiu seu primeiro projeto com depuração real usando STM32CubeIDE!

