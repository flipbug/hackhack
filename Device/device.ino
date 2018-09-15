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

volatile int mevent = 0;

volatile char led = LOW;
volatile char interruptOn = HIGH;

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
  acc_gyro->setPedometerThreshold(LSM6DSL_PEDOMETER_THRESHOLD_MID_LOW);

  acc_gyro->enableFreeFallDetection(LSM6DSL_INT1_PIN);
  acc_gyro->setFreeFallThreshold(0);
  acc_gyro->attachInt1Irq(&triggerMevent);
}

void printAccelerometer() {
      // getXAxes
    acc_gyro->getXAxes(axes);
    char* c;
    char* b;
    
    // b = itoa(axes[0], c, 10);
    // Screen.print(1, b);
    char buffer[50];
    sprintf(buffer, "Axes: x: %d, y: %d, z: %d\n", axes[0], axes[1], axes[2]);
    Screen.print(2, buffer);

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

  if (mevent) {
    mevent = 0;
    LSM6DSL_Event_Status_t status;
    acc_gyro->getEventStatus(&status);
    if (status.FreeFallStatus) {
      Screen.print(3, "Falling...");
      wait(2);
    }
  } else {
    // printAccelerometer();
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

void triggerMevent() {
  mevent = 1;
}