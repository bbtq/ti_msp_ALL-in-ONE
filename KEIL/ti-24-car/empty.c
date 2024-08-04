
#include "board.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "motor.h"
#include "bsp_adc.h"
#include "pid.h"
#include "string.h"
#include "bsp_Filter.h"
#include "bsp_jy901.h"
#include "sw_i2c.h"
#include "gw_grayscale_sensor.h"
#include "trace.h"
#include "i2c_app.h"


int motor_BasePower = 1300; // 1500//1m 2.68s

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

typedef struct{
    int goStraight_plus ;
    int trace_line_plus ;
}score_increases_peed ;

PIDController pid = {
    // ����
    .Kp = 0.9,
    .Ki = 0,
    .Kd = 0.5,

    // �������
    .limMax = 3999,
    .limMin = 0, // ֻ����ǰ

    // ����������
    .limMaxInt = 0.1 * 4000,
    .limMinInt = -0.1 * 4000 - 1500, // ��ȥ����������

};

PIDController trace_pid = {
    // ѭ��pid
    .Kp = 1.5, // 0.8
    .Ki = 0.1,
    .Kd = 1.7, // 0.5

    // �������
    .limMax = 2000,
    .limMin = -2000 - 1500, // ��ȥ����������

    // ����������
    .limMaxInt = 0.1 * 4000,
    .limMinInt = -0.1 * 4000 - 1500, // ��ȥ����������

};

PIDController yaw_pid = {
    // �Ƕ�pid
    .Kp = 5,
    .Ki = 0,
    .Kd = 4,

    // �������
    .limMax = 3000,
    .limMin = -3000 - 1500, // ��ȥ����������

    // ����������
    .limMaxInt = 300,
    .limMinInt = -300, // ��ȥ����������

};

bool event_lock = 1;    // �¼�������
uint8_t event_task = 0; // �����еĸ���С�����
bool nearingTheEnd = 0; // ���һ������ı�־
uint8_t ground = 0;     // Ȧ��
score_increases_peed task4_power_plus = {
    .goStraight_plus = 0,
    .trace_line_plus = 0,
};   //����4���ٲ���
int nearget_downpower = 0;
bool break_noout = 1;

bool distance_1m_flag = 0;    // 1m����ִ��
bool distance_1m_receive = 0; // 1m��ɱ�־
uint16_t turn_on_first = 0;

bool trace_flag = 0;    // ѭ��ִ��
bool line_no_check = 0; // �޺���
float yaw_step = 180.0;
int end_power_decelerate = 0;   //�����٣�����׼ȷ��
int start_power_decelerate = 0; //�ʼ�ļ��٣������ݴ� task4 
uint32_t trace_decelerate_time = 0; //����ʱ�䡣

bool yaw_control_flag = 0;
float yaw_expt = 0;

void turn_dirction(void)
{
    base_direction += 180;
    if (base_direction < -180)
        base_direction = base_direction + 360;
    if (base_direction > 180)
        base_direction = base_direction - 360;
}

int main(void)
{

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

    // �������ʼ��
    board_init();

    // ��Ϊ�Ƽ�ѭ��ģ���ʼ��ȡ������
    /*��ȡ������*/
    uint8_t gray_sensor[8];
    uint8_t digital_data = 8;
    /*��������ģʽ�����Ҷ�ȡ���������ֱ�Ӷ�ȡ*/
    digital_data = GW_GRAY_DIGITAL_MODE;
    i2c_app_write(I2C0_INST, 0x4C, &digital_data, 1);
    /*��ȡ����*/
    i2c_app_read(I2C0_INST, 0x4C, &digital_data, 1);

    // // ������������ʼ��
    //     kalman_init(&kf_adc1, 10, 30, 10, 0);
    //     kalman_init(&kf_adc2, 10, 30, 10, 0);

    // PID������ʼ��
    PIDController_Init(&pid);
    PIDController_Init(&trace_pid);
    PIDController_Init(&yaw_pid);
    int trace_out = 0;
    int base_out = 200; // Ҫ��3��תһ���ǶȵĻ�������
    // Total_PID_Init();

    // motor_control(-1500,-1500);
    // motor_control(500,1500);

    // ��ӡstart����ʼ����ɣ�����ʼ
    printf("start\r\n");

    while (1)
    {

        /*�¼�����*/
        if (start_flag)
        {
            // trace_flag = 1;
            if (event_lock)
            {
                event_lock = 0;
                if (task == 1)
                {
                    if (event_task == 0)
                    { // 1m������-ֱ��
                        defaultwheel.left.count = 0;
                        distance_1m_flag = 1;
                        yaw_expt = base_direction;
                    }
                    if (event_task == 1)
                    {
                        if (distance_1m_receive)
                        { //
                            sound_flag = 1;
                            distance_1m_flag = 0;
                            motor_control(0, 0);
                        }
                    }
                    event_task++;
                }
                if (task == 2)
                {
                    if (event_task == 0)
                    { // 1m������-ֱ��
                        defaultwheel.left.count = 0;
                        distance_1m_flag = 1;
                        yaw_expt = base_direction;
                    }
                    if (event_task == 1)
                    { // ѭ��
                        // turn_dirction(); ����
                        
                        PIDController_Init(&trace_pid);
                        if (distance_1m_receive)
                        {
                            while(jy901_yaw<yaw_return(base_direction+10))motor_control(1500,700);
                            trace_flag = 1;
                            distance_1m_flag = 0;
                            defaultwheel.left.count = 0;
                            sound_flag = 1;
                            yaw_expt = base_BackDirection;
                        }
                    }
                    if (event_task == 2)
                    {
                        if (line_no_check)
                        { // �ǶȽ���
                            event_task++;
                            motor_control(300, 500); // �ȳ�ȥһ�㣬�����߰�Բ���ĽǶ�
                            delay_ms(20);
                        }
                    }
                    if (event_task == 3)
                    {
                        if (yaw_control_flag == 0)
                        { // �Ƕ��Ѱ�������1m����
                            trace_flag = 0;
                            distance_1m_flag = 1;
                            defaultwheel.left.count = 0;
                            sound_flag = 1;
                            yaw_expt = base_BackDirection; // ���̶��������΢���ң��������߼���
                            yaw_pid.prevError = 0;
                            yaw_pid.prevMeasurement = yaw_expt;
                        }
                    }
                    if (event_task == 4)
                    { // ѭ��
                        // turn_dirction(); ����

                        PIDController_Init(&trace_pid);
                        if (distance_1m_receive)
                        {
                            while(jy901_yaw<yaw_return(base_BackDirection+10))motor_control(1500,700);
                            sound_flag = 1;
                            trace_flag = 1;
                            distance_1m_flag = 0;
                            defaultwheel.left.count = 0;

                            yaw_expt = base_direction;
                            nearingTheEnd = 1;
                        }
                    }
                    if (event_task == 5)
                    { // ֹͣ
                        if (line_no_check)
                        {
                            trace_flag = 0;
                            distance_1m_flag = 0;
                            defaultwheel.left.count = 0;
                            sound_flag = 1;
                            motor_control(0, 0);
                            nearingTheEnd = 0;
                        }
                    }

                    event_task++;
                }
                if (task == 3)
                {
                    if (event_task == 0)
                    { // �ǶȽ��� ____ 38.66
                        yaw_expt = yaw_return(base_direction + 39.76);
                        while (fabs(jy901_yaw - yaw_expt) > 3)
                            motor_control(950, -950);
                        motor_control(0, 0);
                        event_task++;
                    }
                    if (event_task == 1)
                    { // ������-ֱ��
                        defaultwheel.left.count = 0;
                        distance_1m_flag = 1;
                        sound_flag = 1;
                        yaw_expt = yaw_return(base_direction + 38.70.6);
                        yaw_pid.prevError = 0;
                        yaw_pid.prevMeasurement = yaw_expt;
                    }
                    if (event_task == 2)
                    { // ѭ��
                        // turn_dirction(); ����
                        sound_flag = 1;
                        yaw_expt = yaw_return(base_direction - 3);
                        while (fabs(jy901_yaw - yaw_expt) > 3)
                            motor_control(-450, 1050);
                        motor_control(500, 500);
                        PIDController_Init(&trace_pid);
                        if (distance_1m_receive)
                        {
                            trace_flag = 1;
                            distance_1m_flag = 0;
                            defaultwheel.left.count = 0;

                            yaw_expt = base_BackDirection;
                        }
                    }
                    if (event_task == 3)
                    {
                        if (line_no_check)
                        { // �ǶȽ��� ____ 38.66
                            trace_flag = 0;
                            distance_1m_flag = 0;
                            defaultwheel.left.count = 0;
                            sound_flag = 1;
                            yaw_expt = yaw_return(base_BackDirection - 38.76);
                            while (fabs(jy901_yaw - yaw_expt) > 3)
                                motor_control(-950, 950);
                            // printf("angle ready: %d, back_dir : %d\r\n",(int)yaw_expt,(int)base_BackDirection);
                            motor_control(0, 0);
                            event_task = 4;
                        }
                    }
                    if (event_task == 4)
                    { // ������-ֱ��
                        defaultwheel.left.count = 0;
                        trace_flag = 0;
                        distance_1m_flag = 1;
                        sound_flag = 1;
                        yaw_expt = yaw_return(base_BackDirection - 38.76);
                        yaw_pid.prevError = 0;
                        yaw_pid.prevMeasurement = yaw_expt;
                        // printf("go: %d\r\n",(int)yaw_expt);
                    }
                    if (event_task == 5)
                    { // ѭ��
                        // turn_dirction(); ����
                        sound_flag = 1;
                        yaw_expt = yaw_return(base_BackDirection + 3);
                        while (fabs(jy901_yaw - yaw_expt) > 3)
                            motor_control(1050, -950);
                        motor_control(0, 0);
                        PIDController_Init(&trace_pid);
                        if (distance_1m_receive)
                        {
                            trace_flag = 1;
                            distance_1m_flag = 0;
                            defaultwheel.left.count = 0;

                            yaw_expt = base_direction;
                            nearingTheEnd = 1;
                        }
                    }
                    if (event_task == 6)
                    { // ֹͣ
                        if (line_no_check)
                        {
                            trace_flag = 0;
                            distance_1m_flag = 0;
                            defaultwheel.left.count = 0;
                            sound_flag = 1;
                            motor_control(0, 0);
                            nearingTheEnd = 0;
                        }
                    }
                    event_task++;
                }
                if (task == 4)
                {
                    if (event_task == 0)
                    { // �ǶȽ��� ____ 38.66
                        task4_power_plus.goStraight_plus = 1700;     //17:15  - - - - old : 900
                        task4_power_plus.trace_line_plus = 1000; //17:15  - - - - old : 600
                        trace_pid.Kp += 0.3;
                        trace_pid.Kd += 1.4;

                        trace_flag = 0;
                        distance_1m_flag = 0;
                        defaultwheel.left.count = 0;
                        sound_flag = 1;
                        yaw_expt = yaw_return(base_direction + 38.76);
                        while (fabs(jy901_yaw - yaw_expt) > 3)
                            motor_control(950, -950);
                        motor_control(0, 0);
                        event_task++;
                    }
                    if (event_task == 1)
                    { // ������-ֱ��
                        defaultwheel.left.count = 0;
                        distance_1m_flag = 1;
                        sound_flag = 1;
                        yaw_expt = yaw_return(base_direction + 38.76);
                        yaw_pid.prevError = 0;
                        yaw_pid.prevMeasurement = yaw_expt;
                    }
                    if (event_task == 2)
                    { // ѭ��
                        // turn_dirction(); ����
                        sound_flag = 1;
                        yaw_expt = yaw_return(base_direction-1);
                        while (fabs(jy901_yaw - yaw_expt) > 3)
                            motor_control(600, 1600);

                        // motor_control(0,0);
                        // delay_ms(10);
                        // // i2c_app_read(I2C0_INST, 0x4C, &digital_data, 1);
                        // while(1) {
                        //     if(!i2c_app_read(I2C0_INST, 0x4C, &digital_data, 1)) break;
                        //     delay_ms(20);}
                        // if(trace_check(digital_data)>=0){
                        //     while(break_noout)
                        //     {
                        //         motor_control(650, 1700);
                        //         if(sys_time_1ms%10){
                        //             if(!i2c_app_read(I2C0_INST, 0x4C, &digital_data, 1)){
                        //                 if(trace_check(digital_data)>20)
                        //                     break_noout = 0;
                        //             }
                        //         }
                        //     }
                        // }
                        // break_noout = 1;

                        motor_control(500, 500);
                        PIDController_Init(&trace_pid);
                        if (distance_1m_receive)
                        {
                            trace_flag = 1;
                            distance_1m_flag = 0;
                            defaultwheel.left.count = 0;

                            yaw_expt = base_BackDirection;
                        }
                    }
                    if (event_task == 3)
                    {
                        if (line_no_check)
                        { // �ǶȽ��� ____ 38.66
                            trace_flag = 0;
                            distance_1m_flag = 0;
                            defaultwheel.left.count = 0;
                            sound_flag = 1;
                            yaw_expt = yaw_return(base_BackDirection -38.76);
                            while (fabs(jy901_yaw - yaw_expt) > 3)
                                motor_control(-950, 950);
                            // printf("angle ready: %d, back_dir : %d\r\n",(int)yaw_expt,(int)base_BackDirection);
                            motor_control(0, 0);
                            event_task = 4;
                        }
                    }
                    if (event_task == 4)
                    { // ������-ֱ��
                        defaultwheel.left.count = 0;
                        trace_flag = 0;
                        distance_1m_flag = 1;
                        sound_flag = 1;
                        yaw_expt = yaw_return(base_BackDirection - 38.76);
                        yaw_pid.prevError = 0;
                        yaw_pid.prevMeasurement = yaw_expt;
                        // printf("go: %d\r\n", (int)yaw_expt);
                    }
                    if (event_task == 5)
                    { // ѭ��
                        // turn_dirction(); ����
                        sound_flag = 1;
                        yaw_expt = yaw_return(base_BackDirection+1);
                        // while (fabs(jy901_yaw - yaw_expt) > 1)
                        //     motor_control(1700, 650);

                        while (fabs(jy901_yaw - yaw_expt) > 3 ){
                            motor_control(1700, 650);
                        }

                        // motor_control(0,0);
                        // delay_ms(10);
                        // while(1) {
                        //     if(!i2c_app_read(I2C0_INST, 0x4C, &digital_data, 1)) break;
                        //     printf("trace\r\n");
                        //     delay_ms(100);
                        //     }
                        // if(trace_check(digital_data)<=0){
                        //     printf("trace\r\n");
                        //     while(break_noout)
                        //     {
                        //         motor_control(1700, 650);
                        //         if(sys_time_1ms%10){
                        //             if(!i2c_app_read(I2C0_INST, 0x4C, &digital_data, 1)){
                        //                 if(trace_check(digital_data)<-20)
                        //                     break_noout = 0;
                        //             }
                        //         }
                        //     }
                        // }
                        // break_noout = 1;

                        // motor_control(0, 0);
                        PIDController_Init(&trace_pid);
                        if (distance_1m_receive)
                        {
                            trace_flag = 1;
                            distance_1m_flag = 0;
                            defaultwheel.left.count = 0;
                            yaw_expt = base_direction;
                            nearingTheEnd = 1;
                        }
                    }
                    if (event_task == 6)
                    { // ����0����һ�ο�ʼ1
                        if (line_no_check)
                        {
                            trace_flag = 0;
                            distance_1m_flag = 0;
                            defaultwheel.left.count = 0;
                            sound_flag = 1;

                            ground++;
                            if (ground == 4)
                            {
                                trace_flag = 0;
                                distance_1m_flag = 0;
                                defaultwheel.left.count = 0;
                                sound_flag = 1;
                                motor_control(0, 0);
                                nearingTheEnd = 1;
                            }
                            else
                            {
                                yaw_expt = yaw_return(base_direction + 38.76);
                                while (fabs(jy901_yaw - yaw_expt) > 3)
                                    motor_control(950, -950);
                                motor_control(0, 0);
                                event_task = 0;
                                event_lock = 1;
                            }
                        }
                    }
                    event_task++;
                }
            if (task == 5)
                {
                    if (event_task == 0)
                    { // �ǶȽ��� ____ 38.66
                        trace_flag = 0;
                        distance_1m_flag = 0;
                        defaultwheel.left.count = 0;
                        sound_flag = 1;
                        yaw_expt = yaw_return(base_direction + 40.76);
                        while (fabs(jy901_yaw - yaw_expt) > 3)
                            motor_control(950, -950);
                        motor_control(0, 0);
                        event_task++;
                    }
                    if (event_task == 1)
                    { // ������-ֱ��
                        defaultwheel.left.count = 0;
                        distance_1m_flag = 1;
                        sound_flag = 1;
                        yaw_expt = yaw_return(base_direction + 40.56);
                        yaw_pid.prevError = 0;
                        yaw_pid.prevMeasurement = yaw_expt;
                    }
                    if (event_task == 2)
                    { // ѭ��
                        // turn_dirction(); ����
                        sound_flag = 1;
                        yaw_expt = yaw_return(base_direction - 1);
                        while (fabs(jy901_yaw - yaw_expt) > 3)
                            motor_control(-450, 950);
                        motor_control(500, 500);
                        PIDController_Init(&trace_pid);
                        if (distance_1m_receive)
                        {
                            trace_flag = 1;
                            distance_1m_flag = 0;
                            defaultwheel.left.count = 0;

                            yaw_expt = base_BackDirection;
                        }
                    }
                    if (event_task == 3)
                    {
                        if (line_no_check)
                        { // �ǶȽ��� ____ 38.66
                            trace_flag = 0;
                            distance_1m_flag = 0;
                            defaultwheel.left.count = 0;
                            sound_flag = 1;
                            yaw_expt = yaw_return(base_BackDirection -40.56);
                            while (fabs(jy901_yaw - yaw_expt) > 3)
                                motor_control(-950, 950);
                            // printf("angle ready: %d, back_dir : %d\r\n",(int)yaw_expt,(int)base_BackDirection);
                            motor_control(0, 0);
                            event_task = 4;
                        }
                    }
                    if (event_task == 4)
                    { // ������-ֱ��
                        defaultwheel.left.count = 0;
                        trace_flag = 0;
                        distance_1m_flag = 1;
                        sound_flag = 1;
                        yaw_expt = yaw_return(base_BackDirection - 39.16);
                        yaw_pid.prevError = 0;
                        yaw_pid.prevMeasurement = yaw_expt;
                        printf("go: %d\r\n", (int)yaw_expt);
                    }
                    if (event_task == 5)
                    { // ѭ��
                        // turn_dirction(); ����
                        sound_flag = 1;
                        yaw_expt = yaw_return(base_BackDirection + 1);
                        while (fabs(jy901_yaw - yaw_expt) > 3)
                            motor_control(1050, -650);
                        motor_control(0, 0);
                        PIDController_Init(&trace_pid);
                        if (distance_1m_receive)
                        {
                            trace_flag = 1;
                            distance_1m_flag = 0;
                            defaultwheel.left.count = 0;
                            yaw_expt = base_direction;
                            nearingTheEnd = 1;
                        }
                    }
                    if (event_task == 6)
                    { // ����0����һ�ο�ʼ1
                        if (line_no_check)
                        {
                            trace_flag = 0;
                            distance_1m_flag = 0;
                            defaultwheel.left.count = 0;
                            sound_flag = 1;

                            ground++;
                            if (ground == 4)
                            {
                                trace_flag = 0;
                                distance_1m_flag = 0;
                                defaultwheel.left.count = 0;
                                sound_flag = 1;
                                motor_control(0, 0);
                                nearingTheEnd = 1;
                            }
                            else
                            {
                                yaw_expt = yaw_return(base_direction + 40.76);
                                while (fabs(jy901_yaw - yaw_expt) > 3)
                                    motor_control(950, -950);
                                motor_control(0, 0);
                                event_task = 0;
                                event_lock = 1;
                            }
                        }
                    }
                    event_task++;
                }
            }
        }
        else
            motor_control(0, 0);
        /* - - -- - - - - - - - - - -- - - - -- - - - - �¼����� - - - - - - - - - - - - - - - -- - - - - - - - - - - - -*/

        /* ѭ��  - - - - - - - ��ȡ���������� ����20msһ��*/
        if (trace_flag && !i2c_app_read(I2C0_INST, 0x4C, &digital_data, 1))
        {
            // digital_data |= 0x80;
            SEP_ALL_BIT8(digital_data,
                         gray_sensor[0],
                         gray_sensor[1],
                         gray_sensor[2],
                         gray_sensor[3],
                         gray_sensor[4],
                         gray_sensor[5],
                         gray_sensor[6],
                         gray_sensor[7]);

            //  printf("8: %d \r\n",gray_sensor[7]);
            //   printf("digital : %d\r\n",digital_data);
            int temp = trace_check(digital_data);
            PIDController_Update(&trace_pid, 0.0f, (float)(35 * temp));

            /*�����Ǹ���*/
            if ((jy901_yaw != 0.00f))
            {
                float temp = jy901_yaw;

                float out = 0; // PIDController_yaw_Update(&yaw_pid,yaw_expt-yaw_step,temp);
            }
            trace_out = (int)(trace_pid.out);
            if(task == 4 ){
                             if(start_power_decelerate == 0 && trace_decelerate_time == 0)
                             {
                                trace_decelerate_time = sys_time_1ms + 500; //17:15  - - - - old : 500
                                start_power_decelerate = task4_power_plus.trace_line_plus;
                             }
                             else{
                                if(sys_time_1ms >= trace_decelerate_time){
                                    start_power_decelerate = 0;
                                }
                             }
                        }
            // printf("trace_out : %d \r\n", trace_out);
            motor_control(motor_BasePower + task4_power_plus.trace_line_plus - start_power_decelerate - trace_out - end_power_decelerate, \
                          motor_BasePower + task4_power_plus.trace_line_plus - start_power_decelerate + trace_out - end_power_decelerate);

            
            if (fabs(yaw_return(jy901_yaw - yaw_expt)) < 20)    //17:15  - - - - old : 20
            {
                end_power_decelerate = 600 + task4_power_plus.trace_line_plus;
                if (fabs(yaw_return(jy901_yaw - yaw_expt)) < 15 && !i2c_app_read(I2C0_INST, 0x4C, &digital_data, 1))
                {
                    if (digital_data == 0xff)
                    {
                        line_no_check = 1; // �Ѿ���ⲻ������
                        event_lock = 1;    // �¼���ɣ�����ָʾ
                        motor_control(0, 0);
                        end_power_decelerate = 0;
                        start_power_decelerate = 0 ;
                        trace_decelerate_time = 0;
                    }
                }
            }
            if (yaw_step)
                yaw_step -= 0.89f;
            delay_ms(20);
        }

        /*ֱ����ʻ*/
        if (distance_1m_flag)
        {

            // printf("1m \r\n");
            /*�����Ǹ���*/
            if ((jy901_yaw != 0.00f))
            {
                float temp = jy901_yaw;

                float out = PIDController_yaw_Update(&yaw_pid, yaw_expt, temp);
                if (turn_on_first)
                {
                    out = 0;
                    turn_on_first++;
                }
                if (turn_on_first == 4)
                {
                    turn_on_first = 0;
                }

                // printf("yaw: %d",(int)temp);

                /*ֱ�и���*/
                motor_control(1760 + task4_power_plus.goStraight_plus - nearget_downpower + (int)(10 * out), \
                              1800 + task4_power_plus.goStraight_plus - nearget_downpower - (int)(10 * out));
                // motor_control(500+(int)(10*out), 500-(int)(10*out));

                delay_ms(20);
            }
            // printf("count : %d\r\n",defaultwheel.left.count);
            i2c_app_read(I2C0_INST, 0x4C, &digital_data, 1);
            if(defaultwheel.left.count >= 80 * 44 && task == 4)nearget_downpower = 450+task4_power_plus.goStraight_plus;
            if (defaultwheel.left.count >= 50 * 44 && digital_data != 0xff)
            {
                nearget_downpower = 0;  //���ý���
                distance_1m_receive = 1; // 12.1*440 ����440һȦ������
                // if(task == 4){motor_control(-500,-500);delay_ms(30);}
                motor_control(0, 0);
                turn_on_first = 1;
                event_lock = 1; // �¼���ɣ�����ָʾ
            }
        }
        /*�Ƕȿ���*/
        // if (yaw_control_flag&&(jy901_yaw!=0.00f))
        // {
        //     float temp = jy901_yaw;

        //     // float out = PIDController_yaw_Update(&yaw_pid,yaw_expt,temp);

        //     /*ֱ�и���*/
        //     // motor_control(1460+3*(int)out, 1500-3*(int)out);

        //     /*ת*/
        //     // float dif_dir_temp = fabs(temp - yaw_expt) ;
        //     // if(dif_dir_temp< 0.5){motor_control(0,0);base_out = 0;yaw_control_flag = 0;event_lock = 1;yaw_pid.Ki = 0;}
        //     // else if(dif_dir_temp< 5) {yaw_pid.Kp = 5;yaw_pid.Ki = 1;}
        //     // motor_control(-(int)(6*out), +(int)(6*out));
        //     // printf("dif yaw : %d\r\n",(int)dif_dir_temp);

        //     delay_ms(20);
        // }
    }
}

/*��������
for(int i = 0; i<900; i+=100){
     motor_control(i,i);
     printf("test : %d\r\n",i);
     delay_ms(500);
     motor_control(0,0);
     delay_ms(500);
}
for(int i = 0; i>-900; i-=100){
     motor_control(i,i);
     printf("test : %d\r\n",i);
     delay_ms(500);
     motor_control(0,0);
     delay_ms(500);
}*/

/*������Ƴ���pid
if (distance_1m_flag)
{
    Total_Controller.Location_Y_Control.FeedBack = defaultwheel.left.count;
    float out = PID_Control(&Total_Controller.Location_Y_Control);
    motor_control(0.5 * out, 0.5 * out);
    printf("count: %d , output : %d \r\n", defaultwheel.left.count, (int)out);
    if (defaultwheel.left.count >= (1281 * 5 - 100))
    {
        defaultwheel.left.count = 0;
        distance_1m_flag = 0;
    }
    delay_ms(20);
}*/

// /**
//          * ���ڵ��Բ��֣�������hc06����Զ�̵��أ�
//          */
//         if (Serial_RxFlag == 1)
//         {
//             Serial_RxFlag = 0;

//             // С����ͣ
//             if (memcmp(Serial_RxPacket, cmd_stop, strlen((char *)cmd_stop)) == 0)
//             {
//                 defaultwheel.left.power_multiple = 0.0f;
//                 defaultwheel.right.power_multiple = 0.0f;
//                 printf("stop\r\n");
//             }
//             else if (memcmp(Serial_RxPacket, cmd_start, strlen((char *)cmd_start)) == 0)
//             {
//                 defaultwheel.left.power_multiple = 0.5f;
//                 defaultwheel.right.power_multiple = 0.5f;
//                 printf("start\r\n");
//             }

//             // С��������
//             else if (memcmp(Serial_RxPacket, cmd_power_up, strlen((char *)cmd_power_up)) == 0)
//             {
//                 defaultwheel.left.power_multiple += 0.1f;
//                 defaultwheel.right.power_multiple += 0.1f;
//                 printf("power_up: %f\r\n", defaultwheel.left.power_multiple);
//             }
//             else if (memcmp(Serial_RxPacket, cmd_power_down, strlen((char *)cmd_power_down)) == 0)
//             {
//                 defaultwheel.left.power_multiple -= 0.1f;
//                 defaultwheel.right.power_multiple -= 0.1f;
//                 printf("power_down: %f\r\n", defaultwheel.left.power_multiple);
//             }

//             // PID�������ڲ���
//             // else if (memcmp(Serial_RxPacket, cmd_pid_kp_up, strlen((char *)cmd_pid_kp_up)) == 0)
//             // {
//             //     pid.Kp += 0.1f;
//             //     printf("p_up: %f\r\n", pid.Kp);
//             // }
//             // else if (memcmp(Serial_RxPacket, cmd_pid_kp_down, strlen((char *)cmd_pid_kp_down)) == 0)
//             // {
//             //     pid.Kp -= 0.1f;
//             //     printf("p_down: %f\r\n", pid.Kp);
//             // }
//             // else if (memcmp(Serial_RxPacket, cmd_pid_kd_up, strlen((char *)cmd_pid_kd_up)) == 0)
//             // {
//             //     pid.Kd += 0.1f;
//             //     printf("d_up: %f\r\n", pid.Kd);
//             // }
//             // else if (memcmp(Serial_RxPacket, cmd_pid_kd_down, strlen((char *)cmd_pid_kd_down)) == 0)
//             // {
//             //     pid.Kd -= 0.1f;
//             //     printf("d_down: %f\r\n", pid.Kd);
//             // }

//             // // pid������
//             // else if (memcmp(Serial_RxPacket, cmd_show_out, strlen((char *)cmd_show_out)) == 0)
//             // {
//             //     printf("pid out : %d \r\n", (int)pid.out);
//             // }

//             // ��·adcѭ��������ȡ
//             else if (memcmp(Serial_RxPacket, cmd_show_adc, strlen((char *)cmd_show_adc)) == 0)
//             {
//                 printf("left: %d right: %d\r\n", adc_buf[0], adc_buf[1]);
//             }

//             // �������ָ��
//             else
//             {
//                 printf("error send data\r\n");
//             }
//         }