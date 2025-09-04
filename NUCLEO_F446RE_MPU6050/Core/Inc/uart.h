
#ifndef INC_UART_H_
#define INC_UART_H_

#pragma once
#include "main.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Envia uma string (sem quebra de linha) */
void UART_SendStr  (UART_HandleTypeDef *huart, const char *s);

/* Envia uma string seguida de "\r\n" */
void UART_SendStrln(UART_HandleTypeDef *huart, const char *s);

/* printf: aceita argumentos variáveis (ex.: UART_Printf(&huart2, "x=%d", x);) */
void UART_Printf   (UART_HandleTypeDef *huart, const char *fmt, ...);

/* printf + quebra de linha automática */
void UART_Printfln (UART_HandleTypeDef *huart, const char *fmt, ...);

/* Envia um float de 3 casas (sem quebra de linha) */
void UART_SendFloat3(UART_HandleTypeDef *huart, float x);


#ifdef __cplusplus
}
#endif



#endif /* INC_UART_H_ */
