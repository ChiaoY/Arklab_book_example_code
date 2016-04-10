
// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "MPU6050.h"

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 accelgyro;
//MPU6050 accelgyro(0x69); // <-- use for AD0 high

int16_t ax, ay, az;
int16_t gx, gy, gz;


#define OUTPUT_READABLE_ACCELGYRO

// 加權遞推平均濾波法
#define FILTER_N 12  //取值的個數

int16_t filter_buf[7][FILTER_N + 1];

int16_t Filter(int(cat),int16_t(val)) {
  long int before_filter;
  long int coe[FILTER_N] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};    // 加權係數表(越高位(越接近現時刻)加權越高)
  int sum_coe = 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 10 + 11 + 12; // 加權細數和
  long int i;
  long int filter_sum = 0;
  int16_t result = 0;
  before_filter = val; //取得最新值
  //cat ax=0,ay=1,az=2,gx=3,gy=4,gz=5
  filter_buf[cat][FILTER_N] = before_filter;
  for(i = 0; i < FILTER_N; i++) {
    filter_buf[cat][i] = filter_buf[cat][i + 1]; // 所有數據左移，低位扔掉
    filter_sum += filter_buf[cat][i] * coe[i]; //加權後的總值
  }
  result = filter_sum / sum_coe;//取加權平均值,加權平均值=Σ(值*加權值)/加權後的總值
  return result;//回傳加權平均值
}

#define LED_PIN 13
bool blinkState = false;

void setup() {
  for(int i = 0;i < 7;i++){
  for(int j = 0;j < (FILTER_N + 2);j++){
    filter_buf[i][j] = 0;
  }
}
    // join I2C bus (I2Cdev library doesn't do this automatically)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    // initialize serial communication
    // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
    // it's really up to you depending on your project)
    Serial.begin(38400);

    // initialize device
    Serial.println("Initializing I2C devices...");
    accelgyro.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

    accelgyro.setXAccelOffset(-1061);
    accelgyro.setYAccelOffset(-1354);
    accelgyro.setZAccelOffset(1106);
    accelgyro.setXGyroOffset(1329);
    accelgyro.setYGyroOffset(-4);
    accelgyro.setZGyroOffset(45);
    
    // configure Arduino LED for
    pinMode(LED_PIN, OUTPUT);
}

void loop() {
    // read raw accel/gyro measurements from device
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    // these methods (and a few others) are also available
    //accelgyro.getAcceleration(&ax, &ay, &az);
    //accelgyro.getRotation(&gx, &gy, &gz);

    #ifdef OUTPUT_READABLE_ACCELGYRO
        // display tab-separated accel/gyro x/y/z values
        Serial.print("a/g:\t");
        Serial.print(ax); Serial.print("\t");
        Serial.print(ay); Serial.print("\t");
        Serial.print(az); Serial.print("\t");
        Serial.print(gx); Serial.print("\t");
        Serial.print(gy); Serial.print("\t");
        Serial.print(gz);
        
        Serial.print("\tafter a/g:\t");
        Serial.print(Filter(0,ax)); Serial.print("\t");
        Serial.print(Filter(1,ay)); Serial.print("\t");
        Serial.print(Filter(2,az)); Serial.print("\t");
        Serial.print(Filter(3,gx)); Serial.print("\t");
        Serial.print(Filter(4,gy)); Serial.print("\t");
        Serial.println(Filter(5,gz));
        delay(500);
    #endif



    // blink LED to indicate activity
    blinkState = !blinkState;
    digitalWrite(LED_PIN, blinkState);
}


