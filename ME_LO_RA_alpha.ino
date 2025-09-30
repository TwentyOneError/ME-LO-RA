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
#include "html_content.cpp"
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET     -1
#define BUTTON_PIN 16
#define BATTERY_PIN 35
#define SS_PIN 5
#define RST_PIN 14
#define DIO0_PIN 2
#define PACKET_SIZE 50                         // Размер пакетов в байтах
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
const char* ssid = "ME_LO_RA_AP";              //название вашей Wi-Fi сети
const char* password = "password_1";           //пароль от сети



//----------------------------Костыли для исправления е*****о String в arduino---------

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
//----------------------------------------------Вернемся к коду---------------------------------





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
  LoRa.setTxPower(20);
  LoRa.setSpreadingFactor(12);
  LoRa.setSyncWord(0x21);
  LoRa.setSignalBandwidth(125E3);
  redraw_display(wifib);
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
  server.handleClient();                      // Обработка входящих запросов веб-клиента
  button = digitalRead(BUTTON_PIN);
  if (millis()-mil>30000){
    mil = millis();
    redraw_display(wifib);
  }
  if(lastb == LOW && button == HIGH){
    mil = millis();
    wifib = !wifib;
    redraw_display(wifib);
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

void redraw_display(bool wifi_state){
  if (wifi_state){
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


//-------------------------------------WEB-UI-----------------------------------------------------------
//---------------------------------------------------------Страница диалога с энтузиастом---------------
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
  String html = HTML_DIALOGUE_1 + aluser[luser_id] + HTML_DIALOGUE_2;
  for (int i = 0;i < mess[luser_id].size();i++){
    if (sended[luser_id][i]){
      html = html + R"rawliteral(<div class="message sent">)rawliteral" + mess[luser_id][i] + R"rawliteral(</div>)rawliteral";
    } else {
      html = html + R"rawliteral(<div class="message received">)rawliteral" + mess[luser_id][i] + R"rawliteral(</div>)rawliteral";
    }
  }
  html = html + HTML_DIALOGUE_3 + String(luser_id) + HTML_DIALOGUE_4;

   server.send(200, "text/html", html);
}



//------------------------------------------------Страница обработки данных регистрации пользователя и добавления энтузиастов----------------
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
  String html = HTML_OBR_1;
  server.send(200, "text/html", html);
}



//-------------------------------------------------Добавление энтузиастов--------------------------------------------------------------------
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
  String html = HTML_LOS_1;
  server.send(200, "text/html", html);
}



//--------------------------------------------------Главная страница с диалогами------------------------------
void messenger() {
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

  String html = HTML_MESSENGER_1 + html_2 + HTML_MESSENGER_2;

  server.send(200, "text/html", html);
}




//--------------------------------------------------------Настройки---------------------------------
void settings() {
  String html = HTML_SETTINGS_1 + R"rawliteral(
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
        </div>)rawliteral" + HTML_SETTINGS_2;
  server.send(200, "text/html", html);
}





//-----------------------------------------------------------CSS файл--------------------------------
void cssstyle() {
  String css = HTML_CSS_1;
  server.send(200, "text/css", css);
}




//------------------------------------------------------Регистрация---------------------------------
void registration() {
  String html = HTML_REG_1;
  server.send(200, "text/html", html);
}



//-------------------------------------------------------------------------------Обработка 404 ошибки------------------------------
void handleNotFound() {
  server.send(404, "text/plain", "Page Not Found");
}
