/*
 * ----------------------------------------------------------------------------
 *  MFRC522 library : https://github.com/miguelbalboa/rfid
 */

#include <SPI.h>
#include <stdio.h>
#include <MFRC522.h>

// http://www.seeedstudio.com/wiki/Grove_-_LCD_RGB_Backlight
// https://github.com/Seeed-Studio/Grove_LCD_RGB_Backlight.git
#include <Wire.h>
#include <rgb_lcd.h>

#include <SD.h>

// Caso o firmware esteja montando em outro diretorio nao funciona
// nesse caso ajustar o /etc/fstab para esse diretorio que eh o utilizado pela biblioteca do SD
#define SD_CARD_BASE "/media/mmcblk0p1"

#define SCRIPT_LIBERADO "cartoes/liberado.sh"
#define SCRIPT_BLOQUEADO "cartoes/bloqueado.sh"
#define SCRIPT_SEM_ACESSO "cartoes/sem_acesso.sh"

#define PATH_LIBERADOS "cartoes/liberados/"
#define PATH_BLOQUEADOS "cartoes/bloqueados/"

#define MAX_ID_BUFFER_LEN 30
#define TRUE 1
#define FALSE 0


#define MFRC_RST_PIN		9		// 
#define MFRC_SS_PIN		10		//
#define TO_HEX(c) ((c) < 10 ? ( (c) + '0') : ((c) - 10 + 'A'))

MFRC522 mfrc522(MFRC_SS_PIN, MFRC_RST_PIN);	// Create MFRC522 instance
rgb_lcd lcd;

char idBuffer[MAX_ID_BUFFER_LEN];
char fname[200];
FILE f;

void preparaIdBuffer() {
         byte b=0;
         for (byte i =0 ; i < mfrc522.uid.size; i++ ) {
             idBuffer[b++] = TO_HEX((mfrc522.uid.uidByte[i] & 0xf0) >> 4);
             idBuffer[b++] = TO_HEX(mfrc522.uid.uidByte[i] & 0x0f);

         }
         idBuffer[b] = 0;
}

void tratarCartao() {
         Serial.print("UID = |");
         Serial.print(idBuffer);
         Serial.println("|");
         
         strcpy(fname, PATH_LIBERADOS);
         strcat(fname, idBuffer);
         Serial.println(fname);
    
         lcd.clear();
         lcd.println(idBuffer);
         
         if (SD.exists(fname)) {
               lcd.setRGB(0x00,0xff,0x00);
               Serial.println("Acesso Liberado");

               strcat(fname, ".sh");
               if (SD.exists(fname)) {
                   system(fname);
               } else {
                   strcpy(fname, SCRIPT_LIBERADO " ");
                   strcat(fname, idBuffer);
                   system(fname);
               }
               
         } else {
             strcpy(fname, PATH_BLOQUEADOS);
             strcat(fname, idBuffer);
             Serial.println(fname);
             if (SD.exists(fname)) {
               lcd.setRGB(0xff,0x00,0x00);
               Serial.println("Acesso Bloqueado");
               strcat(fname, ".sh");
               if (SD.exists(fname)) {
                   system(fname);
               } else {
                   strcpy(fname, SCRIPT_BLOQUEADO " ");
                   strcat(fname, idBuffer);
                   system(fname);
               }


                // TODO usuario tem acesso bloqueado/proibido
             } else {
               Serial.println("Nao tem acesso");
               lcd.setRGB(0xff,0xff,0x00);

                   strcpy(fname, SCRIPT_SEM_ACESSO " ");
                   strcat(fname, idBuffer);
                   system(fname);
             }
         }       

}

void setup() {

        delay(5000);
        lcd.begin(16, 2);
        lcd.clear();
        lcd.setRGB(0,0,0);

        
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
          preparaIdBuffer();
          tratarCartao();

    	  // Dump debug info about the card; PICC_HaltA() is automatically called
	  mfrc522.PICC_DumpToSerial(&(mfrc522.uid));

	}

//        delay(5000);
//        testeArquivos();

}




