#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SPI.h>
#include <printf.h>

const int pinCE = 9;
const int pinCSN = 10;
RF24 radio(pinCE, pinCSN);

// Single radio pipe address for the 2 nodes to communicate.
const uint64_t pipe = 0xE8E8F0F0E1LL;

char data[16];

void setup(void)
{
  Serial.begin(9600);
  if(!radio.begin()){
    Serial.println("Algo ha fallado con la antena");
  }else{
    Serial.println("Todo va bien con la antena");
  }
  radio.setAutoAck(false); // Eliminamos el ACK para poder estableer la comunicación.
  radio.setPALevel(RF24_PA_MIN); // Se establece la potencia
  radio.setDataRate(RF24_250KBPS); // Se establece la velocidad
  radio.openReadingPipe(1,pipe);
  radio.startListening();
  printf_begin();
  radio.printDetails();
}
 
void loop(void)
{
  if (radio.available())
  {
    radio.read(data, sizeof data); 
    Serial.println(data);
  }
}