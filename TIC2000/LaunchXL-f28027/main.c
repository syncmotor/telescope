// LED blink + 40% duty PWM on J6-1 for LAUNCHXL-F28027 (runs from flash).
// LEDs: D2=GPIO0, D3=GPIO1, D4=GPIO2, D5=GPIO3, active-low via SN74LVC2G07 (U2/U3).
//       Evidence: SPRUHH2C Figure 3 schematic (GPIO0..3 → U2/U3 → D2..D5);
//       C2000Ware Example_F2802xLaunchPadDemo.c (GPIO0..3 as LED nibble, setLow = ON).
// PWM: J6-1 = GPIO0 / EPWM1A @ 20 kHz, 40% duty, up-count.
//       Evidence: SPRUHH2C Table 1 (J6 pin 1 → GPIO0 / EPWM1A);
//       f2802x_epwm.c InitEPwm1Gpio (GPAMUX1.GPIO0 = 1 → EPWM1A).
// CONFLICT: J6-1 / EPWM1A shares GPIO0 with LED D2. PWM owns GPIO0; we blink only
//           D4 (GPIO2) and D5 (GPIO3). D2 is lit ~60% of the time (active-low; on while EPWM1A is low), so it looks nearly steadily on. D3 (GPIO1) held off by its pull-up.
// Clock: INTOSC1 10 MHz × PLLCR 12 / DIVSEL 2 = SYSCLKOUT 60 MHz
//        (f2802x_examples.h; SPRUHH2C §4.4 — crystal footprint optional/not required).
// TBCLK = SYSCLKOUT / (HSPCLKDIV×CLKDIV) with both dividers = 1 → 60 MHz.
// CodeStartBranch @ 0x3F7FF6 (F28027.cmd BEGIN); InitFlash in ramfuncs.
#include "DSP28x_Project.h"
#include <string.h>

#define PWM_TBPRD   2999U   // TBCLK=60 MHz / 20 kHz - 1
#define PWM_CMPA    1200U   // 40% of (TBPRD+1)

static void initPwmJ6_1(void)
{
    // J6-1 = GPIO0 = EPWM1A (SPRUHH2C Table 1; InitEPwm1Gpio)
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
    EPwm1Regs.TBCTL.bit.CLKDIV    = TB_DIV1;  // /1 → TBCLK = 60 MHz

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
    InitSysCtrl();   // INTOSC1 → PLL 60 MHz; peripheral clocks (incl. EPWM1)

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

    // User LEDs D4/D5 on GPIO2/GPIO3 (active-low). GPIO0 reserved for EPWM1A / J6-1.
    GpioDataRegs.GPASET.bit.GPIO2 = 1;  // D4 off (latch high before output enable)
    GpioDataRegs.GPASET.bit.GPIO3 = 1;  // D5 off
    EALLOW;
    GpioCtrlRegs.GPAPUD.bit.GPIO1 = 0;  // enable pull-up: D3 (GPIO1) held off
    GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;
    GpioCtrlRegs.GPADIR.bit.GPIO3 = 1;
    EDIS;

    initPwmJ6_1();

    EINT;
    ERTM;

    for(;;)
    {
        GpioDataRegs.GPACLEAR.bit.GPIO2 = 1;  // D4 ON
        GpioDataRegs.GPASET.bit.GPIO3 = 1;    // D5 OFF
        DELAY_US(500000);
        GpioDataRegs.GPASET.bit.GPIO2 = 1;    // D4 OFF
        GpioDataRegs.GPACLEAR.bit.GPIO3 = 1;  // D5 ON
        DELAY_US(500000);
    }
}
