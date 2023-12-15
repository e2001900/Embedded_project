# BATTERY MANAGEMENT SYSTEM

BMS is an electronic system that manages a rechargeable Lithium battery, and protecting the battery from operating outside its safe operating area.
The BMS may monitor the state of the battery as represented by various items, such as:
  Voltage: total voltage, voltages of individual cells.
  Temperature: average temperature of cells.
  Current: current in or out of the battery.
  Health of individual cells.
  State of balance of cells.

## Installation

### Prerequisites

The project requirements: 
  MOSFET driven switch (both charging and discharging)
  Temperature monitoring
  Individual battery cell voltages
  STM32 controller with over/under voltage per cell, over/under temperature (charge: 0C to 40C and discharge: -20C to 40C)
  Communication of cell voltages/temperatures/... over Modbus RTU
  Some kind of visualization of the Modbus data

### Steps

1. **Clone the repository:**

    ```bash
    [git clone https://github.com/username/AwesomeToDo.git](https://github.com/e2001900/Embedded_project)
    ```

2. **Navigate to the project directory:**

    ```bash
    cd AwesomeToDo
    ```

3. **Install dependencies:**

    ```bash
    npm install
    ```

### Configuration

- **Environment Variables:**

  - Rename the `.env.example` file to `.env`.
  - Update the variables in the `.env` file as needed (e.g., database credentials, API keys).

### Database Setup

1. **Create a local database instance (if required):**

    ```bash
    # Example for PostgreSQL
    createdb my_todo_database
    ```

2. **Run database migrations (if applicable):**

    ```bash
    npm run migrate
    ```

### Running the Application

1. **Start the server:**

    ```bash
    npm start
    ```

2. **Access the application:**

    Open your browser and go to `http://localhost:3000` to use the app.

### Testing

- Run tests to ensure everything is working:

    ```bash
    npm test
    ```

### Troubleshooting

- If encountering issues, refer to the project's documentation or raise an issue on GitHub.
