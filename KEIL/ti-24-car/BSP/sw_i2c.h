
/*
 * ͨ�����IICʹ�÷�ʽ
 * 1.������Ҫ������������
 * 
 * ����bit=0 ����bit=1 ��SDA��������Ӧ�ĵ�ƽ��Ȼ������Լ���Ҫ��IICƵ��delay N us
 * void sda_out(uint8_t bit, void *user_data)
 *
 * ��ȡSDA��ƽֵȻ�󷵻ص͵�ƽ0 �����Ǹߵ�ƽ1��Ȼ������Լ���Ҫ��IICƵ��delay N us
 * uint8_t sda_in(void *user_data)
 *
 * ����bit=0 ����bit=1 ��SCL��������Ӧ�ĵ�ƽ��Ȼ������Լ���Ҫ��IICƵ��delay N us
 * void scl_out(uint8_t bit, void *user_data)
 *
 * 2. �����涨��ĺ��� ���� sw_i2c_interface_t
 * 3. Ȼ��Ϳ��԰� sw_i2c_interface_t ��������ʹ����
 */

#ifndef _SW_I2C_H_
#define _SW_I2C_H_


#include <stdint.h>

typedef struct {
	void (*sda_out)(uint8_t bit, void *user_data);
	uint8_t (*sda_in)(void *user_data);
	void (*scl_out)(uint8_t bit, void *user_data);
	void *user_data;
} sw_i2c_interface_t;

/**
 * @brief ��IIC�����ϵ��豸��ȡ����ֽ�
 * @param i2c_interface
 * @param dev_addr ���豸��ַ
 * @param[out] data ��ȡ�����ֽ�����
 * @param data_length ��ȡ��С(�ֽ�)
 * @return 0:�ɹ�, 1:����
 */
int8_t sw_i2c_read(sw_i2c_interface_t *i2c_interface, uint8_t dev_addr, uint8_t *data, uint8_t data_length);
int8_t sw_i2c_write(sw_i2c_interface_t *i2c_interface, uint8_t dev_addr, const uint8_t *data, uint8_t data_length);

/**
 * @brief ��IIC�����ϵ��豸��ȡһ���ֽ�
 * @param i2c_interface
 * @param dev_addr ���豸��ַ
 * @param[out] data ��ȡ�����ֽ�
 * @return 0:�ɹ�, 1:����
 */
int8_t sw_i2c_read_byte(sw_i2c_interface_t *i2c_interface, uint8_t dev_addr, uint8_t *data);
int8_t sw_i2c_write_byte(sw_i2c_interface_t *i2c_interface, uint8_t dev_addr, const uint8_t data);

/**
 * @brief ��ȡIIC���ߴ��豸�ļĴ�������. ����д��Ĵ�����ַ,����ֹλ,��������ȡ��������
 * @param i2c_interface
 * @param dev_addr ���豸��ַ
 * @param mem_addr �Ĵ�����ַ
 * @param[out] data ��ȡ�����ֽ�����
 * @param data_length ��ȡ��С(�ֽ�),�������Ĵ�����ַ����
 * @return 0:�ɹ�, 1:����
 */
int8_t sw_i2c_mem_read(sw_i2c_interface_t *i2c_interface, uint8_t dev_addr, uint8_t mem_addr, uint8_t *data, uint8_t data_length);

/**
 * @brief д��IIC���ߴ��豸�ļĴ���. ����д��Ĵ�����ַ,������д�������е�����
 * @param i2c_interface
 * @param dev_addr ���豸��ַ
 * @param mem_addr �Ĵ�����ַ
 * @param[out] data ����д�������
 * @param data_length ��д����ֽڴ�С,�������Ĵ�����ַ����
 * @return 0:�ɹ�, 1:����
 */
int8_t sw_i2c_mem_write(sw_i2c_interface_t *i2c_interface, uint8_t dev_addr, uint8_t mem_addr, const uint8_t *data, uint8_t data_length);

/**
 * i2c��ַɨ��
 * @param scan_addr ɨ������ĵ�ַ���,��ֵ��Ϊ0��Ϊɨ�赽�ĵ�ַ��ɨ���ĵ�ַ�ᰤ�������������ǰ��
 * @return ����ɨ�赽���豸����, 0Ϊ���豸����
 */
uint8_t i2c_scan(sw_i2c_interface_t *i2c_interface, uint8_t *scan_addr);



#endif
