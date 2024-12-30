
//Settings
#define TEMP_SENSOR_SAMPLE_SIZE 50
#define TEMP_SENSOR_SAMPLE_PERIOD 500 //milliseconds
#define RELAY_CHECK_PERIOD 2000 //milliseconds
#define BAUDRATE 9600

//Teensy Constants
#define ANALOG_RESOLUTION 10
#define TEENSY_MILLIVOLTAGE 3300
#define TEMP_SENSOR_PIN 14
#define RELAY_PIN 13


unsigned long last_relay_check = 0; 
unsigned long last_avg_sensor_reading_update = 0; 
float avg_sensor_reading = 0;
float sensor_millivoltage;
float temperature_celc;
float temperature_fahr;

void update_avg_sensor_reading() {
  static int i = 0;
  static int sensor_readings[TEMP_SENSOR_SAMPLE_SIZE];

  // get next reading
  int new_sensor_reading = analogRead(TEMP_SENSOR_PIN);
  
  //update average
  avg_sensor_reading = avg_sensor_reading - float(new_sensor_reading - sensor_readings[i]) / float(TEMP_SENSOR_SAMPLE_SIZE);
  
  //update sensor readings
  sensor_readings[i++] = new_sensor_reading;
  if (i == SENSOR_SAMPLE_SIZE)
    i = 0;
}

void update_temp_values() {
  sensor_millivoltage =  float(TEENSY_MILLIVOLTAGE) * 1000.0 * float(avg_sensor_reading) / float(ANALOG_RESOLUTION);
  temperature_celc = (sensor_millivoltage - 500.0) / 10.0;
  temperature_fahr = temperature_celc* 9.0 / 5.0 + 32.0;
}

void setup() {
  //Setup Serial
  Serial.begin(BAUDRATE);

  //Initialize avg sensor readings
  for (int i = 0, i < TEMP_SENSOR_SAMPLE_SIZE, i++)
    update_avg_sensor_reading();
}

void loop() {
  // put your main code here, to run repeatedly:

  //check if sensor must be read
  if (millis() > last_avg_sensor_reading_update + TEMP_SENSOR_SAMPLE_PERIOD) {
    update_avg_sensor_reading()
    last_avg_sensor_reading_update = millis();
  }
  
  // check if relay must be triggered
  if (millis() > last_relay_check + RELAY_CHECK_PERIOD) {
    update_temp_values()
    digitalWrite(RELAY_PIN, goal_temp > temperature_fahr);
    last_relay_check = millis();
  }
  
  // 


}
