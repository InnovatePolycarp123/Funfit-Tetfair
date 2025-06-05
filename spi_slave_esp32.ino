#include <ESP32SPISlave.h>

ESP32SPISlave slave;


//initialize vspi with default pins
//SCLK = 18, MISO = 19, MOSI = 23, SS = 5;  the default is vspi
/*
  you get the above when you print this

   Serial.begin(115200);
  Serial.print("MOSI: ");
  Serial.println(MOSI);
  Serial.print("MISO: ");
  Serial.println(MISO);
  Serial.print("SCK: ");
  Serial.println(SCK);
  Serial.print("SS: ");
  Serial.println(SS);
 
  
  */

//initialize hspi with default pins
//SCLK = 14, MISO = 12, MOSI = 13, SS = 15


void parse32bitDataToBufferMSBFirst(uint8_t* buffer, uint32_t data);        // buffer must be atleast 4byte
void parse4ByteBufferTo32bitDataMSBFirst(uint8_t* buffer, uint32_t* data);  // buffer must be atleast 4byte
void parse2ByteBufferTo16bitDataMSBFirst(uint8_t* buffer, uint16_t* data);
void parse16bitDataToBufferMSBFirst(uint8_t* buffer, uint16_t data);

uint32_t SPITransferRecieve32bitData(uint32_t data);
uint16_t SPITransferRecieve16bitData(uint16_t data);
uint8_t SPITransferRecieve8bitData(uint8_t data);


static constexpr size_t BUFFER_SIZE = 4;
static constexpr size_t QUEUE_SIZE = 1;
uint8_t tx_buf[BUFFER_SIZE]{ 1, 2, 3, 4, 5, 6, 7, 8 };
uint8_t rx_buf[BUFFER_SIZE]{ 0, 0, 0, 0, 0, 0, 0, 0 };

void setup() {
  Serial.begin(115200);

  delay(2000);

  slave.setDataMode(SPI_MODE0);   // default: SPI_MODE0
  slave.setQueueSize(QUEUE_SIZE); // default: 1, requres 2 in this example

  // begin() after setting
  slave.begin();  // default: HSPI (please refer README for pin assignments)

  Serial.println("start spi slave");
}

void loop() {
  
  if (slave.hasTransactionsCompletedAndAllResultsHandled()) {

        // queue transaction and trigger it right now
        Serial.println("execute transaction in the background");
        slave.queue(tx_buf, rx_buf, BUFFER_SIZE);
        slave.trigger();

        Serial.println("wait for the completion of the queued transactions...");
    }

    // you can do some other stuff here
    // NOTE: you can't touch dma_tx/rx_buf because it's in-flight in the background

    // if all transactions are completed and all results are ready, handle results
    if (slave.hasTransactionsCompletedAndAllResultsReady(QUEUE_SIZE)) {
        // process received data from slave
        Serial.println("all queued transactions completed. start verifying received data from slave");

        // get the oldeest transfer result
        size_t received_bytes = slave.numBytesReceived();

        // verify and dump difference with received data
        // NOTE: we need only 1st results (received_bytes[0])
        if (verifyAndDumpDifference("slave", tx_buf, BUFFER_SIZE, "master", rx_buf, received_bytes)) {
            Serial.println("successfully received expected data from master");
        } else {
            Serial.println("unexpected difference found between master/slave data");
        }
    }
}


void parse32bitDataToBufferMSBFirst(uint8_t* buffer, uint32_t data) {  // buffer must be of size 4

  if (buffer != NULL) {

    buffer[0] = (data >> 24) & 0xFF;  // store the most significant byte in buffer index zero
    buffer[1] = (data >> 16) & 0xFF;
    buffer[2] = (data >> 8) & 0xFF;
    buffer[3] = data & 0xFF;  // store the least significant byte in buffer index 3
  }
}

void parse4ByteBufferTo32bitDataMSBFirst(uint8_t* buffer, uint32_t* data) {  // buffer must be atleast 4byte

  *data = 0;
  uint32_t tempstore = 0;

  if (buffer != NULL) {
    tempstore = buffer[0];

    *data |= (tempstore << 24);

    tempstore = buffer[1];

    *data |= (tempstore << 16);

    tempstore = buffer[2];

    *data |= (tempstore << 8);

    tempstore = buffer[3];

    *data |= tempstore;


    /* it could also be done this way
      
        *data = buffer[0];
        *data = (*data<<8)| buffer[1];
        *data = (*data<<8)| buffer[2];
        *data = (*data<<8)| buffer[3];
      */
  }
}


void parse16bitDataToBufferMSBFirst(uint8_t* buffer, uint16_t data) {

  if (buffer != NULL) {

    buffer[0] = (data >> 8) & 0xFF;
    buffer[1] = data & 0xFF;
  }
}

void parse2ByteBufferTo16bitDataMSBFirst(uint8_t* buffer, uint16_t* data) {

  if (buffer != NULL) {

    *data = 0;
    *data = buffer[0];
    *data = (*data << 8) | buffer[1];
  }
}

