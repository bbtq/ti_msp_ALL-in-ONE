
#include "sw_i2c_interface.h"
#include "bsp_mpu6050.h"

/* ����sda������� bit=0Ϊ�͵�ƽ bit=1Ϊ�ߵ�ƽ */
void sw_sda_out(uint8_t bit, void *user_data)
{   
    SDA_OUT();
    SDA(bit);
//	GPIO_WriteBit(GPIOB, SW_I2C1_PIN_SDA, (BitAction)bit);      //stm32 ��׼��
	
	/* IIC����ӳ� */
	delay_us(10);
}

/* ����sda��ȡ���� bit Ϊ���صĵ�ƽֵ */
uint8_t sw_sda_in(void *user_data)
{
	uint8_t bit;
    SDA_IN();
    bit = SDA_GET();
//	bit = (uint8_t)GPIO_ReadInputDataBit(GPIOB, SW_I2C1_PIN_SDA);      //stm32 ��׼��
	
	/* IIC����ӳ� */
	delay_us(10);
	return bit;
}

/* ����sclʱ��������� bit=0Ϊ�͵�ƽ bit=1Ϊ�ߵ�ƽ */
void sw_scl_out(uint8_t bit, void *user_data)
{
    
    SCL(bit);
//	GPIO_WriteBit(GPIOB, SW_I2C1_PIN_SCL, (BitAction)bit);      //stm32 ��׼��
	
	/* IIC����ӳ� */
	delay_us(10);
}









