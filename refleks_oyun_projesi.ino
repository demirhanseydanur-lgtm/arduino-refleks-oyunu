//KÜTÜPHANELER //

#include <Wire.h>              
// I2C haberleşmesini sağlar.
// LCD ekran Arduino ile bu protokol üzerinden haberleşir.

#include <LiquidCrystal_I2C.h> 
// I2C LCD ekranına yazı yazmamızı sağlar.
// lcd.print(), lcd.clear() gibi komutlar bu kütüphane sayesinde çalışır.


// LCD TANIMI //

LiquidCrystal_I2C lcd(0x27, 16, 2);
// 0x27  -> LCD I2C adresi
// 16    -> 16 sütun
// 2     -> 2 satır


//DONANIM PINLERİ //

int ledPins[] = {2, 3, 4, 5};
// 4 adet LED kullanılıyor.
// Dizi (array) kullanılarak daha düzenli ve kısa kod yazıldı.

int btnPins[] = {6, 7, 8, 9};
// LED’lere karşılık gelen 4 buton
// LED indexi ile buton indexi birebir eşleşir.

int buzzer = 10;
// Doğru ve yanlış durumlarında ses çıkarmak için

int switchPin = 11;
// Oyunu açıp kapatmak için kullanılan sürgülü anahtar
// INPUT_PULLUP olarak kullanılacak


// OYUN DEĞİŞKENLERİ //

int skor = 0;
// Kullanıcının doğru cevap sayısı

int yanlisSayisi = 0;
// Yapılan yanlış sayısı (3 olunca oyun biter)

int rastgeleLED = -1;
// Hangi LED’in yandığını tutar
// -1 = henüz LED seçilmedi


//OYUN HIZI //

int beklemeSuresi = 1500;
// LED’ler arasındaki bekleme süresi (milisaniye)
// Oyun ilerledikçe bu süre azalır ve oyun hızlanır


//ZAMAN ÖLÇÜMÜ//

unsigned long baslamaZamani;
// LED yandığı anın zamanı (millis ile ölçülür)

unsigned long refleksSuresi;
// Kullanıcının tepki süresi (ms cinsinden)


// SETUP FONKSİYONU//

void setup() {

  // LED ve buton pin yönleri ayarlanıyor
  for (int i = 0; i < 4; i++) {
    pinMode(ledPins[i], OUTPUT); // LED’ler çıkış
    pinMode(btnPins[i], INPUT);  // Butonlar giriş
  }

  pinMode(buzzer, OUTPUT);
  // Buzzer ses çıkışı olduğu için OUTPUT

  pinMode(switchPin, INPUT_PULLUP);
  // INPUT_PULLUP:
  // Anahtar kapalı -> HIGH
  // Anahtar açık   -> LOW

  // LCD ekran başlatılıyor
  lcd.init();
  lcd.backlight();
  lcd.print("Refleks Oyunu");
  delay(1500);
}




void loop() {

  /* OYUN KAPALI DURUMU */
  if (digitalRead(switchPin) == HIGH) {

    lcd.clear();
    lcd.print("Oyun Kapali");

    // Oyun sıfırlanır
    skor = 0;
    yanlisSayisi = 0;
    beklemeSuresi = 1500;

    // Tüm LED’ler söndürülür
    for (int i = 0; i < 4; i++) {
      digitalWrite(ledPins[i], LOW);
    }

    noTone(buzzer);
    delay(300);
    return; // loop başına dön
  }


  /*OYUN BİTİŞ DURUMU*/
  if (yanlisSayisi >= 3) {

    lcd.clear();
    lcd.print("Oyun Bitti");
    lcd.setCursor(0, 1);
    lcd.print("Skor: ");
    lcd.print(skor);

    delay(600);
    return;
  }


  /*RASTGELE LED YAKMA */
  rastgeleLED = random(0, 4);
  // 0 ile 3 arasında rastgele sayı üretir

  digitalWrite(ledPins[rastgeleLED], HIGH);
  // Seçilen LED yakılır

  baslamaZamani = millis();
  // LED yandığı an kaydedilir

  lcd.clear();
  lcd.print("LED Yandi");

  int basilanButon = -1;
  // Henüz butona basılmadı


  /*BUTON BEKLEME*/
  while (basilanButon == -1) {

    // Oyun kapatılırsa LED söndür
    if (digitalRead(switchPin) == HIGH) {
      digitalWrite(ledPins[rastgeleLED], LOW);
      return;
    }

    // Tüm butonlar kontrol edilir
    for (int i = 0; i < 4; i++) {
      if (digitalRead(btnPins[i]) == HIGH) {
        basilanButon = i;
        refleksSuresi = millis() - baslamaZamani;
        delay(200); // Buton sıçrama önleme
        break;
      }
    }
  }


  /*LED SÖNDÜR*/
  digitalWrite(ledPins[rastgeleLED], LOW);
  lcd.clear();


  /*DOĞRU BUTON*/
  if (basilanButon == rastgeleLED) {

    skor++;
    tone(buzzer, 880, 150);
    lcd.print("Dogru");

    // Her 3 doğru cevapta oyun hızlanır
    if (skor % 3 == 0 && beklemeSuresi > 500) {
      beklemeSuresi -= 200;
    }
  }

  /*YANLIŞ BUTON*/
  else {

    yanlisSayisi++;
    tone(buzzer, 330, 250);
    lcd.print("Yanlis");
  }


  /*LCD BİLGİ GÖSTERİMİ*/
  lcd.setCursor(0, 1);
  lcd.print(refleksSuresi);
  lcd.print(" ms S:");
  lcd.print(skor);


  /*TUR BEKLEMESİ*/
  delay(beklemeSuresi);
}