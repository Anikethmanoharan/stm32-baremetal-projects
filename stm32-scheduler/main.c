
#include <stdint.h>
#include <stdio.h>
#include "main.h"


void task1_scheduler(void);  //<-- TASK1
void task2_scheduler(void);  //<-- TASK2
void task3_scheduler(void);  //<-- TASK3
void task4_scheduler(void);  //<-- TASK4

void init_systick_timer(uint32_t tick_hz);
__attribute__ ((naked)) void init_scheduler_stack(uint32_t sched_top_of_stack);
void init_tasks_stack(void);
void enable_processor_faults(void);
__attribute__ ((naked)) void switch_sp_to_psp(void);
uint32_t get_psp_value(void);

uint32_t psp_of_tasks[MAX_TASKS] = {T1_STACK_START,T2_STACK_START,T3_STACK_START,T4_STACK_START};
uint32_t task_handlers[MAX_TASKS];
uint8_t current_task = 0; //0 means task1 is running

int main(void)
{
	enable_processor_faults();
	init_scheduler_stack(SCHED_START_STACK);

	task_handlers[0] = (uint32_t)task1_scheduler;
	task_handlers[1] = (uint32_t)task2_scheduler;
	task_handlers[2] = (uint32_t)task3_scheduler;
	task_handlers[3] = (uint32_t)task4_scheduler;

	init_tasks_stack(); //initialized dummy frames

	init_systick_timer(TICK_HZ); //triggered exception

	switch_sp_to_psp();
	task1_scheduler();
	for(;;);
}


void task1_scheduler(void){
	while(1){
		printf("this is task1\n");
	}
}

void task2_scheduler(void){
	while(1){
		printf("this is task2\n");
	}
}

void task3_scheduler(void){
	while(1){
		printf("this is task3\n");
	}
}

void task4_scheduler(void){
	while(1){
		printf("this is task4\n");
	}
}

void init_systick_timer(uint32_t tick_hz){
	uint32_t *pSRVR = (uint32_t*)0xE000E014;  //SysTick reload value Register
	uint32_t *pSCSR = (uint32_t*)0xE000E010;  //SysTick Control and Status Register

	uint32_t count_value = (SYSTICK_TIM_CLK / tick_hz) - 1;   //we subtarct reload cycle so - 1

	//clear the value of svr
	*pSRVR = *pSRVR & ~(0x00FFFFFFFF);   //24 BITS ARE VALID, OTHER IS RESERVED FOR SRVR

	//load value into SVR
	*pSRVR  = *pSRVR | count_value; //VALUE IN SRVR IS COPIED TO CVR AND DOWNCOUNTING IS DONE

	//settings to do
	*pSCSR = *pSCSR | (1 << 1); //Enables SysTick exception request
	*pSCSR = *pSCSR | (1 << 2); //Indicates the clock source: 1 = processor clock.
	//enable systick
	*pSCSR = *pSCSR | (1 << 0); //Enables the counter
}

__attribute__ ((naked)) void init_scheduler_stack(uint32_t sched_top_of_stack){
	__asm volatile("MSR MSP,R0");  //1ST ARG IN MAIN WILL BE STORED IN R0
	__asm volatile("BX LR"); //THIS IS RETURN FROM FUNCTION CALL
}


void init_tasks_stack(void){
	uint32_t *pPSP;
	for(int i = 0; i < MAX_TASKS; i++){
		pPSP = (uint32_t*)psp_of_tasks[i];
		pPSP--;
		*pPSP = DUMMY_XPSR;//this value should be 0x01000000

		pPSP--;  //PC
		*pPSP = task_handlers[i];

		pPSP--;  //LR
		*pPSP = 0xFFFFFFFD;//tells to use psp and return to threadmode

		for(int j = 0; j < 13 ; j++){
			pPSP--;
			*pPSP = 0;
		}
		psp_of_tasks[i] = (uint32_t)pPSP;//updating new psp after dummy insert
	}
}

void enable_processor_faults(void){
	uint32_t *pSHSCR = (uint32_t*)0xE000ED24;
	*pSHSCR = *pSHSCR | (1 << 16);//MEM MANAGE FAULT
	*pSHSCR = *pSHSCR | (1 << 17);//BUS FAULT
	*pSHSCR = *pSHSCR | (1 << 18);//USAGE FAULT FAULT
}

uint32_t get_psp_value(void){
	return psp_of_tasks[current_task];
}

void save_psp_value(uint32_t current_psp_value){
	psp_of_tasks[current_task] = current_psp_value;
}

void update_next_task(void){
	current_task++;
	current_task = current_task % MAX_TASKS;
}


__attribute__ ((naked)) void switch_sp_to_psp(void){
	//1.initialize psp to task1 stack start address
		//get the value of psp of current task
	__asm volatile("PUSH {LR}");
	__asm volatile("BL get_psp_value"); //call a function
	__asm volatile("MSR PSP,R0");
	__asm volatile("POP {LR}");

	//2.change sp to psp using control register
	__asm volatile("MOV R0,#0x02");
	__asm volatile("MSR CONTROL,R0");
	__asm volatile("BX LR");
}


__attribute__ ((naked)) void SysTick_Handler(void){

	//save the context of current task
	//1,get current running task psp value
	__asm volatile("MRS R0,PSP");

	//2,using that psp value store sf2 (r4 to r11)
	__asm volatile("STMDB R0!, {R4-R11}");      //we use STMDB -- store multiple reg decrement before

	__asm volatile("PUSH {LR}");

	//3,save current value of psp
	__asm volatile("BL save_psp_value");


	//retrieve the context of next task
	//1,decide next task to run
	__asm volatile("BL update_next_task");
	//2,get its past psp value
	__asm volatile("BL get_psp_value");
	//3,using that psp value retrieve sf2(R4-R11)
	__asm volatile("LDMIA R0!, {R4-R11}"); //LDMIA - load multiple register increment after
	//4,update psp and exit
	__asm volatile("MSR PSP,R0");

	__asm volatile("POP {LR}");

	__asm volatile("BX LR");

}


/////////////////////////////implement fault handlers////////////////////////////////////////////////////
void HardFault_Handler(void){
	printf("exception : Hard fault\n");
	while(1);
}
void MemManage_Handler(void){
	printf("exception : Mem-manage fault\n");
	while(1);
}
void BusFault_Handler(void){
	printf("exception : Bus fault\n");
	while(1);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
