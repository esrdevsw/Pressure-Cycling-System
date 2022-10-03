#include <Arduino.h>

/*
  Controle manual e automatico do SCC.
 */
#include <SD.h>
// include the library code:
#include <LiquidCrystal.h>

/* On the Ethernet Shield, CS is pin 4. Note that even if it's not
 // used as the CS pin, the hardware CS pin (10 on most Arduino boards,
 // 53 on the Mega) must be left as an output or the SD library
 functions will not work.
 */
const int chipSelect = 53;
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(27, 29, 49, 48, 47, 46);


boolean led=26;      // led no pino 26
//boolean botao=28;  // botÃ£o no pino 28
int var=0;       // valor instantaneo enviado pelo botÃ£o
int var2=0;     // valor guardado
int estado=0;  // guarda o valor 0 ou 1 (HIGH ou LOW)
int iides = 0;

int bi=50;

int nPM = 10; // numero maximo de pontos para a carga
int nP = 0; // numero de pontos adquiridos

float setP = 0; // pontos set de pressao p carga
float setPold = 0;
// relay
boolean rly01=4;  // relay val 01 - gas
boolean rly02=6;  // relay val 02 - vol A -> camara
boolean rly03=8;  // relay val 03 - camara -> vol B
boolean rly04=2;  // relay val 04 - ATM
boolean rly05=10;  // relay val 05 - vacuo

boolean botaostart=32; // botao iniciar
// relay manual
boolean val01=30; // botao val01
boolean val02=28; // botao val02
boolean val03=26; // botao val03
boolean val04=24; // botao val04
boolean val05=22; // botao val05

boolean ledverde=25;      // led verde
boolean ledvermelho=23;      // led vermelho.

boolean vacuo=38; // botao
boolean atm=40; // botao

boolean Pmais=34; // botao
boolean Pmenos=36; // botao

//boolean btrly01=24; // botao relay
//boolean btrly03=22; // botao relay
// temperatura

uint8_t pontoA = 13;
uint8_t pontoB = 10;
float valorA = 0;
float valorB = 0;
float diferenca = 0;
uint8_t pontoA2 = 11;
uint8_t pontoB2 = 12;
float valorA2 = 0;
float valorB2 = 0;
float diferenca2 = 0;
uint8_t pontoA3 = 9;
uint8_t pontoB3 = 14;
float valorA3 = 0;
float valorB3 = 0;
float diferenca3 = 0;
float R6 = 100;
float R7 = 5000;
float Res01 = 0;
float Res02 = 0;
float Res03 = 0;
float K2 = -2.4202e+02;
float K3 = 2.2228e+00;
float K4 = 2.5859e-03;
float K5 = -4.8260e-06;
float K6 = -2.8183e-08;
float K7 = 1.5243e-10;
float temperatura = 0;
float temperatura2 = 0;
float temperatura3 = 0;

// pressao
float  volA = 0;
float  volB = 0;
float PvolA = 0;
float PvolB = 0;
float pressao = 0;

//////////////////////////////////////////////////////////////////////////
float defPV01 = 0; // set de pressao
float readPV01 = 0;    // leitura pressao volume 01
float TempA = 0; // temperatura volume 01
float TempC = 0; // temperatura da camara
float readPV02 = 0; // leitura pressao volume 02
float TempB = 0; // temperatura volume 02

int ciclos = 0;
int estagio =0;
int Tciclos =1;
float tempo = 1000; // tempo para leitura da pressao milisegundos

float average = 0;
float total = 0;                            // total móvel
float readings[50];                // qtde de amostras
int index = 0;                            // índice da leitura atual

float tempoC = 1;
float averageT = 0;
float totalT = 0;                            // total móvel
float readingsT[50];

float taxaT1=0;
float taxaT2=0;

int taxa = 50; // numero de repetiçoes para calculo da taxa de variação

float taxaP1=0;
float taxaP2=0;

float Dt = 0;
float DP = 0;

float DPDT = 1;

float tempoCin = 0;

int pAtual = 0;
int pProx = 1;

float AGCvacuo = A2; //entrada controle vacuo
float AGC = 0;

//calculo da inclinação da reta
float TempoB = 1;
float tB[50];  // tempos
float sigtB =  0;    // somatoria dos tempos
float PB[50];    // pressoes
float sigPB =  0;    // somatoria pressoes
float txP[50]; // tempo x pressao
float sigtxP =  0;  // somatorio tempo x pressao
float tB2[50]; // tempo ao quagrado
float sigtB2 =  0;   // somatorio tempo ao quagrado
float sigtquad = 0; // (somatoria de tempo) ao quadrado
float deltaA = 1;
float deltaB = 1;
float BA = 300; // inclinacao da reta
float BB = 300; // inclinacao da reta

void setup()
{
  // initialize the digital pin as an output.
  Serial.begin(9600);

  lcd.begin(16, 2);

  lcd.print("Start SCC");

  pinMode(ledverde,OUTPUT);
  pinMode(ledvermelho,OUTPUT);

  pinMode(botaostart,INPUT);

  pinMode(rly01,OUTPUT);
  pinMode(rly02,OUTPUT);
  pinMode(rly03,OUTPUT);
  pinMode(rly04,OUTPUT);
  pinMode(rly05,OUTPUT);


  pinMode (val01,INPUT);
  pinMode (val02,INPUT);
  pinMode (val03,INPUT);
  pinMode (val04,INPUT);
  pinMode (val05,INPUT);

  pinMode (vacuo,INPUT);
  pinMode (atm,INPUT);

  pinMode (volA,INPUT);
  pinMode (volB,INPUT);

  pinMode (AGCvacuo,INPUT);

  lcd.setCursor(0,1); // seta para linha 1, ou seja, a linha de baixo
  lcd.print("Initializing SD card...");

  Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(53, OUTPUT);

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect))
  {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");

  File dataFileCin = SD.open("cinetica.txt", FILE_WRITE);
  dataFileCin.print("ciclos");
  dataFileCin.print(" |   ");
  dataFileCin.print("tempo (s)");
  dataFileCin.print("   ");
  dataFileCin.print("PV01 () ");
  dataFileCin.print("   ");
  dataFileCin.print("T A ()");
  dataFileCin.print("   ");
  dataFileCin.print("T C ()");
  dataFileCin.print("   ");
  dataFileCin.print("PV02 () ");
  dataFileCin.print("   ");
  dataFileCin.println("T B ()");
  dataFileCin.close();
  //  //
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  dataFile.println(" planilha de calculo SCC");
  dataFile.close();


}



void tempsis()
{
  // temperatura
  valorA = analogRead(pontoA);
  valorB = analogRead(pontoB);
  diferenca = valorA - valorB;
  valorA2 = analogRead(pontoA2);
  valorB2 = analogRead(pontoB2);
  diferenca2 = valorA2 - valorB2;
  valorA3 = analogRead(pontoA3);
  valorB3 = analogRead(pontoB3);
  diferenca3 = valorA3 - valorB3;
  diferenca = (diferenca*5000)/1023;
  diferenca2 = (diferenca2*5000)/1023;
  diferenca3 = (diferenca3*5000)/1023;
  Res01 = (((R6*R7)-(((R6*R7)/(R6+R6))-diferenca)*R6)/(((R6*R7)/(R6+R6))-diferenca))-2.37;
  Res02 = (((R6*R7)-(((R6*R7)/(R6+R6))-diferenca2)*R6)/(((R6*R7)/(R6+R6))-diferenca2))-2.37;
  Res03 = (((R6*R7)-(((R6*R7)/(R6+R6))-diferenca3)*R6)/(((R6*R7)/(R6+R6))-diferenca3))-2.37;
  temperatura = K2+K3*Res01+K4*pow(Res01,2)+K5*pow(Res01,3)+K6*pow(Res01,4)+K7*pow(Res01,5);
  temperatura2 = K2+K3*Res02+K4*pow(Res02,2)+K5*pow(Res02,3)+K6*pow(Res02,4)+K7*pow(Res02,5);
  temperatura3 = K2+K3*Res03+K4*pow(Res03,2)+K5*pow(Res03,3)+K6*pow(Res03,4)+K7*pow(Res03,5);
}

void Psis()
{
  AGCvacuo = analogRead(A2);
  volA = analogRead(A3);
  volB = analogRead(A4);

  // conversao de bits para tensao em milivots
  AGC = map(AGCvacuo, 0, 1023, 0, 5000);
  // conversao de bits para pressao em milibar
  PvolA = map(volA, 0, 1023, 0, 25000);
  PvolB = map(volB, 0, 1023, 0, 25000);


}

void setpressao()
{
  if (digitalRead(Pmais)==HIGH)
  {

    defPV01=defPV01+0.01;

    Serial.print("set de  ");
    Serial.print(defPV01);
    Serial.println( " bar para volume A ");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("set de  ");
    lcd.setCursor(0,1); // seta para linha 1, ou seja, a linha de baixo
    lcd.print(defPV01);
    lcd.print( " bar vol A ");
    setP = (defPV01/(nPM-1)); // intervalo dos pontos de carga

  }
  if (digitalRead(Pmenos)==HIGH)
  {

    defPV01=defPV01-0.01;

    Serial.print("set de  ");
    Serial.print(defPV01);
    Serial.println( " bar para volume A ");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("set de  ");
    lcd.setCursor(0,1); // seta para linha 1, ou seja, a linha de baixo
    lcd.print(defPV01);
    lcd.print( " bar vol A ");
    setP = (defPV01/(nPM-1)); // intervalo dos pontos de carga
  }
}

void printela()
{

  Serial.print(ciclos);
  Serial.print("  ");
  Serial.print(nP);
  Serial.print("  ");
  Serial.print(tempoC);
  Serial.print("  ");
  Serial.print(defPV01);
  Serial.print("  ");
  Serial.print((PvolA/1000)+1);
  Serial.print("  ");
  Serial.print(temperatura2);
  Serial.print("  ");
  Serial.print(iides);
  Serial.print("   ");
  Serial.print((PvolB/1000)+1);
  Serial.print("   ");
  Serial.print(temperatura);
  Serial.print("   ");
  Serial.print(temperatura3);
  Serial.print("   ");
  Serial.print(AGC);
  Serial.print("  ");
  Serial.print("|");
  Serial.print("  ");
  Serial.print(index);
  Serial.print("    ");
  Serial.print(pressao );
  Serial.print("    ");
  Serial.print(TempoB);
  Serial.print("    ");
  Serial.print(sigtB);
  Serial.print("    ");
  Serial.print(sigPB);
  Serial.print("    ");
  Serial.print(sigtxP);
  Serial.print("    ");
  Serial.print(sigtB2);
  Serial.print("    ");
  Serial.print(sigtquad);
  Serial.print("    ");
  Serial.print("|");
  Serial.print("    ");
  Serial.print(deltaA);
  Serial.print("    ");
  Serial.println(BA);


  lcd.clear();
  lcd.setCursor(0,0); // seta o cursor para: (coluna = 0, linha = 0)
  lcd.print("ciclo");
  lcd.print(" ");
  lcd.print("nP");
  lcd.print(" ");
  lcd.print("desc");

  lcd.setCursor(0,1); // seta para linha 1, ou seja, a linha de baixo
  lcd.print(ciclos);
  lcd.print(" ");
  lcd.print(nP);
  lcd.print(" ");
  lcd.print(iides);


  delay (250);
  lcd.clear();

  lcd.setCursor(0,0); // seta o cursor para: (coluna = 0, linha = 0)
  lcd.print("PVolA");
  lcd.print(" ");
  lcd.print("PVolB");
  lcd.print(" ");
  lcd.print("Tc");

  lcd.setCursor(0,1); // seta para linha 1, ou seja, a linha de baixo
  lcd.print((PvolA/1000)+1);
  lcd.print(" ");
  lcd.print((PvolB/1000)+1);
  lcd.print(" ");
  lcd.print(temperatura2);
  delay (10);


}
void SDcardCalc()
{
  Psis();
  tempsis();
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile)
  {
    dataFile.print(ciclos);
    dataFile.print("  ");
    dataFile.print(nP);
    dataFile.print("  ");
    dataFile.print(tempoC);
    dataFile.print("  ");
    dataFile.print(defPV01);
    dataFile.print("  ");
    dataFile.print((PvolA/1000)+1);
    dataFile.print("  ");
    dataFile.print(temperatura2);
    dataFile.print("  ");
    dataFile.print(iides);
    dataFile.print("   ");
    dataFile.print((PvolB/1000)+1);
    dataFile.print("   ");
    dataFile.print(temperatura);
    dataFile.print("   ");
    dataFile.print(temperatura3);
    dataFile.print("   ");
    dataFile.print(AGC);
    dataFile.print("  ");
    dataFile.println(BA);
    dataFile.close();

  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
    //    Serial.println(dataString);
  }
}
void SDcardCin()
{
  Psis();
  tempsis();
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFileCin = SD.open("cinetica.txt", FILE_WRITE);
  // if the file is available, write to it:
  if (dataFileCin)
  {
    tempoCin=tempoCin+0.507;

    dataFileCin.print(ciclos);
    dataFileCin.print("  ");
    dataFileCin.print(nP);
    dataFileCin.print("  ");
    dataFileCin.print(tempoC);
    dataFileCin.print("  ");
    dataFileCin.print(defPV01);
    dataFileCin.print("  ");
    dataFileCin.print((PvolA/1000)+1);
    dataFileCin.print("  ");
    dataFileCin.print(temperatura2);
    dataFileCin.print("  ");
    dataFileCin.print(iides);
    dataFileCin.print("   ");
    dataFileCin.print((PvolB/1000)+1);
    dataFileCin.print("   ");
    dataFileCin.print(temperatura);
    dataFileCin.print("   ");
    dataFileCin.print(temperatura3);
    dataFileCin.print("   ");
    dataFileCin.print(AGC);
    dataFileCin.print("  ");
    dataFileCin.print("|");
    dataFileCin.print("  ");
    dataFileCin.print(index);
    dataFileCin.print("    ");
    dataFileCin.print(pressao );
    dataFileCin.print("    ");
    dataFileCin.print(TempoB);
    dataFileCin.print("    ");
    dataFileCin.print(sigtB);
    dataFileCin.print("    ");
    dataFileCin.print(sigPB);
    dataFileCin.print("    ");
    dataFileCin.print(sigtxP);
    dataFileCin.print("    ");
    dataFileCin.print(sigtB2);
    dataFileCin.print("    ");
    dataFileCin.print(sigtquad);
    dataFileCin.print("    ");
    dataFileCin.print("|");
    dataFileCin.print("    ");
    dataFileCin.print(deltaA);
    dataFileCin.print("    ");
    dataFileCin.println(BA);
    dataFileCin.close();

  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening cinetica.txt");
    //    Serial.println(dataString);
  }
}
void(* resetFunc) (void) = 0; //declare reset function @ address 0

void loop()
{
  setpressao();

  var=digitalRead(botaostart); // ler o valor enviado pelo botÃ£o: "HIGH" ou "LOW"

  if ((var == HIGH) && (var2 == LOW))
  {
    estado = 1 - estado;
    delay(50);
  }

  var2=var;
  if (estado == 1) // modo automatico
  {
    for (ciclos = 0; ciclos < Tciclos;)
    {
      defPV01 = setPold + setP;
      setPold = defPV01;

      for (nP = 0; nP < nPM;) // nP numero de pontos nPM numero de pontos maximo.
      {
        //         pAtual = nP;
        pProx = nP+1;

        if (estagio==0) // definição da pressao no volume A
        {
          lcd.clear();
          lcd.noDisplay();
          Serial.print("Ponto numero  ");
          Serial.println(nP);
          Psis();
          Serial.print("Pressao no volume A = ");
          Serial.print(((PvolA/1000)+1));
          Serial.print(" bar ");
          Serial.print(" ==> Pressao desejada = ");
          Serial.print(defPV01);
          Serial.println(" bar ");




          for (int ii = 0; ii < 10; ii++)
          {
            digitalWrite(ledvermelho, HIGH);
            digitalWrite(ledverde, HIGH);
            delay(100);
            digitalWrite(ledvermelho, LOW);
            digitalWrite(ledverde, LOW);
          }
          if (((PvolA/1000)+1) < defPV01)
          {
            lcd.clear();
            lcd.noDisplay();
            Serial.println("iniciar carga do volume A");
            Serial.println("    ");
            digitalWrite(rly02, LOW);
            digitalWrite(rly01, HIGH);
            delay(7);
            digitalWrite(rly01, LOW);
            delay(5000);
            tempoC = (tempoC + 5); // tempo  cinetica em segundos

            Serial.print("Pressao no volume A = ");
            Serial.print(((PvolA/1000)+1));
            Serial.print(" bar ");
            Serial.println("    ");
            lcd.display();
          }
          if (((PvolA/1000)+1) > defPV01)
          {
            lcd.clear();
            lcd.noDisplay();
            Serial.print("Pressao no volume A = ");
            Serial.print(((PvolA/1000)+1));
            Serial.print(" bar ----> Iniciar carga da camara");
            Serial.println("    ");
            Serial.print(nP);
            Serial.print("    ");
            Serial.println(((PvolA/1000)+1));

            File dataFile = SD.open("datalog.txt", FILE_WRITE);

            dataFile.print("Pressao no volume A = ");
            dataFile.print(((PvolA/1000)+1));
            dataFile.print(" bar ----> Iniciar carga da camara");
            dataFile.println("    ");
            dataFile.print(nP);
            dataFile.print("    ");
            dataFile.println(((PvolA/1000)+1));

            dataFile.close();
            SDcardCalc();

            SDcardCin();
            printela();
            delay(5000);
            tempoC = (tempoC + 5); // tempo  cinetica em segundos
            estagio=1;
          }
        }

        if ((estagio==1))// carregar a amostra
        {
          lcd.display();
          setpressao();

          Serial.print(" iniciar estagio de carga da amostra para o ponto ");
          Serial.println(nP);
          delay(100);
          //        resetFunc();
          SDcardCin();

          index = 0;
          TempoB = 1;
          sigtB = 0;
          sigPB= 0;
          sigtxP= 0;
          sigtB2= 0;
          sigtquad= 0;
          for (int i = 0; i < taxa; i++)
          {
            setpressao();
            Psis();
            tempsis();
            digitalWrite(ledvermelho, HIGH);
            digitalWrite(ledverde, LOW);
            digitalWrite(rly02, HIGH);
            delay(1000);

            tempoC = (tempoC + 1); // tempo  cinetica em segundos

            TempoB = (TempoB + 1); // tempo  cinetica em segundos
            pressao = PvolA;
            tB[index] = TempoB;  // tempos
            sigtB +=  tB[index];    // somatoria dos tempos
            PB[index] = pressao;    // pressoes
            sigPB +=  PB[index];    // somatoria pressoes
            txP[index] = TempoB*pressao; // tempo x pressao
            sigtxP +=  txP[index];  // somatorio tempo x pressao
            tB2[index] = TempoB*TempoB; // tempo ao quagrado
            sigtB2 +=  tB2[index];   // somatorio tempo ao quagrado
            sigtquad = sigtB*sigtB; // (somatoria de tempo) ao quadrado
            index = (index + 1);     // avançar ao próximo índice

            SDcardCin();
            printela();
          }

          deltaA = taxa*sigtB2-sigtquad;
          BA =(((taxa*sigtxP)-(sigtB*sigPB))/deltaA)*(-1); // inclinacao da reta
          Serial.println("================================================================");
          Serial.print("  deltaA  ");
          Serial.print("    ");
          Serial.println(" BA   ");
          Serial.print(deltaA);
          Serial.print("    ");
          Serial.println(BA);
          Serial.println("================================================================");





          if (BA >= 0 && BA <= 5 &&  estagio==1)
          {

            Serial.print(" >>>>>>>>>>>>>>>>>>>>  fim do ponto ");
            Serial.println(nP);

            digitalWrite(rly02, LOW);
            delay(5000);
            tempoC = (tempoC + 5); // tempo  cinetica em segundos

            File dataFile = SD.open("datalog.txt", FILE_WRITE);
            dataFile.println(" >>>>>>>>>>>>>>>>>>>>  fim do ponto ");
            dataFile.print(nP);
            dataFile.print("    ");
            dataFile.println(((PvolA/1000)+1));
            dataFile.close();
            SDcardCalc();

            SDcardCin();
            printela();

            nP = nP+1;

            if (nP < nPM && estagio==1)
            {
              Serial.print(" >>>>>>>>>>>>>>>>>>>>     ");
              Serial.println("iniciar nova carga do volume A para o proximo ponto");

              defPV01 = setPold + setP;
              setPold = defPV01;

              estagio=0;
              delay(5000);
              tempoC = (tempoC + 5); // tempo  cinetica em segundos
            }


            if (nP == nPM && estagio==1)
            {
              Serial.println(" xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx     ");
              Serial.println("Fim dos pontos - carga da amostra");
              digitalWrite(rly02, LOW);
              delay (5000);
              tempoC = (tempoC + 5); // tempo  cinetica em segundos
              Serial.println("iniciar descarga da amostra");

              File dataFile = SD.open("datalog.txt", FILE_WRITE);
              dataFile.println(" xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx     ");
              dataFile.println("Fim dos pontos - carga da amostra");
              dataFile.print(nP);
              dataFile.print("    ");
              dataFile.println(((PvolA/1000)+1));
              dataFile.close();
              SDcardCalc();

              PvolB=PvolA;
              Serial.println(" xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx     ");
              printela();
              Serial.println(" xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx     ");
              SDcardCin();
              printela();
              Serial.println("iniciar descarga  ");

              for (iides = 0; iides < 5; iides++)
              {
                delay(5000);
                tempoC = (tempoC + 5); // tempo  cinetica em segundos
                digitalWrite(rly03, HIGH);

                for (int i = 0; i < 150; i++)
                {
                  delay(1000);
                  SDcardCin();
                  printela();
                  tempoC = (tempoC + 1); // tempo  cinetica em segundos
                }
                delay(1000);
                tempoC = (tempoC + 1); // tempo  cinetica em segundos

                File dataFile = SD.open("datalog.txt", FILE_WRITE);
                dataFile.print("Pressao no volume B = ");
                dataFile.print(((PvolB/1000)+1));
                dataFile.print(" bar ----> descarga");
                dataFile.println("    ");
                dataFile.print(iides);
                dataFile.print("    ");
                dataFile.println(((PvolB/1000)+1));

                if (((PvolB/1000)+1)>12 && AGC > 50)
                {
                  digitalWrite(rly03, LOW);
                  SDcardCin();
                  printela();
                  tempoC = (tempoC + 1); // tempo  cinetica em segundos

                  File dataFile = SD.open("datalog.txt", FILE_WRITE);
                  dataFile.print("Pressao no volume B = ");
                  dataFile.print(((PvolB/1000)+1));
                  dataFile.print(" bar ----> descarga");
                  dataFile.println("    ");
                  dataFile.print(iides);
                  dataFile.print("    ");
                  dataFile.println(((PvolB/1000)+1));
                  dataFile.close();

                  delay(2000);
                  tempoC = (tempoC + 2); // tempo  cinetica em segundos
                  lcd.clear();
                  lcd.noDisplay();
                  digitalWrite(rly04, HIGH);
                  delay(100);
                  digitalWrite(rly04, LOW);
                  delay(2000);
                  tempoC = (tempoC + 2); // tempo  cinetica em segundos
                  lcd.display();
                  printela();
                  SDcardCin();
                  digitalWrite(rly05, HIGH);
                  delay(30000);
                  tempoC = (tempoC + 30); // tempo  cinetica em segundos
                  digitalWrite(rly05, LOW);
                }

                digitalWrite(rly03, HIGH);
                delay(5000);
                tempoC = (tempoC + 5); // tempo  cinetica em segundos

                digitalWrite(rly03, LOW);
                delay(5000);
                tempoC = (tempoC + 5); // tempo  cinetica em segundos

                SDcardCin();
                printela();

                dataFile.print("Pressao no volume B = ");
                dataFile.print(((PvolB/1000)+1));
                dataFile.print(" bar ----> descarga");
                dataFile.println("    ");
                dataFile.print(iides);
                dataFile.print("    ");
                dataFile.println(((PvolB/1000)+1));
                dataFile.close();

                for (int i = 0; i < 20; i++)
                {
                  digitalWrite(rly05, HIGH);
                  SDcardCin();
                  printela();
                  delay(1000);
                  tempoC = (tempoC + 1); // tempo  cinetica em segundos
                }

                delay(1000);
                tempoC = (tempoC + 1); // tempo  cinetica em segundos
                digitalWrite(rly05, LOW);

                if ((iides+1) == 5)
                {
                  Serial.println(" xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx     ");
                  printela();
                  Serial.println(" xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx     ");
                  Serial.println("fim da descarga");

                  File dataFile = SD.open("datalog.txt", FILE_WRITE);

                  dataFile.print("Pressao no volume B = ");
                  dataFile.print(((PvolB/1000)+1));
                  dataFile.print(" bar ----> fim da descarga");
                  dataFile.println("    ");
                  dataFile.print(iides);
                  dataFile.print("    ");
                  dataFile.println(((PvolB/1000)+1));

                  dataFile.close();

                  lcd.clear();
                  lcd.setCursor(0,0);
                  lcd.print("fim da descarga");
                  lcd.clear();
                  lcd.setCursor(0,1);
                  lcd.print("Limpando a camara");
                  delay(1000);
                  tempoC = (tempoC + 1); // tempo  cinetica em segundos
                  SDcardCin();
                  printela();

                  dataFile.print("Pressao no volume B = ");
                  dataFile.print(((PvolB/1000)+1));
                  dataFile.print(" bar ----> fim da descarga");
                  dataFile.println("    ");
                  dataFile.print(iides);
                  dataFile.print("    ");
                  dataFile.println(((PvolB/1000)+1));
                  dataFile.close();

                  for (int i = 0; i < 50; i++)
                  {
                    delay(1000);
                    tempoC = (tempoC + 1); // tempo  cinetica em segundos
                    SDcardCin();
                    printela();
                    digitalWrite(rly03, HIGH);
                    delay(2000);
                    tempoC = (tempoC + 2); // tempo  cinetica em segundos
                    SDcardCin();
                    printela();
                    digitalWrite(rly05, HIGH);
                    tempoC = (tempoC + 1); // tempo  cinetica em segundos
                    delay(1000);
                    SDcardCin();
                    printela();
                  }

                  delay(1000);
                  digitalWrite(rly05, LOW);
                  delay(1000);
                  tempoC = (tempoC + 2); // tempo  cinetica em segundos
                  SDcardCin();
                  printela();

                  //                  for (int i = 0; i < taxa; i++)
                  //                  {
                  //                    delay(1000);
                  //                    SDcardCin();
                  //                    printela();
                  //                    digitalWrite(rly03, HIGH);
                  //                    delay(2000);
                  //                    tempoC = (tempoC + 3); // tempo  cinetica em segundos
                  //                  }

                  dataFile.print("Pressao no volume B = ");
                  dataFile.print(((PvolB/1000)+1));
                  dataFile.print(" bar ----> fim da descarga");
                  dataFile.println("    ");
                  dataFile.print(iides);
                  dataFile.print("    ");
                  dataFile.println(((PvolB/1000)+1));
                  dataFile.close();

                  //                  for (int i = 0; i < (taxa); i++)
                  //                  {
                  //                    delay(1000);
                  //                    tempoC = (tempoC + 1); // tempo  cinetica em segundos
                  //                    SDcardCin();
                  //                    printela();
                  //                    digitalWrite(rly03, HIGH);
                  //                    delay(2000);
                  //                    tempoC = (tempoC + 2); // tempo  cinetica em segundos
                  //                    SDcardCin();
                  //                    printela();
                  //                    digitalWrite(rly05, HIGH);
                  //                    tempoC = (tempoC + 1); // tempo  cinetica em segundos
                  //                    delay(1000);
                  //                    SDcardCin();
                  //                    printela();
                  //                  }

                  digitalWrite(rly05, LOW);
                  delay(2000);
                  digitalWrite(rly03, LOW);
                  delay(2000);
                  lcd.clear();
                  lcd.setCursor(0,1);
                  lcd.print("iniciar novo ciclo");

                  dataFile.print("=========================================================== ");

                  dataFile.print(" iniciar novo ciclo ");
                  dataFile.print("=========================================================== ");
                  dataFile.println("    ");
                  dataFile.close();

                  ciclos=ciclos+1;
                  nP = 0;
                  estagio=0;
                  delay(5000);
                  tempoC = (tempoC + 9); // tempo  cinetica em segundos
                }
              }
            }
            if ( BA > 0.7 &&  estagio==1)
            {
              nP = pProx-1;
            }
          }
        }
      }
      if ((ciclos+1) == Tciclos )
      {
        for (int ii = 0; ii < 50; ii++)
        {
          digitalWrite(ledvermelho, HIGH);
          digitalWrite(ledverde, HIGH);
          delay(500);
          digitalWrite(ledvermelho, LOW);
          digitalWrite(ledverde, LOW);

          File dataFile = SD.open("datalog.txt", FILE_WRITE);
          dataFile.print(" >>>>>>>>>>>>>>>>>>>>     ");
          dataFile.println("fim do ciclo  ");
          dataFile.println(ciclos);
          dataFile.close();
          SDcardCalc();

        }
        resetFunc();
      }
    }
  }

  else // modo manual
  {
    // liga o led indicando controle
    digitalWrite(ledverde, HIGH);

    // controle valvulas
    if (digitalRead(val01)==HIGH)
    {
      digitalWrite(rly01, HIGH);
    }
    if (digitalRead(val01)==LOW)
    {
      digitalWrite(rly01, LOW);
    }

    if (digitalRead(val02)==HIGH)
    {
      digitalWrite(rly02, HIGH);
    }
    if (digitalRead(val02)==LOW)
    {
      digitalWrite(rly02, LOW);
    }

    if (digitalRead(val03)==HIGH)
    {
      digitalWrite(rly03, HIGH);
    }
    if (digitalRead(val03)==LOW)
    {
      digitalWrite(rly03, LOW);
    }

    if (digitalRead(val04)==HIGH)
    {
      digitalWrite(rly04, HIGH);
    }
    if (digitalRead(val04)==LOW)
    {
      digitalWrite(rly04, LOW);
    }

    if (digitalRead(val05)==HIGH)
    {
      digitalWrite(rly05, HIGH);
    }
    if (digitalRead(val05)==LOW)
    {
      digitalWrite(rly05, LOW);
    }

  }
}







