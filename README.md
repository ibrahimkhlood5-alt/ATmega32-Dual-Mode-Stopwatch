# ⏱️ ATmega32 Dual-Mode Stopwatch

A dual-mode stopwatch system implemented using the **ATmega32 microcontroller** and simulated in **Proteus**.

The project supports both **Count-Up** and **Count-Down** modes, with external interrupts for reset, pause, and resume control.

---

## 🚀 Features

* ⬆️ Count-Up Stopwatch
* ⬇️ Count-Down Stopwatch
* ⏸️ Pause / Resume functionality
* 🔄 Reset functionality
* ⏰ Adjustable hours, minutes, and seconds
* 🔔 Buzzer alarm when the countdown reaches `00:00:00`
* 🚦 LED indicators for different states
* 🖥️ Six multiplexed 7-segment displays
* 🎛️ Push-button control
* ⚡ Timer1 CTC interrupt for time counting
* 🔌 External interrupts for Reset, Pause, and Resume
* 🧪 Full Proteus simulation

---

## 🛠️ Technologies & Tools

* **Microcontroller:** ATmega32
* **Programming Language:** Embedded C
* **Clock Frequency:** 8 MHz
* **Simulation:** Proteus
* **Compiler/IDE:** AVR C development environment

---

## 🔧 Main Peripherals Used

| Peripheral | Function                         |
| ---------- | -------------------------------- |
| Timer1     | Generates the 1-second time base |
| INT0       | Reset                            |
| INT1       | Pause                            |
| INT2       | Resume                           |
| PORTA      | 7-segment digit enable           |
| PORTC      | BCD data output                  |
| PORTB      | Control buttons                  |
| PORTD      | Buzzer and LED indicators        |

---

## ⏱️ Timer Configuration

Timer1 is configured in **CTC (Clear Timer on Compare Match) mode**.

* CPU Frequency: `8 MHz`
* Prescaler: `1024`
* Compare Value (`OCR1A`): `7812`
* Interrupt interval: approximately **1 second**

The Timer1 interrupt updates the stopwatch time every second.

---

## 🔼 Count-Up Mode

In Count-Up mode, the stopwatch starts from:

`00:00:00`

and increments continuously:

`00:00:01 → 00:00:02 → ...`

The hours, minutes, and seconds can be adjusted using the control buttons.

---

## 🔽 Count-Down Mode

In Count-Down mode, the timer decreases from the selected time until it reaches:

`00:00:00`

When the countdown finishes, the system activates the alarm using the buzzer and LED indicators.

---

## 🎮 Controls

| Button | Function                          |
| ------ | --------------------------------- |
| PB7    | Toggle Count-Up / Count-Down mode |
| PB1    | Increase Hours                    |
| PB0    | Decrease Hours                    |
| PB4    | Increase Minutes                  |
| PB3    | Decrease Minutes                  |
| PB6    | Increase Seconds                  |
| PB5    | Decrease Seconds                  |
| INT0   | Reset                             |
| INT1   | Pause                             |
| INT2   | Resume                            |

---

## 🖥️ Display

The stopwatch uses **six 7-segment displays**:

* 2 digits → Hours
* 2 digits → Minutes
* 2 digits → Seconds

The displays are driven using **multiplexing**, with:

* `PORTA` for digit selection
* `PORTC` for BCD data

---

## 🔔 Alarm

In Count-Down mode, when the timer reaches:

`00:00:00`

the system activates an alarm using:

* 🔊 Buzzer
* 🔴 Red LED
* 🟡 Yellow LED

---

## 🧠 Embedded Systems Concepts

This project demonstrates practical implementation of:

* Microcontroller programming
* Embedded C
* Timer interrupts
* External interrupts
* GPIO configuration
* Button handling
* Software debouncing
* Multiplexed 7-segment displays
* CTC timer mode
* State/mode control
* Countdown logic
* Alarm control
* Proteus simulation

---

## 📁 Project Files

* `*.c` → Embedded C source code
* `*.pdsprj` → Proteus simulation project

---

## 🧪 Simulation

The project was tested and simulated using **Proteus** to verify the stopwatch functionality, display operation, buttons, interrupts, and alarm behavior.

🎥 A demonstration video of the Proteus simulation is available in the project's LinkedIn post.

---

## 👩‍💻 Author

**Khlood Ibrahim Ahmed**

Mechatronics Engineering Student

---

## 📌 Project Purpose

This project was developed to strengthen practical understanding of **AVR microcontrollers, timers, interrupts, GPIO, display multiplexing, and embedded C programming** through a complete stopwatch application.
