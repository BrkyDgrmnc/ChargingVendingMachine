#include<Wire.h>
#include<LiquidCrystal_I2C.h>
#include<EEPROM.h>

#define usb_pin       6
#define para_boyut    2
#define sec 9
#define yukari 10
#define asagi 11
#define analogPin A2


int analog_olcum = 0;
float voltaj_degeri = 0;
float hesapli_deger = 0;
int yuzde = 0;
int min_deger = 819;
int deger = 0;
float R1 = 20000;
float R2 = 10000; 
int para = 0;
int saat = 0; 
int dakika = 0 ;
int saniye = 0 ;
int sure;
int menusecbutton=0;
int altmenubutton=0;
int say = 4;
int a = 0;
bool b = false;
bool c = false;
bool x = false;

volatile unsigned long sonArkSuresi;
long gecikme = 10;

LiquidCrystal_I2C lcd(0x27 , 16 , 2);

byte enerji[] = {
  0x01,
  0x02,
  0x04,
  0x0F,
  0x1E,
  0x04,
  0x08,
  0x10
};

byte TL[] = {
  0x08,
  0x08,
  0x0C,
  0x18,
  0x0C,
  0x18,
  0x09,
  0x06
};


void setup() {
  Serial.begin(9600);
  sure = EEPROM.read(0);
  para = EEPROM.read(1);
  pinMode(para_boyut , INPUT);
  pinMode(sec , INPUT);
  pinMode(yukari, INPUT);
  pinMode(asagi, INPUT);
  pinMode(usb_pin, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(usb_pin, LOW);
 
  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = 15624;
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS12) | (0 << CS11) | (0 << CS10);
  TIMSK1 |= (1 << OCIE1A);
  sei();

  attachInterrupt(digitalPinToInterrupt(2), Algilama , FALLING);
 
  lcd.init();
  lcd.backlight();

  lcd.createChar(4, enerji); 
  lcd.createChar(6, TL);
}

ISR(TIMER1_COMPA_vect){
  say++;
  if (say == 5){
    analog_olcum = analogRead(analogPin);
    deger = analog_olcum ;
    if(deger >= 955){
      deger = 955;
    }
    if(deger <= 819){
      deger = 819;
    }
    yuzde = map(deger , 819 , 955 , 0 , 100);
    voltaj_degeri = (analog_olcum * 5.0) / 1024;
    hesapli_deger = voltaj_degeri / (R2/(R1+R2));

    Serial.println(hesapli_deger);
    Serial.println(yuzde);

    /*lcd.setCursor(12,1);
    lcd.print("%");
    lcd.print(yuzde);*/
    say = 0;

  }
}

void loop() {
 
  if(yuzde >= 0){
    if(x == true){
      calis();
    }   
    else{
      lcd.setCursor(0,0);   
      lcd.print("1 TL ile calisir");
      lcd.setCursor(15,1);
      lcd.write((byte)4);
      lcd.setCursor(0,1);
      lcd.print("Para Atiniz ");
      
      //lcd.setCursor(15,1);
      //lcd.write((byte)6);       
    }
  }

  else{   
    lcd.setCursor(0,0);
    lcd.print("   Dusuk Pil   ");
    lcd.setCursor(15,0);
    //lcd.createChar(3, usb);
    lcd.write((byte)4);
    lcd.setCursor(0,1);
    lcd.print("Sarja Takin  %");
    lcd.print(yuzde); 
  }
 
  if(digitalRead(yukari)){      
    a++;
    delay(10);
    Serial.println(a);
    if(a >= 20){
      a=0;
      b = 1;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("MENU");
      delay(1000);
      menu();
    }
  }
  if(!digitalRead(yukari) && a > 0){    
    a=0; 
  } 
}

void Algilama(){
 // digitalWrite(13,HIGH);
  //if((long)(micros() - sonArkSuresi) >= gecikme * 0){
    //paraSay();
    //sonArkSuresi = micros();
  //} 
  x = true;
  para++;
  EEPROM.write(1,para);
  dakika = dakika + sure;

  if (dakika  >= 60){
    saat++;
    dakika = dakika - 60;
    lcd.clear(); 
  }  
 
  
  if (saniye == 0)
    saniye = 1;   
}

void paraSay(){
  
  

}



void calis(){
  
  lcd.clear();

  while ( ( ( saniye ) > ( 0 ) ) || ( ( dakika ) > ( 0 ) ) || ( ( saat ) > ( 0 ) ) ){
    
    digitalWrite(usb_pin,HIGH);  // sayac başladığında röleyi aktif ediyoruz.
      
    saniye = ( saniye - 1 ) ; // saniye geriye doğru saymaya başlıyor.
    delay( 1000 );  // 1 saniye aralıkla saymasını sağlıyoruz
    
    if(saniye == 9){
      lcd.clear();
    }  

    lcd.setCursor(0,0);
    lcd.print( "Sarj Oluyor..." );

    lcd.setCursor(0,1);
    lcd.print( saat );
    lcd.print( ":" );
      
    lcd.print( dakika );
    lcd.print( ":" );
      
    lcd.print( saniye );

 
      
    if (( ( ( 0 ) < ( dakika ) ) && ( ( 0 ) == ( saniye ) ) )) // saniye sıfır olduğunda dakika sıfırdan büyükse
    {
      saniye = 60 ;  // tekrar saniyeyi 60 yapacak
      dakika = ( dakika - 1 ) ;  // dadikayı bir azaltacak
    }

    if (( ( ( 0 ) == ( saniye ) ) && ( ( ( 0 ) < ( saat ) ) && ( ( 0 ) == ( dakika ) ) ) ))  // saniye ve dakika sıfır olduğu zaman saat sıfırdan büyükse
    {
      saniye = 60 ; // saniyeyi 60 yapacak
      dakika = 59 ; // dakikayı 59 yapacak
      saat = ( saat - 1 ) ; // saati 1 azaltacak
    }
   
  }

  digitalWrite(usb_pin,LOW);
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Sarj Bitti !");
 
  for(int i = 0 ; i < 10 ; i++){
    delay(500);
    lcd.noBacklight();
    delay(500);
    lcd.backlight();
  }
  x = false;    
}
void menu(){

  while(b){
    
    if(!digitalRead(yukari)){
      menusecbutton++;    
      while(!digitalRead(yukari)){
        a++;
        delay(10);

        if(a == 150){
          a=0;
          AltMenu();   
        }

        if(digitalRead(yukari) && a > 0){    
          a=0;
        } 
      }

      menusec();  
    } 
  }
}

void menusec(){

  switch(menusecbutton){

    case 0:
    break;

    case 1:
      
      lcd.setCursor(0,0);
      lcd.print(">Sure Ayar      ");
      lcd.setCursor(0,1);
      lcd.print(" Toplam Para    ");
      altmenubutton = 1;
    break;

    case 2:
     
      lcd.setCursor(0,0);
      lcd.print(" Sure Ayar      ");
      lcd.setCursor(0,1);
      lcd.print(">Toplam Para    ");
      altmenubutton = 2;
    break;

    case 3:
     
      lcd.setCursor(0,0);
      lcd.print(" Toplam Para    ");
      lcd.setCursor(0,1);
      lcd.print(">Pil Durumu     ");
      altmenubutton = 3;
    break;   
    
    case 4:
     
      lcd.setCursor(0,0);
      lcd.print(" Pil Durumu     ");
      lcd.setCursor(0,1);
      lcd.print(">Menuden Cik    ");
      altmenubutton = 4;
      menusecbutton = 0;
    break;   
  } 
}

void AltMenu(){

  switch (altmenubutton){

    case 1 :
      sure = EEPROM.read(0);
      lcd.clear();
      lcd.home();
      lcd.print("Sure Ayari");
      lcd.setCursor(0,1);
      lcd.print(sure);
      lcd.print(" dakika");
      c = 1;

      while(c){
      
        if(!digitalRead(yukari) && digitalRead(asagi)){
          delay(30);

          while(!digitalRead(yukari) && digitalRead(asagi)){
            a++;
            delay(10);
              if(a == 150){
              a=0;
              EEPROM.write(0,sure);
              lcd.clear();                    
              lcd.print("Kaydedildi..."); 
              delay(1000);
              c = 0;
            }  
          }

          sure = sure + 1;         
          lcd.clear();
          lcd.home();
          lcd.print("Sure Ayari");
          lcd.setCursor(0,1);
          lcd.print(sure);
          lcd.print(" dakika");              
        }
     
        else if(!digitalRead(asagi) && digitalRead(yukari)){
          delay(10);
          while(!digitalRead(asagi) && digitalRead(yukari)){
            delay(5);
          }
          sure = sure - 1;
          
          lcd.clear();
          lcd.home();
          lcd.print("Sure Ayari");
          lcd.setCursor(0,1);
          lcd.print(sure);
          lcd.print(" dakika");     
        }

        else if(digitalRead(yukari) && digitalRead(asagi) && a > 0){     
          a=0;
        }
      }
   
    break;

    case 2 :
      lcd.clear();
      lcd.setCursor(2,0);
      lcd.print("Toplam Para=");
      lcd.setCursor(0,1);
      lcd.print(para);
      lcd.print("TL");
      c = true;
      while(c){
        if(!digitalRead(yukari)){
          para = para + 1;
          while(!digitalRead(yukari)){
            Serial.println(a);
            a++;
            delay(10);
            if(a == 150){
              a=0;
              EEPROM.write(0,para);
              lcd.clear();                    
              lcd.print("Kaydedildi..."); 
              delay(1000);
              c = false;
            } 
          }
            
          lcd.setCursor(2,0);                   
          lcd.print("Toplam Para=");
          lcd.setCursor(0,1);
          lcd.print(para);
          lcd.print("TL");
               
        }
     
        else if(!digitalRead(asagi)){
          para = para - 1;
          if(para < 0){
            para = 0;
          }
              
          while(!digitalRead(asagi)){              
            a++;
            delay(10);

            if(a == 100){
              a=0;
              para = 0;
              lcd.clear();
              break;                                   
            }  
          }

          if(para < 10 && para > 8){
            lcd.clear();
          }

          lcd.setCursor(2,0);                    
          lcd.print("Toplam Para=");
          lcd.setCursor(0,1);
          lcd.print(para);
          lcd.print("TL");     
        }

        if(a > 0){             
          a=0;
        }       
      }
    break;

    case 3 :
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Pil Voltaji=");
      lcd.print(hesapli_deger);
      lcd.setCursor(0,1);
      lcd.print("Pil Yuzdesi %");
      lcd.print(yuzde);
      c = true;
      while(c){
        if(!digitalRead(yukari)){
          while(!digitalRead(yukari)){
            a++;
            delay(10);
            if(a == 150){
              a=0;
              c = false;
            } 
          }
        }
      }
    break;

    case 4 :
      lcd.clear();
      lcd.print("Cikiliyor...");
      delay(1500);
      b=0;
      
    break;
  }
}
