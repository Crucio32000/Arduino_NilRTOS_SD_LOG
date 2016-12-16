#include <Wire.h>
#include "I2Cdev.h"
#include "MPU6050.h"


MPU6050 acc_gyro(0x68);
boolean mpu_ready = false;

void mpu_init()
{
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    Wire.begin();
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
    Fastwire::setup(400, true);
  #endif
  TWBR = 12;  // Set I2C to max speed allowed by Nano
  // Init communication
  acc_gyro.initialize();
  // Set DLPF
  acc_gyro.setDLPFMode(6);  //0 -> 260/256Hz 6 -> 5Hz
  // Enable FIFO
  //acc_gyro.setTempFIFOEnabled(true);
  //acc_gyro.setAccelFIFOEnabled(true);
  // Enable interrupt
  //acc_gyro.setInterruptMode(true);  // INT_PIN_CFG
  //acc_gyro.setIntDataReadyEnabled(true);  // INT_ENABLE
  // Set flag
  mpu_ready = acc_gyro.testConnection();
  if(mpu_ready)
  {
    Serial.println(F("MPU Ready"));
  }
}

void mpu_read_routine(record_row *p)
{
  /*acc_gyro.getAcceleration(&p->acc[0], &p->acc[1],& p->acc[2]);
  acc_gyro.getRotation(&p->gyro[0], &p->gyro[1], &p->gyro[2]); */
  //mpu_temp = acc_gyro.getTemperature();
  p->acc[0] = acc_gyro.getAccelerationX();
  p->acc[1] = acc_gyro.getAccelerationY();
  p->acc[2] = acc_gyro.getAccelerationZ();
  p->gyro[0] = acc_gyro.getRotationX();
  p->gyro[1] = acc_gyro.getRotationY();
  p->gyro[2] = acc_gyro.getRotationZ(); 
  //acc_gyro.getMotion6(p->acc[0], p->acc[1], p->acc[2], p->gyro[0], p->gyro[1], p->gyro[2]); 
}


/*void acc_calibration()
{
  int x,y,z = 0;
  for(int i=0; i<1000; i++)
  {
    x += acc_gyro.getAccelerationX();
    y += acc_gyro.getAccelerationY();
    z += acc_gyro.getAccelerationZ();
  }
  ax_offset = x/1000;
  ay_offset = y/1000;
  az_offset = z/1000;
} */

