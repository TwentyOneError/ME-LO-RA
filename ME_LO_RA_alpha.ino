#include <SPIFFS.h>
#include "Bigint.cpp"
#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <LoRa.h>
#include "sha256.h"
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET     -1
#define BUTTON_PIN 16
#define BATTERY_PIN 35
#define SS_PIN 5
#define RST_PIN 14
#define DIO0_PIN 2
#define PACKET_SIZE 50 // Размер пакетов в байтах
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
const char* ssid = "ME_LO_RA_AP";              //название вашей Wi-Fi сети
const char* password = "password_1";          //пароль от сети



//----------------------------Костыли для исправления ебучего String в arduino---------

//функция определения длины символа UTF-8
int getUtf8CharSize(byte firstByte) {
  if ((firstByte & 0x80) == 0x00) return 1;      // 1-byte ASCII
  if ((firstByte & 0xE0) == 0xC0) return 2;      // 2-byte character
  if ((firstByte & 0xF0) == 0xE0) return 3;      // 3-byte character
  if ((firstByte & 0xF8) == 0xF0) return 4;      // 4-byte character
  return 1; // Default to 1 byte (invalid marker, but safe)
}

//Функция получения количества символов (не байтов)
unsigned int utf8_strlen(const String &str) {
  unsigned int charCount = 0;
  for (unsigned int i = 0; i < str.length();) {
    i += getUtf8CharSize(str[i]);
    charCount++;
  }
  return charCount;
}

//функция получения символа по индексу
String utf8_charAt(const String &str, unsigned int charIndex) {
  unsigned int currentChar = 0;
  unsigned int byteIndex = 0;
  while (byteIndex < str.length() && currentChar <= charIndex) {
    int charSize = getUtf8CharSize(str[byteIndex]);
    if (currentChar == charIndex) {
      return str.substring(byteIndex, byteIndex + charSize);
    }
    byteIndex += charSize;
    currentChar++;
  }
  return String(); //возвращаем пустую строку при неверном индексе
}
// Функция преобразования символьного индекса в байтовый
unsigned int utf8_charIndexToByteIndex(const String &str, unsigned int charIndex) {
  unsigned int currentChar = 0;
  unsigned int byteIndex = 0;
  while (byteIndex < str.length() && currentChar < charIndex) {
    byteIndex += getUtf8CharSize(str[byteIndex]);
    currentChar++;
  }
  return byteIndex;
}

//Функция поиска подстроки с возвратом символьного индекса
int utf8_indexOf(const String &str, const String &substr, unsigned int startCharIndex = 0) {
  // преобразуем стартовый символьный индекс в байтовый
  unsigned int startByteIndex = utf8_charIndexToByteIndex(str, startCharIndex);
  //стандартный поиск в байтах
  int byteIndex = str.indexOf(substr, startByteIndex);
  //подстрока не найдена
  if (byteIndex == -1) return -1;
  //преобразуем байтовый индекс позиции в символьный
  unsigned int charCount = 0;
  unsigned int currentByteIndex = 0;
  while (currentByteIndex < (unsigned int)byteIndex) {
    currentByteIndex += getUtf8CharSize(str[currentByteIndex]);
    charCount++;
  }
  // проверка корректности позиции
  if (currentByteIndex != (unsigned int)byteIndex) {
    // найденная позиция не совпадает с границей символа
    // продолжаем поиск с следующего символа
    return utf8_indexOf(str, substr, charCount + 1);
  }
  return charCount;
}

// функция для проверки корректности UTF-8 символа в позиции
bool isUtf8CharStart(const String &str, unsigned int byteIndex) {
  if (byteIndex >= str.length()) return false;
  // первый байт никогда не является продолжением
  if (byteIndex == 0) return true;
  // проверяем, что предыдущий байт не является частью мультибайтового символа
  byte prev = str[byteIndex - 1];
  return (prev & 0xC0) != 0x80; //не является continuation byte
}

int countOccurrences(const String& text, const String& search) {
  int count = 0;
  int pos = 0;
  while ((pos = text.indexOf(search, pos)) != -1) {
    count++;
    pos += search.length();
  }
  return count;
}
//------------------------------------------------------вернемся к коду----------------





WebServer server(80);                        // Создание сервера на порту 80

                                             // используемый алфавит
String alphabet = "~`o❆☼☀ФТ;💥?☁🌥УН☔🌀🌗🌓♻x⛱PA]🌡{С🌑🤔🍁⛄э⌚еш⚠Ь🌕⛅х¢⛈🌠€ьЖ🦇wЦ}🔎(Ч⏳.2😘G🐻🌤н🦈🕗🐽Ы🐍🐊🐔VЭ🐀 °🌚⭐🌟💣c🐖|🐄й🐷f❎ЮH🦊🌦£🐉XЙ❌uFZ🐕🌧D🐧✉Kо🌪🐈)3[:💀LlЁЯв📈щ^rд⌛s☺✍®ы🗝☝☟М🌩hQз🍿ЛЩОт❗%🍒а☞☹☻😁😂😃😆😇😈😉😊😋😌😍😎😏😐😒🚤😓😔😖😘😚😜😝😞😠😡😢😣😥😨😩😪😫😭😰🌏🍀😱😲😳😵😶😷😸😹😺😻😼😽😾😿🙀🙅🙆🙇🙈🙉🙊🙋🙌🙍🙎✋🐲👀🐝💢☘☐☒✇✎▁▂▃▄▅▆▇█░▒▓𝌴∫∮∲∳∆∇≤≥≈√∀∃∄∅∈∉∋∌∏∐∑✌∞©🐾💋👣✆🚗☠🚀🙏☢☣🚃🚄🚅🚇🚉🚌🚏🚑🚒🚓🚕😄😅🚙🚚🚢JК&8✅9с☜❓яП☑-m,⚡0🤙И💪O🐳W☮n🖤кЪч💰жД🔒jvёa@🥇у😤g§_❤б6фz+🍋З🍬pdT©🛒Nki🍄CХb🍓/RMБ☯>иEB!юe4t☭рГ💕$гI🔱ъмЕл📹<пА🎁=Y*qUВ#📊y1цШ57РS🔥🎃👻";
String alphab = "~`o;?xPA]{w}(.2GVc|fHXuFZDK)3[:Ll^rshQ%J&89-m,0OWnjva@g_6z+pdTNkiCbRM>EB!e4t$I<=Y*qU#y157S";// однобайтовые символы для кодирования (кроме служебных)
String htmlContent = "";
String user_name = "";
BigInt e_exp = 0;
BigInt module = 0;
BigInt d_exp = 0;
BigInt ue_exp = 0;
BigInt umodule = 0;
std::vector<String> aluser;
std::vector<BigInt> almodule;
std::vector<BigInt> ale_exp;
std::vector<std::vector<String>> mess;
std::vector<std::vector<boolean>> sended;
boolean logged = false;
int button = 0;
int lastb = HIGH;
boolean wifib = true;
int charge = 0;
int ssi = 0;
int snrv = 0;
unsigned long mil = 0;
String last_chckm = "";
String last_mess = "";
int last_usid = -1;
unsigned long last_mil = 0;
std::vector<String> last_m;
std::vector<int> last_id;
String incoming_str = "";
unsigned long incoming_mil = 0;





void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(115200);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Дисплей не обнаружен");
  }
  display.clearDisplay();
                                              // Инициализация SPIFFS (пока не используется)
  if (!SPIFFS.begin(true)) {
    Serial.println("Ошибка при монтировании SPIFFS");
  }
  // Включение точки доступа Wi-Fi
  WiFi.softAP(ssid, password);
  Serial.print("Включение Wi-Fi");
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  // Регистрация обработчиков:
  server.on("/styles.css", cssstyle);         // css
  server.on("/", registration);               // Регистрация
  server.onNotFound(registration);            // Обработка неизвестных URL (до регистрации)
  server.on("/registration", HTTP_POST, obr); // получение регистрационных данных, добавление друзей
  server.begin();                             // Запуск сервера
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.println("Log in!");
  display.display();
  display.startscrolldiagright(0x00, 0x0F);
  LoRa.setPins(SS_PIN, RST_PIN, DIO0_PIN);
  if (!LoRa.begin(433E6)) {
    display.stopscroll();
    display.clearDisplay();
    Serial.println("Starting LoRa failed!");
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.println("LoRa Fatal Error!");
    display.display();
    while (1);
  }
  while (logged == false){
    server.handleClient();                    // Обработка входящих запросов
  }
  display.stopscroll();
  display.clearDisplay();
  server.onNotFound(handleNotFound);          // Обработка неизвестных URL
  server.on("/messenger", messenger);         // Главная страница
  server.on("/dialogue", dialogue);           // Диалог
  server.on("/add", HTTP_POST, los);          // Добавление абонентов
  server.on("/settings", HTTP_POST, settings);// Параметры
  Serial.println("HTTP сервер запущен");
  aluser.push_back("ALL");                    // Инициализация данных диалогов
  almodule.push_back(512);
  ale_exp.push_back(1);
  mess.push_back(std::vector<String>());
  sended.push_back(std::vector<boolean>());
  mess[0].push_back("Здесь сообщения без шифрования для всех участников сети!");
  sended[0].push_back(false);
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println("WI-FI AP");
  display.println("is active!");
  LoRa.setTxPower(20);
  LoRa.setSpreadingFactor(12);
  LoRa.setSyncWord(0x21);
  LoRa.setSignalBandwidth(125E3);
  ssi = (LoRa.rssi()+128)/30;
  Serial.println(ssi);
  if (ssi>=4){
    display.fillRect(60, 0, 3, 16, WHITE);
  }else{
    display.drawRect(60, 0, 3, 16, WHITE);
  }
  if (ssi>=3){
    display.fillRect(64, 4, 3, 12, WHITE);
  }else{
    display.drawRect(64, 4, 3, 12, WHITE);
  }
  if (ssi>=2){
    display.fillRect(68, 8, 3, 8, WHITE);
  }else{
    display.drawRect(68, 8, 3, 8, WHITE);
  }
  if (ssi>=1){
    display.fillRect(72, 12, 3, 4, WHITE);
  }else{
    display.drawRect(72, 12, 3, 4, WHITE);
  }
  snrv = LoRa.packetSnr();
  display.setCursor(76, 0);
  display.setTextSize(1);
  display.println(String(snrv));
  display.setCursor(78, 8);
  display.println("SNR");
  Serial.println(analogRead(BATTERY_PIN));
  charge = ((analogRead(BATTERY_PIN)-1648)/146);
  if (charge>5){charge = 5;}
  if (charge==5){
        display.setCursor(90, 0);
        display.setTextSize(2);
        display.println("USB");
      }else{
        if (charge>3){
          display.fillRect(120, 2, 4, 12, WHITE);
        }
        if (charge>2){
          display.fillRect(114, 2, 4, 12, WHITE);
        }
        if (charge>1){
          display.fillRect(108, 2, 4, 12, WHITE);
        }
        if (charge>=1){
          display.fillRect(102, 2, 4, 12, WHITE);
        }
        display.drawRect(100, 0, 25, 16, WHITE);
        display.drawRect(125, 5, 3, 6, WHITE);
      }
  display.display();
  mil = millis();
  LoRa.onReceive(onReceive);
  LoRa.receive();
}


//--------------------------------------------Отправка сообщений--------------------------------------------------
void SendMess(String messag_e, BigInt modules, BigInt e_exps, unsigned int recipient, bool resend = false){
  String cr = "";
  String chckm = "";
  if(resend==false){
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.println("encrypting...");
    display.display();
    display.startscrolldiagright(0x00, 0x0F);
    randomSeed(millis()); 
    chckm = String(random(1000));
    cr = codeRSA(messag_e + "/ " + codeRSA("Y", d_exp, module) + "/ " + user_name, e_exps, modules);// Пока подпись, мягко говоря, слабовата и уязвима для фальсификации, в будущем планирую сделать более хитрую подпись.
    cr = aluser[recipient] + "/ " + chckm + "/ " + cr + "/ " + SHA256::hash(aluser[recipient] + "/ " + chckm + "/ " + cr) + "/ ";
    display.stopscroll();
    display.clearDisplay();
  }else{
    cr = messag_e;
    unsigned int last_z = utf8_indexOf(messag_e,"/ ");
    for(int i = last_z+2; i < utf8_indexOf(messag_e,"/ ",last_z+1);i++){
      chckm += utf8_charAt(messag_e,i);
    }
  }
  Serial.println(cr);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println("sending...");
  display.display();
  display.startscrolldiagright(0x00, 0x0F);
  bool sende = true;
  int c_try = 0;
  while (sende){
    for (int i = 0;i <= utf8_strlen(cr)/PACKET_SIZE; i++){
      while (LoRa.beginPacket() == 0) {
        Serial.print("waiting for radio ... ");
        delay(100);
      }
      LoRa.beginPacket();
      for (int j=0;j < PACKET_SIZE;j++){
        if (i*PACKET_SIZE+j < utf8_strlen(cr)){
          String s_ = utf8_charAt(cr,i*PACKET_SIZE+j);
          if (s_.length() > 1){
            LoRa.print(s_);
          }else{
            LoRa.write(s_[0]);
          }
        }
      }
      LoRa.endPacket();
      delay(500);
    }
    display.stopscroll();
    display.clearDisplay();
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.println("listening...");
    display.display();
    display.startscrolldiagright(0x00, 0x0F);
    mil = millis();
    while (millis()-mil<cr.length()/PACKET_SIZE*1000+11000){
      LoRa.receive();
      if (LoRa.parsePacket()>0){
        Serial.println("Got respond...");
        String s_ = "";
        while (LoRa.available()) {
          s_ += (char)LoRa.read();
        }
        s_ = decodeRSA(s_,e_exps,modules);
        if (s_==chckm){
          Serial.println("success");
          sende = false;
          if (resend){
            last_m.pop_back();
            last_id.pop_back();
          }
        }
      }
      incoming_str = "";
    }
    display.stopscroll();
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    if (c_try == 2){
      if (resend==false){
        last_m.push_back(cr);
        last_id.push_back(recipient);
        last_mil = millis();
      }
      display.stopscroll();
      display.clearDisplay();
      display.setCursor(0, 0);
      display.setTextSize(1);
      mil = 0;
      return;
    }
    display.println("sending...");
    display.display();
    display.startscrolldiagright(0x00, 0x0F);
    c_try++;
  }
  mil = 0;
  display.stopscroll();
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
}

void loop() {
  server.handleClient();                      // Обработка входящих запросов
  button = digitalRead(BUTTON_PIN);
  if (millis()-mil>30000){
    mil = millis();
    if (wifib){
      WiFi.softAP(ssid, password);
      charge = ((analogRead(BATTERY_PIN)-1648)/146);
      ssi = (LoRa.rssi()+128)/30;
      if (charge>5){charge = 5;}
      display.clearDisplay();
      display.setCursor(0, 0);
      display.setTextSize(1);
      display.println("WI-FI AP");
      display.println("is active!");
      if (ssi>=4){//------------------------------RSSI------SIGNAL STRENGTH------------
        display.fillRect(60, 0, 3, 16, WHITE);
      }else{
        display.drawRect(60, 0, 3, 16, WHITE);
      }
      if (ssi>=3){
        display.fillRect(64, 4, 3, 12, WHITE);
      }else{
        display.drawRect(64, 4, 3, 12, WHITE);
      }
      if (ssi>=2){
        display.fillRect(68, 8, 3, 8, WHITE);
      }else{
        display.drawRect(68, 8, 3, 8, WHITE);
      }
      if (ssi>=1){
        display.fillRect(72, 12, 3, 4, WHITE);
      }else{
        display.drawRect(72, 12, 3, 4, WHITE);
      }
      snrv = LoRa.packetSnr();
      display.setCursor(76, 0);
      display.setTextSize(1);
      display.println(String(snrv));
      display.setCursor(78, 8);
      display.println("SNR");
      if (charge==5){//-----------------------BATTERY CHARGE LEVEL-----------------------------
        display.setCursor(90, 0);
        display.setTextSize(2);
        display.println("USB");
      }else{
        if (charge>3){
          display.fillRect(120, 2, 4, 12, WHITE);
        }
        if (charge>2){
          display.fillRect(114, 2, 4, 12, WHITE);
        }
        if (charge>1){
          display.fillRect(108, 2, 4, 12, WHITE);
        }
        if (charge>=1){
          display.fillRect(102, 2, 4, 12, WHITE);
        }
        display.drawRect(100, 0, 25, 16, WHITE);
        display.drawRect(125, 5, 3, 6, WHITE);
      }
      display.display();
    }else{
      WiFi.mode(WIFI_OFF);
      charge = ((analogRead(BATTERY_PIN)-1648)/146);
      ssi = (LoRa.rssi()+128)/30;
      if (charge>5){charge = 5;}
      display.clearDisplay();
      display.setCursor(0, 0);
      display.setTextSize(1);
      display.println("WI-FI OFF");
      if (ssi>=4){//-------------------------RSSI------SIGNAL STRENGTH------------------
        display.fillRect(60, 0, 3, 16, WHITE);
      }else{
        display.drawRect(60, 0, 3, 16, WHITE);
      }
      if (ssi>=3){
        display.fillRect(64, 4, 3, 12, WHITE);
      }else{
        display.drawRect(64, 4, 3, 12, WHITE);
      }
      if (ssi>=2){
        display.fillRect(68, 8, 3, 8, WHITE);
      }else{
        display.drawRect(68, 8, 3, 8, WHITE);
      }
      if (ssi>=1){
        display.fillRect(72, 12, 3, 4, WHITE);
      }else{
        display.drawRect(72, 12, 3, 4, WHITE);
      }
      snrv = LoRa.packetSnr();
      display.setCursor(76, 0);
      display.setTextSize(1);
      display.println(String(snrv));
      display.setCursor(78, 8);
      display.println("SNR");
      if (charge==5){//-----------------------BATTERY CHARGE LEVEL-----------------------------
        display.setCursor(90, 0);
        display.setTextSize(2);
        display.println("USB");
      }else{
        if (charge>3){
          display.fillRect(120, 2, 4, 12, WHITE);
        }
        if (charge>2){
          display.fillRect(114, 2, 4, 12, WHITE);
        }
        if (charge>1){
          display.fillRect(108, 2, 4, 12, WHITE);
        }
        if (charge>=1){
          display.fillRect(102, 2, 4, 12, WHITE);
        }
        display.drawRect(100, 0, 25, 16, WHITE);
        display.drawRect(125, 5, 3, 6, WHITE);
      }
      display.display();
    }
    if ((millis()-last_mil>300000)and(last_mil>0)){
      SendMess(last_m[last_m.size()-1],512,1,last_id[last_m.size()-1],true);
      last_mil = millis();
    }
  }
  if(lastb == LOW && button == HIGH){
    mil = millis();
    wifib = !wifib;
    if (wifib){
      WiFi.softAP(ssid, password);
      charge = ((analogRead(BATTERY_PIN)-1648)/146);
      ssi = (LoRa.rssi()+128)/30;
      if (charge>5){charge = 5;}
      display.clearDisplay();
      display.setCursor(0, 0);
      display.setTextSize(1);
      display.println("WI-FI AP");
      display.println("is active!");
      if (ssi>=4){//------------------------------RSSI------SIGNAL STRENGTH------------
        display.fillRect(60, 0, 3, 16, WHITE);
      }else{
        display.drawRect(60, 0, 3, 16, WHITE);
      }
      if (ssi>=3){
        display.fillRect(64, 4, 3, 12, WHITE);
      }else{
        display.drawRect(64, 4, 3, 12, WHITE);
      }
      if (ssi>=2){
        display.fillRect(68, 8, 3, 8, WHITE);
      }else{
        display.drawRect(68, 8, 3, 8, WHITE);
      }
      if (ssi>=1){
        display.fillRect(72, 12, 3, 4, WHITE);
      }else{
        display.drawRect(72, 12, 3, 4, WHITE);
      }
      snrv = LoRa.packetSnr();
      display.setCursor(76, 0);
      display.setTextSize(1);
      display.println(String(snrv));
      display.setCursor(78, 8);
      display.println("SNR");
      if (charge==5){//-----------------------BATTERY CHARGE LEVEL-----------------------------
        display.setCursor(90, 0);
        display.setTextSize(2);
        display.println("USB");
      }else{
        if (charge>3){
          display.fillRect(120, 2, 4, 12, WHITE);
        }
        if (charge>2){
          display.fillRect(114, 2, 4, 12, WHITE);
        }
        if (charge>1){
          display.fillRect(108, 2, 4, 12, WHITE);
        }
        if (charge>=1){
          display.fillRect(102, 2, 4, 12, WHITE);
        }
        display.drawRect(100, 0, 25, 16, WHITE);
        display.drawRect(125, 5, 3, 6, WHITE);
      }
      display.display();
    }else{
      WiFi.mode(WIFI_OFF);
      charge = ((analogRead(BATTERY_PIN)-1648)/146);
      ssi = (LoRa.rssi()+128)/30;
      if (charge>5){charge = 5;}
      display.clearDisplay();
      display.setCursor(0, 0);
      display.setTextSize(1);
      display.println("WI-FI OFF");
      if (ssi>=4){//-------------------------RSSI------SIGNAL STRENGTH------------------
        display.fillRect(60, 0, 3, 16, WHITE);
      }else{
        display.drawRect(60, 0, 3, 16, WHITE);
      }
      if (ssi>=3){
        display.fillRect(64, 4, 3, 12, WHITE);
      }else{
        display.drawRect(64, 4, 3, 12, WHITE);
      }
      if (ssi>=2){
        display.fillRect(68, 8, 3, 8, WHITE);
      }else{
        display.drawRect(68, 8, 3, 8, WHITE);
      }
      if (ssi>=1){
        display.fillRect(72, 12, 3, 4, WHITE);
      }else{
        display.drawRect(72, 12, 3, 4, WHITE);
      }
      snrv = LoRa.packetSnr();
      display.setCursor(76, 0);
      display.setTextSize(1);
      display.println(String(snrv));
      display.setCursor(78, 8);
      display.println("SNR");
      if (charge==5){//-----------------------BATTERY CHARGE LEVEL-----------------------------
        display.setCursor(90, 0);
        display.setTextSize(2);
        display.println("USB");
      }else{
        if (charge>3){
          display.fillRect(120, 2, 4, 12, WHITE);
        }
        if (charge>2){
          display.fillRect(114, 2, 4, 12, WHITE);
        }
        if (charge>1){
          display.fillRect(108, 2, 4, 12, WHITE);
        }
        if (charge>=1){
          display.fillRect(102, 2, 4, 12, WHITE);
        }
        display.drawRect(100, 0, 25, 16, WHITE);
        display.drawRect(125, 5, 3, 6, WHITE);
      }
      display.display();
    }
  }
  lastb = button;
  LoRa.receive();
  if ((incoming_str.length()>0)and(millis()-incoming_mil>5000)){
    checkIncoming(incoming_str);
    incoming_str = "";
    incoming_mil = millis();
  }
}


void onReceive(int packetSize){//Обработка получаемых пакетов
  if (packetSize == 0) return;
  String incoming = "";
  while (LoRa.available()) {
    incoming += (char)LoRa.read();
  }
  incoming_str += incoming;
  incoming_mil = millis();
  Serial.print("recieved 1 packet with size:");
  Serial.println(packetSize);
}


void checkIncoming(String incoming){// Обработка полученного сообщения
  Serial.println("check...");
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println("Recieving?");
  display.display();
  display.startscrolldiagright(0x00, 0x0F);
  Serial.println(incoming);
  if (utf8_indexOf(incoming,user_name)>=0){
    Serial.println("found for me");
    if (countOccurrences(incoming,"/ ")<4){
      if (countOccurrences(incoming,"/ ")==0){
        if (last_usid > -1){
          if (decodeRSA(incoming,ale_exp[last_usid],almodule[last_usid])==last_chckm){
            while (LoRa.beginPacket() == 0) {
              Serial.print("waiting for radio ... ");
              delay(100);
            }
            LoRa.beginPacket();
            LoRa.print(incoming);
            LoRa.endPacket(true);
          }
        }
      }else{
        Serial.println("bad message (not full)");
        display.stopscroll();
        display.clearDisplay();
        display.setCursor(0, 0);
        display.setTextSize(1);
        return;
      }
    }else{
      Serial.print("Correct syntax");
      unsigned int last_z = utf8_indexOf(incoming,"/ ");
      String chckm = "";
      for(int i = last_z+2; i < utf8_indexOf(incoming,"/ ",last_z+1);i++){
        chckm += utf8_charAt(incoming,i);
      }
      String cr = "";
      last_z = utf8_indexOf(incoming,"/ ",last_z+1);
      for(int i = last_z+2; i < utf8_indexOf(incoming,"/ ",last_z+1);i++){
        cr += utf8_charAt(incoming,i);
      }
      String summ256 = "";
      last_z = utf8_indexOf(incoming,"/ ",last_z+1);
      for(int i = last_z+2; i < utf8_indexOf(incoming,"/ ",last_z+1);i++){
        summ256 += utf8_charAt(incoming,i);
      }
      Serial.println("checking summ");
      if (SHA256::hash(user_name + "/ " + chckm + "/ " + cr) == summ256){
        Serial.print("Summ ok");
        while (LoRa.beginPacket() == 0) {
          Serial.print("waiting for radio ... ");
          delay(100);
        }
        String confirm = codeRSA(chckm,d_exp,module);
        for(int i = 0;i<3;i++){
          LoRa.beginPacket();
          LoRa.print(confirm);
          LoRa.endPacket();
          delay(500);
        }
        if (last_chckm == chckm){
          Serial.println("message dublicate");
        }else{
          last_chckm = chckm;
          last_mess = "";
          last_usid = -1;
          String cr_ = decodeRSA(cr,d_exp,module) + "/ ";
          last_z = utf8_indexOf(cr_,"/ ");
          String messag_e = "";
          for(int i = 0; i < last_z; i++){
            messag_e += utf8_charAt(cr_,i);
          }
          String Y_ = "";
          for(int i = last_z+2; i < utf8_indexOf(cr_,"/ ",last_z+1); i++){
            Y_ += utf8_charAt(cr_,i);
          }
          last_z = utf8_indexOf(cr_,"/ ",last_z+1);
          String sender = "";
          for(int i = last_z+2; i < utf8_indexOf(cr_,"/ ",last_z+1); i++){
            sender += utf8_charAt(cr_,i);
          }
          int u = -1;
          for (int i = 0; i < aluser.size(); i++) {
            if (aluser[i] == sender) {
              u = i;
              break;
            }
          }
          if (u == -1) {
            display.stopscroll();
            display.clearDisplay();
            display.setCursor(0, 0);
            display.setTextSize(1);
            return;
          }else{
            if (decodeRSA(Y_,ale_exp[u],almodule[u])=="Y"){
              Serial.print("Message verification ok");
              mess[u].push_back(messag_e);
              sended[u].push_back(false);
            }
          }
        }
      }
    }
  }else if((utf8_indexOf(incoming,"ALL")>=0)and(countOccurrences(incoming,"/ ")==3)){
    unsigned int last_z = utf8_indexOf(incoming,"/ ");
    String messag_e = "";
    for(int i = last_z+2; i < utf8_indexOf(incoming,"/ ",last_z+1);i++){
      messag_e += utf8_charAt(incoming,i);
    }
    last_z = utf8_indexOf(incoming,"/ ",last_z+1);
    String summ256 = "";
    for(int i = last_z+2; i < utf8_indexOf(incoming,"/ ",last_z+1);i++){
      summ256 += utf8_charAt(incoming,i);
    }
    if(SHA256::hash(messag_e) == summ256){
      int u_u = mess[0].size();
      bool flg = false;
      for (int i = u_u-1;i>-1;i--){
        if(mess[0][i]==messag_e){
          flg = true;
          break;
        }
      }
      if(flg==false){
        mess[0].push_back(messag_e);
        sended[0].push_back(false);
        allsen(messag_e);
      }
    }
  }else if(countOccurrences(incoming,"/ ")<4){
    String recipient  = "";
    for(int i = 0; i < utf8_indexOf(incoming,"/ ");i++){
      recipient += utf8_charAt(incoming,i);
    }
    int u = -1;
    for (int i = 0; i < aluser.size(); i++) {
      if (aluser[i] == recipient) {
        u = i;
        break;
      }
    }
    if (u == -1) {
      Serial.println("undefined user");
      display.stopscroll();
      display.clearDisplay();
      display.setCursor(0, 0);
      display.setTextSize(1);
      return;
    }else{
      unsigned int last_z = utf8_indexOf(incoming,"/ ");
      String chckm = "";
      for(int i = last_z+2; i < utf8_indexOf(incoming,"/ ",last_z+1);i++){
        chckm += utf8_charAt(incoming,i);
      }
      String cr = "";
      last_z = utf8_indexOf(incoming,"/ ",last_z+1);
      for(int i = last_z+2; i < utf8_indexOf(incoming,"/ ",last_z+1);i++){
        cr += utf8_charAt(incoming,i);
      }
      String summ256 = "";
      last_z = utf8_indexOf(incoming,"/ ",last_z+1);
      for(int i = last_z+2; i < utf8_indexOf(incoming,"/ ",last_z+1);i++){
        summ256 += utf8_charAt(incoming,i);
      }
      if (SHA256::hash(recipient + "/ " + chckm + "/ " + cr) == summ256){
        last_chckm = chckm;
        last_mess = incoming;
        last_usid = u;
        for (int i = 0;i <= utf8_strlen(incoming)/PACKET_SIZE; i++){
          while (LoRa.beginPacket() == 0) {
            Serial.print("waiting for radio ... ");
            server.handleClient();
            delay(100);
          }
          LoRa.beginPacket();
          for (int j=0;j < PACKET_SIZE;j++){
            if (i*PACKET_SIZE+j < utf8_strlen(incoming)){
              String s_ = utf8_charAt(incoming,i*PACKET_SIZE+j);
              if (s_.length() == 2){
                LoRa.write(s_[0]);
                LoRa.write(s_[1]);
              }else{
                LoRa.write(s_[0]);
              }
            }
          }
          LoRa.endPacket(true);
        }
      }
    }
  }
  display.stopscroll();
  display.clearDisplay();
  mil = 0;
}





void allsen(String cr){// --------------------------широковещательные сообщения без шифровния.
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println("sending...");
  display.display();
  display.startscrolldiagright(0x00, 0x0F);
  cr = "ALL/ " + cr + "/ " + SHA256::hash(cr);
  for (int i_ = 0;i_ < 3;i_++){ 
    for (int i = 0;i <= utf8_strlen(cr)/PACKET_SIZE; i++){
      while (LoRa.beginPacket() == 0) {
        Serial.print("waiting for radio ... ");
        delay(100);
      }
      LoRa.beginPacket();
      for (int j=0;j < PACKET_SIZE;j++){
        if (i*PACKET_SIZE+j < utf8_strlen(cr)){
          String s_ = utf8_charAt(cr,i*PACKET_SIZE+j);
          if (s_.length() > 1){
            LoRa.print(s_);
          }else{
            LoRa.write(s_[0]);
          }
        }
      }
      LoRa.endPacket();
      delay(500);
    }
  }
  display.stopscroll();
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
}


BigInt mod_exp(BigInt base, BigInt exponent, BigInt mod) {// Быстрое возведение в степень по модулю.
    if (mod == 0) {
        return 0;
    }
    if (mod == 1) {
        return 0;
    }
    BigInt result = 1;
    base = base % mod;
    while (exponent > 0) {
        if (exponent % 2 == 1) {
            result = (result * base) % mod;
        }
        base = (base * base) % mod;
        exponent = exponent / 2;
    }
    return result;
}

BigInt powe(BigInt base, int expo){//возведение в степень для больших чисел.
  BigInt base_ = base;
  if (expo==0){
    base = 1;
  }
  while (expo>1){
    base = base * base_;
    expo = expo - 1; 
  }
  return base;
}
//------------------------------------------------------------------RSA cryptography------------------------------------------
String decodeRSA(String str, BigInt d, BigInt n){
  int st = str.length();
  String str_ = "";
  String s_ = "";
  for (int i = 0; i<st; i++){
    if (utf8_charAt(str,i)[0]==' '){
      String _ = mod_exp(unzipn(s_),d,n).toString();
      if(_.length()>3){
        int i_ = _.toInt();
        str_ = str_ + utf8_charAt(alphabet, i_/1000);
        str_ = str_ + utf8_charAt(alphabet, i_%1000);
        s_ = "";
      }else{
        int i_ = _.toInt();
        str_=str_ + utf8_charAt(alphabet, i_);
        s_ = "";
      }
    }else{
      s_=s_ + str[i];
    }
  }
  return str_;
}

String codeRSA(String str, BigInt e, BigInt n){
  int st = utf8_strlen(str);
  int st_ = st;
  st = st / 2 + st % 2;
  String str_ = "";
  String s_ = "";
  for (int i = 0; i<st; i++){
    int u_i = utf8_indexOf(alphabet,utf8_charAt(str,i*2));
    if (i*2+1<st_){
      int u_i_ = utf8_indexOf(alphabet,utf8_charAt(str,i*2+1));
      if(u_i_ > -1){
        str_ = str_ + zipn(mod_exp(u_i*1000+u_i_,e,n)) + " ";
      }else{
        if(u_i > -1){
          str_ = str_ + zipn(mod_exp(u_i,e,n)) + " ";
        }
      }
    }else{
      if(u_i > -1){
        str_ = str_ + zipn(mod_exp(u_i,e,n)) + " ";
      }
    }
    
  }
  return str_;
}

//------------------------------кодирование чисел в символы (для экономии передаваемых данных).
String zipn(BigInt bui){
  String s_ = "";
  while (bui > 0){
    BigInt ui = bui % (alphab.length());
    String _ = ui.toString();//-----------------------здесь используются только строки в один байт в целях оптимизации
    int uii = _.toInt();
    s_ = alphab[uii] + s_;
    bui = bui / (alphab.length());
  }
  return s_;
}

BigInt unzipn(String str){
  BigInt bui = 0;
  for(int i = 0; i < str.length();i++){
    BigInt ui = alphab.indexOf(str[i]);//-------------здесь тоже-----
    bui = bui + ui*powe(alphab.length(),str.length()-1-i);
  }
  return bui;
}


//--------------------------WEB-UI---------------------------------------------------------------------------------------

void dialogue() {
  int luser_id = server.arg("dialog_id").toInt();
  int se = server.arg("sended").toInt();
  if (se==1){
    String textm = server.arg("text");
    if(luser_id > 0){
      mess[luser_id].push_back(textm);
      sended[luser_id].push_back(true);
      SendMess(textm, almodule[luser_id],ale_exp[luser_id],luser_id);
    }else{
      mess[luser_id].push_back(textm);
      sended[luser_id].push_back(true);
      allsen(textm);
    }
  }
  String html = R"rawliteral(
  <!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ME-LO-RA Chat</title>
    <link rel="stylesheet" href="styles.css">
</head>
<body>
    <div class="scanline"></div>
    
    <div class="container">
        <div class="cyber-navbar">
            <form method="post" action="/messenger">
                <button type="submit" class="cyber-button">←</button>
            </form>
            <div class="name neon-text">)rawliteral" + aluser[luser_id] + R"rawliteral(</div>
          <div class="logo neon-text">///ME-LO-RA///</div>
        </div>
        
        <!-- Область сообщений -->
        <div class="messages-container" id="messages">
            <!-- Сообщения будут добавляться здесь -->)rawliteral";
  for (int i = 0;i < mess[luser_id].size();i++){
    if (sended[luser_id][i]){
      html = html + R"rawliteral(<div class="message sent">)rawliteral" + mess[luser_id][i] + R"rawliteral(</div>)rawliteral";
    } else {
      html = html + R"rawliteral(<div class="message received">)rawliteral" + mess[luser_id][i] + R"rawliteral(</div>)rawliteral";
    }
  }
  html = html + R"rawliteral(</div>
    <div class="input-area" style="padding:20px;">
            <form method="post" action="/dialogue">
                <div style="display:flex;gap:10px;">
                    <input 
                        name="text"
                        type="text" 
                        class="cyber-input" 
                        id="messageInput" 
                        placeholder="ВВЕДИТЕ СООБЩЕНИЕ..."
                        style="flex:1;"
                    >
                    <input type="hidden" name="dialog_id" value=")rawliteral" + String(luser_id) + R"rawliteral(">
                    <input type="hidden" name="sended" value="1">
                    <button type="submit" class="cyber-button">➤</button>
                </div>
            </form>
        </div>
    </div>
    
    <script>
        document.addEventListener('DOMContentLoaded', function() {
            const messagesContainer = document.getElementById('messages');
            messagesContainer.scrollTop = messagesContainer.scrollHeight;
            
            const messageForm = document.querySelector('.input-area form');
            const messageInput = document.getElementById('messageInput');
            
            // Add glitch effect to all neon text periodically
            setInterval(() => {
                document.querySelectorAll('.neon-text').forEach(el => {
                    el.style.animation = 'none';
                    setTimeout(() => {
                        el.style.animation = 'glitch 12s infinite';
                    }, 10);
                });
            }, 15000);
        });
    </script>
</body>
</html>)rawliteral";

   server.send(200, "text/html", html);
}


void obr() {
  user_name = server.arg("user_name");
  Serial.println(user_name);
  String e_exp_str = server.arg("e_exp");
  e_exp = BigInt(e_exp_str);
  Serial.println(e_exp.toString());
  String module_str = server.arg("module");
  module = BigInt(module_str);
  Serial.println(module.toString());
  String d_exp_str = server.arg("d_exp");
  d_exp = BigInt(d_exp_str);
  Serial.println(d_exp.toString());
  logged = true;
  String html = R"rawliteral(
                <!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ME-LO-RA Panel</title>
    <link rel="stylesheet" href="styles.css">
</head>
<body>
    <div class="scanline"></div>
    
    <div class="container">
        <div class="cyber-header">
            <h1 class="neon-text">///ME-LO-RA///</h1>
            <h2 class="neon-pink">ДОБАВЛЕНИЕ ЭНТУЗИАСТОВ</h2>
        </div>
        
        <div class="form-container">
            <div class="cyber-panel pink-glow">
                <form name="testl" method="post" action="/add">
                    <div class="form-row">
                        <p>ПСЕВДОНИМ:</p>
                        <input class="cyber-input" name="l_name" pattern="^[a-zA-Z\s]+$" value="SOMEBODY">
                    </div>
                    
                    <div class="form-row">
                        <p>ОТКРЫТАЯ ЭКСПОНЕНТА (e):</p>
                        <input class="cyber-input" name="ue_exp" pattern="^[ 0-9]+$" value="706571603">
                    </div>
                    
                    <div class="form-row">
                        <p>МОДУЛЬ (n):</p>
                        <input class="cyber-input" name="umodule" pattern="^[ 0-9]+$" value="541432178183809597">
                    </div>
                    
                    <div class="text-center grid-3">
                        <button type="submit" class="cyber-button">ДОБАВИТЬ</button>
                        <a href="/messenger" class="cyber-button pink">ВОЙТИ В СЕТЬ</a>
                    </div>
                </form>
            </div>
        </div>
    </div>
    
    <script>
        document.addEventListener('DOMContentLoaded', function() {
            // Add glitch effect to all neon text periodically
            setInterval(() => {
                document.querySelectorAll('.neon-text, .neon-pink').forEach(el => {
                    el.style.animation = 'none';
                    setTimeout(() => {
                        el.style.animation = 'glitch 12s infinite';
                    }, 10);
                });
            }, 15000);
            
            // Form input focus effect
            document.querySelectorAll('.cyber-input').forEach(input => {
                input.addEventListener('focus', function() {
                    this.style.boxShadow = '0 0 20px var(--neon-pink)';
                });
                
                input.addEventListener('blur', function() {
                    this.style.boxShadow = '';
                });
            });
        });
    </script>
</body>
</html>)rawliteral";
  server.send(200, "text/html", html);
}




void los() {
  String uuser_name = server.arg("l_name");
  if (uuser_name==""){
    Serial.println("null user");
  }else{
    Serial.println(uuser_name);
    String ue_exp_str = server.arg("ue_exp");
    ue_exp = BigInt(ue_exp_str);
    Serial.println(ue_exp.toString());
    String umodule_str = server.arg("umodule");
    umodule = BigInt(umodule_str);
    Serial.println(umodule.toString());
    int u = -1;
    Serial.println(aluser.size());
    for (int i = 0; i < aluser.size(); i++) {
      if (aluser[i] == uuser_name) {
        u = i;
        break;
      }
    }
    if (u > -1) {
      almodule[u] = umodule;
      ale_exp[u] = ue_exp;
    } else {
      aluser.push_back(uuser_name);
      almodule.push_back(umodule);
      ale_exp.push_back(ue_exp);
      mess.push_back(std::vector<String>());
      mess[mess.size()-1].push_back("Привет, теперь я здесь!");
      sended.push_back(std::vector<boolean>());
      sended[sended.size()-1].push_back(false);
    }
  }
  String html = R"rawliteral(
                <!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ME-LO-RA Panel</title>
    <link rel="stylesheet" href="styles.css">
</head>
<body>
    <div class="scanline"></div>
    
    <div class="container">
        <div class="cyber-header">
            <h1 class="neon-text">///ME-LO-RA///</h1>
            <h2 class="neon-pink">ДОБАВЛЕНИЕ ЭНТУЗИАСТОВ</h2>
        </div>
        
        <div class="form-container">
            <div class="cyber-panel pink-glow">
                <form name="testl" method="post" action="/add">
                    <div class="form-row">
                        <p>ПСЕВДОНИМ:</p>
                        <input class="cyber-input" name="l_name" pattern="^[a-zA-Z\s]+$" value="SOMEBODY">
                    </div>
                    
                    <div class="form-row">
                        <p>ОТКРЫТАЯ ЭКСПОНЕНТА (e):</p>
                        <input class="cyber-input" name="ue_exp" pattern="^[ 0-9]+$" value="706571603">
                    </div>
                    
                    <div class="form-row">
                        <p>МОДУЛЬ (n):</p>
                        <input class="cyber-input" name="umodule" pattern="^[ 0-9]+$" value="541432178183809597">
                    </div>
                    
                    <div class="text-center grid-3">
                        <button type="submit" class="cyber-button">ДОБАВИТЬ</button>
                        <a href="/messenger" class="cyber-button pink">ВОЙТИ В СЕТЬ</a>
                    </div>
                </form>
            </div>
        </div>
    </div>
    
    <script>
        document.addEventListener('DOMContentLoaded', function() {
            // Add glitch effect to all neon text periodically
            setInterval(() => {
                document.querySelectorAll('.neon-text, .neon-pink').forEach(el => {
                    el.style.animation = 'none';
                    setTimeout(() => {
                        el.style.animation = 'glitch 12s infinite';
                    }, 10);
                });
            }, 15000);
            
            // Form input focus effect
            document.querySelectorAll('.cyber-input').forEach(input => {
                input.addEventListener('focus', function() {
                    this.style.boxShadow = '0 0 20px var(--neon-pink)';
                });
                
                input.addEventListener('blur', function() {
                    this.style.boxShadow = '';
                });
            });
        });
    </script>
</body>
</html>)rawliteral";
  server.send(200, "text/html", html);
}

void messenger() {
  String html_1 = R"rawliteral(
              <!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ME-LO-RA Network</title>
    <link rel="stylesheet" href="styles.css">
</head>
<body>
    <div class="scanline"></div>
    <div class="container">
        <div class="cyber-header">
            <h1 class="neon-text">///ME-LO-RA///</h1>
            <h2 class="neon-pink">СЕТЬ</h2>
        </div>
        
        <main>
            <div class="dialogs-container">
  )rawliteral";
  String html_2 = "";
  for ( int i = 0; i < aluser.size(); i++) {
    html_2 = html_2 + R"rawliteral(<form method="post" action="/dialogue"><input type="hidden" name="sended" value="0"><input type="hidden" name="dialog_id" value=")rawliteral" + String(i) + R"rawliteral(">
                            <div class="dialog-item" onclick="this.closest('form').submit()">
                        <div class="dialog-name">)rawliteral" + aluser[i] + R"rawliteral(</div>
                        <div class="dialog-message">)rawliteral" + mess[i][mess[i].size() - 1] + R"rawliteral(</div></div></form>)rawliteral";
  }
  //  String html_2 = R"rawliteral(
  //                        <form method="post" action="/open_dialog">
  //                            <input type="hidden" name="dialog_id" value="1">
  //                            <div class="dialog" onclick="this.closest('form').submit()">
  //                                <div class="dialog-name">Алексей Петров</div>
  //                                <div class="dialog-message">Привет! Как твои дела? Я тут новую фишку в проекте придумал, надо обсудить...</div>
  //                            </div>
  //                        </form>
  //  )rawliteral"; Пример блока диалога------------------------------------------------------------------------------------------------------------------------

  String html_3 = R"rawliteral(
                      </div>
                    </main>
                    <!-- Нижнее меню с кнопками -->
                    <div class="cyber-footer">
            <form method="post" action="/add">
                <button type="submit" class="cyber-button">ДОБАВИТЬ ЭНТУЗИАСТОВ</button>
            </form>
            
            <form method="post" action="/">
                <button type="submit" class="cyber-button pink">РАЗЛОГИНИТЬСЯ</button>
            </form>
            
            <form method="post" action="/settings">
                <button type="submit" class="cyber-button">НАСТРОЙКИ</button>
            </form>
        </div>
    </div>
    
    <script>
        document.addEventListener('DOMContentLoaded', function() {
            // Add glitch effect to all neon text periodically
            setInterval(() => {
                document.querySelectorAll('.neon-text, .neon-pink').forEach(el => {
                    el.style.animation = 'none';
                    setTimeout(() => {
                        el.style.animation = 'glitch 12s infinite';
                    }, 10);
                });
            }, 15000);
            
            // Dialog item click effect
            document.querySelectorAll('.dialog-item').forEach(item => {
                item.addEventListener('click', function() {
                    this.style.boxShadow = '0 0 30px var(--neon-pink)';
                    this.style.transform = 'scale(1.02)';
                    setTimeout(() => {
                        this.style.boxShadow = '';
                        this.style.transform = '';
                    }, 500);
                });
            });
        });
    </script>
</body>
</html>
  )rawliteral";
  String html = html_1 + html_2 + html_3;

  server.send(200, "text/html", html);
}

void settings() {
  String html = R"rawliteral(
  <!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ME-LO-RA Настройки</title>
    <link rel="stylesheet" href="styles.css">
    <style>
        /* Дополнительные стили для страницы настроек */
        .stats-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
            gap: 20px;
            margin-bottom: 30px;
        }
        
        .stat-item {
            background: linear-gradient(to right, var(--darker-bg), rgba(15, 25, 50, 0.7));
            border: 1px solid var(--neon-blue);
            padding: 15px;
            border-radius: 4px;
            position: relative;
            overflow: hidden;
            box-shadow: var(--panel-glow);
        }
        
        .stat-item::before {
            content: "";
            position: absolute;
            top: 0;
            left: 0;
            width: 5px;
            height: 100%;
            background: linear-gradient(to bottom, var(--neon-blue), var(--neon-purple));
        }
        
        .stat-label {
            color: var(--neon-blue);
            text-shadow: var(--text-glow);
            font-size: 0.9rem;
            margin-bottom: 5px;
            text-transform: uppercase;
            letter-spacing: 1px;
        }
        
        .stat-value {
            color: #fff;
            font-size: 1.4rem;
            text-shadow: var(--text-glow);
        }
        
        .data-section {
            margin-top: 30px;
        }
        
        .data-toggle {
            background: linear-gradient(45deg, var(--darker-bg), #0f1a2a);
            border: 1px solid var(--neon-pink);
            color: var(--neon-pink);
            padding: 12px 24px;
            font-size: 16px;
            cursor: pointer;
            text-shadow: var(--text-pink-glow);
            box-shadow: var(--panel-pink-glow);
            width: 100%;
            text-align: left;
            position: relative;
            transition: all 0.3s ease;
            margin-bottom: 10px;
        }
        
        .data-toggle::after {
            content: "▼";
            position: absolute;
            right: 15px;
            transition: transform 0.3s ease;
        }
        
        .data-toggle.active::after {
            transform: rotate(180deg);
        }
        
        .data-content {
            display: none;
            background-color: rgba(5, 10, 20, 0.7);
            border: 1px solid var(--neon-purple);
            padding: 15px;
            border-radius: 4px;
            box-shadow: inset 0 0 20px rgba(160, 0, 255, 0.3);
            max-height: 300px;
            overflow-y: auto;
        }
        
        .data-content.active {
            display: block;
            animation: fadeIn 0.5s ease;
        }
        
        .data-text {
            color: #a0a0ff;
            font-family: 'Courier New', monospace;
            line-height: 1.5;
            white-space: pre-wrap;
        }
        
        .progress-bar {
            height: 10px;
            background-color: rgba(5, 10, 20, 0.7);
            border-radius: 5px;
            margin-top: 10px;
            overflow: hidden;
            position: relative;
        }
        
        .progress-fill {
            height: 100%;
            background: linear-gradient(to right, var(--neon-blue), var(--neon-purple));
            border-radius: 5px;
            width: 65%; /* Значение для примера */
            position: relative;
        }
        
        .progress-fill::after {
            content: "";
            position: absolute;
            top: 0;
            left: 0;
            right: 0;
            bottom: 0;
            background: linear-gradient(90deg, 
                transparent, 
                rgba(255, 255, 255, 0.4), 
                transparent);
            animation: progressShine 2s infinite;
        }
        
        @keyframes progressShine {
            0% { transform: translateX(-100%); }
            100% { transform: translateX(100%); }
        }
        
        /* Анимация изменения значений */
        @keyframes valueChange {
            0% { color: #fff; text-shadow: var(--text-glow); }
            50% { color: var(--neon-pink); text-shadow: var(--text-pink-glow); }
            100% { color: #fff; text-shadow: var(--text-glow); }
        }
        
        .value-updated {
            animation: valueChange 1s ease;
        }
    </style>
</head>
<body>
    <div class="scanline"></div>
    <div class="container">
        <div class="cyber-header">
            <h1 class="neon-text">///ME-LO-RA///</h1>
            <h2 class="neon-pink">СИСТЕМНЫЕ НАСТРОЙКИ</h2>
        </div>
        <div class="cyber-panel">
            <h3 class="neon-text" style="margin-bottom: 20px;">СИСТЕМНЫЕ ПОКАЗАТЕЛИ</h3>
            <div class="stats-grid">
                <div class="stat-item">
                    <div class="stat-label">НАПРЯЖЕНИЕ БАТАРЕИ</div>
                    <div class="stat-value" id="battery-voltage">)rawliteral" + String(round(analogRead(BATTERY_PIN)*2*0.0094)*100) + R"rawliteral( мВ </div>
                    <div class="progress-bar">
                        <div class="progress-fill" style="width: )rawliteral" + String(round((analogRead(BATTERY_PIN)-1648)/590.0*100)) + R"rawliteral(%;"></div>
                    </div>
                </div>
                <div class="stat-item">
                    <div class="stat-label">УРОВЕНЬ СИГНАЛА (RSSI)</div>
                    <div class="stat-value" id="rssi-value">)rawliteral" + String(LoRa.rssi()) + R"rawliteral(dBm</div>
                    <div class="progress-bar">
                        <div class="progress-fill" style="width: )rawliteral" + String(round((128+LoRa.rssi())/128.0*100)) + R"rawliteral(%;"></div>
                    </div>
                </div>
                <div class="stat-item">
                    <div class="stat-label">ОТНОШЕНИЕ СИГНАЛ/ШУМ (SNR)</div>
                    <div class="stat-value" id="snr-value">)rawliteral" + String(LoRa.packetSnr()) + R"rawliteral(dB </div>
                    <div class="progress-bar">
                        <div class="progress-fill" style="width: )rawliteral" + String(round((LoRa.packetSnr()+5)/40.0*100)) + R"rawliteral(%;"></div>
                    </div>
                </div>
                
                <div class="stat-item">
                    <div class="stat-label">СВОБОДНОЕ МЕСТО (пока не используется)</div>
                    <div class="stat-value" id="storage-value">3.2 PB</div>
                    <div class="progress-bar">
                        <div class="progress-fill" style="width: 42%;"></div>
                    </div>
                </div>
            </div>
            
            <div class="data-section">
                <button class="data-toggle" id="data-toggle">ДАННЫЕ</button>
                <div class="data-content" id="data-content">
                    <div class="data-text">
Пока никак не используется...
                    </div>
                </div>
            </div>
        </div>
        
        <div style="text-align: center; margin-top: 30px;">
            <form method="post" action="/messenger">
                <button type="submit" class="cyber-button">ВЕРНУТЬСЯ</button>
            </form>
        </div>
    </div>
    
    <script>
        document.addEventListener('DOMContentLoaded', function() {
            // Добавление эффекта глитча для неонового текста
            setInterval(() => {
                document.querySelectorAll('.neon-text, .neon-pink').forEach(el => {
                    el.style.animation = 'none';
                    setTimeout(() => {
                        el.style.animation = 'glitch 12s infinite';
                    }, 10);
                });
            }, 15000);
            
            // Переключение отображения данных
            const dataToggle = document.getElementById('data-toggle');
            const dataContent = document.getElementById('data-content');
            
            dataToggle.addEventListener('click', function() {
                this.classList.toggle('active');
                dataContent.classList.toggle('active');
            });
        });
    </script>
</body>
</html>)rawliteral";
  server.send(200, "text/html", html);
}

void cssstyle() {
  String css = R"rawliteral(
:root {
    --neon-blue: #00f3ff;
    --neon-pink: #ff00ea;
    --neon-purple: #a600ff;
    --dark-bg: #0a0a14;
    --darker-bg: #050510;
    --panel-bg: rgba(10, 15, 30, 0.8);
    --text-glow: 0 0 8px var(--neon-blue);
    --text-pink-glow: 0 0 8px var(--neon-pink);
    --panel-glow: 0 0 15px rgba(0, 243, 255, 0.7);
    --panel-pink-glow: 0 0 15px rgba(255, 0, 234, 0.5);
}

* {
    margin: 0;
    padding: 0;
    box-sizing: border-box;
    font-family: 'Courier New', monospace;
}

body {
    background-color: var(--dark-bg);
    color: #e0e0ff;
    background-image: 
        linear-gradient(rgba(0, 243, 255, 0.05) 1px, transparent 1px),
        linear-gradient(90deg, rgba(0, 243, 255, 0.05) 1px, transparent 1px);
    background-size: 20px 20px;
    min-height: 100vh;
    overflow-x: hidden;
    position: relative;
}

body::before {
    content: "";
    position: fixed;
    top: 0;
    left: 0;
    width: 100%;
    height: 100%;
    background: repeating-linear-gradient(
        0deg,
        rgba(0, 0, 0, 0.15),
        rgba(0, 0, 0, 0.15) 1px,
        transparent 1px,
        transparent 2px
    );
    pointer-events: none;
    z-index: 1000;
}

/* SCANLINE ANIMATION */
@keyframes scanline {
    0% { transform: translateY(-100%); }
    100% { transform: translateY(100vh); }
}

.scanline {
    position: fixed;
    top: 0;
    left: 0;
    width: 100%;
    height: 100%;
    background: linear-gradient(
        to bottom,
        rgba(0, 243, 255, 0) 0%,
        rgba(0, 243, 255, 0.2) 10%,
        rgba(0, 243, 255, 0) 100%
    );
    animation: scanline 8s linear infinite;
    pointer-events: none;
    z-index: 1001;
}

/* GLITCH EFFECT */
@keyframes glitch {
    0% { text-shadow: var(--text-glow); }
    2% { text-shadow: 0.1em 0 0 var(--neon-pink), -0.1em -0.05em 0 var(--neon-blue); }
    4% { text-shadow: var(--text-glow); }
    96% { text-shadow: var(--text-glow); }
    98% { text-shadow: -0.1em 0 0 var(--neon-pink), 0.1em 0.05em 0 var(--neon-blue); }
    100% { text-shadow: var(--text-glow); }
}

/* NEON GLOW */
.neon-text {
    text-shadow: var(--text-glow);
    animation: glitch 12s infinite;
    color: #fff;
}

.neon-pink {
    text-shadow: var(--text-pink-glow);
    color: #ffb0f7;
}

/* PANELS AND CONTAINERS */
.cyber-panel {
    background-color: var(--panel-bg);
    border: 1px solid var(--neon-blue);
    box-shadow: var(--panel-glow), inset var(--panel-glow);
    padding: 20px;
    border-radius: 4px;
    margin-bottom: 20px;
    position: relative;
    overflow: hidden;
}

.cyber-panel::before {
    content: "";
    position: absolute;
    top: 0;
    left: 0;
    width: 5px;
    height: 100%;
    background: linear-gradient(to bottom, var(--neon-blue), var(--neon-purple));
    box-shadow: 0 0 10px var(--neon-blue);
}

.cyber-panel.pink-glow {
    border-color: var(--neon-pink);
    box-shadow: var(--panel-pink-glow), inset var(--panel-pink-glow);
}

.cyber-panel.pink-glow::before {
    background: linear-gradient(to bottom, var(--neon-pink), var(--neon-purple));
    box-shadow: 0 0 10px var(--neon-pink);
}

/* BUTTONS */
.cyber-button {
    background: linear-gradient(45deg, var(--darker-bg), #0f1a2a);
    border: 1px solid var(--neon-blue);
    color: var(--neon-blue);
    padding: 12px 24px;
    font-size: 16px;
    cursor: pointer;
    position: relative;
    overflow: hidden;
    text-shadow: var(--text-glow);
    box-shadow: var(--panel-glow);
    transition: all 0.3s ease;
    outline: none;
    border-radius: 2px;
    text-transform: uppercase;
    letter-spacing: 2px;
}

.cyber-button:hover {
    background: linear-gradient(45deg, #0f1a2a, var(--darker-bg));
    box-shadow: 0 0 20px var(--neon-blue);
    text-shadow: 0 0 15px var(--neon-blue);
    transform: translateY(-2px);
}

.cyber-button:active {
    transform: translateY(1px);
}

.cyber-button.pink {
    border-color: var(--neon-pink);
    color: var(--neon-pink);
    text-shadow: var(--text-pink-glow);
    box-shadow: var(--panel-pink-glow);
}

.cyber-button.pink:hover {
    box-shadow: 0 0 20px var(--neon-pink);
    text-shadow: 0 0 15px var(--neon-pink);
}

.cyber-button::before {
    content: "";
    position: absolute;
    top: -10px;
    left: -10px;
    right: -10px;
    bottom: -10px;
    background: linear-gradient(45deg, 
        var(--neon-blue), 
        transparent, 
        transparent, 
        var(--neon-blue));
    z-index: -1;
    transform: rotate(45deg) scale(0);
    transition: transform 0.5s ease;
}

.cyber-button:hover::before {
    transform: rotate(45deg) scale(1.5);
}

.cyber-button.pink::before {
    background: linear-gradient(45deg, 
        var(--neon-pink), 
        transparent, 
        transparent, 
        var(--neon-pink));
}

/* INPUTS */
.cyber-input {
    background-color: rgba(5, 10, 20, 0.7);
    border: 1px solid var(--neon-blue);
    color: var(--neon-blue);
    padding: 12px 15px;
    font-size: 16px;
    width: 100%;
    margin-bottom: 15px;
    box-shadow: inset 0 0 10px rgba(0, 243, 255, 0.3), 0 0 10px rgba(0, 243, 255, 0.2);
    transition: all 0.3s ease;
    outline: none;
    text-shadow: var(--text-glow);
}

.cyber-input:focus {
    border-color: var(--neon-pink);
    box-shadow: inset 0 0 15px rgba(255, 0, 234, 0.5), 0 0 15px rgba(255, 0, 234, 0.4);
}

/* HEADER STYLES */
.cyber-header {
    background-color: var(--darker-bg);
    padding: 20px;
    text-align: center;
    border-bottom: 2px solid var(--neon-blue);
    box-shadow: 0 0 20px rgba(0, 243, 255, 0.5);
    margin-bottom: 30px;
    position: relative;
    z-index: 10;
}

.cyber-header h1 {
    font-size: 2.5rem;
    letter-spacing: 4px;
    margin-bottom: 10px;
}

.cyber-header h2 {
    font-size: 1.5rem;
    color: var(--neon-pink);
    text-shadow: var(--text-pink-glow);
}

/* NAVBAR STYLES */
.cyber-navbar {
    display: flex;
    justify-content: space-between;
    align-items: center;
    padding: 15px 20px;
    background-color: var(--darker-bg);
    border-bottom: 1px solid var(--neon-blue);
    box-shadow: 0 5px 15px rgba(0, 243, 255, 0.3);
    position: relative;
}

.cyber-navbar .logo {
    font-size: 1.8rem;
    letter-spacing: 3px;
    position: absolute;
    left: 50%;
    transform: translateX(-50%);
}

/* MESSAGES STYLES */
.messages-container {
    max-height: 60vh;
    overflow-y: auto;
    padding: 20px;
    background-color: rgba(5, 10, 20, 0.5);
    border: 1px solid var(--neon-purple);
    margin-bottom: 20px;
    box-shadow: inset 0 0 20px rgba(160, 0, 255, 0.3);
}

.message {
    padding: 12px 20px;
    margin-bottom: 15px;
    border-radius: 4px;
    max-width: 80%;
    position: relative;
    animation: fadeIn 0.5s ease;
    border: 1px solid transparent;
}

@keyframes fadeIn {
    from { opacity: 0; transform: translateY(10px); }
    to { opacity: 1; transform: translateY(0); }
}

.message.received {
    background: linear-gradient(to right, rgba(10, 25, 50, 0.8), rgba(5, 15, 40, 0.8));
    border-color: var(--neon-blue);
    box-shadow: 0 0 10px rgba(0, 243, 255, 0.3);
    align-self: flex-start;
    border-left: 3px solid var(--neon-blue);
}

.message.sent {
    background: linear-gradient(to left, rgba(50, 10, 50, 0.8), rgba(40, 5, 40, 0.8));
    border-color: var(--neon-pink);
    box-shadow: 0 0 10px rgba(255, 0, 234, 0.3);
    margin-left: auto;
    border-right: 3px solid var(--neon-pink);
}

.message::after {
    content: "";
    position: absolute;
    bottom: 5px;
    font-size: 0.7rem;
    color: #aaa;
}

/* DIALOG LIST */
.dialogs-container {
    display: grid;
    gap: 15px;
    padding: 20px;
}

.dialog-item {
    background: linear-gradient(to right, var(--darker-bg), rgba(15, 25, 50, 0.7));
    border: 1px solid var(--neon-blue);
    padding: 15px;
    border-radius: 4px;
    cursor: pointer;
    transition: all 0.3s ease;
    position: relative;
    overflow: hidden;
}

.dialog-item:hover {
    transform: translateX(5px);
    box-shadow: 0 0 20px rgba(0, 243, 255, 0.5);
    border-color: var(--neon-pink);
}

.dialog-name {
    font-size: 1.2rem;
    color: var(--neon-blue);
    margin-bottom: 5px;
    text-shadow: var(--text-glow);
}

.dialog-message {
    font-size: 0.9rem;
    color: #a0a0ff;
    overflow: hidden;
    text-overflow: ellipsis;
    white-space: nowrap;
}

/* FOOTER STYLES */
.cyber-footer {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
    gap: 15px;
    padding: 20px;
    background-color: var(--darker-bg);
    border-top: 1px solid var(--neon-blue);
    box-shadow: 0 -5px 15px rgba(0, 243, 255, 0.3);
    position: fixed;
    bottom: 0;
    width: 100%;
}

/* FORM LAYOUT */
.form-container {
    max-width: 600px;
    margin: 30px auto;
    padding: 20px;
}

.form-row {
    margin-bottom: 20px;
}

.form-row p {
    margin-bottom: 8px;
    color: var(--neon-blue);
    text-shadow: var(--text-glow);
}

/* UTILITY CLASSES */
.text-center {
    text-align: center;
}

.mb-20 {
    margin-bottom: 20px;
}

.mt-20 {
    margin-top: 20px;
}

.grid-3 {
    display: grid;
    grid-template-columns: repeat(3, 1fr);
    gap: 15px;
}

/* RESPONSIVE */
@media (max-width: 768px) {
    .grid-3 {
        grid-template-columns: 1fr;
    }
    
    .cyber-navbar .logo {
        font-size: 1.4rem;
    }
    
    .cyber-header h1 {
        font-size: 2rem;
    }
    
    .cyber-footer {
        grid-template-columns: 1fr;
        position: static;
    }
}
  )rawliteral";
  server.send(200, "text/css", css);
}

void registration() {
  String html = R"rawliteral(
              <!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ME-LO-RA Registration</title>
    <link rel="stylesheet" href="styles.css">
    <style>
        /* Terminal boot screen styles */
        #boot-screen {
            position: fixed;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            background-color: #000;
            z-index: 2000;
            display: flex;
            align-items: center;
            justify-content: center;
            font-family: 'Courier New', monospace;
        }
        
        #boot-content {
            color: #0f0;
            font-size: 18px;
            padding: 20px;
            max-width: 800px;
        }
        
        .terminal-text {
            display: inline-block;
            overflow: hidden;
            white-space: nowrap;
            border-right: 2px solid #0f0;
            animation: 
                terminalTyping 3s steps(40, end),
                blinkCursor 0.75s step-end infinite;
        }
        
        @keyframes terminalTyping {
            from { width: 0; }
            to { width: 100%; }
        }
        
        @keyframes blinkCursor {
            from, to { border-color: transparent; }
            50% { border-color: #0f0; }
        }
    </style>
</head>
<body>
    <div class="scanline"></div>
    
    <!-- Terminal Boot Animation -->
    <div id="boot-screen">
        <div id="boot-content">
            <div>> Booting ME-LO-RA Network OS...</div>
            <div>> Initializing cyber modules <span id="loading-dots">...</span></div>
            <div>> Establishing neural connection</div>
            <div>> <span class="terminal-text">Access granted. Welcome to the network.</span></div>
        </div>
    </div>
    
    <div class="container" style="display:none;" id="content">
        <div class="cyber-header">
            <h1 class="neon-text">///ME-LO-RA///</h1>
            <h2 class="neon-pink">УСТАНОВОЧНЫЕ НАСТРОЙКИ</h2>
        </div>
        
        <div class="form-container">
            <div class="cyber-panel pink-glow">
                <form name="test" method="post" action="/registration">
                    <div class="form-row">
                        <p>ВВЕДИТЕ ПСЕВДОНИМ:</p>
                        <input class="cyber-input" name="user_name" pattern="^[a-zA-Z\s]+$" value="USERNAME">
                    </div>
                    
                    <div class="form-row">
                        <p>ВВЕДИТЕ ОТКРЫТУЮ ЭКСПОНЕНТУ (e):</p>
                        <input class="cyber-input" name="e_exp" pattern="^[ 0-9]+$" value="0">
                    </div>
                    
                    <div class="form-row">
                        <p>ВВЕДИТЕ МОДУЛЬ (n):</p>
                        <input class="cyber-input" name="module" pattern="^[ 0-9]+$" value="0">
                    </div>
                    
                    <div class="form-row">
                        <p>ВВЕДИТЕ ЗАКРЫТУЮ ЭКСПОНЕНТУ (d):</p>
                        <input class="cyber-input" name="d_exp" pattern="^[ 0-9]+$" value="0">
                    </div>
                    
                    <div class="text-center grid-3">
                        <button type="submit" class="cyber-button">ДАЛЕЕ</button>
                        <button type="reset" class="cyber-button pink">ОЧИСТИТЬ</button>
                    </div>
                </form>
            </div>
        </div>
    </div>
    
    <script>
        document.addEventListener('DOMContentLoaded', function() {
            const bootScreen = document.getElementById('boot-screen');
            const content = document.getElementById('content');
            const loadingDots = document.getElementById('loading-dots');
            
            // Animate loading dots
            let dots = 0;
            const dotsInterval = setInterval(() => {
                dots = (dots + 1) % 4;
                loadingDots.textContent = '.'.repeat(dots);
            }, 500);
            
            // Simulate boot process
            setTimeout(() => {
                clearInterval(dotsInterval);
                bootScreen.style.opacity = '0';
                
                setTimeout(() => {
                    bootScreen.style.display = 'none';
                    content.style.display = 'block';
                    
                    // Add glitch effect to all neon text periodically
                    setInterval(() => {
                        document.querySelectorAll('.neon-text, .neon-pink').forEach(el => {
                            el.style.animation = 'none';
                            setTimeout(() => {
                                el.style.animation = 'glitch 12s infinite';
                            }, 10);
                        });
                    }, 15000);
                }, 1000);
            }, 5000);
            
            // Form input focus effect
            document.querySelectorAll('.cyber-input').forEach(input => {
                input.addEventListener('focus', function() {
                    this.style.boxShadow = '0 0 20px var(--neon-pink)';
                });
                
                input.addEventListener('blur', function() {
                    this.style.boxShadow = '';
                });
            });
        });
    </script>
</body>
</html>
  )rawliteral";
  server.send(200, "text/html", html);
}

// Обработка 404 ошибки
void handleNotFound() {
  server.send(404, "text/plain", "Page Not Found");
}
