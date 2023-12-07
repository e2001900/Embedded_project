import minimalmodbus
import psycopg2
from psycopg2 import sql
import time

# Modbus RTU settings
modbus_port = '/dev/ttyUSB0'  # Replace with the appropriate serial port on your system
modbus_slave_address = 1  # Replace with the Modbus slave address of your STM32

# PostgreSQL connection settings
db_host = 'your_postgres_host'
db_port = 'your_postgres_port'
db_name = 'your_database_name'
db_user = 'your_database_user'
db_password = 'your_database_password'

# Modbus instrument configuration
modbus_instrument = minimalmodbus.Instrument(modbus_port, modbus_slave_address)
modbus_instrument.serial.baudrate = 9600
modbus_instrument.serial.timeout = 1

# PostgreSQL connection
try:
    conn = psycopg2.connect(
        host=db_host,
        port=db_port,
        dbname=db_name,
        user=db_user,
        password=db_password
    )
    cursor = conn.cursor()
except psycopg2.Error as e:
    print(f"Error connecting to PostgreSQL: {e}")
    exit(1)

try:
    while True:
        # Read data from STM32 using Modbus RTU
        try:
            temperature = modbus_instrument.read_register(0, functioncode=4)  # Replace with the appropriate Modbus address
            humidity = modbus_instrument.read_register(1, functioncode=4)     # Replace with the appropriate Modbus address
        except (IOError, ValueError) as e:
            print(f"Error reading data from STM32 using Modbus RTU: {e}")
            time.sleep(1)
            continue

        # Insert data into PostgreSQL
        try:
            insert_query = sql.SQL("INSERT INTO sensor_data (temperature, humidity) VALUES ({}, {});").format(
                sql.Literal(temperature),
                sql.Literal(humidity)
            )
            cursor.execute(insert_query)
            conn.commit()
        except psycopg2.Error as e:
            print(f"Error inserting data into PostgreSQL: {e}")

        # Wait for a specific interval (e.g., 5 seconds) before fetching and inserting the next data point
        time.sleep(5)

except KeyboardInterrupt:
    print("Program terminated by user.")
finally:
    # Close connections
    cursor.close()
    conn.close()
