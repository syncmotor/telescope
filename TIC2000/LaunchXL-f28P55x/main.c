// LED blink + 25% duty PWM on GPIO0 for LAUNCHXL-F28P55X (CPU1).
// LEDs: LED4=GPIO20, LED5=GPIO21 (active-low), alternate every 0.5 s.
// PWM: EPWM1A on GPIO0 @ 20 kHz, 25% duty, up-count.
// EPWMCLK = SYSCLK = 150 MHz (no EPWMCLKDIV on F28P55x).
#include "driverlib.h"
#include "device.h"

#define PWM_TBPRD   7499U   // TBCLK=150 MHz / 20 kHz - 1
#define PWM_CMPA    1875U   // 25% of (TBPRD+1)

static void initPwmGpio0(void)
{
    GPIO_setPinConfig(GPIO_0_EPWM1_A);
    GPIO_setPadConfig(0, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(0, GPIO_QUAL_SYNC);

    // F28P55x: no EPWMCLKDIV; EPWMCLK = SYSCLK = 150 MHz.
    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

    EPWM_setTimeBasePeriod(EPWM1_BASE, PWM_TBPRD);
    EPWM_setTimeBaseCounter(EPWM1_BASE, 0);
    EPWM_setTimeBaseCounterMode(EPWM1_BASE, EPWM_COUNTER_MODE_UP);
    EPWM_setClockPrescaler(EPWM1_BASE, EPWM_CLOCK_DIVIDER_1, EPWM_HSCLOCK_DIVIDER_1);
    EPWM_disablePhaseShiftLoad(EPWM1_BASE);
    EPWM_setPhaseShift(EPWM1_BASE, 0);

    EPWM_setCounterCompareValue(EPWM1_BASE, EPWM_COUNTER_COMPARE_A, PWM_CMPA);
    EPWM_setCounterCompareShadowLoadMode(EPWM1_BASE, EPWM_COUNTER_COMPARE_A,
                                         EPWM_COMP_LOAD_ON_CNTR_ZERO);

    EPWM_setActionQualifierAction(EPWM1_BASE, EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_HIGH,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
    EPWM_setActionQualifierAction(EPWM1_BASE, EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_LOW,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);

    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);
}

void main(void)
{
    Device_init();
    Device_initGPIO();

    GPIO_setPadConfig(DEVICE_GPIO_PIN_LED1, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(DEVICE_GPIO_PIN_LED1, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(DEVICE_GPIO_PIN_LED2, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(DEVICE_GPIO_PIN_LED2, GPIO_DIR_MODE_OUT);

    initPwmGpio0();

    Interrupt_initModule();
    Interrupt_initVectorTable();
    EINT; ERTM;

    for(;;)
    {
        GPIO_writePin(DEVICE_GPIO_PIN_LED1, 0); // active-low LEDs
        GPIO_writePin(DEVICE_GPIO_PIN_LED2, 1);
        DEVICE_DELAY_US(500000);
        GPIO_writePin(DEVICE_GPIO_PIN_LED1, 1);
        GPIO_writePin(DEVICE_GPIO_PIN_LED2, 0);
        DEVICE_DELAY_US(500000);
    }
}
