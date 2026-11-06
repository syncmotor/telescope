// LED blink + 40% duty PWM on J4-40 for LAUNCHXL-F280025C (CPU1).
// LEDs: LED4=GPIO31 (red), LED5=GPIO34 (green), active-low
//       (SPRUIW8 §3.1.2; SPRR425 schematic; device.h _LAUNCHXL_F280025C).
// PWM: J4.40 = GPIO0 / EPWM1A @ 20 kHz, 40% duty, up-count.
// EPWMCLK = SYSCLK = 100 MHz (no SysCtl_setEPWMClockDivider / EPWMCLKDIV
// field on F28002x; datasheet f(EPWM) max = f(SYSCLK) max = 100 MHz).
#include "driverlib.h"
#include "device.h"

#define PWM_TBPRD   4999U   // TBCLK=100 MHz / 20 kHz - 1
#define PWM_CMPA    2000U   // 40% of (TBPRD+1)

static void initPwmJ4_40(void)
{
    // J4-40 (BoosterPack site 1) = GPIO0 = EPWM1A
    // Evidence: LAUNCHXL_F280025C.syscfg.json header pin 40 -> GPIO0;
    // SPRR425 schematic net J4 pin 40 / GPIO0; pin_map.h GPIO_0_EPWM1_A;
    // syscfg PWM_LOC1 EPWM_A on ball 63 = GPIO0.
    GPIO_setPinConfig(GPIO_0_EPWM1_A);
    GPIO_setPadConfig(0, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(0, GPIO_QUAL_SYNC);

    // F28002x: no SysCtl_setEPWMClockDivider — hw_sysctl.h has only a
    // PERCLKDIVSEL lock bit, no EPWMCLKDIV field. EPWMCLK = SYSCLK = 100 MHz
    // (datasheet f(EPWM) max = 100 MHz).
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

    // LEDs: active-low via SN74LVC2G07 (SPRUIW8 §3.1.2).
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
