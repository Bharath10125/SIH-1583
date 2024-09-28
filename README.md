Here's an updated README file reflecting that Node-RED sends data to Flask and receives normal or abnormal classifications:

---

# Tunnel Booster Fan Monitoring System

This project implements a real-time monitoring system for tunnel booster fans using an ESP32 microcontroller, sensors for various parameters, and a Flask server for data handling. The data is visualized in Node-RED with live graphs, allowing for monitoring of critical metrics like vibration, noise, voltage, current, RPM, temperature, and efficiency.

## Project Structure

The repository includes the following key files:

- **data.php**: PHP script that connects to the MySQL database hosted on XAMPP. It inserts incoming sensor data from the ESP32 into a table named based on the current date, storing the following parameters: vibration, noise, voltage, current, RPM, efficiency, and their classification (normal/abnormal).
  
- **SIh1583.ino**: Arduino sketch for the ESP32 that collects data from connected sensors (temperature, vibration, noise, voltage, current, RPM) and sends the data as JSON to the Flask server.

- **app.py**: Flask application that runs on the command line. It receives JSON data from Node-RED, processes it for classification using a pre-trained machine learning model, and sends back the classification (normal or abnormal) to Node-RED for visualization.

- **Node-RED Flow**: Configuration for Node-RED to visualize sensor data in real-time. It includes nodes for MQTT subscription to receive sensor data from the ESP32, sends HTTP requests to the Flask server for classification, and uses chart nodes for displaying data.

## Requirements

- **Hardware**: ESP32 microcontroller, various sensors (DS18B20, MPU6050, ZMPT101B, ZMCT103C, noise sensor, proximity sensor).
  
- **Software**:
  - XAMPP (for MySQL)
  - Python 3.x
  - Flask
  - Node-RED

## Installation and Setup

1. **Clone the Repository**:
   ```bash
   git clone <repository-url>
   cd <repository-directory>
   ```

2. **Set Up XAMPP**:
   - Install XAMPP and start the MySQL service.
   - Create a database for storing sensor data.

3. **Configure MySQL**:
   - Edit `data.php` to ensure the database connection settings are correct (username, password, database name).

4. **Run the Flask Server**:
   - Install required Python packages:
     ```bash
     pip install flask
     ```
   - Run the Flask application:
     ```bash
     python app.py
     ```

5. **Upload the Arduino Sketch**:
   - Open the `SIh1583.ino` file in the Arduino IDE.
   - Ensure the correct board and port are selected, then upload the sketch to the ESP32.

6. **Configure Node-RED**:
   - Open Node-RED in your browser (usually at `http://localhost:1880`).
   - Import the provided flow and deploy it.

## Usage

- **Data Collection**: The ESP32 collects data from the sensors and sends it to Node-RED via MQTT.
- **Data Processing**: Node-RED sends the received data to the Flask server as JSON for classification.
- **Classification**: The Flask server processes the data and returns a classification (normal or abnormal) back to Node-RED.
- **Data Visualization**: Node-RED displays real-time graphs based on the received sensor data and classification results.

## Contributing

Feel free to fork the repository and submit pull requests. Any contributions to improve functionality or documentation are welcome.

## License

This project is licensed under the MIT License. See the LICENSE file for more information.

---

You can modify any specific sections or details to suit your preferences!
