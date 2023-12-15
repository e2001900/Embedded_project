# BATTERY MANAGEMENT SYSTEM

BMS is an electronic system that manages a rechargeable Lithium battery, and protecting the battery from operating outside its safe operating area.

The BMS may monitor the state of the battery as represented by various items, such as:
  Voltage: total voltage, voltages of individual cells.
  Temperature: average temperature of cells.
  Current: current in or out of the battery.
  Health of individual cells.
  State of balance of cells.

Initial requirements
  MOSFET driven switch (both charging and discharging)
  Temperature monitoring
  Individual battery cell voltages
  STM32 controller with over/under voltage per cell, over/under temperature (charge: 0C to 40C and discharge: -20C to 40C)
  Communication of cell voltages/temperatures/... over Modbus RTU
  Some kind of visualization of the Modbus data
