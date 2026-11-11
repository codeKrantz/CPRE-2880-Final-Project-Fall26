
#include "adc.h"
#include "timer.h"
#include "lcd.h"

void adc_init(void)
{
    // Enable GPIO Port B and ADC0 clocks
    SYSCTL_RCGCGPIO_R |= 0x02;
    SYSCTL_RCGCADC_R  |= 0x01;

    // Wait until GPIO Port B and ADC0 are ready , may not need
    //while ((SYSCTL_PRGPIO_R & 0x02) == 0) {}
    //while ((SYSCTL_PRADC_R & 0x01) == 0) {}

    // Configure PB4 for AIN10
    GPIO_PORTB_DIR_R    &= ~0x10;
    GPIO_PORTB_AFSEL_R  |=  0x10;
    GPIO_PORTB_DEN_R    &= ~0x10;
    GPIO_PORTB_AMSEL_R  |=  0x10;

    // No GPIO-triggered conversions
    // GPIOADCCTL defaults to disabled as per the lecture notes

    // Configure ADC0 Sample Sequencer 3
    ADC0_ACTSS_R &= ~0x08;
    ADC0_EMUX_R  &= ~0xF000;
    ADC0_SSMUX3_R = 0x0A;
    ADC0_SSCTL3_R = 0x06;
    ADC0_SAC_R    = 0x04; //averages 16 reads
    ADC0_IM_R    &= ~0x08;

    // Clear pending completion flag
    ADC0_ISC_R = 0x08;

    // Enable Sample Sequencer 3
    ADC0_ACTSS_R |= 0x08;
}

int adc_read(void)
{
    // Start ADC0 SS3 conversion
    ADC0_PSSI_R = 0x08;

    // Poll until conversion completes
    while ((ADC0_RIS_R & 0x08) == 0) { //DEFFINTALY NEED
        //do nothing loop
    }

    // Read 12-bit result
    int value = ADC0_SSFIFO3_R & 0x0FFF;

    // Clear conversion-complete flag
    ADC0_ISC_R = 0x08;

    return value;
}
