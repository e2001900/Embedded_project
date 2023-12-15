# BATTERY MANAGEMENT SYSTEM
## Functionality

The BMS monitors and manages various aspects of the battery's state to guarantee safe and optimal performance, including:

- **Voltage Monitoring:**
  - Total voltage of the battery.
  - Individual cell voltages within the battery pack.

- **Temperature Control:**
  - Average temperature of the battery cells.

- **Current Supervision:**
  - Monitoring the flow of current in or out of the battery.

- **Cell Health Assessment:**
  - Evaluation of the health status of individual battery cells.

- **Cell Balancing:**
  - Ensuring the state of balance among the battery cells.

## Details

### Voltage Monitoring

The BMS keeps track of the total voltage of the battery and the voltages of each individual cell, ensuring they stay within safe limits.

### Temperature Control

Monitoring and maintaining the average temperature of the battery cells to prevent overheating or operating outside safe temperature ranges.

### Current Supervision

Tracking the flow of current in or out of the battery to manage charging and discharging processes effectively.

### Cell Health Assessment

Assessing the health status of individual battery cells to identify any degradation or issues affecting overall battery performance.

### Cell Balancing

Ensuring an optimal state of balance among the battery cells, preventing overcharging or deep discharging.

## Installation

### Prerequisites

The project requirements: 
  MOSFET driven switch (both charging and discharging)
  Temperature monitoring
  Individual battery cell voltages
  STM32 controller with over/under voltage per cell, over/under temperature (charge: 0C to 40C and discharge: -20C to 40C)
  Communication of cell voltages/temperatures/... over Modbus RTU
  Some kind of visualization of the Modbus data

### Components name

1. **Battery cells voltage measurement:**
  - Multiplexer: 4051BE
  - OpAmp: LM324N
2. **MOSFET:**
  - Mosfet FDS6898AZ
3. **Current shunt:**
  - WLBR010FE
4. **Temperature sensor:**
  - Temperature sensor LM35DZ 


