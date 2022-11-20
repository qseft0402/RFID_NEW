
/*
 * --------------------------------------------------------------------------------------------------------------------
 * Example sketch/program showing how to read data from a PICC to serial.
 * --------------------------------------------------------------------------------------------------------------------
 * This is a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid
 * 
 * Example sketch/program showing how to read data from a PICC (that is: a RFID Tag or Card) using a MFRC522 based RFID
 * Reader on the Arduino SPI interface.
 * 
 * When the Arduino and the MFRC522 module are connected (see the pin layout below), load this sketch into Arduino IDE
 * then verify/compile and upload it. To see the output: use Tools, Serial Monitor of the IDE (hit Ctrl+Shft+M). When
 * you present a PICC (that is: a RFID Tag or Card) at reading distance of the MFRC522 Reader/PCD, the serial output
 * will show the ID/UID, type and any data blocks it can read. Note: you may see "Timeout in communication" messages
 * when removing the PICC from reading distance too early.
 * 
 * If your reader supports it, this sketch/program will read all the PICCs presented (that is: multiple tag reading).
 * So if you stack two or more PICCs on top of each other and present them to the reader, it will first output all
 * details of the first and then the next PICC. Note that this may take some time as all data blocks are dumped, so
 * keep the PICCs at reading distance until complete.
 * 
 * @license Released into the public domain.
 * 
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 */

#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>

#define RST_PIN         9          // Configurable, see typical pin layout above
#define SS_PIN          10         // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
LiquidCrystal_I2C lcd(0x3F, 12, 2);
int buttonPin=13;
int i=1;
byte uuid_New[4] = {0xFF, 0xFF, 0xFF, 0xFF};
void setup() {
  Serial.begin(9600);    // Initialize serial communications with the PC
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
  Mode1_LCD_Description_Setup();
}

void loop() {
  
  if(digitalRead(buttonPin)==HIGH){
    i++;
    if(i>=5) i=1;
  }
  
  if(i==1){//mode=1 說明
    Mode1_LCD_Description_Setup();
  }
  else if(i==2){//mode=2 讀取卡片資料
    LCDStringSetup("M:2 Reading           ");
    Mode2_ReadAndShowCard();
  }
  else if(i==3){//mode=3 顯示讀取資料
    LCDStringSetup("M:3 ShowTmpInfo       ");
    Mode3_ShowTmpCardInfo();
  }
  else if(i==4){//mode=4 更改卡片
    LCDStringSetup("M:4 Changing          ");
    Mode4_ChangeCard();
  }
  Serial.println(i);
  delay(1000);
}

void Mode2_ReadAndShowCard(){
  Serial.println("Mode2");
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Dump debug info about the card; PICC_HaltA() is automatically called
  mfrc522.PICC_DumpToSerial(&(mfrc522.uid));

  
  byte* uuid = mfrc522.RETURN_UUID();

  //uuid_New=(byte)uuid;
  LCDShow(uuid);
  char* ID;
  ID=toHEX(uuid);
  for(int i=0;i<4;i++){
    Serial.print(ID[i*2]);
    Serial.print(ID[i*2+1]);
    Serial.print(" ");
  }
delay(1000);
}

char* toHEX(byte* uuid){
 static char result[9];
 result[8]="\0";
 Serial.print("{");
 for(int i=0;i<4;i++){
   unsigned int unNum = (int) uuid[i] ;
   result[i*2] = DecToHex(unNum / 16) ;
   result[i*2+1] = DecToHex(unNum % 16);
   Serial.print("0x");
  Serial.print(result[i*2]);
  Serial.print(result[i*2+1]);
  if(i==3) break;
  Serial.print(",");
 }
 Serial.println("}");
 return result;
}
char DecToHex (unsigned int n)
{
    char hex[] = {'0', '1', '2', '3','4','5'
                 ,'6','7','8','9','A','B','C'
                 ,'D','E','F'
                 };
    return hex[n] ;
}

void LCDStringSetup(String str){
  lcd.setCursor(0,0);
  lcd.print(str);
}


void Mode1_LCD_Description_Setup(){
    // initialize the LCD
  lcd.begin();
  
  // Turn on the blacklight and print a message.
  //lcd.backlight();
  lcd.setBacklight(true);
  lcd.setCursor(0,0);
  lcd.print("M:1 1.Des 2.Read      ");
  lcd.setCursor(0,1);
  lcd.print("3.TmpInfo 4.Chg       ");

}

void LCDShow(byte* ID){
  Serial.print("LCDShow!!!");
    lcd.setCursor(0,1);
    
    for(int i=0;i<4;i++){
      if(ID[i]<10)
        lcd.print("0");
      uuid_New[i]=ID[i];
      Serial.print(ID[i]);
      Serial.print(" ");
      lcd.print(ID[i],HEX);
      lcd.print(" ");
  }
  lcd.print("         ");
}

void Mode3_ShowTmpCardInfo(){
    Serial.print("Mode3_ShowTmpCardInfo!!!");
    lcd.setCursor(0,1);
    lcd.print("Tmp: ");
    for(int i=0;i<4;i++){
      if(uuid_New[i]<10)
        lcd.print("0");
      Serial.print(uuid_New[i]);
      Serial.print(" ");
      lcd.print(uuid_New[i],HEX);
      lcd.print(" ");
  }

  
}
void Mode4_ChangeCard(){
  Serial.println("Mode3_ChangeCard");
  Serial.println(uuid_New[0]);
  if ( mfrc522.MIFARE_SetUid(uuid_New, (byte)4, true) ) {
    Serial.println(F("Wrote new UID to card."));
  }
  
  // Halt PICC and re-select it so DumpToSerial doesn't get confused
  mfrc522.PICC_HaltA();
  if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
    return;
  }
  
  // Dump the new memory contents
  Serial.println(F("New UID and contents:"));
  lcd.setCursor(0,1);
  lcd.print("                ");
  lcd.print("Done!");
  mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
  
  delay(2000);
  
}




















 
