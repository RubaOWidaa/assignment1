#include "TM4C123GH6PM.h"
#include "uart.h"


void UART0_Init(void) {
    // Enable clock for UART0 and PortA
    SYSCTL->RCGCUART |= (1<<0);   // Enable clock for UART0
    SYSCTL->RCGCGPIO |= (1<<0);   // Enable clock for PortA
   
    // Configure UART0 pins
    
	  GPIOA->AFSEL |= (1<<1) | (1<<0);   // Enable alternate function for PA1, PA0
		GPIOA->PCTL &=~0XFF;               //Clear PCTL
    GPIOA->PCTL |= (1<<0) | (1<<4);    // Configure PA1, PA0 for UART
    GPIOA->DEN |= (1<<1) | (1<<0);     // Digital enable for PA1, PA0
    
    // UART0 Configuration
   
	  UART0->CTL &= ~(1<<0);             // Disable UART0
    UART0->IBRD = 104;                 // Set baud rate: Integer part
    UART0->FBRD = 11;                  // Set baud rate: Fractional part
    UART0->LCRH = (0x3<<5);            // 8-bit, no parity, 1-stop bit, no FIFO
    UART0->CC = 0x0;                   // Use system clock for UART
    UART0->IM |= (1<<4);               // Enable receive interrupt
    UART0->CTL |= (1<<9) | (1<<8) | (1<<0); // Enable UART0, TX, RX
    
    // Enable UART0 Interrupts in NVIC
    NVIC->ISER[0] |= (1<<5);           // Enable IRQ5 for UART0
		__enable_irq(); }

void UART0_Transmit(char data) {
    while ((UART0->FR & (1<<5)) != 0);  // Wait until the transmit FIFO is not full
    UART0->DR = data;                   // Transmit the data
}

volatile char received_char;
void UART0_Handler(void) {
    if (UART0->MIS & (1<<4)) {          // Check if the receive interrupt occurred
        received_char = UART0->DR;      // Read the received character
        
        // Check if the character is lowercase
        if (received_char >= 'a' && received_char <= 'z') {
            received_char = received_char - 32;  // Convert to uppercase
        } else {
            received_char = received_char + 1;   // Send successive character
        }
        
        UART0_Transmit(received_char);  // Transmit the processed character back
        
        UART0->ICR |= (1<<4);           // Clear the interrupt flag
    }
}

char UART0_Receive(void) {
    while ((UART0->FR & (1<<4)) != 0);  // Wait until the receive FIFO is not empty
    return UART0->DR;                   // Read the received data
}
