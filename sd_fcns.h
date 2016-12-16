#include <SdFat.h>
SdFat SD;

#define FILENAME_TEST "TEST.CSV"
#define FILE_SIZE_CNT 65536
#define SAMPLING_DELAY 100  // Simulating data collect
#define WRITE_TH 10
#define BUFFER_SIZE 194

#define WRITE_LED 6
#define READ_LED 7


//
// BUFFER DEF
//
struct record_row {
  int16_t acc[3];
  int16_t gyro[3];
  uint16_t overrun;
  uint16_t dt;
};
const size_t FIFO_SIZE = BUFFER_SIZE/sizeof(record_row);
NilStatsFIFO<record_row, FIFO_SIZE> fifo;

//
// NilRTOS
//
//SEMAPHORE_DECL(data_idx, 0);

//
// SD Functions
// 
const int CS_SD = 10;
boolean sd_ready = false;
SdFile data_row;


void sd_init()
{
  pinMode(11, OUTPUT);
  if (!SD.begin(CS_SD, SPI_FULL_SPEED) ) 
  {
    SD.initErrorHalt();
    sd_ready = false;
  } else {
    Serial.println(F("SD Card Ready!"));
    sd_ready = true;
  }
}

void file_remove()
{
  if( SD.exists(FILENAME_TEST) )
  {
    Serial.println("Deleting file...");
    if(!SD.remove(FILENAME_TEST))
    {
      Serial.println("Error deleting file!");
    }else{
      Serial.println("File deleted!");
      sd_ready = true;
    }
  }else{
    sd_ready = true;
  }
}

void write_SD(record_row *p)
{
  for(uint8_t i=0; i<3; i++)
  {
    data_row.printField(p->acc[i], ',');
    data_row.printField(p->gyro[i], ',');
  }
  data_row.printField(p->overrun, ',');
  data_row.printField(p->dt, '\n');
}


void read_SD()
{
  data_row.open(FILENAME_TEST, O_READ);
  char buffer_c;
  Serial.println("Testing SD read/write operation.");
  while(data_row.available())
  {
    buffer_c = data_row.read();
    Serial.print(buffer_c);
  } 
}

void read_SD_bytes()
{
  data_row.open(FILENAME_TEST, O_READ);
  record_row buf;
  Serial.println("Testing SD read/write operation.");
  //data_row.rewind();  // Move cursor to the beginning of the file
  // Read sector by sector
  while(!Serial.available() && data_row.read(&buf, sizeof(record_row)) == sizeof(record_row))
  {
    Serial.println(buf.dt);
    delay(10);
  }
  data_row.close();
}

