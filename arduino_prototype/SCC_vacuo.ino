// ensaio para vacuo

boolean rly01=4;  // relay val 01 - gas
boolean rly02=6;  // relay val 02 - vol A -> camara
boolean rly03=8;  // relay val 03 - camara -> vol B
boolean rly04=10;  // relay val 04 - ATM
boolean rly05=12;  // relay val 05 - vacuo

boolean botaostart=32; // botao iniciar

boolean ledverde=25;      // led verde
boolean ledvermelho=23;      // led vermelho

void setup()
{
  Serial.begin(115200);

  pinMode(ledverde,OUTPUT);
  pinMode(ledvermelho,OUTPUT);

  pinMode(botaostart,INPUT);

  pinMode(rly01,OUTPUT);
  pinMode(rly02,OUTPUT);
  pinMode(rly03,OUTPUT);
  pinMode(rly04,OUTPUT);
  pinMode(rly05,OUTPUT);

}

void loop()
{

  if (digitalRead(botaostart)==HIGH)
  {
    for (int j = 0; j < 10; j++)
    {
      digitalWrite(ledverde, LOW);
      delay(10);
      digitalWrite(ledvermelho, HIGH);

      digitalWrite(rly05, HIGH);
      delay(2000);
      digitalWrite(rly03, HIGH);
      delay(2000);
      digitalWrite(rly02, HIGH);
      delay(2000);
//      digitalWrite(rly01, HIGH);
//      delay(2000);

      delay(6000);

//      digitalWrite(rly01, LOW);
//      delay(10);
      digitalWrite(rly02, LOW);
      delay(10);
      digitalWrite(rly03, LOW);
      delay(10);
      digitalWrite(rly05, LOW);
      delay(10);


      digitalWrite(ledvermelho, LOW);
      digitalWrite(ledverde, HIGH);
      delay(2000);
    }
  }
  else
  {
    digitalWrite(ledverde, HIGH);
  }

}




