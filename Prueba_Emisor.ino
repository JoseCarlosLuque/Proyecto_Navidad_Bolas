#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SPI.h>
#include <printf.h>

const int pinCE = 9;
const int pinCSN = 10;
RF24 radio(pinCE, pinCSN);

// Single radio pipe address for the 2 nodes to communicate.
const byte pipe[6] = "DISCO";
 
char data[16]="Hola mundo" ;

void setup(void)
{
  Serial.begin(9600);
  if(!radio.begin()){
    Serial.println("Algo ha fallado con la antena");
  }else{
    Serial.println("Todo va bien con la antena");
  }
  radio.stopListening();           // IMPORTANTE PONER ESTA LINEA ANTES DEL WRITING PIPE.
  radio.openWritingPipe(pipe);
  radio.setAutoAck(false); // Eliminamos el ACK para establecer la comunicacion
  radio.setPALevel(RF24_PA_MIN); // Se establece la potencia.
  radio.setDataRate(RF24_250KBPS); // Se establece la velocidad.
  
 
  printf_begin();
  radio.printDetails();
}
 
void loop(void)
{
  radio.write(data, sizeof data);
  delay(2000);
  Serial.println("Mensaje mandado....");
}