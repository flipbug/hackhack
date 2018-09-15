#include <string>

#include "AZ3166WiFi.h"
#include "DevKitMQTTClient.h"
#include "OledDisplay.h"
#include "Sensor.h"

static DevI2C *ext_i2c;
static LSM6DSLSensor *acc_gyro;

int axes[3];
int16_t raws[3];

float data;

static bool hasWifi = false;
static bool hasIoTHub = false;
static bool isFalling = false;

volatile byte led = LOW;
volatile byte interruptOn = HIGH;

void setup() {
  // put your setup code here, to run once:
  Screen.init();

  setupWifi();
  setupAccelerometer();

  attachInterrupt(USER_BUTTON_A, blink, CHANGE);
}

static void setupWifi()
{
  Screen.clean();
  Screen.print(1, "IoT DevKit");
  Screen.print(2, "Connecting...");
  
  if (WiFi.begin() == WL_CONNECTED)
  {
    IPAddress ip = WiFi.localIP();
    Screen.print(1, ip.get_address());
    hasWifi = true;
    Screen.print(2, "Running... \r\n");
  }
  else
  {
    hasWifi = false;
    Screen.print(1, "No Wi-Fi\r\n ");
  }
}

void setupAccelerometer() {
  ext_i2c = new DevI2C(D14, D15);
  acc_gyro = new LSM6DSLSensor(*ext_i2c, D4, D5);
  acc_gyro->init(NULL); 
  acc_gyro->enableAccelerator();
  acc_gyro->enableGyroscope();
  acc_gyro->enablePedometer();
  acc_gyro->enableFreeFallDetection(LSM6DSL_INT1_PIN);
  acc_gyro->setPedometerThreshold(LSM6DSL_PEDOMETER_THRESHOLD_MID_LOW);

  attachInterrupt(D4, falling, FALLING);
}

void printAccelerometer() {
      // getXAxes
    acc_gyro->getXAxes(axes);
    Screen.print(1, std::to_string(axes[0]).c_str());
    // Screen.print("Axes: x: %d, y: %d, z: %d\n", axes[0], axes[1], axes[2]);
    // getXSensitivity
    // acc_gyro->getXSensitivity(&data);
    //Screen.print("Sensitivity: ");
    //Screen.println(data);
    // getXAxesRaw
    //acc_gyro->getXAxesRaw(raws);
    // Serial.printf("Raw: x: %d, y: %d, z: %d\n", raws[0], raws[1], raws[2]);

}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_USER, led);

        
  if (isFalling) {
    Screen.print(3, "Falling...");
  } else {
    printAccelerometer();
  }


  /*
  if (hasIoTHub && hasWifi)
  {
    char buff[128];

    // replace the following line with your data sent to Azure IoTHub
    snprintf(buff, 128, "{\"topic\":\"iot\"}");
    
    if (DevKitMQTTClient_SendEvent(buff))
    {
      Screen.print(1, "Sending...");
    }
    else
    {
      Screen.print(1, "Failure...");
    }
    delay(2000);
  } else {
      Screen.print(1, "No IoTHub");
  }
  */
}

void blink()
{
  led = !led;
}

void falling() {
  isFalling != isFalling;
}