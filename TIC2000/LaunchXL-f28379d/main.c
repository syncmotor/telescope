// LED blink for LAUNCHXL-F28379D (CPU1), based on C2000Ware led_ex1_blinky.
// Toggles LD10 (GPIO31) and LD9 (GPIO34) alternately every 0.5 s.
#include "driverlib.h"
#include "device.h"
void main(void)
{
    Device_init();          // with _FLASH: copies ramfuncs to RAM, inits flash
    Device_initGPIO();
    GPIO_setPadConfig(DEVICE_GPIO_PIN_LED1, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(DEVICE_GPIO_PIN_LED1, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(DEVICE_GPIO_PIN_LED2, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(DEVICE_GPIO_PIN_LED2, GPIO_DIR_MODE_OUT);
    Interrupt_initModule();
    Interrupt_initVectorTable();
    EINT; ERTM;
    for(;;)
    {
        GPIO_writePin(DEVICE_GPIO_PIN_LED1, 0);   // LEDs are active-low
        GPIO_writePin(DEVICE_GPIO_PIN_LED2, 1);
        DEVICE_DELAY_US(500000);
        GPIO_writePin(DEVICE_GPIO_PIN_LED1, 1);
        GPIO_writePin(DEVICE_GPIO_PIN_LED2, 0);
        DEVICE_DELAY_US(500000);
    }
}
