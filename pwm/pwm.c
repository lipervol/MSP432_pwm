#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdio.h>
#include <oled.h>

float PER;
char SHOW[20];

Timer_A_PWMConfig pwmConfig =
{
        TIMER_A_CLOCKSOURCE_SMCLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_1,
        32000,
        TIMER_A_CAPTURECOMPARE_REGISTER_1,
        TIMER_A_OUTPUTMODE_RESET_SET,
        3200
};

int main(void)
{
    MAP_WDT_A_holdTimer();

    MAP_CS_initClockSignal(CS_MCLK,CS_MODOSC_SELECT,CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_SMCLK, CS_MODOSC_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_PCM_setPowerState(PCM_AM_LF_VCORE0);

    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN4,
            GPIO_PRIMARY_MODULE_FUNCTION);
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);

    MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);

    MAP_Interrupt_enableInterrupt(INT_PORT1);
    MAP_Interrupt_enableSleepOnIsrExit();
    MAP_Interrupt_enableMaster();

    OLED_DisplayOn();
    OLED_Clear();
    PER=(float)pwmConfig.dutyCycle/320;
    sprintf(SHOW,"Current: %.1f %%",PER);
    OLED_ShowString(10,3,(uint8_t*)SHOW,8);

    while (1)
    {
        MAP_PCM_gotoLPM0();
    }
}

void PORT1_IRQHandler(void)
{
    uint32_t status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, status);

    if (status & GPIO_PIN1)
    {
        if(pwmConfig.dutyCycle == 28800){
            pwmConfig.dutyCycle = 3200;
            PER=(float)pwmConfig.dutyCycle/320;
            sprintf(SHOW,"Current: %.1f %%",PER);
            OLED_ShowString(10,3,(uint8_t*)SHOW,8);
        }
        else{
            pwmConfig.dutyCycle += 3200;
            PER=(float)pwmConfig.dutyCycle/320;
            sprintf(SHOW,"Current: %.1f %%",PER);
            OLED_ShowString(10,3,(uint8_t*)SHOW,8);
        }

        MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
    }
}
