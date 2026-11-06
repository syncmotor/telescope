// LED blink + 40% duty PWM on J4-40 for LAUNCHXL-F280049C (CPU1).
// LEDs: LED4=GPIO23 (red), LED5=GPIO34 (green), active-low (SPRUII7B).
// PWM: J4.40 = GPIO10 / EPWM6A @ 20 kHz, 40% duty, up-count.
// EPWMCLK = SYSCLK = 100 MHz (EPWMCLKDIV hardwired /1 on F28004x).
#include "driverlib.h"
#include "device.h"

#define PWM_TBPRD   4999U   // TBCLK=100 MHz / 20 kHz - 1
#define PWM_CMPA    2000U   // 40% of (TBPRD+1)

static void initPwmJ4_40(void)
{
    // J4-40 (BoosterPack site 1) = GPIO10 = EPWM6A
    // Evidence: SPRUII7B Fig.3 / connector table; TIDU986A Table 2 (J4[40] GPIO10 EPWM6_A)
    GPIO_setPinConfig(GPIO_10_EPWM6_A);
    GPIO_setPadConfig(10, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(10, GPIO_QUAL_SYNC);

    // F28004x: no SysCtl_setEPWMClockDivider — EPWMCLKDIV fixed at /1, EPWMCLK=SYSCLK.
    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

    EPWM_setTimeBasePeriod(EPWM6_BASE, PWM_TBPRD);
    EPWM_setTimeBaseCounter(EPWM6_BASE, 0);
    EPWM_setTimeBaseCounterMode(EPWM6_BASE, EPWM_COUNTER_MODE_UP);
    EPWM_setClockPrescaler(EPWM6_BASE, EPWM_CLOCK_DIVIDER_1, EPWM_HSCLOCK_DIVIDER_1);
    EPWM_disablePhaseShiftLoad(EPWM6_BASE);
    EPWM_setPhaseShift(EPWM6_BASE, 0);

    EPWM_setCounterCompareValue(EPWM6_BASE, EPWM_COUNTER_COMPARE_A, PWM_CMPA);
    EPWM_setCounterCompareShadowLoadMode(EPWM6_BASE, EPWM_COUNTER_COMPARE_A,
                                         EPWM_COMP_LOAD_ON_CNTR_ZERO);

    EPWM_setActionQualifierAction(EPWM6_BASE, EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_HIGH,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
    EPWM_setActionQualifierAction(EPWM6_BASE, EPWM_AQ_OUTPUT_A,
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

    initPwmJ4_40();

    Interrupt_initModule();
    Interrupt_initVectorTable();
    EINT; ERTM;

    for(;;)
    {
        GPIO_writePin(DEVICE_GPIO_PIN_LED1, 0); // active-low
        GPIO_writePin(DEVICE_GPIO_PIN_LED2, 1);
        DEVICE_DELAY_US(500000);
        GPIO_writePin(DEVICE_GPIO_PIN_LED1, 1);
        GPIO_writePin(DEVICE_GPIO_PIN_LED2, 0);
        DEVICE_DELAY_US(500000);
    }
}
