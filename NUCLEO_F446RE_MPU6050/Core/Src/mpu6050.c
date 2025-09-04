#include "mpu6050.h"


/* --- API do acelerômetro --- */

HAL_StatusTypeDef MPU6050_SetAccelRange_2g(I2C_HandleTypeDef *hi2c) {

	if (hi2c == NULL) return HAL_ERROR;

	/* ACCEL_CONFIG: AFS_SEL[4:3] = 00 (±2g), demais bits 0 (sem self-test/HPF) */
    uint8_t val = 0x00;

    return HAL_I2C_Mem_Write(hi2c,
                             (MPU6050_ADDR << 1),
                             MPU6050_REG_ACCEL_CONFIG,
                             I2C_MEMADD_SIZE_8BIT,
                             &val, 1, 100);
}


HAL_StatusTypeDef MPU6050_ReadAccelRaw(I2C_HandleTypeDef *hi2c,
                                       int16_t *ax, int16_t *ay, int16_t *az) {
    if (hi2c == NULL || ax == NULL || ay == NULL || az == NULL) return HAL_ERROR;

    uint8_t ACCEL_OUT[6];
    HAL_StatusTypeDef st = HAL_I2C_Mem_Read(hi2c,
                                            (MPU6050_ADDR << 1),
                                            MPU6050_REG_ACCEL_XOUT_H,
                                            I2C_MEMADD_SIZE_8BIT,
											ACCEL_OUT, 6, 100);
    if (st != HAL_OK) return st;

    *ax = (int16_t)((ACCEL_OUT[0] << 8) | ACCEL_OUT[1]);
    *ay = (int16_t)((ACCEL_OUT[2] << 8) | ACCEL_OUT[3]);
    *az = (int16_t)((ACCEL_OUT[4] << 8) | ACCEL_OUT[5]);
    return HAL_OK;
}

void MPU6050_AccelRawTo_g_2g(int16_t ax, int16_t ay, int16_t az,
                             float *axg, float *ayg, float *azg) {

    const float sens = 16384.0f; /* LSB por g em ±2g */

    if (axg) *axg = (float)ax / sens;
    if (ayg) *ayg = (float)ay / sens;
    if (azg) *azg = (float)az / sens;
}


/* --- API da temperatura--- */

HAL_StatusTypeDef MPU6050_ReadTempRaw(I2C_HandleTypeDef *hi2c, int16_t *temp_raw) {
    if (hi2c == NULL || temp_raw == NULL) return HAL_ERROR;

    uint8_t TEMP_OUT[2];
    HAL_StatusTypeDef st = HAL_I2C_Mem_Read(hi2c,
                                            (MPU6050_ADDR << 1),
                                            MPU6050_REG_TEMP_OUT_H,
                                            I2C_MEMADD_SIZE_8BIT,
											TEMP_OUT, 2, 100);
    if (st != HAL_OK) return st;

    *temp_raw = (int16_t)((TEMP_OUT[0] << 8) | TEMP_OUT[1]);
    return HAL_OK;
}



/* Retorna temperatura em centésimos de °C (ex.: 2534 => 25.34 °C) */
HAL_StatusTypeDef MPU6050_ReadTemp_centiC(I2C_HandleTypeDef *hi2c, int16_t *t_centi) {
    if (hi2c == NULL || t_centi == NULL) return HAL_ERROR;

    int16_t raw = 0;
    HAL_StatusTypeDef st = MPU6050_ReadTempRaw(hi2c, &raw);
    if (st != HAL_OK) return st;

    /* T_centi = round( (raw*100)/340 ) + 3653.
       Arredondamento simétrico sem float (±170 = 340/2). */

    int32_t num = (int32_t)raw * 100;
    if (num >= 0) num = (num + 170) / 340;
    else          num = (num - 170) / 340;

    int32_t t = num + 3653;             // 36.53 °C = 3653 centésimos
    if (t > INT16_MAX) t = INT16_MAX_TEMP ;   // clamp de segurança
    if (t < INT16_MIN) t = INT16_MIN_TEMP ;	  // clamp de segurança

    *t_centi = (int16_t)t;
    return HAL_OK;
}

/* --- API do giroscópio --- */

HAL_StatusTypeDef MPU6050_SetGyroRange_250dps(I2C_HandleTypeDef *hi2c) {

	if (hi2c == NULL) return HAL_ERROR;

	 /* GYRO_CONFIG: FS_SEL[4:3] = 00 => ±250 dps; demais bits 0 */
    uint8_t val = 0x00;

    return HAL_I2C_Mem_Write(hi2c,
                             (MPU6050_ADDR << 1),
							 MPU6050_REG_GYRO_CONFIG,
                             I2C_MEMADD_SIZE_8BIT,
                             &val, 1, 100);
}

HAL_StatusTypeDef MPU6050_ReadGyroRaw(I2C_HandleTypeDef *hi2c,
                                       int16_t *gx, int16_t *gy, int16_t *gz) {
    if (hi2c == NULL || gx == NULL || gy == NULL || gz == NULL) return HAL_ERROR;

    uint8_t GYRO_XOUT[6];
    HAL_StatusTypeDef st = HAL_I2C_Mem_Read(hi2c,
                                            (MPU6050_ADDR << 1),
											MPU6050_REG_GYRO_XOUT_H,
                                            I2C_MEMADD_SIZE_8BIT,
											GYRO_XOUT, 6, 100);
    if (st != HAL_OK) return st;

    *gx = (int16_t)((GYRO_XOUT[0] << 8) | GYRO_XOUT[1]);
    *gy = (int16_t)((GYRO_XOUT[2] << 8) | GYRO_XOUT[3]);
    *gz = (int16_t)((GYRO_XOUT[4] << 8) | GYRO_XOUT[5]);
    return HAL_OK;
}

/* Helper interno para divisão com arredondamento simétrico. */
static int16_t mpu6050_div_round_131_to_cdps(int16_t raw) {
    /* cdps = round(raw * 100 / 131)  (centi-deg/s, ±25000  */
    int32_t t = (int32_t)raw * 100;     // até ±3.27e6
    if (t >= 0) t += 131/2; else t -= 131/2;   // arredonda
    return (int16_t)(t / 131);
}

void MPU6050_GyroRawTo_cdegps_250dps(int16_t gx, int16_t gy, int16_t gz,
                                     int16_t *gx_cdps, int16_t *gy_cdps, int16_t *gz_cdps) {
    if (gx_cdps) *gx_cdps = mpu6050_div_round_131_to_cdps(gx);
    if (gy_cdps) *gy_cdps = mpu6050_div_round_131_to_cdps(gy);
    if (gz_cdps) *gz_cdps = mpu6050_div_round_131_to_cdps(gz);
}


