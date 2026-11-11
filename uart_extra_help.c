/*
 *
 *   uart_extra_help.c
 * Description: This is file is meant for those that would like a little
 *              extra help with formatting their code, and followig the Datasheet.
 */

#include "uart_extra_help.h"
#include "timer.h"
#define REPLACE_ME 0x00

void uart_init(int baud)
{
    SYSCTL_RCGCGPIO_R |= 0x02;      // enable clock GPIOB (page 340)
    SYSCTL_RCGCUART_R |= 0x02;      // enable clock UART1 (page 344)
    GPIO_PORTB_AFSEL_R |= 0x03;     // sets PB0 and PB1 as peripherals (page 671)
    GPIO_PORTB_PCTL_R = 0x11; // pmc0 and pmc1       (page 688)  also refer to page 650
    GPIO_PORTB_DEN_R |= 0x03;        // enables pb0 and pb1
    GPIO_PORTB_DIR_R = 0x02;        // sets pb0 as output, pb1 as input

    //compute baud values [UART clock= 16 MHz]
    double fbrd;
    int ibrd;

    //fbrd = REPLACE_ME; // page 903
    ibrd = 8;
    fbrd = 44;

    UART1_CTL_R &= 0xFFFFFFFE;      // disable UART1 (page 918)
    UART1_IBRD_R = 0x8;        // write integer portion of BRD to IBRD
    UART1_FBRD_R = 0x2C;   // write fractional portion of BRD to FBRD
    UART1_LCRH_R = 0x60; // write serial communication parameters (page 916) * 8bit and no parity
    UART1_CC_R = 0x00;          // use system clock as clock source (page 939)
    UART1_CTL_R |= 0x1;        // enable UART1

}

void uart_sendChar(char data)
{
    //TODO
    while ((UART1_FR_R & 0x20) != 0)
        ;
    UART1_DR_R = data;

}

char uart_receive(void)
{
    //Gets the input from putty as a char, don't need to limit here yet
    uint32_t received;
    char return_Data;

    while ((UART1_FR_R & 0x10) != 0)
        ; //do nothing loop
    received = UART1_DR_R;

    if (received & 0xF00)
    {
        // may not be needed? GPIO_PORTB_DIR_R = 0xF; // error Check - be Cautious about GPIOB
        return '\0';
    }
    else
    {
        return_Data = (char) (received & 0xFF);
    }

    return return_Data;
}

void send_string(const char *data)
{
    //TODO
    while (*data != '\0')
    {
        uart_sendChar(*data);
        data++;
    }
}

// _PART3

void uart_interrupt_init()
{
    // Enable interrupts for receiving bytes through UART1
    UART1_IM_R |= 0x10; //enable interrupt on receive - page 924

    // Find the NVIC enable register and bit responsible for UART1 in table 2-9
    // Note: NVIC register descriptions are found in chapter 3.4
    NVIC_EN0_R |= 0x40; //enable uart1 interrupts - page 104

    // Find the vector number of UART1 in table 2-9 ! UART1 is 22 from vector number page 104 - (0x00000058 - vector address)
    IntRegister(INT_UART1, uart_interrupt_handler); //give the microcontroller the address of our interrupt handler - page 104 22 is the vector number

}

void uart_interrupt_handler()
{
// STEP1: Check the Masked Interrupt Status

//STEP2:  Copy the data

//STEP3:  Clear the interrupt

    UART1_ICR_R = 0x50;

    while ((UART1_FR_R & 0x10) == 0)
    {
        char idata = (UART1_DR_R & 0xFF);
        //uart_sendChar(idata);

        /*if (idata == '\r')
         {
         uart_sendChar('\n');
         }*/
        //attempt at toggle
        if (idata == 't')
        {
            if (flag == 0)
            {
                send_string("INTERUPT CAUGHT T FOR TOGGLE OFF\r\n");
                flag = 1;
            }
            else{
                send_string("INTERUPT CAUGHT T FOR TOGGLE ON\r\n");
                flag = 0;

            }
        }
        /* else if(idata == 'b')
         {
         send_string("AUTO MODE REENABLED\r\n");
         flag  = 0;
         }*/
    }
}
