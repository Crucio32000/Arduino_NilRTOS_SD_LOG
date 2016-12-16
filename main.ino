#include <NilRTOS.h>
#include <NilFIFO.h>
#include <NilSerial.h>
#include <NilTimer1.h>
#define Serial NilSerial

#include "sd_fcns.h"
#include "mpu_fcns_i2cdev.h"
//#include "gps_fcns.h"
#include "logThread.h"



NIL_THREADS_TABLE_BEGIN()
NIL_THREADS_TABLE_ENTRY(NULL, Thread1, NULL, waThread1, sizeof(waThread1))
//NIL_THREADS_TABLE_ENTRY(NULL, Thread2, NULL, waThread2, sizeof(waThread2))
NIL_THREADS_TABLE_END()




void setup()
{
  pinMode(WRITE_LED, OUTPUT);
  pinMode(READ_LED, OUTPUT);
  pinMode(SPECIAL_LED, OUTPUT);
  Serial.begin(115200);
  Serial.print(F("BUFFER SIZE -> ")); Serial.println(BUFFER_SIZE);
  Serial.println(F("Type any char to begin"));
  while(Serial.read() < 0);
  sd_init();
  mpu_init();
  //gps_init();
  nilSysBegin();  // Start Kernel
}


void loop()
{ // It is coded as to execute one time(see while(1) at the end)
  while(!sd_ready)
  {
    Serial.println(F("SD IS NOT READY!"));
    delay(5000);
    sd_init();
  }
  while(!mpu_ready)
  {
    Serial.println(F("MPU IS NOT READY!"));
    delay(5000);
    mpu_init();    
  }

  // Sd is ready, so get SdFile ready
  file_remove();
  data_row.open(FILENAME_TEST, O_CREAT | O_WRITE | O_TRUNC);

  uint32_t max_latency = 0;
  uint8_t max_overrun = 0;
  uint16_t max_sampling = 0;
  uint16_t overrun_cnt = 0;
  uint16_t latency_cnt = 0;
  uint32_t bn = 0;

  while(!Serial.available())
  { 
    // Thread implemented to execute code ASAP (when FIFO is filled)
    record_row *p = fifo.waitData(TIME_IMMEDIATE);  // Wait for an available data record in FIFO

    if(!p) continue;  // If pointer is empty, go to next iteration

    digitalWrite(WRITE_LED, HIGH);
    uint16_t dt_write = micros();
    write_SD(p);
    dt_write = micros() - dt_write;
    digitalWrite(WRITE_LED, LOW);

    // Update max write time
    if(dt_write > max_latency) {
      max_latency = dt_write;  
      latency_cnt++;
    }
    // Update max overrun
    if(p->overrun > max_overrun){
      max_overrun = p->overrun;  
      overrun_cnt++;
    }
    // Update Sampling
    if(p->dt > max_sampling){
      max_sampling = p->dt;
    }
    // Update writing CNT
    bn++;

    //Signal the logThread that record is free
    fifo.signalFree();

    // Stop writing when bn =
    if(bn > FILE_SIZE_CNT)
    {
      break;
    }
    if(bn%100 == 0)
    {
      Serial.println(bn);
    }
    
    
  }

  // Print data
  data_row.close();
  Serial.print(F("Max Latency -> "));  Serial.println(max_latency);
  Serial.print(F("Times Latency -> "));  Serial.println(latency_cnt);
  Serial.print(F("Max Overrun -> "));  Serial.println(max_overrun);
  Serial.print(F("Times Overrun -> "));  Serial.println(overrun_cnt);
  Serial.print(F("Max Sampling -> "));  Serial.println(max_sampling);
  nilPrintUnusedStack(&Serial);
  fifo.printStats(&Serial);
  delay(1000);
  while(!Serial.available()) {}
  read_SD();
  while(1)  {}
  
}

