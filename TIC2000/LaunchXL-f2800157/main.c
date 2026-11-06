// LED blink + 40% duty PWM on J4-40 for LAUNCHXL-F2800157 (CPU1).
// LEDs: LED4=GPIO35 (red), LED5=GPIO49 (green), active-low
//       (SPRR476 schematic; device.h _LAUNCHXL_F2800157).
//       Note: SPRUJ54A §3.1.3 text saying GPIO20/22 is wrong vs schematic.
// PWM: J4.40 = GPIO30 / EPWM1A @ 20 kHz, 40% duty, up-count.
// EPWMCLK = SYSCLK = 120 MHz (no PERCLKDIVSEL / EPWMCLKDIV on F280015x;
// datasheet f(EPWM) max = 120 MHz).
#include "driverlib.h"
#include "device.h"

#define PWM_TBPRD   5999U   // TBCLK=120 MHz / 20 kHz - 1
#define PWM_CMPA    2400U   // 40% of (TBPRD+1)

static void initPwmJ4_40(void)
{
    // J4-40 (BoosterPack site 1) = GPIO30 = EPWM1A
    // Evidence: LAUNCHXL_F2800157.syscfg.json header pin 40 -> GPIO30;
    // SPRR476 schematic net J4_40 / GPIO30; pin_map.h GPIO_30_EPWM1_A.
    GPIO_setPinConfig(GPIO_30_EPWM1_A);
    GPIO_setPadConfig(30, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(30, GPIO_QUAL_SYNC);

    // F280015x: no SysCtl_setEPWMClockDivider — no PERCLKDIVSEL in hw_sysctl.h.
    // EPWMCLK = SYSCLK = 120 MHz (f(EPWM) max 120 MHz per datasheet).
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

    // LEDs need explicit pinmux (TI demo / E2E); active-low via open-drain buffer.
    GPIO_setPinConfig(DEVICE_GPIO_CFG_LED1);
    GPIO_setPadConfig(DEVICE_GPIO_PIN_LED1, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(DEVICE_GPIO_PIN_LED1, GPIO_DIR_MODE_OUT);
    GPIO_setPinConfig(DEVICE_GPIO_CFG_LED2);
    GPIO_setPadConfig(DEVICE_GPIO_PIN_LED2, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(DEVICE_GPIO_PIN_LED2, GPIO_DIR_MODE_OUT);

    initPwmJ4_40();

    Interrupt_initModule();
    Interrupt_initVectorTable();
    EINT; ERTM;

    for(;;)
    {
        GPIO_writePin(DEVICE_GPIO_PIN_LED1, 0); // active-low ON
        GPIO_writePin(DEVICE_GPIO_PIN_LED2, 1);
        DEVICE_DELAY_US(500000);
        GPIO_writePin(DEVICE_GPIO_PIN_LED1, 1);
        GPIO_writePin(DEVICE_GPIO_PIN_LED2, 0);
        DEVICE_DELAY_US(500000);
    }
}
