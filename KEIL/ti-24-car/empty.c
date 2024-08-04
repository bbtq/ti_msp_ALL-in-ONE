
#include "board.h"
#include <stdio.h>
#include "bsp_mpu6050.h"
#include "inv_mpu.h"
#include "bsp_ultrasonic.h"
#include "motor.h"
#include "bsp_adc.h"
#include "pid.h"
#include "string.h"
#include "bsp_Filter.h"
#include "gw_grayscale_sensor.h"
#include "sw_i2c_interface.h"
#include "sw_i2c.h"

#define motor_BasePower 1000

char cmd_stop[] = "stop\n";
char cmd_start[] = "start\n";
char cmd_power_up[] = "pup\n";
char cmd_power_down[] = "pdown\n";
char cmd_pid_kp_up[] = "kp_up\n";
char cmd_pid_kp_down[] = "kp_down\n";
char cmd_pid_kd_up[] = "kd_up\n";
char cmd_pid_kd_down[] = "kd_down\n";
char cmd_show_adc[] = "adc\n";
char cmd_show_out[] = "out\n";

PIDController pid = {
    .Kp = 0.9,
    .Ki = 0,
    .Kd = 0.5,

    //
    .tau = 2,

    // �������
    .limMax = 3999,
    .limMin = -3999 - motor_BasePower, // ��ȥ����������

    // ����������
    .limMaxInt = 0.1 * 4000,
    .limMinInt = -0.1 * 4000 - motor_BasePower, // ��ȥ����������

    // ������Ʒ�ɼ�ʱ�� ��λ��s
    .T = 50 / 1000000,

};

int main(void)
{

    /**
    **  ������
    **/
    // uint8_t ret = 1;
    // float pitch=0,roll=0,yaw=0;   //ŷ����

    /**
    **  adcģ��������
    **/
    uint16_t adc_buf[2] = {0, 0};
    unsigned int voltage_value = 0;

/**
**  ��Ϊ�Ƽ�ѭ��ģ��i2c
**/
    /* ���ɨ�赽�ĵ�ַ */
    uint8_t scan_addr[128] = {0};
    volatile uint8_t count;
    uint8_t ping_response;
    /* �������IIC���� */
    sw_i2c_interface_t i2c_interface =
        {
            .sda_in = sw_sda_in,
            .scl_out = sw_scl_out,
            .sda_out = sw_sda_out,
            .user_data = 0, // �û����ݣ������������������õ�
        };

// �������ʼ��
    board_init();

// 6�������ǳ�ʼ��
    //  MPU6050_Init();

// ��������ʼ��
    //  Ultrasonic_Init();
// ��������������
    //  Hcsr04Start();

// sw_i2c����
    /* ��һ��IICͨѶ��ʧ�ܣ���Ϊ���IIC������start�����ֶ�����stopҲ������ */
    sw_i2c_mem_read(&i2c_interface, 0x4C << 1, GW_GRAY_PING, &ping_response, 1);
    /* ����IICͨѶ�������� */
    sw_i2c_mem_read(&i2c_interface, 0x4C << 1, GW_GRAY_PING, &ping_response, 1);

    /* ɨ�迪ʼ */
    count = i2c_scan(&i2c_interface, scan_addr);
//��Ϊ�Ƽ�ѭ��ģ���ʼ��ȡ������
    /* ��ȡģ�������� */    
    uint8_t analog_data[8]; // analog_data[0:7] ��1~8��̽ͷģ������
    /* ��ģ��ֵģʽ, ���Ҷ�ȡģ����ֵ, �������ֱ�Ӷ�ȡ */
    sw_i2c_mem_read(&i2c_interface, 0x4C << 1, GW_GRAY_ANALOG_MODE, analog_data, 8);
    /* ֱ�Ӷ�ȡ */
    sw_i2c_read(&i2c_interface, 0x4C << 1, analog_data, 8);

// ����ADC�ж�
    NVIC_EnableIRQ(adc_INST_INT_IRQN);
// ����adcת��
    adc_start();
// ������������ʼ��
    kalman_init(&kf_adc1, 10, 30, 10, 0);
    kalman_init(&kf_adc2, 10, 30, 10, 0);

// PID������ʼ��
    PIDController_Init(&pid);

    // DMP��ʼ��
    //  while( mpu_dmp_init() )
    //  {
    //  printf("dmp error\r\n");
    //  delay_ms(200);
    // }
    // printf("Initialization Data Succeed \r\n");

    //    motor_control(600,0);

//��ӡstart����ʼ����ɣ�����ʼ
    printf("start\r\n");

    while (1)
    {
        /* ��ȡģ�������� ����1msһ��*/
        sw_i2c_read(&i2c_interface, 0x4C << 1, analog_data, 8);
        delay_us(1000);

        // �����������ģ��ѭ��ģ���adcת��
        if (gCheckADC1 && gCheckADC2)
        {
            gCheckADC1 = false;
            gCheckADC2 = false;
            adc_getValue(adc_buf);
            adc_buf[0] = median_filter(adc_buf[0]);
            adc_buf[1] = median_filter(adc_buf[1]);
            PIDController_Update(&pid, 0.0, (float)(adc_buf[0] - adc_buf[1]));
            motor_control(motor_BasePower + pid.out, motor_BasePower - pid.out);
            adc_start();
            // printf("\r\nadc  : %d   %d\r\n",adc_buf[0],adc_buf[1]);
            // printf("adc  dif : %d \r\n",(adc_buf[0]-adc_buf[1]));
            //            printf("pid out : %d \r\n",(int)pid.out);
        }

        /*
        //ʹ��mpu6050�ٷ���dmp��ȡ�Ƕ�
        //�������õĲ����ʣ�����������ʱ����
        //��ȡŷ����
        u8 error = mpu_dmp_get_data(&pitch,&roll,&yaw);
        if( error == 0 )
        {
            printf("\r\npitch =%d\r\n", (int)pitch);
            printf("\r\nroll =%d\r\n", (int)roll);
            printf("\r\nyaw =%d\r\n", (int)yaw);
            delay_ms(19);
        }
        else printf("\r\nerror = %d\r\n",error);
        */

        /*
        //������ģ����
        if(SR04_LEN_GET_FLAG){
        uint32_t Value = (int)Hcsr04GetLength();
        printf((const char *)"Distance = %dCM\r\n", Value);
        Hcsr04Start();
        }
        */

        /**
         * ���ڵ��Բ��֣�������hc06����Զ�̵��أ�
         */
        if (Serial_RxFlag == 1)
        {
            Serial_RxFlag = 0;

            // С����ͣ
            if (memcmp(Serial_RxPacket, cmd_stop, strlen((char *)cmd_stop)) == 0)
            {
                defaultwheel.left.power_multiple = 0.0f;
                defaultwheel.right.power_multiple = 0.0f;
                printf("stop\r\n");
            }
            else if (memcmp(Serial_RxPacket, cmd_start, strlen((char *)cmd_start)) == 0)
            {
                defaultwheel.left.power_multiple = 0.5f;
                defaultwheel.right.power_multiple = 0.5f;
                printf("start\r\n");
            }

            // С��������
            else if (memcmp(Serial_RxPacket, cmd_power_up, strlen((char *)cmd_power_up)) == 0)
            {
                defaultwheel.left.power_multiple += 0.1f;
                defaultwheel.right.power_multiple += 0.1f;
                printf("power_up: %f\r\n", defaultwheel.left.power_multiple);
            }
            else if (memcmp(Serial_RxPacket, cmd_power_down, strlen((char *)cmd_power_down)) == 0)
            {
                defaultwheel.left.power_multiple -= 0.1f;
                defaultwheel.right.power_multiple -= 0.1f;
                printf("power_down: %f\r\n", defaultwheel.left.power_multiple);
            }

            // PID�������ڲ���
            else if (memcmp(Serial_RxPacket, cmd_pid_kp_up, strlen((char *)cmd_pid_kp_up)) == 0)
            {
                pid.Kp += 0.1f;
                printf("p_up: %f\r\n", pid.Kp);
            }
            else if (memcmp(Serial_RxPacket, cmd_pid_kp_down, strlen((char *)cmd_pid_kp_down)) == 0)
            {
                pid.Kp -= 0.1f;
                printf("p_down: %f\r\n", pid.Kp);
            }
            else if (memcmp(Serial_RxPacket, cmd_pid_kd_up, strlen((char *)cmd_pid_kd_up)) == 0)
            {
                pid.Kd += 0.1f;
                printf("d_up: %f\r\n", pid.Kd);
            }
            else if (memcmp(Serial_RxPacket, cmd_pid_kd_down, strlen((char *)cmd_pid_kd_down)) == 0)
            {
                pid.Kd -= 0.1f;
                printf("d_down: %f\r\n", pid.Kd);
            }

            // pid������
            else if (memcmp(Serial_RxPacket, cmd_show_out, strlen((char *)cmd_show_out)) == 0)
            {
                printf("pid out : %d \r\n", (int)pid.out);
            }

            // ��·adcѭ��������ȡ
            else if (memcmp(Serial_RxPacket, cmd_show_adc, strlen((char *)cmd_show_adc)) == 0)
            {
                printf("left: %d right: %d\r\n", adc_buf[0], adc_buf[1]);
            }

            // �������ָ��
            else
            {
                printf("error send data\r\n");
            }
        }
    }
}
