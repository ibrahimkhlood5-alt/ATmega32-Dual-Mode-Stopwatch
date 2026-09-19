/*/*
 * Stopwatch_Dual_Mode.c
 * Microcontroller: ATmega32
 * Clock Frequency: 8 MHz
 * Author: Embedded Systems Engineer
 */

#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// ------------------- Macro Definitions -------------------
#define COUNT_UP    0
#define COUNT_DOWN  1

// Global Variables
volatile unsigned char g_seconds = 0;
volatile unsigned char g_minutes = 0;
volatile unsigned char g_hours   = 0;

volatile unsigned char g_mode = COUNT_UP; // Default Mode: Count Up
volatile unsigned char g_alarm_flag = 0;

// ------------------- Function Prototypes -------------------
void System_Init(void);
void Timer1_CTC_Init(void);
void INT0_Init(void);
void INT1_Init(void);
void INT2_Init(void);
void Display_Multiplexed(void);
void Buttons_Handling(void);

// ------------------- Main Function -------------------
int main(void)
{
    System_Init();

    while (1)
    {
        Display_Multiplexed();
        Buttons_Handling();
    }
}

// ------------------- Interrupt Service Routines -------------------

/* Timer1 Compare Match A ISR - Triggers every 1 Second at 8MHz */
ISR(TIMER1_COMPA_vect)
{
    if (g_mode == COUNT_UP)
    {
        g_seconds++;
        if (g_seconds == 60)
        {
            g_seconds = 0;
            g_minutes++;
            if (g_minutes == 60)
            {
                g_minutes = 0;
                g_hours++;
                if (g_hours == 100)
                {
                    g_hours = 0;
                }
            }
        }
    }
    else if (g_mode == COUNT_DOWN)
    {
        if (g_seconds == 0 && g_minutes == 0 && g_hours == 0)
        {
            // Reached Zero -> Activate Alarm
            PORTD |= (1 << PD0); // Turn ON Buzzer
            g_alarm_flag = 1;
        }
        else
        {
            if (g_seconds == 0)
            {
                g_seconds = 59;
                if (g_minutes == 0)
                {
                    g_minutes = 59;
                    if (g_hours > 0)
                    {
                        g_hours--;
                    }
                }
                else
                {
                    g_minutes--;
                }
            }
            else
            {
                g_seconds--;
            }
        }
    }
}

/* INT0 ISR - Reset Stopwatch (Falling Edge) */
ISR(INT0_vect)
{
    g_seconds = 0;
    g_minutes = 0;
    g_hours   = 0;
    PORTD &= ~(1 << PD0); // Turn OFF Buzzer
    g_alarm_flag = 0;
}

/* INT1 ISR - Pause Stopwatch (Rising Edge) */
ISR(INT1_vect)
{
    // Stop Timer1 Clock (No Clock Source)
    TCCR1B &= ~((1 << CS10) | (1 << CS11) | (1 << CS12));
}

/* INT2 ISR - Resume Stopwatch (Falling Edge) */
ISR(INT2_vect)
{
    // Resume Timer1 Clock with Prescaler 1024
    TCCR1B |= (1 << CS10) | (1 << CS12);
}

// ------------------- System Initializations -------------------

void System_Init(void)
{
    // 1. Decoder (PC0-PC3) Output
    DDRC |= 0x0F;
    PORTC &= ~0x0F;

    // 2. 7-Segment Enablers (PA0-PA5) Output
    DDRA |= 0x3F;
    PORTA |= 0x3F; // All enabled initially

    // 3. Status LEDs & Buzzer (PD0, PD4, PD5) Output
    DDRD |= (1 << PD0) | (1 << PD4) | (1 << PD5);
    PORTD &= ~(1 << PD0); // Buzzer OFF
    PORTD |= (1 << PD4);  // Red LED ON (Count Up by default)
    PORTD &= ~(1 << PD5); // Yellow LED OFF

    // 4. Mode Button & Adjustment Buttons (PORTB) Input with Internal Pull-Ups
    DDRB &= ~0xFF;  // PORTB all inputs
    PORTB |= 0xFF;  // Enable Internal Pull-Ups for PB0-PB7

    // 5. Initialize Interrupts & Timer
    INT0_Init();
    INT1_Init();
    INT2_Init();
    Timer1_CTC_Init();

    // Enable Global Interrupts
    sei();
}

void Timer1_CTC_Init(void)
{
    TCNT1 = 0;
    OCR1A = 7812; // (8MHz / (1024 * 1Hz)) - 1 = 7811.5 ≈ 7812 for 1 second interval

    // Mode: CTC (Clear Timer on Compare Match) -> WGM12 = 1
    TCCR1A = (1 << FOC1A);
    TCCR1B = (1 << WGM12) | (1 << CS10) | (1 << CS12); // Prescaler = 1024

    TIMSK |= (1 << OCIE1A); // Enable Compare Match A Interrupt
}

void INT0_Init(void)
{
    DDRD &= ~(1 << PD2);   // INT0 Pin (PD2) Input
    PORTD |= (1 << PD2);   // Enable Internal Pull-Up
    MCUCR |= (1 << ISC01); // Falling Edge
    MCUCR &= ~(1 << ISC00);
    GICR |= (1 << INT0);   // Enable INT0
}

void INT1_Init(void)
{
    DDRD &= ~(1 << PD3);    // INT1 Pin (PD3) Input (External Pull-Down used)
    MCUCR |= (1 << ISC11) | (1 << ISC10); // Rising Edge
    GICR |= (1 << INT1);    // Enable INT1
}

void INT2_Init(void)
{
    DDRB &= ~(1 << PB2);    // INT2 Pin (PB2) Input
    PORTB |= (1 << PB2);    // Enable Internal Pull-Up
    MCUCSR &= ~(1 << ISC2); // Falling Edge
    GICR |= (1 << INT2);    // Enable INT2
}

// ------------------- Multiplexed Display & Button Logic -------------------

void Display_Multiplexed(void)
{
    unsigned char digits[6];

    digits[0] = g_seconds % 10;
    digits[1] = g_seconds / 10;
    digits[2] = g_minutes % 10;
    digits[3] = g_minutes / 10;
    digits[4] = g_hours % 10;
    digits[5] = g_hours / 10;

    for (unsigned char i = 0; i < 6; i++)
    {
        PORTA = (1 << i);                   // Enable segment i
        PORTC = (PORTC & 0xF0) | digits[i]; // Output BCD digit
        _delay_us(500);                     // Persistence of vision delay
    }
}

void Buttons_Handling(void)
{
    static unsigned char pb7_prev = 1;
    unsigned char pb7_curr = (PINB & (1 << PB7)) ? 1 : 0;

    // Check PB7 Toggle Button for Mode Change
    if (pb7_curr == 0 && pb7_prev == 1) // Falling Edge Detection
    {
        _delay_ms(30); // Debounce
        if (!(PINB & (1 << PB7)))
        {
            g_mode ^= 1; // Toggle Mode
            if (g_mode == COUNT_DOWN)
            {
                PORTD &= ~(1 << PD4); // Red LED OFF
                PORTD |= (1 << PD5);  // Yellow LED ON
            }
            else
            {
                PORTD |= (1 << PD4);  // Red LED ON
                PORTD &= ~(1 << PD5); // Yellow LED OFF
                PORTD &= ~(1 << PD0); // Buzzer OFF
            }
        }
    }
    pb7_prev = pb7_curr;

    // Time Adjustment Buttons (Active Low - Internal Pull-up)
    // Hours Increment (PB1)
    if (!(PINB & (1 << PB1)))
    {
        _delay_ms(30);
        if (!(PINB & (1 << PB1)))
        {
            if (g_hours < 99) g_hours++;
            while (!(PINB & (1 << PB1))) Display_Multiplexed();
        }
    }
    // Hours Decrement (PB0)
    if (!(PINB & (1 << PB0)))
    {
        _delay_ms(30);
        if (!(PINB & (1 << PB0)))
        {
            if (g_hours > 0) g_hours--;
            while (!(PINB & (1 << PB0))) Display_Multiplexed();
        }
    }
    // Minutes Increment (PB4)
    if (!(PINB & (1 << PB4)))
    {
        _delay_ms(30);
        if (!(PINB & (1 << PB4)))
        {
            if (g_minutes < 59) g_minutes++;
            while (!(PINB & (1 << PB4))) Display_Multiplexed();
        }
    }
    // Minutes Decrement (PB3)
    if (!(PINB & (1 << PB3)))
    {
        _delay_ms(30);
        if (!(PINB & (1 << PB3)))
        {
            if (g_minutes > 0) g_minutes--;
            while (!(PINB & (1 << PB3))) Display_Multiplexed();
        }
    }
    // Seconds Increment (PB6)
    if (!(PINB & (1 << PB6)))
    {
        _delay_ms(30);
        if (!(PINB & (1 << PB6)))
        {
            if (g_seconds < 59) g_seconds++;
            while (!(PINB & (1 << PB6))) Display_Multiplexed();
        }
    }
    // Seconds Decrement (PB5)
    if (!(PINB & (1 << PB5)))
    {
        _delay_ms(30);
        if (!(PINB & (1 << PB5)))
        {
            if (g_seconds > 0) g_seconds--;
            while (!(PINB & (1 << PB5))) Display_Multiplexed();
        }
    }
}

/*
 * stop_watch.c
 *
 *  Created on: 18 Sept 2026
 *      Author: loq
 */


