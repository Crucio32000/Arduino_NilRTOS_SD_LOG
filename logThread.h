#define MPU_SAMPLING_TIME 10  // Every 10 ms
#define GPS_SAMPLING_TIME 1000  //Every sec

#define LOG_THREAD_SIZE 128 // Must be big as one record_row of the FIFO + I2C readings

#define SPECIAL_LED 8

NIL_WORKING_AREA(waThread1, LOG_THREAD_SIZE);

unsigned long cnt_tmp = 0;
uint16_t dt = 0;

NIL_THREAD(Thread1, arg) {

  // Init prep for Thread1
  nilTimer1Start(MPU_SAMPLING_TIME*1000);  //usec
  dt = millis();

  while(!Serial.available())
  {
    
    if(!nilTimer1Wait())
    {
      fifo.countOverrun();
      continue;
    }

    record_row *p = fifo.waitFree(TIME_IMMEDIATE);

    if(!p) continue;

    // Save sampling time before I2C read, which may affect significantly this param
    dt = millis() - dt;
    p->dt = dt;
    dt = millis();

    // Read DATA and store to FIFO
    digitalWrite(READ_LED, HIGH);
    mpu_read_routine(p);
    digitalWrite(READ_LED, LOW);
    
    p->overrun = fifo.overrunCount();
    cnt_tmp++;
    // Take overrun through fifo.overrunCount()
    fifo.signalData();  // Signal SD thread(in loop) that new data are ready

    //nilSemSignal(&gps_sem);

  }

  nilThdSleep(TIME_INFINITE);
}

