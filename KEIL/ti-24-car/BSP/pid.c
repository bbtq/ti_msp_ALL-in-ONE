#include "PID.h"
#include "math.h"
#include "stdio.h"

void PIDController_Init(PIDController *pid) {

	/* Clear controller variables */
	pid->integrator = 0.0f;
	pid->prevError  = 0.0f;

	pid->differentiator  = 0.0f;
	pid->prevMeasurement = 0.0f;

	pid->out = 0.0f;

}

float PIDController_Update(PIDController *pid, float setpoint, float measurement) {

	/*
	* Error signal
	*/
    float error = setpoint - measurement;


	/*
	* Proportional
	*/
    float proportional = pid->Kp * error;


	/*
	* Integral
	*/
    pid->integrator = pid->integrator + 0.5f * pid->Ki * pid->T * (error + pid->prevError);

	/* Anti-wind-up via integrator clamping */
    if (pid->integrator > pid->limMaxInt) {

        pid->integrator = pid->limMaxInt;

    } else if (pid->integrator < pid->limMinInt) {

        pid->integrator = pid->limMinInt;

    }

	pid->differentiator = -pid->Kd * (measurement - pid->prevMeasurement);

	/*
	* Compute output and apply limits
	*/
    pid->out = proportional + pid->integrator + pid->differentiator;

    if (pid->out > pid->limMax) {

        pid->out = pid->limMax;

    } else if (pid->out < pid->limMin) {

        pid->out = pid->limMin;

    }

	/* Store error and measurement for later use */
    pid->prevError       = error;
    pid->prevMeasurement = measurement;

	/* Return controller output */
    return pid->out;

}


float PIDController_yaw_Update(PIDController *pid, float setpoint, float measurement) {

	/*
	* Error signal
	*/
	// printf("%d\r\n",(int)measurement);
	// static bool temp = 1;
	// if(temp)temp = 0;
	// else{
	// 	if(fabs(pid->prevMeasurement - measurement)>10)measurement = pid->prevMeasurement;
	// }
	// printf("set : %d mea: %d\r\n",(int)setpoint,(int)measurement);

	   /***********************ƫ����ƫ���+-180����*****************************/
    if(setpoint <-180)  setpoint = setpoint + 360;
    if( setpoint > 180)  setpoint = setpoint - 360;


	float error = setpoint - measurement;

   /***********************ƫ����ƫ���+-180����*****************************/
    if(error <-180)  error = error + 360;
    if(error > 180)  error = error - 360;

	/*
	* Proportional
	*/
    float proportional = pid->Kp * error;


	/*
	* Integral
	*/
    pid->integrator = pid->integrator + 0.5f * pid->Ki * pid->T * (error + pid->prevError);

	/* Anti-wind-up via integrator clamping */
    if (pid->integrator > pid->limMaxInt) {

        pid->integrator = pid->limMaxInt;

    } else if (pid->integrator < pid->limMinInt) {

        pid->integrator = pid->limMinInt;

    }

	pid->differentiator = -pid->Kd * (measurement - pid->prevMeasurement);

	/*
	* Compute output and apply limits
	*/
    pid->out = proportional + pid->integrator + pid->differentiator;

    if (pid->out > pid->limMax) {

        pid->out = pid->limMax;

    } else if (pid->out < pid->limMin) {

        pid->out = pid->limMin;

    }

	/* Store error and measurement for later use */
    pid->prevError       = error;
    pid->prevMeasurement = measurement;

	/* Return controller output */
    return pid->out;

}

float yaw_return(float exp)
{
		   /***********************ƫ����ƫ���+-180����*****************************/
    if(exp <-180)  exp = exp + 360;
    if( exp > 180)  exp = exp - 360;
	return exp;
}

// AllControler Total_Controller; //�ܿ�����PID


// /*
// 1ƫ���޷���־��  2�����޷���־��3���ַ����־��   4������
// 5����            6ƫ�        7�ϴ�ƫ�       8ƫ���޷�ֵ��
// 9���ַ���ƫ��ֵ��10����ֵ       11�����޷�ֵ��    12���Ʋ���Kp��
// 13���Ʋ���Ki��   14���Ʋ���Kd�� 15�������������  16�ϴο����������
// 17������޷��ȣ� 18����ֿ���ʱ�Ļ�������
// */
// const float Control_Unit[3][20] = {
//     /*                                     Kp     Ki     Kd                     */
//     /*1 2  3  4  5  6   7  8   9 10   11   12     13     14  15 16   17  18*/
//     {1, 1, 1, 0, 0, 0, 0, 180, 0, 0, 100, 4.00, 1.0000,  2.00, 0, 0,  400, 1, 1, 1}, //Yaw_Angle;ƫ����   0

//     /*                                     Kp    Ki   Kd                     */
//     /*1 2  3  4  5  6   7  8   9 10   11   12    13   14  15   16  17  18*/
//     {1, 1, 1, 0, 0, 0, 0, 9000, 0, 0, 100, 1.0,  0.000, 0.2 , 0,   0,  3000, 1, 1, 1}, //Distance;�������	1

//     /*                                     Kp    Ki   Kd                     */
//     /*1 2  3  4  5  6   7  8   9 10   11   12    13   14  15   16  17  18*/
//     {1, 1, 1, 0, 0, 0, 0, 2000, 0,  0,  1,  2.0,  0.000, 0.5 , 0,   0,  2000, 1, 1, 1}, //Trace;ѭ�� 2


// };


// /**
//   * @brief  PID_Init(PID������ʼ������)
//   * @param  *Controler PID������
//   * @param  Label PID������
//   */
// void PID_Init(PID_Controler *Controler, uint8_t Label)	
// {
//     Controler->Err_Limit_Flag = (uint8_t)(Control_Unit[Label][0]);            //1ƫ���޷���־
//     Controler->Integrate_Limit_Flag = (uint8_t)(Control_Unit[Label][1]);      //2�����޷���־
//     Controler->Integrate_Separation_Flag = (uint8_t)(Control_Unit[Label][2]); //3���ַ����־
//     Controler->Expect = Control_Unit[Label][3];                               //4����
//     Controler->FeedBack = Control_Unit[Label][4];                             //5����ֵ
//     Controler->Err = Control_Unit[Label][5];                                  //6ƫ��
//     Controler->Last_Err = Control_Unit[Label][6];                             //7�ϴ�ƫ��
//     Controler->Err_Max = Control_Unit[Label][7];                              //8ƫ���޷�ֵ
//     Controler->Integrate_Separation_Err = Control_Unit[Label][8];             //9���ַ���ƫ��ֵ
//     Controler->Integrate = Control_Unit[Label][9];                            //10����ֵ
//     Controler->Integrate_Max = Control_Unit[Label][10];                       //11�����޷�ֵ
//     Controler->Kp = Control_Unit[Label][11];                                  //12���Ʋ���Kp
//     Controler->Ki = Control_Unit[Label][12];                                  //13���Ʋ���Ki
//     Controler->Kd = Control_Unit[Label][13];                                  //14���Ʋ���Ki
//     Controler->Control_OutPut = Control_Unit[Label][14];                      //15�����������
//     Controler->Last_Control_OutPut = Control_Unit[Label][15];                 //16�ϴο����������
//     Controler->Control_OutPut_Limit = Control_Unit[Label][16];                //17�ϴο����������
//     Controler->Scale_Kp = Control_Unit[Label][17];
//     Controler->Scale_Ki = Control_Unit[Label][18];
//     Controler->Scale_Kd = Control_Unit[Label][19];
// }

// void Total_PID_Init(void){
// 	PID_Init(&Total_Controller.Yaw_Angle_Control,0);
// 	PID_Init(&Total_Controller.Location_Y_Control,1);
//     PID_Init(&Total_Controller.Trace_Control,2);
// }

// /**
//   * @brief  ͨ�� PID ������
//   * @param *Controler ��Ӧ PID ������
//   */
// float PID_Control(PID_Controler *Controler)
// {
//     /*******ƫ�����*********************/
//     Controler->Last_Err = Controler->Err;                     //�����ϴ�ƫ��
//     Controler->Err = Controler->Expect - Controler->FeedBack; //������ȥ�����õ�ƫ��
//     if (Controler->Err_Limit_Flag == 1)                       //ƫ���޷��ȱ�־λ
//     {
//         Controler->Err = constrain(Controler->Err, -Controler->Err_Max, Controler->Err_Max);
//     }
//     /*******���ּ���*********************/
//     if (Controler->Integrate_Separation_Flag == 1) //���ַ����־λ
//     {
//         if (fabs(Controler->Err) <= Controler->Integrate_Separation_Err)
//             Controler->Integrate += Controler->Scale_Ki * Controler->Ki * Controler->Err;
//     }
//     else
//     {
//         Controler->Integrate += Controler->Scale_Ki * Controler->Ki * Controler->Err;
//     }
//     /*******�����޷�*********************/
//     if (Controler->Integrate_Limit_Flag == 1) //�������Ʒ��ȱ�־
//     {
//         Controler->Integrate = constrain(Controler->Integrate, -Controler->Integrate_Max, Controler->Integrate_Max);
//     }
//     /*******���������*********************/
//     Controler->Last_Control_OutPut = Controler->Control_OutPut; //���ֵ����
//     Controler->Control_OutPut =
//         Controler->Scale_Kp * Controler->Kp * Controler->Err      //����
//         + Controler->Integrate                                    //����
//         + Controler->Kd * (Controler->Err - Controler->Last_Err); //΢��
//     /*******������޷�*********************/
//     Controler->Control_OutPut = constrain(Controler->Control_OutPut, -Controler->Control_OutPut_Limit, Controler->Control_OutPut_Limit);
//     /*******���������*********************/
//     return Controler->Control_OutPut;
// }

// /**
//   * @brief  �Ƕ� PID ������
//   * @param *Controler ��Ӧ PID ������
//   */
// float PID_Control_Yaw(PID_Controler *Controler)
// {
//     /*******ƫ�����*********************/
//     Controler->Last_Err = Controler->Err;                     //�����ϴ�ƫ��
//     Controler->Err = Controler->Expect - Controler->FeedBack; //������ȥ�����õ�ƫ��  FeedBack
//     /***********************ƫ����ƫ���+-180����*****************************/
//     if(Controler->Err <-180)  Controler->Err = Controler->Err + 360;
//     if(Controler->Err > 180)  Controler->Err = Controler->Err - 360;

//     if (Controler->Err_Limit_Flag == 1) //ƫ���޷��ȱ�־λ
//     {
//         Controler->Err = constrain(Controler->Err, -Controler->Err_Max, Controler->Err_Max);
//     }
//     /*******���ּ���*********************/
//     if (Controler->Integrate_Separation_Flag == 1) //���ַ����־λ
//     {
//         if (fabs(Controler->Err) <= Controler->Integrate_Separation_Err)
//             Controler->Integrate += Controler->Scale_Ki * Controler->Ki * Controler->Err;
//     }
//     else
//     {
//         Controler->Integrate += Controler->Scale_Ki * Controler->Ki * Controler->Err;
//     }
//     /*******�����޷�*********************/
//     if (Controler->Integrate_Limit_Flag == 1) //�������Ʒ��ȱ�־
//     {
//         Controler->Integrate = constrain(Controler->Integrate, -Controler->Integrate_Max, Controler->Integrate_Max);
//     }
//     /*******���������*********************/
//     Controler->Last_Control_OutPut = Controler->Control_OutPut; //���ֵ����
//     Controler->Control_OutPut =
//         Controler->Scale_Kp * Controler->Kp * Controler->Err      //����  λ��ʽ
//         + Controler->Integrate                                    //����
//         + Controler->Kd * (Controler->Err - Controler->Last_Err); //΢��
//     /*******������޷�*********************/
//     Controler->Control_OutPut = constrain(Controler->Control_OutPut, -Controler->Control_OutPut_Limit, Controler->Control_OutPut_Limit);
//     /*******���������*********************/
//     return Controler->Control_OutPut;
// }




