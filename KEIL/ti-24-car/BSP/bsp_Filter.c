#include "bsp_Filter.h"
#include "stdlib.h"

/**
 * ����������
*/
KalmanFilter kf_adc1;
KalmanFilter kf_adc2;

// ��ʼ���������˲���
void kalman_init(KalmanFilter *kf, float process_noise, float measurement_noise, float estimated_error, float initial_value) {
    kf->Q = process_noise;
    kf->R = measurement_noise;
    kf->P = estimated_error;
    kf->x = initial_value;
}

// �������˲�����
float kalman_update(KalmanFilter *kf, float measurement) {
    // Ԥ�����
    kf->P = kf->P + kf->Q;

    // ���㿨��������
    kf->K = kf->P / (kf->P + kf->R);

    // ���¹���ֵ
    kf->x = kf->x + kf->K * (measurement - kf->x);

    // ���¹������Э����
    kf->P = (1 - kf->K) * kf->P;

    return kf->x;
}

/**
 * ��ֵ�˲�����
*/

#define WINDOW_SIZE 5

// �������������ڱȽ����������Ĵ�С
int compare(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

// ѭ��ģ��ģ�������ݵ���ֵ�˲�
int median_filter( int new_sample) {
    static int window[WINDOW_SIZE] = {0};
    static unsigned int index = 0;

    window[index] = new_sample;
    index = (index + 1) % WINDOW_SIZE;

    // ����һ����ʱ������������
    int temp[WINDOW_SIZE];
    for (int i = 0; i < WINDOW_SIZE; i++) {
        temp[i] = window[i];
    }

    // ����ʱ�����������
    qsort(temp, WINDOW_SIZE, sizeof(int), compare);

    // ���������������м�ֵ
    return temp[WINDOW_SIZE / 2];
}



