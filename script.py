import serial
import psycopg2
from psycopg2 import sql

# Serial connection settings
serial_port = 'COM3'  # Replace with the appropriate serial port on your system
baud_rate = 115200

# PostgreSQL connection settings
db_host = 'localhost'
db_port = '5432' #6432
db_name = 'postgres'
db_user = 'postgres'
db_password = ''

# Establish serial connection to STM32
try:
    ser = serial.Serial(serial_port, baud_rate, timeout=1)
except serial.SerialException as e:
    print(f"Error opening serial port: {e}")
    exit(1)

# Establish PostgreSQL connection
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
    ser.close()
    exit(1)

try:
    while True:
        # Read data from STM32
        try:
            stm32_data = ser.readline().decode('utf-8').strip()
        except UnicodeDecodeError:
            print("Error decoding data from STM32. Skipping.")
            continue

        # Process and insert data into PostgreSQL
        if stm32_data:
            try:
                # Assuming STM32 data format is 'temperature,humidity'
                temperature, humidity = map(float, stm32_data.split(','))

                # Insert data into PostgreSQL
                insert_query = sql.SQL("INSERT INTO sensor_data (temperature, humidity) VALUES ({}, {});").format(
                    sql.Literal(temperature),
                    sql.Literal(humidity)
                )
                cursor.execute(insert_query)
                conn.commit()

            except (ValueError, psycopg2.Error) as e:
                print(f"Error processing or inserting data into PostgreSQL: {e}")

except KeyboardInterrupt:
    print("Program terminated by user.")
finally:
    # Close connections
    ser.close()
    cursor.close()
    conn.close()
