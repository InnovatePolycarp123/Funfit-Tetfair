#include <SPI.h>


void parse32bitDataToBufferMSBFirst(uint8_t* buffer, uint32_t data);        // buffer must be atleast 4byte
void parse4ByteBufferTo32bitDataMSBFirst(uint8_t* buffer, uint32_t* data);  // buffer must be atleast 4byte
uint32_t SPITransferRecieve32bitData(uint32_t data);
uint16_t SPITransferRecieve16bitData(uint16_t data);
uint8_t SPITransferRecieve8bitData(uint8_t data);
void parse2ByteBufferTo16bitDataMSBFirst(uint8_t* buffer, uint16_t* data);
void parse16bitDataToBufferMSBFirst(uint8_t* buffer, uint16_t data);
void exampleFor16bitData();
void exampleFor32bitData();



void setup() {
  Serial.begin(9600);
  pinMode(SS, OUTPUT);
  SPI.begin();
}

void loop() {


  exampleFor16bitData();
  exampleFor32bitData();
}



uint32_t SPITransferRecieve32bitData(uint32_t data) {

  constexpr size_t BUFFER_SIZE = 4;
  uint8_t buffer[BUFFER_SIZE];
  uint32_t recieved_data = 0;


  parse32bitDataToBufferMSBFirst(buffer, data);
  SPI.beginTransaction(SPISettings(250000, MSBFIRST, SPI_MODE0));
  digitalWrite(SS, 0);
  SPI.transfer(buffer, 4);
  parse4ByteBufferTo32bitDataMSBFirst(buffer, &recieved_data);
  digitalWrite(SS, 1);
  SPI.endTransaction();
  return recieved_data;
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

uint16_t SPITransferRecieve16bitData(uint16_t data) {

  constexpr size_t BUFFER_SIZE = 2;
  uint8_t buffer[BUFFER_SIZE];
  uint16_t recieved_data = 0;

  parse16bitDataToBufferMSBFirst(buffer, data);
  SPI.beginTransaction(SPISettings(250000, MSBFIRST, SPI_MODE0));
  digitalWrite(SS, 0);
  SPI.transfer(buffer, 2);
  digitalWrite(SS, 1);
  SPI.endTransaction();
  parse2ByteBufferTo16bitDataMSBFirst(buffer, &recieved_data);

  return recieved_data;

  /* it could also be done this way
      
      SPI.beginTransaction(SPISettings(250000, MSBFIRST, SPI_MODE0));
      digitalWrite(SS, 0);
      recieved_data = SPI.transfer16(data);
      digitalWrite(SS, 1);

        return recieved_data;

        however, my slave may only be able to accept 8 bit at once;
      */
}

uint8_t SPITransferRecieve8bitData(uint8_t data) {

  uint16_t recieved_data = 0;

  SPI.beginTransaction(SPISettings(250000, MSBFIRST, SPI_MODE0));
  digitalWrite(SS, 0);
  recieved_data = SPI.transfer(data);
  digitalWrite(SS, 1);
  SPI.endTransaction();
  
  return recieved_data;
}


void exampleFor16bitData() {

  uint16_t data = 12869;
  uint16_t data2 = 0;
  constexpr size_t BUFFER_SIZE = 2;
  uint8_t buffer[BUFFER_SIZE];

  parse16bitDataToBufferMSBFirst(buffer, data);

  Serial.print("buffer[0]: ");
  Serial.println(buffer[0], HEX);
  Serial.print("buffer[1]: ");
  Serial.println(buffer[1], HEX);

  parse2ByteBufferTo16bitDataMSBFirst(buffer, &data2);


  Serial.print("data2: ");
  Serial.println(data2);

  delay(1000);
}


void exampleFor32bitData() {


  uint32_t data = 305419896;
  uint32_t data2 = 0;
  constexpr size_t BUFFER_SIZE = 4;
  uint8_t buffer[BUFFER_SIZE];

  parse32bitDataToBufferMSBFirst(buffer, data);

  Serial.print("buffer[0]: ");
  Serial.println(buffer[0], HEX);
  Serial.print("buffer[1]: ");
  Serial.println(buffer[1], HEX);
  Serial.print("buffer[2]: ");
  Serial.println(buffer[2], HEX);
  Serial.print("buffer[3]: ");
  Serial.println(buffer[3], HEX);

  parse4ByteBufferTo32bitDataMSBFirst(buffer, &data2);


  Serial.print("data2: ");
  Serial.println(data2);

  delay(1000);
}
