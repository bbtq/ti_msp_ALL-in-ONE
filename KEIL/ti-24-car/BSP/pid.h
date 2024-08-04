#ifndef _PID_H_
#define _PID_H_

#include "board.h"

#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt))) //�޷�����



typedef struct {

	/* Controller gains */
	float Kp;
	float Ki;
	float Kd;

	/* Derivative low-pass filter time constant */
	float tau;

	/* Output limits */
	float limMin;
	float limMax;
	
	/* Integrator limits */
	float limMinInt;
	float limMaxInt;

	/* Sample time (in seconds) */
	float T;

	/* Controller "memory" */
	float integrator;
	float prevError;			/* Required for integrator */
	float differentiator;
	float prevMeasurement;		/* Required for differentiator */

	/* Controller output */
	float out;

} PIDController;


void  PIDController_Init(PIDController *pid);
float PIDController_Update(PIDController *pid, float setpoint, float measurement);
float PIDController_yaw_Update(PIDController *pid, float setpoint, float measurement);
float yaw_return(float exp);


#endif





// /* �� PID �����������ṹ�� */
// typedef struct
// {
//     uint8_t Err_Limit_Flag : 1;            //ƫ���޷���־   λ����
//     uint8_t Integrate_Limit_Flag : 1;      //�����޷���־
//     uint8_t Integrate_Separation_Flag : 1; //���ַ����־
//     float Expect;                          //����
//     float FeedBack;                        //����ֵ
//     float Err;                             //ƫ��
//     float Last_Err;                        //�ϴ�ƫ��
//     float Err_Max;                         //ƫ���޷�ֵ
//     float Integrate_Separation_Err;        //���ַ���ƫ��ֵ
//     float Integrate;                       //����ֵ
//     float Integrate_Max;                   //�����޷�ֵ
//     float Kp;                              //���Ʋ���Kp  12
//     float Ki;                              //���Ʋ���Ki  13
//     float Kd;                              //���Ʋ���Kd  14
//     float Control_OutPut;                  //�����������
//     float Last_Control_OutPut;             //�ϴο����������
//     float Control_OutPut_Limit;            //����޷�
//     /***************************************/
//     float Pre_Last_Err;         //���ϴ�ƫ��
//     float Adaptable_Kd;         //����Ӧ΢�ֲ���
//     float Last_FeedBack;        //�ϴη���ֵ
//     float Dis_Err;              //΢����
//     float Dis_Error_History[5]; //��ʷ΢����
//     float Err_LPF;
//     float Last_Err_LPF;
//     float Dis_Err_LPF;
//     float Last_Dis_Err_LPF;
//     float Pre_Last_Dis_Err_LPF;
//     float Scale_Kp;
//     float Scale_Ki;
//     float Scale_Kd;
    
// } PID_Controler;

// /* �� PID �����������ṹ�� */
// typedef struct
// {
//     PID_Controler Yaw_Angle_Control;    //ƫ���Ƕ�

//     PID_Controler Location_Y_Control;   //Distance �������

//     PID_Controler Trace_Control;   //Trace ѭ������

// } AllControler;

// extern AllControler Total_Controller; //�ܿ�����PID

// void Total_PID_Init(void);
// float PID_Control(PID_Controler *Controler);
// float PID_Control_Yaw(PID_Controler *Controler);
