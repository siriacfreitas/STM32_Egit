/*
 * i2c_scanner.h
 *
 *  Created on: Sep 3, 2025
 *      Author: Lenovo
 */

#ifndef INC_I2C_SCANNER_H_
#define INC_I2C_SCANNER_H_

#pragma once
#include "main.h"
#include "uart.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Endereço padrão (AD0=GND). Se AD0=VCC, use 0x69 */
#define MPU6050_ADDR               (0x68U)

/* Registradores usados */
#define MPU6050_REG_WHOAMI         (0x75U)
#define MPU6050_REG_PWR1           (0x6BU)
#define MPU6050_REG_ACCEL_XOUT_H   (0x3BU)
#define MPU6050_REG_TEMP_OUT_H     (0x41U)
#define MPU6050_REG_ACCEL_CFG      (0x1CU)

/* ---- I2C Scanner ---- */
void I2C_ScanBus(I2C_HandleTypeDef *hi2c, UART_HandleTypeDef *huart);

/* ---- MPU6050 mínimos ---- */
HAL_StatusTypeDef MPU6050_ReadWhoAmI(I2C_HandleTypeDef *hi2c, uint8_t *who);
HAL_StatusTypeDef MPU6050_WakeUp(I2C_HandleTypeDef *hi2c);

/* Função utilitária: faz WHO_AM_I + wake-up e reporta pela UART */
void MPU6050_Check(I2C_HandleTypeDef *hi2c, UART_HandleTypeDef *huart);

#ifdef __cplusplus
}
#endif

#endif /* INC_I2C_SCANNER_H_ */
