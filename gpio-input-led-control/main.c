
#include <stdint.h>

int main(void)
{
    //1. ENABLE CLK
	uint32_t *pClkCtrlReg = (uint32_t*)0x40023830;
    uint32_t *pPortAModeReg = (uint32_t*)0x40020000;   //address same for both pa0 and led
    uint32_t *pPortAOutReg = (uint32_t*)0x40020014; // for led we set 5th bit high to turn on led
    uint32_t *pPortAInReg = (uint32_t*)0x40020010;

    // RCC CLK ENB
    *pClkCtrlReg = *pClkCtrlReg | (1 << 0); //RCC for gpioa is enb

    //MODE SET FOR LED AND PA0
    *pPortAModeReg = *pPortAModeReg & ~(3 << 10); //clearing 10 and 11 th bit to 00
    *pPortAModeReg = *pPortAModeReg | (1 << 10);  //we set 10th bit to 1 so its in output mode
    //FOR PA0
    *pPortAModeReg = *pPortAModeReg & ~(3 << 0); //0 and 1 becomes 00,now it is in input mode

    while(1){
    	//READING THE PA0 PIN
    	uint32_t pinStatus = (uint32_t)*pPortAInReg & 0x1; //zero out all other bit other that bit zero
    	//we do the masking bcause to isolate the 0th bit only there may be other values on other pin but we need only the zeroth bit

    	//0000 1000
    	//0000 0001
    	//----------
    	//0000 0000

    	if(pinStatus){
    		//turn on led
    		*pPortAOutReg = *pPortAOutReg | (1 << 5);
    	}
    	else{
    		//turn off led
    		*pPortAOutReg = *pPortAOutReg & ~(1 << 5);
    	}
    }

}
