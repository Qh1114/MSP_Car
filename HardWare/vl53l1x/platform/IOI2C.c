#include "IOI2C.h"
#include "ti_msp_dl_config.h"
#include "Delay.h"

#define TRUE 1
#define FALSE 0
#define I2C_TIME_OUT_BASE   10
#define I2C_TIME_OUT_BYTE   1
// static uint8_t IIC_Start(void);				
// static void IIC_Stop(void);	  		
// static void IIC_Send_Byte(uint8_t txd);	
// static uint8_t IIC_Read_Byte(void);
// static uint8_t IIC_Wait_Ack(void); 	
// static void IIC_Ack(void);				
// static void IIC_NAck(void);		


// static void IIC_delay(void)
// {
// 	delay_cycles(10);
// }


// /**************************实现函数********************************************
// *函数原型:		void IIC_Start(void)
// *功　　能:		产生IIC起始信号
// *******************************************************************************/
// static uint8_t IIC_Start(void)
// {
// 	SDA_H;
// 	SCL_H;
// 	IIC_delay();
// 	if(!SDA_read)
// 		return FALSE;	
// 	SDA_L;
// 	IIC_delay();
// 	if(SDA_read)
// 		return FALSE;	
// 	SDA_L;
// 	IIC_delay();
// 	return TRUE;
// }

// /**************************实现函数********************************************
// *函数原型:		void IIC_Stop(void)
// *功　　能:	    //产生IIC停止信号
// *******************************************************************************/
// static void IIC_Stop(void)
// {
// 	SCL_L;
// 	IIC_delay();
// 	SDA_L;
// 	IIC_delay();
// 	SCL_H;
// 	IIC_delay();
// 	SDA_H;
// 	IIC_delay();
// }

// /**************************实现函数********************************************
// *函数原型:		uint8_t IIC_Wait_Ack(void)
// *功　　能:	    等待应答信号到来
// //返回值：1，接收应答失败
// //        0，接收应答成功
// *******************************************************************************/
// static uint8_t IIC_Wait_Ack(void) 	
// {
// 	SCL_L;
// 	IIC_delay();
// 	SDA_H;
// 	IIC_delay();
// 	SCL_H;
// 	IIC_delay();
// 	if(SDA_read)
// 	{
//     SCL_L;
// 	  IIC_delay();
//       return FALSE;
// 	}
// 	SCL_L;
// 	IIC_delay();
// 	return TRUE;
// }

// /**************************实现函数********************************************
// *函数原型:		void IIC_Ack(void)
// *功　　能:	    产生ACK应答
// *******************************************************************************/
// static void IIC_Ack(void)
// {
// 	SCL_L;
// 	IIC_delay();
// 	SDA_L;
// 	IIC_delay();
// 	SCL_H;
// 	IIC_delay();
// 	SCL_L;
// 	IIC_delay();
// }

// /**************************实现函数********************************************
// *函数原型:		void IIC_NAck(void)
// *功　　能:	    产生NACK应答
// *******************************************************************************/
// static void IIC_NAck(void)
// {
// 	SCL_L;
// 	IIC_delay();
// 	SDA_H;
// 	IIC_delay();
// 	SCL_H;
// 	IIC_delay();
// 	SCL_L;
// 	IIC_delay();
// }

// /**************************实现函数********************************************
// *函数原型:		void IIC_Send_Byte(uint8_t txd)
// *功　　能:	    IIC发送一个字节
// *******************************************************************************/
// static void IIC_Send_Byte(uint8_t SendByte) 
// {
//     uint8_t i=8;
//     while(i--)
//     {
// 			SCL_L;
// 			IIC_delay();
// 			if(SendByte&0x80)
// 				SDA_H;
// 			else
// 				SDA_L;
// 			SendByte<<=1;
// 			IIC_delay();
// 			SCL_H;
// 			IIC_delay();
//     }
//     SCL_L;
// }

// /**************************实现函数********************************************
// *函数原型:		uint8_t IIC_Read_Byte(unsigned char ack)
// *功　　能:	    //读1串字节，ack=1时，发送ACK，ack=0，发送nACK
// *******************************************************************************/
// static unsigned char IIC_Read_Byte(void)  
// {
//     uint8_t i=8;
//     uint8_t ReceiveByte=0;

//     SDA_H;
//     while(i--)
//     {
// 			ReceiveByte<<=1;
// 			SCL_L;
// 			IIC_delay();
// 			SCL_H;
// 			IIC_delay();
// 			if(SDA_read)
// 			{
// 				ReceiveByte|=0x01;
// 			}
//     }
//     SCL_L;
//     return ReceiveByte;
// }

void IIC_Delay_ms(uint32_t ms)
{
	Delay_ms(ms);
}

uint32_t IIC_Get_ms(void)
{
	return Get_ms();
}
/**************************实现函数********************************************
*函数原型:		unsigned char IIC_ReadOneByte(unsigned char I2C_Addr,unsigned char addr)
*功　　能:	    读取指定设备 指定寄存器的一个值
输入	I2C_Addr  目标设备地址
		addr	   寄存器地址
返回   读出来的值
*******************************************************************************/

uint8_t IIC_ReadOneByte(uint8_t SlaveAddress,uint16_t REG_Address,uint8_t* data)
{
	int i2c_time_out = I2C_TIME_OUT_BASE + I2C_TIME_OUT_BYTE;
	unsigned int cnt = 2;
	unsigned int start = 0;
	unsigned int cur = 0;
	uint8_t reg_buf[2];
	const uint8_t *ptr = reg_buf;
	unsigned int got = 0;

	if (!data)
		return FALSE;

	reg_buf[0] = (uint8_t)(REG_Address >> 8);
	reg_buf[1] = (uint8_t)(REG_Address & 0x00ff);

	start = Get_ms();

	DL_I2C_clearInterruptStatus(I2C_0_INST, DL_I2C_INTERRUPT_CONTROLLER_TX_DONE);

	while (!(DL_I2C_getControllerStatus(I2C_0_INST) & DL_I2C_CONTROLLER_STATUS_IDLE));

	DL_I2C_startControllerTransfer(I2C_0_INST, (SlaveAddress >> 1),
		DL_I2C_CONTROLLER_DIRECTION_TX, cnt);

	do {
		unsigned int fillcnt;
		fillcnt = DL_I2C_fillControllerTXFIFO(I2C_0_INST, ptr, cnt);
		cnt -= fillcnt;
		ptr += fillcnt;

		cur = Get_ms();
		if ((unsigned int)(cur - start) >= (unsigned int)i2c_time_out)
			return FALSE;
	} while (!DL_I2C_getRawInterruptStatus(I2C_0_INST, DL_I2C_INTERRUPT_CONTROLLER_TX_DONE));

	start = Get_ms();
	DL_I2C_clearInterruptStatus(I2C_0_INST, DL_I2C_INTERRUPT_CONTROLLER_RX_DONE);

	while (!(DL_I2C_getControllerStatus(I2C_0_INST) & DL_I2C_CONTROLLER_STATUS_IDLE));

	DL_I2C_startControllerTransfer(I2C_0_INST, (SlaveAddress >> 1),
		DL_I2C_CONTROLLER_DIRECTION_RX, 1);

	do {
		if (!DL_I2C_isControllerRXFIFOEmpty(I2C_0_INST)) {
			*data = DL_I2C_receiveControllerData(I2C_0_INST);
			got = 1;
		}

		cur = Get_ms();
		if ((unsigned int)(cur - start) >= (unsigned int)i2c_time_out)
			return FALSE;
	} while (!DL_I2C_getRawInterruptStatus(I2C_0_INST, DL_I2C_INTERRUPT_CONTROLLER_RX_DONE));

	if (!got && !DL_I2C_isControllerRXFIFOEmpty(I2C_0_INST)) {
		*data = DL_I2C_receiveControllerData(I2C_0_INST);
		got = 1;
	}

	return got ? TRUE : FALSE;
}


/**************************实现函数********************************************
*函数原型:		uint8_t IICreadBytes(uint8_t dev, uint8_t reg, uint8_t length, uint8_t *data)
*功　　能:	    读取指定设备 指定寄存器的 length个值
输入	dev  目标设备地址
		reg	  寄存器地址
		length 要读的字节数
		*data  读出的数据将要存放的指针
返回   读出来的字节数量
*******************************************************************************/
uint8_t IICreadBytes(uint8_t SlaveAddress,uint16_t REG_Address,uint8_t len,uint8_t *data)
{
	int i2c_time_out = I2C_TIME_OUT_BASE + (int)len * I2C_TIME_OUT_BYTE;
	unsigned int cnt = 2;
	unsigned int start = 0;
	unsigned int cur = 0;
	unsigned int i = 0;
	uint8_t reg_buf[2];
	const uint8_t *ptr = reg_buf;

	if (!data && len)
		return FALSE;

	reg_buf[0] = (uint8_t)(REG_Address >> 8);
	reg_buf[1] = (uint8_t)(REG_Address & 0x00ff);

	start = Get_ms();

	DL_I2C_clearInterruptStatus(I2C_0_INST, DL_I2C_INTERRUPT_CONTROLLER_TX_DONE);

	while (!(DL_I2C_getControllerStatus(I2C_0_INST) & DL_I2C_CONTROLLER_STATUS_IDLE));

	DL_I2C_startControllerTransfer(I2C_0_INST, (SlaveAddress >> 1),
		DL_I2C_CONTROLLER_DIRECTION_TX, cnt);

	do {
		unsigned int fillcnt;
		fillcnt = DL_I2C_fillControllerTXFIFO(I2C_0_INST, ptr, cnt);
		cnt -= fillcnt;
		ptr += fillcnt;

		cur = Get_ms();
		if ((unsigned int)(cur - start) >= (unsigned int)i2c_time_out)
			return FALSE;
	} while (!DL_I2C_getRawInterruptStatus(I2C_0_INST, DL_I2C_INTERRUPT_CONTROLLER_TX_DONE));

	start = Get_ms();

	DL_I2C_clearInterruptStatus(I2C_0_INST, DL_I2C_INTERRUPT_CONTROLLER_RX_DONE);

	while (!(DL_I2C_getControllerStatus(I2C_0_INST) & DL_I2C_CONTROLLER_STATUS_IDLE));

	DL_I2C_startControllerTransfer(I2C_0_INST, (SlaveAddress >> 1),
		DL_I2C_CONTROLLER_DIRECTION_RX, len);

	do {
		if (!DL_I2C_isControllerRXFIFOEmpty(I2C_0_INST)) {
			uint8_t c = DL_I2C_receiveControllerData(I2C_0_INST);
			if (i < len) {
				data[i] = c;
				++i;
			}
		}

		cur = Get_ms();
		if ((unsigned int)(cur - start) >= (unsigned int)i2c_time_out)
			return FALSE;
	} while (!DL_I2C_getRawInterruptStatus(I2C_0_INST, DL_I2C_INTERRUPT_CONTROLLER_RX_DONE));

	while (i < len && !DL_I2C_isControllerRXFIFOEmpty(I2C_0_INST)) {
		data[i] = DL_I2C_receiveControllerData(I2C_0_INST);
		++i;
	}

	return (i == len) ? len : FALSE;
}

/**************************实现函数********************************************
*函数原型:		uint8_t IICwriteBytes(uint8_t dev, uint8_t reg, uint8_t length, uint8_t* data)
*功　　能:	    将多个字节写入指定设备 指定寄存器
输入	dev  目标设备地址
		reg	  寄存器地址
		length 要写的字节数
		*data  将要写的数据的首地址
返回   返回写入长度
*******************************************************************************/
uint8_t IICwriteBytes(uint8_t dev, uint16_t reg, uint16_t length, uint8_t* data)
{
	int i2c_time_out = I2C_TIME_OUT_BASE + (int)length * I2C_TIME_OUT_BYTE;
	unsigned int start = 0;
	unsigned int cur = 0;
	unsigned int header_remaining = 2;
	unsigned int data_remaining = length;
	unsigned int total_remaining = (unsigned int)length + 2;
	uint8_t header[2];
	const uint8_t *header_ptr = header;
	const uint8_t *data_ptr = data;

	if (!data && length)
		return FALSE;

	header[0] = (uint8_t)(reg >> 8);
	header[1] = (uint8_t)(reg & 0x00ff);

	start = Get_ms();

	DL_I2C_clearInterruptStatus(I2C_0_INST, DL_I2C_INTERRUPT_CONTROLLER_TX_DONE);

	while (!(DL_I2C_getControllerStatus(I2C_0_INST) & DL_I2C_CONTROLLER_STATUS_IDLE));

	DL_I2C_startControllerTransfer(I2C_0_INST, (dev >> 1),
		DL_I2C_CONTROLLER_DIRECTION_TX, total_remaining);

	do {
		unsigned int fillcnt;

		if (header_remaining) {
			fillcnt = DL_I2C_fillControllerTXFIFO(I2C_0_INST, header_ptr, header_remaining);
			header_remaining -= fillcnt;
			header_ptr += fillcnt;
			total_remaining -= fillcnt;
		}

		if (data_remaining) {
			fillcnt = DL_I2C_fillControllerTXFIFO(I2C_0_INST, data_ptr, data_remaining);
			data_remaining -= fillcnt;
			data_ptr += fillcnt;
			total_remaining -= fillcnt;
		}

		cur = Get_ms();
		if ((unsigned int)(cur - start) >= (unsigned int)i2c_time_out)
			return FALSE;
	} while (!DL_I2C_getRawInterruptStatus(I2C_0_INST, DL_I2C_INTERRUPT_CONTROLLER_TX_DONE));

	return length; //status == 0;
}


/**************************实现函数********************************************
*函数原型:		unsigned char IICwriteByte(unsigned char dev, unsigned char reg, unsigned char data)
*功　　能:	    写入指定设备 指定寄存器一个字节
输入	dev  目标设备地址
		reg	   寄存器地址
		data  将要写入的字节
返回   1
*******************************************************************************/
uint8_t IICwriteByte(uint8_t dev, uint16_t reg, uint8_t data)
{
    return IICwriteBytes(dev, reg, 1, &data);
}


/**************************实现函数********************************************
*函数原型:		uint8_t IICwriteBit(uint8_t dev, uint8_t reg, uint8_t bitNum, uint8_t data)
*功　　能:	    读 修改 写 指定设备 指定寄存器一个字节 中的1个位
输入	dev  目标设备地址
		reg	   寄存器地址
		bitNum  要修改目标字节的bitNum位
		data  为0 时，目标位将被清0 否则将被置位
返回   成功 为1
 		失败为0
*******************************************************************************/
uint8_t IICwriteBit(uint8_t dev, uint16_t reg, uint8_t bitNum, uint8_t data)
{
    uint8_t b;
    IIC_ReadOneByte(dev, reg, &b);
    b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
    return IICwriteByte(dev, reg, b);
}

//------------------End of File----------------------------
