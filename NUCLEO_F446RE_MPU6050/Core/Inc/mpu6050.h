#ifndef MPU6050_H_
#define MPU6050_H_

#pragma once
#include "main.h"
#include "uart.h"
#include "i2c_scanner.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


//#define I2C_SLAVE_ADDRESS 0x2A << 1  // HAL usa 8 bits (endereços precisam ser deslocados)

/* --- novos registradores para o MPU6050 --- */
#define MPU6050_REG_ACCEL_CONFIG    (0x1CU)
#define MPU6050_REG_ACCEL_XOUT_H    (0x3BU)

#define MPU6050_REG_TEMP_OUT_H      (0x41U)

#define MPU6050_REG_GYRO_CONFIG     (0x1BU)
#define MPU6050_REG_GYRO_XOUT_H     (0x43U)

// Valores de segurança da temperatura
#define INT16_MAX_TEMP  32767
#define INT16_MIN_TEMP  -32767



/* --- API do acelerômetro --- */

/* Configura ACCEL_CONFIG para ±2g (AFS_SEL=00). */
HAL_StatusTypeDef MPU6050_SetAccelRange_2g(I2C_HandleTypeDef *hi2c);

/* Lê os 6 bytes do acelerômetro (bruto). */
HAL_StatusTypeDef MPU6050_ReadAccelRaw(I2C_HandleTypeDef *hi2c,
                                       int16_t *ax, int16_t *ay, int16_t *az);

/* Converte amostras brutas para “g” assumindo faixa ±2g (16384 LSB/g). */
void MPU6050_AccelRawTo_g_2g(int16_t ax, int16_t ay, int16_t az,
                             float *axg, float *ayg, float *azg);

/* --- API da temperatura--- */

/* Lê o registrador bruto de temperatura (16 bits signed). */
HAL_StatusTypeDef MPU6050_ReadTempRaw(I2C_HandleTypeDef *hi2c, int16_t *temp_raw);

/* Converte para centésimos de °C (°C * 100), sem float.
   Fórmula datasheet: T[°C] = temp_raw/340 + 36.53  ->  T_centi = (temp_raw*100)/340 + 3653 */
HAL_StatusTypeDef MPU6050_ReadTemp_centiC(I2C_HandleTypeDef *hi2c, int16_t *t_centi);


/* --- API do giroscópio--- */

/* Configura GYRO_CONFIG para ±250 dps (FS_SEL=00). */
HAL_StatusTypeDef MPU6050_SetGyroRange_250dps(I2C_HandleTypeDef *hi2c);

/* Lê os 6 bytes do giroscópio bruto (X, Y, Z). */
HAL_StatusTypeDef MPU6050_ReadGyroRaw(I2C_HandleTypeDef *hi2c,
                                      int16_t *gx, int16_t *gy, int16_t *gz);

/* Converte bruto para centi-(°/s) assumindo ±250 dps (131 LSB/(°/s)).
   Ex.: retorno 1234 => 12.34 °/s. */
void MPU6050_GyroRawTo_cdegps_250dps(int16_t gx, int16_t gy, int16_t gz,
                                     int16_t *gx_cdps, int16_t *gy_cdps, int16_t *gz_cdps);


#ifdef __cplusplus
}
#endif



#endif /* MPU6050_H_ */
