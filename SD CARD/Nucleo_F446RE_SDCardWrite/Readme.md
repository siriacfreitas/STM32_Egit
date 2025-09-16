# Gerenciador de Arquivos em Cartão SD com STM32

Este projeto é uma demonstração de como manipular arquivos e diretórios em um cartão SD usando um microcontrolador STM32. 💾

O código inicializa o sistema de arquivos **FatFs** em um cartão SD (comunicação via **SPI**), cria um arquivo `.csv` na raiz, escreve alguns dados nele, cria um novo diretório (pasta) e, em seguida, cria um arquivo `.txt` dentro desse novo diretório. Um LED pisca para indicar que as operações foram concluídas com sucesso.

-----

## Funcionalidade Principal ⚙️

Ao ser ligado, o microcontrolador executa as seguintes ações:

1.  **Monta o Cartão SD**: Tenta acessar o sistema de arquivos do cartão.
2.  **Sinaliza Sucesso**: Se a montagem for bem-sucedida, um LED é aceso para indicar o início das operações.
3.  **Cria e Escreve um Arquivo CSV**:
      * Cria um arquivo chamado `Arquivo.csv` na raiz do cartão. Se o arquivo já existir, ele será sobrescrito.
      * Escreve duas linhas de dados formatados como CSV no arquivo.
      * Fecha o arquivo para salvar as alterações.
4.  **Cria um Diretório**:
      * Cria uma nova pasta chamada `PASTA` na raiz do cartão.
5.  **Cria um Arquivo Dentro do Diretório**:
      * Cria um novo arquivo chamado `File.txt` dentro da `PASTA` recém-criada.
      * Escreve uma frase nesse arquivo.
      * Fecha o arquivo.
6.  **Sinaliza Fim**: Após uma pequena pausa, o LED é apagado, indicando que todas as tarefas foram finalizadas.
7.  **Loop Infinito**: O programa entra em um estado de espera, não realizando mais nenhuma ação.

-----

## Análise do Código 🔍

### Função `main()` - O Centro de Comando

A função `main()` orquestra todas as operações, desde a inicialização do hardware até a manipulação dos arquivos.

```c
int main(void)
{
    /* ... Inicialização do MCU e Periféricos (HAL_Init, Clock, GPIO, SPI, FATFS) ... */

    /* Variáveis locais para o FatFs */
    FATFS meuFATFS; // Objeto que representa o sistema de arquivos
    FIL meuArquivo; // Objeto que representa um arquivo
    UINT testeByte; // Variável para registrar o número de bytes escritos

    // 1. Tenta montar (acessar) o cartão SD
    if (f_mount(&meuFATFS, USERPath, 1) == FR_OK) {
        // Sucesso! Liga o LED para sinalizar
        HAL_GPIO_TogglePin(GPIOB, led_Pin);

        // 2. Cria e escreve no arquivo CSV
        f_open(&meuArquivo, "Arquivo.csv", FA_WRITE | FA_CREATE_ALWAYS);
        char meusdados[] = "Escrevendo linha 1,teste 1\r\nEscrevendo linha 2,teste 2\0";
        f_write(&meuArquivo, meusdados, sizeof(meusdados), &testeByte);
        f_close(&meuArquivo);

        // 3. Cria um novo diretório (pasta)
        f_mkdir("PASTA");

        // 4. Cria e escreve em um arquivo dentro da nova pasta
        f_open(&meuArquivo, "/PASTA/File.txt", FA_WRITE | FA_CREATE_ALWAYS);
        char dadosfile[] = "Criando um arquivo dentro de uma pasta\0";
        f_write(&meuArquivo, dadosfile, sizeof(dadosfile), &testeByte);
        f_close(&meuArquivo);

        // 5. Sinaliza o fim das operações
        HAL_Delay(500);
        HAL_GPIO_TogglePin(GPIOB, led_Pin);
    }
    
    // 6. Entra em loop infinito
    while (1) {
    }
}
```

  - **`f_mount`**: É o ponto de partida. Se esta função falhar, o bloco `if` não será executado, e nada acontecerá.
  - **`f_open`**: Abre ou cria um arquivo. As *flags* são muito importantes:
      * `FA_WRITE`: Permite a escrita no arquivo.
      * `FA_CREATE_ALWAYS`: Cria um novo arquivo. Se ele já existir, seu conteúdo anterior é apagado.
  - **`f_write`**: Escreve os dados de um array de `char` no arquivo aberto.
  - **`f_mkdir`**: Cria um novo diretório com o nome especificado.
  - **`f_close`**: Essencial\! Fecha o arquivo, garantindo que todos os dados sejam salvos corretamente no cartão SD.
  - **`HAL_GPIO_TogglePin`**: Inverte o estado do pino do LED (se estava apagado, acende; se estava aceso, apaga).

-----

## Como Usar 🚀

1.  **Hardware**: Conecte um módulo de cartão SD aos pinos **SPI1** do seu STM32. Certifique-se de que o pino do LED (`led_Pin`) e o Chip Select do SPI (`SPI1_CS_Pin`) estejam configurados corretamente no `MX_GPIO_Init()`.
2.  **Cartão SD**: Use um cartão SD formatado com **FAT32**. Não é necessário criar nenhum arquivo ou pasta previamente.
3.  **Compilar e Carregar**: Envie o código para a sua placa STM32.
4.  **Verificar o Resultado**:
      * Ao ligar ou resetar a placa, o LED deve piscar uma vez.
      * Após a execução, remova o cartão SD e coloque-o em um computador.
      * Você deverá encontrar um arquivo `Arquivo.csv` e uma pasta `PASTA`. Dentro da pasta, haverá um arquivo `File.txt` com os respectivos conteúdos que o código escreveu.
