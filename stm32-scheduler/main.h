
#ifndef MAIN_H_
#define MAIN_H_

#define MAX_TASKS			4U
                               /*stack memory calculation*/

#define SIZE_TASK_STACK 	1024U   //1KB
#define SIZE_SCHED_STACK    1024U

#define SRAM_START			0x20000000U //from memory map (rm0390)
#define SIZE_SRAM			((128) * (1024))  //128KB FOR THE BOARD
#define SRAM_END			((SRAM_START) + (SIZE_SRAM))   //RAM END, SINCE WE DECIDED T1 IS AT RAM END

#define T1_STACK_START		SRAM_END
#define T2_STACK_START		((SRAM_END) - (SIZE_TASK_STACK))//'-' BECAUSE STACK CONSUMPTION IS FROM BACK TO FRONT
#define T3_STACK_START		((SRAM_END) - (2 * SIZE_TASK_STACK))
#define T4_STACK_START		((SRAM_END) - (3 * SIZE_TASK_STACK))
#define SCHED_START_STACK   ((SRAM_END) - (4 * SIZE_TASK_STACK))
						////////////////////////////////////////////////////

#define TICK_HZ				1000U
#define HSI_CLK				16000000U  //->16MHz
#define SYSTICK_TIM_CLK		HSI_CLK

#define DUMMY_XPSR	0x01000000U  //to run in thumb state


#endif /* MAIN_H_ */
