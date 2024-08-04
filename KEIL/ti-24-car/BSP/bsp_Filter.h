#ifndef _BSP_FILTER_H_
#define _BSP_FILTER_H_

// �������˲��ṹ��
typedef struct {
    float Q; // ��������Э����
    float R; // ��������Э����
    float x; // ����ֵ
    float P; // �������Э����
    float K; // ����������
} KalmanFilter;

extern KalmanFilter kf_adc1;
extern KalmanFilter kf_adc2;

//�������˲�
void kalman_init(KalmanFilter *kf, float process_noise, float measurement_noise, float estimated_error, float initial_value) ;
float kalman_update(KalmanFilter *kf, float measurement);

//��ֵ�˲�
int median_filter( int new_sample);

#endif

