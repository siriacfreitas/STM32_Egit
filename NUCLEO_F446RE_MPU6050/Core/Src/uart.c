#include "uart.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#define UART_PRINTF_BUF  128  /* ajuste se precisar de mensagens maiores */

static inline void uart_tx(UART_HandleTypeDef *huart, const void *buf, uint16_t len) {
    if (!huart || !buf || !len) return;
    HAL_UART_Transmit(huart, (uint8_t*)buf, len, HAL_MAX_DELAY);
}

static inline void uart_crlf(UART_HandleTypeDef *huart) {
    static const char CRLF[] = "\r\n";
    uart_tx(huart, CRLF, (uint16_t)(sizeof(CRLF) - 1U));
}

void UART_SendStr(UART_HandleTypeDef *huart, const char *s) {
    if (!huart || !s) return;
    uart_tx(huart, s, (uint16_t)strlen(s));
}

void UART_SendStrln(UART_HandleTypeDef *huart, const char *s) {
    UART_SendStr(huart, s);
    uart_crlf(huart);
}

/* Versão interna: recebe va_list para evitar duplicar código */
static void UART_vPrintf(UART_HandleTypeDef *huart, const char *fmt, va_list ap) {
    if (!huart || !fmt) return;

    char buf[UART_PRINTF_BUF];
    int n = vsnprintf(buf, sizeof(buf), fmt, ap);
    if (n < 0) return;

    /* Envia só o que cabe (sem o '\0' de término) */
    uint16_t to_send = (n < (int)sizeof(buf)) ? (uint16_t)n
                                              : (uint16_t)(sizeof(buf) - 1U);
    uart_tx(huart, buf, to_send);
}

void UART_Printf(UART_HandleTypeDef *huart, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    UART_vPrintf(huart, fmt, ap);
    va_end(ap);
}

void UART_Printfln(UART_HandleTypeDef *huart, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    UART_vPrintf(huart, fmt, ap);
    va_end(ap);
    uart_crlf(huart);
}



void UART_SendFloat3(UART_HandleTypeDef *huart, float x) {
    if (isnan(x)) { UART_SendStr(huart, "nan"); return; }
    if (isinf(x)) { UART_SendStr(huart, (x < 0) ? "-inf" : "inf"); return; }
    if (x < 0) { UART_SendStr(huart, "-"); x = -x; }

    unsigned int i = (unsigned int)x;
    unsigned int frac = (unsigned int)((x - (float)i) * 1000.0f + 0.5f);
    if (frac >= 1000U) { i += 1U; frac -= 1000U; } // arredondamento

    char buf[16];
    int n = snprintf(buf, sizeof buf, "%u.", i);
    HAL_UART_Transmit(huart, (uint8_t*)buf, (uint16_t)n, HAL_MAX_DELAY);

    char f3[3] = {
        (char)('0' + (char)((frac / 100U) % 10U)),
        (char)('0' + (char)((frac / 10U)  % 10U)),
        (char)('0' + (char)( frac         % 10U))
    };
    HAL_UART_Transmit(huart, (uint8_t*)f3, 3, HAL_MAX_DELAY);
}

