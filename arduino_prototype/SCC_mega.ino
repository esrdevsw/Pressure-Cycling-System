int led=26;      // led no pino 26
int botao=28;  // botÃ£o no pino 28
int var=0;       // valor instantaneo enviado pelo botÃ£o
int var2=0;     // valor guardado
int estado=0;  // guarda o valor 0 ou 1 (HIGH ou LOW)

// relay
int rly01=2;  // relay digital 02
int rly02=3;  // relay digital 03
int rly03=4;  // relay digital 04
int rly04=5;  // relay digital 05
int rly05=6;  // relay digital 06

// relay manual
int btrly01=24; // botao relay
int btrly03=22; // botao relay

// SimPlot - 4 canais
int buffer[20]; //Buffer needed to store data packet for transmission
int data1;
int data2;
int data3;
int data4;

int X;
int cont = 1;




void setup()
{
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
  pinMode(led,OUTPUT);
  pinMode(botao,INPUT);


  pinMode(rly01,OUTPUT);
  pinMode(rly02,OUTPUT);
  pinMode(rly03,OUTPUT);
  pinMode(rly04,OUTPUT);
  pinMode(rly05,OUTPUT);

  pinMode(btrly01,INPUT);
  pinMode(btrly03,INPUT);
}
void loop()
{ 
  X=cont;
  PCrtA();
  
  var=digitalRead(botao); // ler o valor enviado pelo botÃ£o: "HIGH" ou "LOW"
  if ((var == HIGH) && (var2 == LOW)) 
  {
    estado = 1 - estado;
    delay(50); 
  }
  var2=var;
  if (estado == 1) // modo automatico
  {
    digitalWrite(led, LOW);  // desliga o led //    Serial.println("Automatico "); 
    digitalWrite(rly01, HIGH); 
    delay(100); 
    digitalWrite(rly01, LOW); 
    delay(100); 
    digitalWrite(rly03, HIGH); 
    delay(100); 
    digitalWrite(rly03, LOW); 
    delay(100);

  }
  else // modo manual
  {
    digitalWrite(led, HIGH); // liga o led indicando controle 
    manualVALV();

  } 
}






