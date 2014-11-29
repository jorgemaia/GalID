/*
 * ----------------------------------------------------------------------------
 *  MFRC522 library : https://github.com/miguelbalboa/rfid
 */

#include <SPI.h>
#include <stdio.h>
#include <MFRC522.h>

// https://github.com/Seeed-Studio/Grove_LCD_RGB_Backlight.git
#include <Wire.h>
#include <rgb_lcd.h>

#include <SD.h>

#define RST_PIN		9		// 
#define SS_PIN		10		//

MFRC522 mfrc522(SS_PIN, RST_PIN);	// Create MFRC522 instance

#define TRUE 1
#define FALSE 0

#define MAX_ID_BUFFER_LEN 30

rgb_lcd lcd;


// Caso o firmware esteja montando em outro diretorio nao funciona
// nesse caso ajustar o /etc/fstab para esse diretorio que eh o utilizado pela biblioteca do SD
#define SD_CARD_BASE "/media/mmcblk0p1"

#define PATH_LIBERADOS "cartoes/liberados/"
#define PATH_BLOQUEADOS "cartoes/bloqueados/"

char idBuffer[MAX_ID_BUFFER_LEN];
char fname[200];
FILE f;

void preparaIdBuffer() {
         byte b=0;
         for (byte i =0 ; i < mfrc522.uid.size; i++ ) {
             idBuffer[b++] = ((mfrc522.uid.uidByte[i] & 0xf0) >> 8) + '0';
             idBuffer[b++] = (mfrc522.uid.uidByte[i] & 0x0f) + '0';

         }
         idBuffer[b] = 0;
}

void tratarCartao() {
         Serial.print("UID = ");
         Serial.println(idBuffer);
         strcpy(fname, PATH_LIBERADOS);
         strcat(fname, idBuffer);
         Serial.println(fname);
    
         lcd.print(idBuffer);
         
         if (SD.exists(fname)) {
             //TODO usuario tem acesso
               Serial.println("Acesso Liberado");
               lcd.setRGB(0x00,0xff,0x00);
         } else {
             strcpy(fname, PATH_BLOQUEADOS);
             strcat(fname, idBuffer);
             Serial.println(fname);
             if (SD.exists(fname)) {
               lcd.setRGB(0xff,0x00,0x00);
               Serial.println("Acesso Bloqueado");
                // TODO usuario tem acesso bloqueado/proibido
             } else {
               Serial.println("Nao tem acesso");
               lcd.setRGB(0xff,0xff,0x00);
             // TODO usuario nao tem acesso
             }
         }       

}

void setup() {

        delay(5000);
        lcd.begin(16, 2);
        
	Serial.begin(115200);		// Initialize serial communications with the PC
	SPI.begin();			// Init SPI bus
	mfrc522.PCD_Init();		// Init MFRC522
	Serial.println("Scan tags...");

//PIN not required for galileo
  if (!SD.begin(0)) {
    Serial.println("SD initialization failed!");
  } 

        system("mkdir -p " SD_CARD_BASE "/" PATH_LIBERADOS);
        system("mkdir -p " SD_CARD_BASE "/" PATH_BLOQUEADOS);


}

void testeArquivos() {

// Ids para  teste
        system("echo 1> " SD_CARD_BASE "/" PATH_LIBERADOS "/00000000");        
        system("echo 1> " SD_CARD_BASE "/" PATH_BLOQUEADOS "/00000001");

        strcpy(idBuffer,"00000000");
        tratarCartao();
        
        strcpy(idBuffer,"00000001");
        tratarCartao();

        strcpy(idBuffer,"00000002");
        tratarCartao();

}

void loop() {
	// Look for new cards
	if ( mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    	  // Dump debug info about the card; PICC_HaltA() is automatically called
	  mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
          preparaIdBuffer();
          tratarCartao();
	}

//        delay(5000);
//        testeArquivos();

}




