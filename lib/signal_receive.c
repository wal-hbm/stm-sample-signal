#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "tim.h"
#include "usart.h"


#define DMA_ITEMS    16 /* number of array items, NOT size in bytes */
#define DMA_BUFFERS  8

/* 8 buffers, 16 entries each */
volatile uint32_t data[DMA_BUFFERS][DMA_ITEMS];


volatile int  dma_buffer_index = 0;
volatile int  dma_capture_done = 0;


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if (htim == &htim2) {
//		HAL_GPIO_TogglePin(LD1_green_GPIO_Port, LD1_green_Pin);

		dma_buffer_index++;

		if (dma_buffer_index >= DMA_BUFFERS) {
			dma_capture_done = 1;
		} else {
			if (HAL_OK != HAL_TIM_IC_Start_DMA(&htim2, TIM_CHANNEL_1, &(data[dma_buffer_index][0]), DMA_ITEMS)) {
				/* DMA start failed */
				dma_capture_done = -1;
			}
		}
	}
}


void read_tim2_ch1_capture_dma()
{
	/*
	 * TIM2 GPIO Configuration
	 * PA0/WKUP     ------> TIM2_CH1
	 */

	char msg[100];
	sprintf(msg, "\r\nstarting:\r\n");
	HAL_UART_Transmit(&huart3, (uint8_t*)msg, strlen(msg), 100);

	dma_buffer_index = 0;
	dma_capture_done = 0;

	/* start first DMA transfer "manually", successive transfers are started from the IRQ handler  */
	HAL_TIM_IC_Start_DMA(&htim2, TIM_CHANNEL_1, &(data[dma_buffer_index][0]), DMA_ITEMS);

	while (1) {
		if (dma_capture_done == 1) {

			sprintf(msg, "DMA succeeded:\r\n");
			HAL_UART_Transmit(&huart3, (uint8_t*)msg, strlen(msg), 100);

			uint32_t hz = 107500000;

			uint32_t ticks;
			uint32_t capture_last = 0;
			uint32_t ticks_last = 0;

			for (int buffer = 0; buffer < DMA_BUFFERS; buffer++) {
				for (int i = 0; i < DMA_ITEMS; i++) {
					ticks = data[buffer][i] - capture_last;
					capture_last = data[buffer][i];
					sprintf(msg, "%2i %10lu %10lu %+5i %s %10lu Hz\r\n",
							i, capture_last, ticks,
							(int)(ticks-ticks_last), abs(ticks-ticks_last) > 2 ? "!" : " ",
							hz/ticks);
					ticks_last = ticks;
					HAL_UART_Transmit(&huart3, (uint8_t*)msg, strlen(msg), 100);
				}
			}

			dma_capture_done = 0;
		}
	}
}


/*
starting:
DMA succeeded:
 0          0          0    +0            0 Hz
 1          0          0    +0            0 Hz
 2         17         17   +17 !    6323529 Hz
 3         44         27   +10 !    3981481 Hz
 4         71         27    +0      3981481 Hz
 5         97         26    -1      4134615 Hz
 6        124         27    +1      3981481 Hz
 7        151         27    +0      3981481 Hz
 8        178         27    +0      3981481 Hz
 9        205         27    +0      3981481 Hz
10        232         27    +0      3981481 Hz
11        259         27    +0      3981481 Hz
12        286         27    +0      3981481 Hz
13        312         26    -1      4134615 Hz
14        339         27    +1      3981481 Hz
15        366         27    +0      3981481 Hz
 0       1119        753  +726 !     142762 Hz
 1       1146         27  -726 !    3981481 Hz
 2       1172         26    -1      4134615 Hz
 3       1199         27    +1      3981481 Hz
 4       1226         27    +0      3981481 Hz
 5       1280         54   +27 !    1990740 Hz
 6       1307         27   -27 !    3981481 Hz
 7       1334         27    +0      3981481 Hz
 8       1361         27    +0      3981481 Hz
 9       1387         26    -1      4134615 Hz
10       1414         27    +1      3981481 Hz
11       1441         27    +0      3981481 Hz
12       1468         27    +0      3981481 Hz
13       1495         27    +0      3981481 Hz
14       1522         27    +0      3981481 Hz
15       1548         26    -1      4134615 Hz
 0       2301        753  +727 !     142762 Hz
 1       2328         27  -726 !    3981481 Hz
 2       2355         27    +0      3981481 Hz
 3       2409         54   +27 !    1990740 Hz
 4       2436         27   -27 !    3981481 Hz
 5       2462         26    -1      4134615 Hz
 6       2489         27    +1      3981481 Hz
 7       2516         27    +0      3981481 Hz
 8       2543         27    +0      3981481 Hz
 9       2570         27    +0      3981481 Hz
10       2597         27    +0      3981481 Hz
11       2623         26    -1      4134615 Hz
12       2651         28    +2      3839285 Hz
13       2677         26    -2      4134615 Hz
14       2704         27    +1      3981481 Hz
15       2731         27    +0      3981481 Hz
 0       3430        699  +672 !     153791 Hz
 1       3457         27  -672 !    3981481 Hz
 2       3483         26    -1      4134615 Hz
 3       3511         28    +2      3839285 Hz
 4       3564         53   +25 !    2028301 Hz
 5       3591         27   -26 !    3981481 Hz
 6       3618         27    +0      3981481 Hz
 7       3645         27    +0      3981481 Hz
 8       3672         27    +0      3981481 Hz
 9       3698         26    -1      4134615 Hz
10       3725         27    +1      3981481 Hz
11       3752         27    +0      3981481 Hz
12       3779         27    +0      3981481 Hz
13       3806         27    +0      3981481 Hz
14       3833         27    +0      3981481 Hz
15       3860         27    +0      3981481 Hz
 0       4585        725  +698 !     148275 Hz
 1       4612         27  -698 !    3981481 Hz
 2       4639         27    +0      3981481 Hz
 3       4666         27    +0      3981481 Hz
 4       4693         27    +0      3981481 Hz
 5       4747         54   +27 !    1990740 Hz
 6       4773         26   -28 !    4134615 Hz
 7       4800         27    +1      3981481 Hz
 8       4827         27    +0      3981481 Hz
 9       4854         27    +0      3981481 Hz
10       4881         27    +0      3981481 Hz
11       4908         27    +0      3981481 Hz
12       4934         26    -1      4134615 Hz
13       4961         27    +1      3981481 Hz
14       4988         27    +0      3981481 Hz
15       5015         27    +0      3981481 Hz
 0       5741        726  +699 !     148071 Hz
 1       5768         27  -699 !    3981481 Hz
 2       5795         27    +0      3981481 Hz
 3       5822         27    +0      3981481 Hz
 4       5875         53   +26 !    2028301 Hz
 5       5902         27   -26 !    3981481 Hz
 6       5929         27    +0      3981481 Hz
 7       5956         27    +0      3981481 Hz
 8       5983         27    +0      3981481 Hz
 9       6010         27    +0      3981481 Hz
10       6037         27    +0      3981481 Hz
11       6063         26    -1      4134615 Hz
12       6090         27    +1      3981481 Hz
13       6117         27    +0      3981481 Hz
14       6144         27    +0      3981481 Hz
15       6171         27    +0      3981481 Hz
 0       6897        726  +699 !     148071 Hz
 1       6923         26  -700 !    4134615 Hz
 2       6950         27    +1      3981481 Hz
 3       7004         54   +27 !    1990740 Hz
 4       7031         27   -27 !    3981481 Hz
 5       7058         27    +0      3981481 Hz
 6       7085         27    +0      3981481 Hz
 7       7112         27    +0      3981481 Hz
 8       7138         26    -1      4134615 Hz
 9       7165         27    +1      3981481 Hz
10       7192         27    +0      3981481 Hz
11       7219         27    +0      3981481 Hz
12       7246         27    +0      3981481 Hz
13       7273         27    +0      3981481 Hz
14       7299         26    -1      4134615 Hz
15       7327         28    +2      3839285 Hz
 0       8025        698  +670 !     154011 Hz
 1       8052         27  -671 !    3981481 Hz
 2       8079         27    +0      3981481 Hz
 3       8106         27    +0      3981481 Hz
 4       8133         27    +0      3981481 Hz
 5       8187         54   +27 !    1990740 Hz
 6       8213         26   -28 !    4134615 Hz
 7       8240         27    +1      3981481 Hz
 8       8267         27    +0      3981481 Hz
 9       8294         27    +0      3981481 Hz
10       8321         27    +0      3981481 Hz
11       8348         27    +0      3981481 Hz
12       8375         27    +0      3981481 Hz
13       8402         27    +0      3981481 Hz
14       8428         26    -1      4134615 Hz
15       8455         27    +1      3981481 Hz

 */
