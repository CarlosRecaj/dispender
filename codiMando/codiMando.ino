#include <IRremote.hpp>
#include <LiquidCrystal.h>

const int RECV_PIN = 11;
int numeroFinal;
boolean perfils = false;
LiquidCrystal lcd(7,6,5,4,3,2);
const int surtidorA = 10;
const int surtidorM = 12;
int perfil1,perfil2,perfil3,perfil4;

void setup() {
  Serial.begin(9600);
  IrReceiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK);
  lcd.begin(16,2);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Perfils ?       ");
  lcd.setCursor(0,1);
  lcd.print("0-No 1-Si       ");
  int select = -1;
  while (select != 0 && select != 1) {
    select = esperarDigito();  
  }
  if(select == 0){
    perfils = false; 
  }
  else{
    perfils = true;
    perfil1 =0; 
    perfil2 = 0; 
    perfil3 = 0; 
    perfil4 = 0; 
  }
  pinMode(surtidorA, OUTPUT);
  pinMode(surtidorM, OUTPUT);
}

int translateIR(uint8_t command) {
  switch (command) {
    case 0x16: return 0;
    case 0x0C: return 1;
    case 0x18: return 2;
    case 0x5E: return 3;
    case 0x08: return 4;
    case 0x1C: return 5;
    case 0x5A: return 6;
    case 0x42: return 7;
    case 0x52: return 8;
    case 0x4A: return 9;
    case 0x45: return 101;  // Botó ON
    case 0x0D: return 102;  // Botó REPT 
    case 0x47: return 9999; //ESC
    default: return -1; // Valor inválido
  }
}

int esperarDigito() {
  while (true) {
    if (IrReceiver.decode()) {
      int valor = translateIR(IrReceiver.decodedIRData.command);
      IrReceiver.resume();
      if (valor >= 0 && valor <= 9 || valor == 101 || valor == 102 || valor == 9999) {
        return valor;
        delay(500);
      }
    }
  }
}

int dosDigitsOK(){
  int primer = esperarDigito();
  if(primer == 9999){
    return primer;
  }
  if(10 <= numeroFinal && numeroFinal <= 90 && primer == 101){
    int aux = numeroFinal; 
    numeroFinal = 0; 
    return aux;
  }
  else if(primer == 101 || primer == 102){ 
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print(0);
    return -1; 
  }
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print(primer);

  int segundo = esperarDigito();
  if(segundo == 9999){
    return segundo;
  }
  if(segundo == 102){
    primer = 0; 
    segundo = 0; 
    lcd.print(primer);
  }
  lcd.setCursor(1,1);
  lcd.print(segundo);

  numeroFinal = primer * 10 + segundo;
  delay(1000); 
  return -1;
}

int tresDigitsOK() {
  int primer = esperarDigito();
  if(primer == 9999){
    return 9999;
  }
  if ((numeroFinal >= 100 && numeroFinal <= 999) && primer == 101) {
    int aux = numeroFinal;
    numeroFinal = 0;
    return aux;
  } else if (primer == 101 || primer == 102) {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print(0);
    return -1;
  }

  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print(primer);

  int segundo = esperarDigito();
  if(segundo == 9999){
    return 9999;
  }
  if (primer == 101 || primer == 102) {
    numeroFinal = 0;
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print(0);
    return -1;
  }

  lcd.setCursor(1, 1);
  lcd.print(segundo);

  int tercero = esperarDigito();
  if(tercero == 9999){
    return 9999;
  }
  if (primer == 101 || primer == 102) {
    numeroFinal = 0;
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print(0);
    return -1;
  }

  lcd.setCursor(2, 1);
  lcd.print(tercero);

  numeroFinal = primer * 100 + segundo * 10 + tercero;
  delay(300);
  return -1;
}

int demanaBeguda(int midaGot){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.println("% d'alcohol ?    "); 
  int alcohol = -1; 
  while(alcohol == -1){
    alcohol = dosDigitsOK();
    delay(300);
  } 
  if(alcohol != 9999){
    int mezcla = 100 - alcohol;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Preparant");
    lcd.setCursor(10,0);
    lcd.print(alcohol);
    lcd.print("-");
    lcd.print(mezcla);
    //SURTIDORS
    long tempsTotal = midaGot*70; //conversió segons les proves
    long t1 = tempsTotal*alcohol/100; 
    digitalWrite(surtidorA, HIGH);
    delay(t1);
    digitalWrite(surtidorA, LOW);
    long t2 = tempsTotal - t1; 
    digitalWrite(surtidorM, HIGH);
    delay(t2);
    digitalWrite(surtidorM, LOW);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Gaudeix-lo");
    return alcohol; 
  }
  return 9999; 
}

int tamany(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Mida Got(ml) ?  ");
  int mida = -1; 
  while(mida == -1){
    mida = tresDigitsOK();
    delay(300);
  }
  return mida;  
}

float gels() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Tens gels?      ");
  lcd.setCursor(0, 1);
  lcd.print("0-No 1-Pocs 2-Molts");
  int seleccion = -1;
  while (seleccion != 0 && seleccion != 1 && seleccion != 2 && seleccion != 9999) {
    seleccion = esperarDigito();  // debe devolver 0, 1, 2 o 9999
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Seleccionado:");
  lcd.setCursor(0, 1);
  switch (seleccion) {
    case 0:
      lcd.print("No");
      return 1; 
      break;
    case 1:
      lcd.print("Poc");
      return 0.8; 
      break;
    case 2:
      lcd.print("Molt");
      return 0.66; 
      break;
    case 9999:
      return 9999; 
      break; 
  }
  delay(1000); 
  return seleccion;
}

int comanda(){
  int midaGot = tamany(); 
    if(midaGot == 9999){
      return 9999; 
    }
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Got de :");
    lcd.setCursor(8,0);
    lcd.print(midaGot);
    delay(1000); 
    float multiplicadorGels = gels(); 
    if(multiplicadorGels == 9999){
      return 9999;
    }
    int quantitatAOmplir = round(midaGot*multiplicadorGels);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.println("% d'alcohol ?    ");
    delay(1000);
    int percentAlcohol = demanaBeguda(quantitatAOmplir);
    if(percentAlcohol == 9999){
      return 9999; 
    }
    int quantitatAlcohol = quantitatAOmplir*percentAlcohol/100; 
    return quantitatAlcohol;
}
void loop() {
  if(perfils == true){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Selec. perfil   ");
    lcd.setCursor(0, 1);
    lcd.print("1-2-3-4");
    int perf = -1;
    delay(200);
    while (perf != 1 && perf != 2 && perf != 3 && perf != 4) {
      perf = esperarDigito();  
      delay(300);
    }
    int alcohol = 9999;
    lcd.clear();
    switch (perf) {
    case 1:
      lcd.print("Perfil 1");
      delay(1500);
      alcohol = comanda(); 
      if(alcohol != 9999){
        perfil1 = perfil1 + alcohol; 
        lcd.setCursor(0,0);
        lcd.print("Perfil 1 cons:  ");
        lcd.setCursor(0,1);
        lcd.print(perfil1);
        lcd.setCursor(3, 1);
        lcd.print("ml");
        delay(2000);
      }
      break;
    case 2:
      lcd.print("Perfil 2");
      delay(1500);
      alcohol = comanda(); 
      if(alcohol != 9999){
        perfil2 = perfil2 + alcohol; 
        lcd.setCursor(0,0);
        lcd.print("Perfil 2 cons:  ");
        lcd.setCursor(0,1);
        lcd.print(perfil2);
        lcd.setCursor(3, 1);
        lcd.print("ml");
        delay(2000);
      }
      break;
    case 3:
      lcd.print("Perfil 3");
      delay(1500);
      alcohol = comanda(); 
      if(alcohol != 9999){
        perfil3 = perfil3 + alcohol;
        lcd.setCursor(0,0);
        lcd.print("Perfil 3 cons:  ");
        lcd.setCursor(0,1);
        lcd.print(perfil3);
        lcd.setCursor(3, 1);
        lcd.print("ml");
        delay(2000); 
      }
      break;
    case 4:
      lcd.print("Perfil 4");
      delay(1500);
      alcohol = comanda(); 
      if(alcohol != 9999){
        perfil4 = perfil4 + alcohol; 
        lcd.setCursor(0,0);
        lcd.print("Perfil 4 cons:  ");
        lcd.setCursor(0,1);
        lcd.print(perfil4);
        lcd.setCursor(3, 1);
        lcd.print("ml");
        delay(2000);
      }
      break;
    default:
      break;
  }
  }
  else{
    int a = comanda();
  }
  delay(3000);
  lcd.clear(); 
  delay(1000);
}
