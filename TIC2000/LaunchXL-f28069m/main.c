// LED blink + 40% duty PWM on J4-40 for LAUNCHXL-F28069M (runs from flash).
// LEDs: D9=GPIO34 (red), D10=GPIO39 (blue), active-low via SN74LVC2G07 (U9).
//       Evidence: SPRUI11B schematic F28069_B; C2000Ware Example_F2806xLaunchPadDemo.c;
//       MathWorks C2000 blockset F28069M LED pins 34/39.
// PWM: J4-40 = GPIO0 / EPWM1A @ 20 kHz, 40% duty, up-count.
//       Evidence: SPRUI11B Table 4 (J4 pin 40 → GPIO0 / EPWM1A);
//       F2806x_EPwm.c InitEPwm1Gpio (GPAMUX1.GPIO0 = 1 → EPWM1A).
// Clock: INTOSC1 10 MHz × PLLCR 18 / DIVSEL 2 = SYSCLKOUT 90 MHz
//        (F2806x_Examples.h; SPRUI11B §5.4 — crystal footprint optional/not fitted).
// TBCLK = SYSCLKOUT / (HSPCLKDIV×CLKDIV) with both dividers = 1 → 90 MHz.
// CodeStartBranch @ 0x3F7FF6 (F28069M.cmd BEGIN); InitFlash in ramfuncs.
#include "DSP28x_Project.h"
#include <string.h>

#define PWM_TBPRD   4499U   // TBCLK=90 MHz / 20 kHz - 1
#define PWM_CMPA    1800U   // 40% of (TBPRD+1)

static void initPwmJ4_40(void)
{
    // J4-40 = GPIO0 = EPWM1A (SPRUI11B Table 4; InitEPwm1Gpio)
    EALLOW;
    GpioCtrlRegs.GPAPUD.bit.GPIO0 = 1;    // disable pull-up on EPWM1A
    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1;   // GPIO0 → EPWM1A
    EDIS;

    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
    EDIS;

    EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;
    EPwm1Regs.TBPRD = PWM_TBPRD;
    EPwm1Regs.TBCTR = 0;
    EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;
    EPwm1Regs.TBPHS.half.TBPHS = 0;
    EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;  // /1
    EPwm1Regs.TBCTL.bit.CLKDIV    = TB_DIV1;  // /1 → TBCLK = 90 MHz

    EPwm1Regs.CMPA.half.CMPA = PWM_CMPA;
    EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;

    // High at ZRO, low at CAU → 40% high in up-count
    EPwm1Regs.AQCTLA.bit.ZRO = AQ_SET;
    EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR;

    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EDIS;
}

void main(void)
{
    InitSysCtrl();   // INTOSC1 → PLL 90 MHz; peripheral clocks (incl. EPWM1)

#ifdef _FLASH
    // Copy ramfuncs (InitFlash) from Flash to RAM, then configure flash WS.
    memcpy(&RamfuncsRunStart, &RamfuncsLoadStart, (size_t)&RamfuncsLoadSize);
    InitFlash();
#endif

    DINT;
    InitPieCtrl();
    IER = 0x0000;
    IFR = 0x0000;
    InitPieVectTable();

    // User LEDs: GPIO34 (red) / GPIO39 (blue), active-low
    EALLOW;
    GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0;
    GpioCtrlRegs.GPBMUX1.bit.GPIO39 = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;
    GpioCtrlRegs.GPBDIR.bit.GPIO39 = 1;
    EDIS;
    GpioDataRegs.GPBSET.bit.GPIO34 = 1;  // off
    GpioDataRegs.GPBSET.bit.GPIO39 = 1;  // off

    initPwmJ4_40();

    EINT;
    ERTM;

    for(;;)
    {
        GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1;  // red ON
        GpioDataRegs.GPBSET.bit.GPIO39 = 1;  // blue OFF
        DELAY_US(500000);
        GpioDataRegs.GPBSET.bit.GPIO34 = 1;  // red OFF
        GpioDataRegs.GPBCLEAR.bit.GPIO39 = 1;  // blue ON
        DELAY_US(500000);
    }
}
