/*
 * ������������Ӳ�������������չ����Ӳ�����Ϲ���ȫ����Դ
 * �����������www.lckfb.com
 * ����֧�ֳ�פ��̳���κμ������⻶ӭ��ʱ����ѧϰ
 * ������̳��https://oshwhub.com/forum
 * ��עbilibili�˺ţ������������塿���������ǵ����¶�̬��
 * ��������׬Ǯ���������й�����ʦΪ����
 * Change Logs:
 * Date           Author       Notes
 * 2024-05-23     LCKFB     first version
 */
#include "board.h"
#include "stdio.h"
#include "bsp_jy901.h"
#include "motor.h"

bool sound_flag = 0;
bool start_flag = 0;
uint8_t task = 0;
uint32_t sys_time_1ms = 0;
float base_direction = 0;
float base_BackDirection = 0;
volatile unsigned int delay_times = 0;
volatile unsigned char uart_data = 0;

void board_init(void)
{
	// SYSCFG��ʼ��
	SYSCFG_DL_init();

	// �����ʱ���жϱ�־
	NVIC_ClearPendingIRQ(TIMER_0_INST_INT_IRQN);
	// ʹ�ܶ�ʱ���ж�
	NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);

	// ��������жϱ�־
	NVIC_ClearPendingIRQ(UART_0_INST_INT_IRQN);
	// ʹ�ܴ����ж�
	NVIC_EnableIRQ(UART_0_INST_INT_IRQN);

	NVIC_EnableIRQ(GPIO_MULTIPLE_GPIOA_INT_IRQN); // �������������������GPIOA�˿��ж�

	printf("Board Init [[ ** LCKFB ** ]]\r\n");
}

// ����δ�ʱ��ʵ�ֵľ�ȷus��ʱ
void delay_us(unsigned long __us)
{
	uint32_t ticks;
	uint32_t told, tnow, tcnt = 38;

	// ������Ҫ��ʱ���� = �ӳ�΢���� * ÿ΢���ʱ����
	ticks = __us * (32000000 / 1000000);

	// ��ȡ��ǰ��SysTickֵ
	told = SysTick->VAL;

	while (1)
	{
		// �ظ�ˢ�»�ȡ��ǰ��SysTickֵ
		tnow = SysTick->VAL;

		if (tnow != told)
		{
			if (tnow < told)
				tcnt += told - tnow;
			else
				tcnt += SysTick->LOAD - tnow + told;

			told = tnow;

			// ����ﵽ����Ҫ��ʱ���������˳�ѭ��
			if (tcnt >= ticks)
				break;
		}
	}
}
// ����δ�ʱ��ʵ�ֵľ�ȷms��ʱ
void delay_ms(unsigned long ms)
{
	delay_us(ms * 1000);
}

void delay_1us(unsigned long __us) { delay_us(__us); }
void delay_1ms(unsigned long ms) { delay_ms(ms); }

// ���ڷ��͵����ַ�
void uart0_send_char(char ch)
{
	// ������0æ��ʱ��ȴ�����æ��ʱ���ٷ��ʹ��������ַ�
	while (DL_UART_isBusy(UART_0_INST) == true)
		;
	// ���͵����ַ�
	DL_UART_Main_transmitData(UART_0_INST, ch);
}
// ���ڷ����ַ���
void uart0_send_string(char *str)
{
	// ��ǰ�ַ�����ַ���ڽ�β ���� �ַ����׵�ַ��Ϊ��
	while (*str != 0 && str != 0)
	{
		// �����ַ����׵�ַ�е��ַ��������ڷ������֮���׵�ַ����
		uart0_send_char(*str++);
	}
}

#if !defined(__MICROLIB)
// ��ʹ��΢��Ļ�����Ҫ�������ĺ���
#if (__ARMCLIB_VERSION <= 6000000)
// �����������AC5  �Ͷ�����������ṹ��
struct __FILE
{
	int handle;
};
#endif

FILE __stdout;

// ����_sys_exit()�Ա���ʹ�ð�����ģʽ
void _sys_exit(int x)
{
	x = x;
}
#endif

// printf�����ض���
int fputc(int ch, FILE *stream)
{
	// ������0æ��ʱ��ȴ�����æ��ʱ���ٷ��ʹ��������ַ�
	while (DL_UART_isBusy(UART_0_INST) == true)
		;

	DL_UART_Main_transmitData(UART_0_INST, ch);

	return ch;
}

// ��ʱ�����жϷ����� ������Ϊ1�������
void TIMER_0_INST_IRQHandler(void)
{
	static uint32_t exp_time = 0;
	// ��������˶�ʱ���ж�
	switch (DL_TimerG_getPendingInterrupt(TIMER_0_INST))
	{
	case DL_TIMER_IIDX_ZERO: // �����0����ж� 1ms��ʱ�ѵ�
		/*�����ٶȣ������ü���������һ�μ���*/
		// printf("l count : %d \r\n",defaultwheel.left.count);
		// defaultwheel.left.count = 0;
		if(sound_flag){
			exp_time = sys_time_1ms+300;
			DL_GPIO_setPins(LED1_PORT,LED1_PIN_14_PIN);
			sound_flag = 0;
		}
		if(exp_time == sys_time_1ms){
			DL_GPIO_clearPins(LED1_PORT,LED1_PIN_14_PIN);
		}
		// if(sys_time_1ms%20 == 0) printf("yaw: %d\r\n",(int)jy901_yaw);
		sys_time_1ms++;

		break;

	default: // �����Ķ�ʱ���ж�
		break;
	}
}

void led_flashing(uint8_t time)
{
	for (uint8_t i = 0; i < time; i++)
	{
		DL_GPIO_togglePins(LED1_PORT, LED1_PIN_14_PIN);
		delay_ms(100);
		DL_GPIO_togglePins(LED1_PORT, LED1_PIN_14_PIN);
		delay_ms(100);
	}
}

void GROUP1_IRQHandler(void) // Group1���жϷ�����
{
	static bool key_lock = 0;
	// ��ȡGroup1���жϼĴ���������жϱ�־λ
	switch (DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1))
	{
	case GPIO_MULTIPLE_GPIOA_INT_IIDX:
		// ����Ƿ��ǻ���������������
		if (DL_GPIO_readPins(motor_count_PORT, motor_count_L_A_PIN) > 0)
		{
			defaultwheel.left.count++;
		}
		if (DL_GPIO_readPins(motor_count_PORT, motor_count_R_A_PIN) > 0)
		{
			defaultwheel.right.count++;
		}
		if (KEY_PIN_18_IIDX)
		{
			if(!key_lock){
				delay_ms(5);
				if(DL_GPIO_readPins(KEY_PORT, KEY_PIN_18_PIN) != 0){
					delay_ms(600);
					if (DL_GPIO_readPins(KEY_PORT, KEY_PIN_18_PIN) != 0)
					{
						if(task == 0){	//��¼����λ
							DL_GPIO_togglePins(LED1_PORT, LED1_PIN_14_PIN);
							
							for (uint8_t i = 0; i < 10; i++)
							{
								
								base_BackDirection += jy901_yaw;
								delay_ms(20);
							}
							base_BackDirection /= 10.0f;
							delay_ms(600);
							DL_GPIO_togglePins(LED1_PORT, LED1_PIN_14_PIN);
						}
						else {		//��¼����λ
							DL_GPIO_togglePins(LED1_PORT, LED1_PIN_14_PIN);
							start_flag = 1;
							for (uint8_t i = 0; i < 10; i++)
							{
								while (jy901_yaw == 0.00)
								; // �ȴ���ʼ��
								base_direction += jy901_yaw;
								delay_ms(20);
							}
							base_direction /= 10.0f;
							key_lock = 1;
							delay_ms(600);
							DL_GPIO_togglePins(LED1_PORT, LED1_PIN_14_PIN);
							// printf("start task%d\r\n",task);
						}
					}
					else if(DL_GPIO_readPins(KEY_PORT, KEY_PIN_18_PIN)==0){
						task++;
						led_flashing(task);	
					}
				}
			}
		}
		break;
	}
}

// ���ڵ��жϷ�����
char Serial_RxPacket[100]; //"@MSG\r\n"
uint8_t Serial_RxFlag;
void UART_0_INST_IRQHandler(void)
{
	static uint8_t RxState = 0;
	static uint8_t pRxPacket = 0;
	uint8_t RxData = 0;

	// ��������˴����ж�
	switch (DL_UART_getPendingInterrupt(UART_0_INST))
	{
	case DL_UART_IIDX_RX: // ����ǽ����ж�

		// �ӷ��͹��������ݱ����ڱ�����
		RxData = DL_UART_Main_receiveData(UART_0_INST);
		copeJY901_data(RxData);

		// ������������ٷ��ͳ�ȥ
		//			uart0_send_char(RxData);

		if (RxState == 0)
		{
			if (RxData == '@' && Serial_RxFlag == 0)
			{
				RxState = 1;
				pRxPacket = 0;
			}
		}
		else if (RxState == 1)
		{
			if (RxData == '*')
			{
				RxState = 2;
			}
			else
			{
				Serial_RxPacket[pRxPacket] = RxData;
				pRxPacket++;
			}
		}
		else if (RxState == 2)
		{
			if (RxData == '#')
			{
				RxState = 0;
				Serial_RxPacket[pRxPacket] = '\n';
				Serial_RxFlag = 1;
			}
		}
		break;

	default: // �����Ĵ����ж�
		break;
	}
}
