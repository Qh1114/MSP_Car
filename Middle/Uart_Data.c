#include "Uart_Data.h"
#include "Usart.h"

#define UART_FRAME_HEADER 0xAA
#define UART_FRAME_TAIL   0xBB

static uint8_t BcdNibbleToDigit(uint8_t nibble)
{
    if (nibble > 9) {
        return 0xFF;
    }

    return nibble;
}

static uint8_t BcdByteToValue(uint8_t bcd, uint8_t* value)
{
    uint8_t high = BcdNibbleToDigit((uint8_t)(bcd >> 4));
    uint8_t low = BcdNibbleToDigit((uint8_t)(bcd & 0x0F));

    if (high == 0xFF || low == 0xFF) {
        return 0;
    }

    *value = (uint8_t)(high * 10U + low);
    return 1;
}

uint8_t Uart0_Receive_Command(float* command)
{
    uint8_t buffer[30];
    uint8_t length = UART_ReadCommand(buffer);

    if (buffer[0] != UART_FRAME_HEADER || buffer[length - 1] != UART_FRAME_TAIL) {
        return 0;
    }

    uint8_t payloadLength = (uint8_t)(length - 2);
    uint8_t frameCount = (uint8_t)(payloadLength / 2U);

    for (uint8_t i = 0; i < frameCount; i++) {
        uint8_t integerPart;
        uint8_t decimalPart;
        uint8_t payloadIndex = (uint8_t)(1U + (i * 2U));

        if (!BcdByteToValue(buffer[payloadIndex], &integerPart)) {
            return 0;
        }

        if (!BcdByteToValue(buffer[payloadIndex + 1U], &decimalPart)) {
            return 0;
        }

        command[i] = (float)integerPart + ((float)decimalPart / 100.0f);
    }

    return frameCount;
}