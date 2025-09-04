#include "i2c_scanner.h"


/* ---------------- I2C Scanner ---------------- */

void I2C_ScanBus(I2C_HandleTypeDef *hi2c, UART_HandleTypeDef *huart)
{
    /* Separador visual no terminal */
    UART_SendStr(huart, "\r\n");

    /* Título do teste atual (apenas endereço 0x68) */
    UART_SendStrln(huart, "I2C scan porta 0x68");

    /* --- Scan opcional de todo o range (0x03..0x77) ---
       Descomente este bloco para testar todos os enderecos.

    for (uint8_t addr = 0x03; addr <= 0x77; addr++) {
        HAL_StatusTypeDef st_loop = HAL_I2C_IsDeviceReady(hi2c, (addr << 1), 2, 5);
        const char *stat_loop =
            (st_loop == HAL_OK)      ? "OK" :
            (st_loop == HAL_BUSY)    ? "BUSY" :
            (st_loop == HAL_TIMEOUT) ? "TIMEOUT" : "ERROR";
        UART_Printfln(huart, "0x%02X: %s", addr, stat_loop);
    }
    ---------------------------------------------------- */

    /* Teste somente do endereco 0x68 */
    const uint8_t target = 0x68;
    HAL_StatusTypeDef st = HAL_I2C_IsDeviceReady(hi2c, (target << 1), 2, 5);
    const char *stat =
        (st == HAL_OK)      ? "OK" :
        (st == HAL_BUSY)    ? "BUSY" :
        (st == HAL_TIMEOUT) ? "TIMEOUT" : "ERROR";

    UART_Printfln(huart, "0x%02X: %s", target, stat);

    /* Rodape */
    UART_SendStrln(huart, "scan done.");
}

/* ---------------- MPU6050 mínimos ---------------- */

HAL_StatusTypeDef MPU6050_ReadWhoAmI(I2C_HandleTypeDef *hi2c, uint8_t *who) {
    return HAL_I2C_Mem_Read(hi2c,
                            (MPU6050_ADDR << 1),
                            MPU6050_REG_WHOAMI,
                            I2C_MEMADD_SIZE_8BIT,
                            who,
                            1,
                            100);
}

HAL_StatusTypeDef MPU6050_WakeUp(I2C_HandleTypeDef *hi2c) {
    /* PWR_MGMT_1 = 0x00 (sai do sleep, clock interno) */
    uint8_t zero = 0x00;
    return HAL_I2C_Mem_Write(hi2c,
                             (MPU6050_ADDR << 1),
                             MPU6050_REG_PWR1,
                             I2C_MEMADD_SIZE_8BIT,
                             &zero,
                             1,
                             100);
}

void MPU6050_Check(I2C_HandleTypeDef *hi2c, UART_HandleTypeDef *huart) {
    uint8_t who = 0;
    if (MPU6050_ReadWhoAmI(hi2c, &who) == HAL_OK) {
        UART_Printfln(huart, "WHO_AM_I=0x%02X (esperado 0x68)", who);
    } else {
        UART_SendStrln(huart, "Falha ao ler WHO_AM_I");
    }

    if (MPU6050_WakeUp(hi2c) == HAL_OK) {
        UART_SendStrln(huart, "Acordei o MPU6050 (PWR_MGMT_1=0x00)");
    } else {
        UART_SendStrln(huart, "Falha ao acordar o MPU6050");
    }
}
