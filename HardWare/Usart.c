#include "ti_msp_dl_config.h"
#include "Usart.h"
#include "Delay.h"
#include "string.h"
#include "stdarg.h"
#include <stdio.h>

#define BufferSize 256          //缓冲区大小
#define CommandMiniSize 4       //最小命令长度
#define CommandStartByte 0xAA   //命令起始字节
#define CommandEndByte 0xBB     //命令结束字节

uint8_t ReadIndex = 0;
uint8_t WriteIndex = 0;
uint8_t Buffer[BufferSize];
uint8_t data;

void Uart_Init(void)
{
    NVIC_ClearPendingIRQ(UART_INST_INT_IRQN);
    (void)DL_UART_Main_receiveData(UART_INST);
    NVIC_EnableIRQ(UART_INST_INT_IRQN);
}

//串口发送单个字符
void Uart0_Send_Char(char ch)
{
    while(DL_UART_isBusy(UART_INST) == true);
    DL_UART_Main_transmitData(UART_INST, ch);
}

//串口发送字符串
void Uart0_Send_String(char* str)
{
    while(str!=0 && *str!=0)
    {
        Uart0_Send_Char(*str++);
    }
}

//串口打印
void Uart0_Printf(const char* format, ...)
{
    char tmp[128];  
    va_list argptr;

    va_start(argptr, format);
    vsnprintf(tmp, sizeof(tmp) - 1, format, argptr);
    va_end(argptr);

    Uart0_Send_String(tmp);
}
//
//@简介：增加写索引
//@参数：Length:增加的长度
//
static void AddReadIndex(uint8_t Length)
{
    ReadIndex += Length;
    ReadIndex %= BufferSize;
}

static uint8_t Read(uint8_t i)
{
    uint8_t index = i % BufferSize;
    return Buffer[index];
}

//
//@简介：获取当前缓冲区数据长度
//@返回值：数据长度，最大为BufferSize - 1
//        
static uint8_t GetLength(void)
{
    return (WriteIndex + BufferSize - ReadIndex) % BufferSize;
}

//
//@简介：获取缓冲区剩余空间长度
//@返回值：剩余空间长度，最小为0
//
static uint8_t GetRemain(void)
{
    return BufferSize - GetLength() - 1;
}

//
//@简介：写入数据到缓冲区
//@参数：data:要写入的数据
//@参数：size:数据长度
//@返回值：写入成功返回1，失败返回0
//
static uint8_t Write(uint8_t* data, uint16_t size)
{
    if(size >= GetRemain()){
        return 0;
    }
    if(WriteIndex + size < BufferSize){
        memcpy(&Buffer[WriteIndex], data, size);
        WriteIndex += size;
        WriteIndex %= BufferSize;
    } else {
        uint16_t firstPart = BufferSize - WriteIndex;
        memcpy(&Buffer[WriteIndex], data, firstPart);
        uint16_t secondPart = size - firstPart;
        memcpy(Buffer, &data[firstPart], secondPart);
        WriteIndex = secondPart;
    }
    return 1;
}

//
//@简介：从缓冲区读取一个完整命令
//@参数：command:存放读取命令的缓冲区
//@返回值：命令长度，失败返回0
//
uint8_t UART_ReadCommand(uint8_t* command)
{
    while(1)
    {
        uint8_t dataLength = GetLength();
        if(dataLength < CommandMiniSize){
            return 0;
        }

        if(Buffer[ReadIndex] != CommandStartByte){
            AddReadIndex(1);
            continue;
        }

        dataLength = GetLength();
        if(dataLength < CommandMiniSize){
            return 0;
        }

        for(uint8_t i = 1; i < dataLength; i++){
            if(Buffer[(ReadIndex + i) % BufferSize] == CommandEndByte){
                dataLength = i + 1;
                break;
            }
            if(i == dataLength - 1){
                return 0;
            }
        }

        //含校验位代码
/*
        uint8_t Sum = 0;
        for(uint8_t i = 0; i < dataLength; i++){
            Sum += Read(ReadIndex + i);
        }
        if(Sum != Read(ReadIndex + dataLength - 1)){
            AddReadIndex(1);
            continue;
        }
        uint8_t cmdLength = Buffer[ReadIndex + 1];
        if(dataLength < cmdLength){
            AddReadIndex(1);
            continue;
        }
*/

        for (uint8_t i = 0; i < dataLength; i++) {
            command[i] = Read(ReadIndex + i);
        }
        AddReadIndex(dataLength);
        return dataLength;
    }
}


//uint16_t count = 0;
//uint8_t Buffer2[128];
void UART_INST_IRQHandler(void)
{
    //Uart0_Printf("Enter");
    switch( DL_UART_getPendingInterrupt(UART_INST) )
    {
        case DL_UART_IIDX_RX:
            //Uart0_Printf("Enter_");
            data = DL_UART_Main_receiveData(UART_INST);
            Write(&data, 1);
            //Buffer2[count % 128] = data;
            //count++;
            break;
        default:
            break;
    }
}

