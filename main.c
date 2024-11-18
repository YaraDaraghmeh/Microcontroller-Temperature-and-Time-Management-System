/*
 * File:   main.c
 * Author: Yara_2
 *
 * Created on October 23, 2024, 1:06 PM
 */

#define _XTAL_FREQ   4000000UL     // needed for the delays, set to 4 MH= your crystal frequency
// CONFIG1H
#pragma config OSC = XT         // Oscillator Selection bits (XT oscillator)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 3         // Brown Out Reset Voltage bits (Minimum setting)

// CONFIG2H
#pragma config WDT = ON         // Watchdog Timer Enable bit (WDT enabled)
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = PORTC   // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = ON      // PORTB A/D Enable bit (PORTB<4:0> pins are configured as analog input channels on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = ON         // Single-Supply ICSP Enable bit (Single-Supply ICSP enabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-003FFFh) not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (004000-007FFFh) not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (008000-00BFFFh) not code-protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (00C000-00FFFFh) not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-003FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (004000-007FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (008000-00BFFFh) not write-protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (00C000-00FFFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (004000-007FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (008000-00BFFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (00C000-00FFFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF   



#include <xc.h>
#include <xc.h>
#include <stdio.h>
#include "my_ser.h"
#include "my_adc.h"

#include "lcd_x8.h"
#include <string.h> 


#define STARTVALUE  3036    // Timer0 reload value


// Global Variables
char Buffer1[16], Buffer2[16], Buffer3[16], Buffer4[16];
int sec_sel = 0, min_sel = 0, hour_sel = 0;
int Seconds = 0;
int Minutes = 0;
int Hours = 0;

int change_mode = 0;   // 0-> sec, 1-> min, 2-> hour
float Temperature = 0;
int AnORName = 0;      // 0->AN, 1->Names
int coolerStatus =  0;
int heaterStatus = 0;
int Current_Mode;
#define   MODE_SETUP 1
#define  MODE_NORMAL 0

unsigned char current_mode = MODE_NORMAL;


void reloadTimer0(void);
void setupPorts(void);
void initTimers0(void);
void increment_clock(void);
void display(void);

char g;
char time_buffer[10];
int time_index = 0;

void RX_isr(void) {
    g = RCREG;

    if (g == 't') {
        char time_response[30];
        sprintf(time_response, "Time: %02d:%02d:%02d\r\n", Hours, Minutes, Seconds);
        send_string_no_lib((unsigned char *)time_response);
    } 
    else if (g == 'T') {
        char temp_response[20];
        sprintf(temp_response, "T: %.2f\r\n", Temperature);
        send_string_no_lib((unsigned char *)temp_response);
    } 
    else if (g == 's') {
        char status_response[50];
        sprintf(status_response, "Heater: %s\r\nCooler: %s\r\n", 
                heaterStatus ? "ON" : "OFF", 
                coolerStatus ? "ON" : "OFF");
        send_string_no_lib((unsigned char *)status_response);
    }
    else if (g == 'w') {
        time_index = 0;
    } 
    else if (time_index < 9) {
        time_buffer[time_index++] = g;
        
        if (time_index == 9) {
            time_buffer[9] = '\0';
            int hours, minutes, seconds;
            
            if (time_buffer[2] == ':' && time_buffer[5] == ':') {
                hours = (time_buffer[0] - '0') * 10 + (time_buffer[1] - '0');
                minutes = (time_buffer[3] - '0') * 10 + (time_buffer[4] - '0');
                seconds = (time_buffer[6] - '0') * 10 + (time_buffer[7] - '0');
                
                if (hours >= 0 && hours < 24 && minutes >= 0 && minutes < 60 && seconds >= 0 && seconds < 60) {
                    Hours = hours;
                    Minutes = minutes;
                    Seconds = seconds;
                    send_string_no_lib((unsigned char *)"Time updated successfully\r\n");
                } else {
                    send_string_no_lib((unsigned char *)"Invalid time format\r\n");
                }
            } else {
                send_string_no_lib((unsigned char *)"Invalid time format\r\n");
            }
            
            time_index = 0;
        }
    }
}

   
void reloadTimer0(void) {
    TMR0H = (unsigned char)((STARTVALUE >> 8) & 0x00FF);
    TMR0L = (unsigned char)(STARTVALUE & 0x00FF);    INTCON3bits.INT1F = 0;

}
void __interrupt(high_priority) highIsr(void) { 
    // Timer0 interrupt handling
    if(INTCONbits.TMR0IF) {
        
        reloadTimer0();
        if(Current_Mode == MODE_NORMAL) {  // Only increment in normal mode
            increment_clock();
        }
        INTCONbits.TMR0IF = 0;
         Temperature= 100 *((read_adc_voltage((unsigned char) 2)));
    }
    
    // Mode change interrupt (INT1)
   if(INTCON3bits.INT1F) {
    __delay_ms(100);  // Initial debounce
    if(!PORTBbits.RB1) {  // Check if button is pressed (active low)
        Current_Mode = !Current_Mode;  // Toggle mode
        if(Current_Mode == MODE_SETUP) {
            T0CONbits.TMR0ON = 0;  // Stop timer in setup
            change_mode = 0;        // Start with seconds
        } else {
            T0CONbits.TMR0ON = 1;  // Resume timer
        }
    }
    INTCON3bits.INT1F = 0;  // Clear the interrupt flag
}

// INT2 handler for changing between hours/minutes/seconds
if(INTCON3bits.INT2IF) {
    __delay_ms(70);  // Debounce
    if(!PORTBbits.RB2 && Current_Mode == MODE_SETUP) {  // Check if button is pressed and in setup mode
        if(change_mode == 0) change_mode = 2;      // From seconds to hours
        else if(change_mode == 2) change_mode = 1;  // From hours to minutes
        else change_mode = 0;                       // From minutes to seconds
    }
    INTCON3bits.INT2IF = 0;  // Clear the interrupt flag
}
    
    //cooler
    else if (INTCONbits.INT0IF) {
        __delay_ms(50); 
       if (!PORTBbits.RB0) {
       LATCbits.LATC2 = !LATCbits.LATC2; 
       if (LATCbits.LATC2) {
           coolerStatus=1;
            send_string_no_lib((unsigned char *)"Cooler ON\r\n");
        } else {
           coolerStatus=0;
            send_string_no_lib((unsigned char *)"Cooler OFF\r\n");
        }
           
            while (!PORTBbits.RB0); // Wait until button is released
        }
        INTCONbits.INT0IF = 0; 
    }
    
    else if(PIR1bits.RCIF) RX_isr();
    
    
}












void setupPorts(void) {
    ADCON0 = 0;
    ADCON1 = 0b000001100;    // 3 analog channels, A12
    
    TRISB = 0xFF;           // All pushbuttons are inputs
    TRISC = 0x80;           // RX input, others output 
    TRISA = 0xFF;           // All inputs
    TRISD = 0x00;           // All outputs
    TRISE = 0x00;           // All outputs
    TRISCbits.TRISC2=0;
    TRISCbits.TRISC5=0;
    LATCbits.LATC2=0;
    LATCbits.LATC5=0;
    
    LATCbits.LATC2 = 0;  // Turn on cooler
    
    
    // here if i want to setup the serial 
    setupSerial(); 
}

void initTimers0(void) {
    // Clear all interrupt flags first
    INTCON = 0;
    
    // Enable specific interrupts
    INTCONbits.INT0E = 1;
    INTCONbits.T0IE = 1;
    
    // Set interrupt edges
    INTCON2 = 0;
    INTCON2bits.INTEDG0 = 1;
    INTCON2bits.INTEDG1 = 1;
    INTCON2bits.INTEDG2 = 1;
    
    // Enable external interrupts
    INTCON3 = 0;
    INTCON3bits.INT1E = 1;
    INTCON3bits.INT2E = 1;
    
    
    // Setup Timer0
    T0CON = 0;
    reloadTimer0();
    
    // Interrupt priority and peripheral settings
    RCONbits.IPEN = 0;      // Disable interrupt priority
    PIE1 = 0;
    PIE1bits.RCIE = 1;
    PIR1 = 0;
    PIE2 = 0;
    
    // Enable global interrupts
    INTCONbits.GIEH = 1;
    INTCONbits.GIEL = 1;
    
    // Configure and start Timer0
    T0CONbits.T0PS = 0b011; // 16 Prescaler
    T0CONbits.TMR0ON = 1;   // Start timer
}

void increment_clock(void) {
    Seconds++;
    if(Seconds >= 60) {
        Seconds = 0;
        Minutes++;
        if(Minutes >= 60) {
            Minutes = 0;
            Hours++;
            if(Hours >= 24) {
                Hours = 0;
            }
        }
    }
}

void display(void) {
    char buffer[21];  // Buffer for each line
    
    // Ensure display is on and cursor is off at start of display update
    lcd_gotoxy(1, 1);
    sprintf(buffer, "%02d:%02d:%02d T:%3.2f", Hours, Minutes, Seconds, Temperature);
    lcd_puts(buffer);

    // Line 2: Status (Heater and Cooler)
    lcd_gotoxy(1, 2);
    sprintf(buffer, "H:%s C:%s        ", 
            heaterStatus ? "ON " : "OFF",
            coolerStatus ? "ON " : "OFF");
    lcd_puts(buffer);

    // Line 3: Mode display
    lcd_gotoxy(1, 3);
    if(Current_Mode == MODE_SETUP) {
        sprintf(buffer, "Mode: Setup-%c     ", 
                (change_mode == 0) ? 'S' : 
                (change_mode == 1) ? 'M' : 'H');
    } else {
        sprintf(buffer, "Mode: Normal      ");
    }
    lcd_puts(buffer);

    // Line 4: Names with explicit positioning and padding
    lcd_gotoxy(1, 4);
    strcpy(buffer, "   Yara - Deema       ");  // Using strcpy for fixed string
    lcd_puts(buffer);
    
    // Return cursor to home position
    lcd_gotoxy(1, 1);
}

// Add this to your initialization section in main()
void init_lcd_settings(void) {
    lcd_init();
    __delay_ms(100);      // Add delay after initialization
    lcd_gotoxy(1, 1);     // Set initial cursor position
}


void main(void) {
    // Initialize peripherals
    setupPorts();
    lcd_init();
    init_adc_no_lib();
  
    initTimers0();
    setupSerial();
    
    Current_Mode = MODE_NORMAL;
    
    
    LATCbits.LATC2 = 0; 
    TRISC = 0x80;
    TRISCbits.TRISC2=0;
    TRISCbits.TRISC5=0;
    LATCbits.LATC2=0;
    LATCbits.LATC5=0;
    
     send_string_no_lib((unsigned char *)"\r\nStart Of Program \r\n");
    while(1) {
        if(Current_Mode == MODE_SETUP) {
            // Setup mode button handling
            if(PORTBbits.RB4 == 0) {  // Decrement
                __delay_ms(100);
                switch(change_mode) {
                    case 0:  // Seconds
                        Seconds = (Seconds > 0) ? Seconds - 1 : 59;
                        break;
                    case 1:  // Minutes
                        Minutes = (Minutes > 0) ? Minutes - 1 : 59;
                        break;
                    case 2:  // Hours
                        Hours = (Hours > 0) ? Hours - 1 : 23;
                        break;
                }
            }
            
            if(PORTBbits.RB3 == 0) {  // Increment
                __delay_ms(200);
                switch(change_mode) {
                    case 0:  // Seconds
                        Seconds = (Seconds + 1) % 60;
                        break;
                    case 1:  // Minutes
                        Minutes = (Minutes + 1) % 60;
                        break;
                    case 2:  // Hours
                        Hours = (Hours + 1) % 24;
                        break;
                }
            }
        } else {
            // Normal mode button handling
           
            if(PORTBbits.RB4 == 0) {  // Cooler toggle
                __delay_ms(200);
                heaterStatus = !heaterStatus;
                LATCbits.LATC5 =heaterStatus;
                PORTCbits.RC5=heaterStatus;
            }
        }
        
        display();
       
    }
}