/*
 * ----------------------------------------------------------------------------
 *  MFRC522 library : https://github.com/miguelbalboa/rfid
 */

#include <SPI.h>
#include <stdio.h>
#include <MFRC522.h>
#include <SD.h>

#define RST_PIN		9		// 
#define SS_PIN		10		//

MFRC522 mfrc522(SS_PIN, RST_PIN);	// Create MFRC522 instance

#define MAX_ID_BUFFER_LEN 30
#define PATH_LIBERADOS "/media/card/cartoes/liberados/"
#define PATH_BLOQUEADOS "/media/card/cartoes/bloqueados/"

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
  
         if (SD.exists(fname)) {
             //TODO usuario tem acesso
               Serial.println("Acesso Liberado");
         } else {
             strcpy(fname, PATH_BLOQUEADOS);
             strcat(fname, idBuffer);
             Serial.println(fname);
             if (SD.exists(fname)) {
               Serial.println("Acesso Bloqueado");
                // TODO usuario tem acesso bloqueado/proibido
             } else {
               Serial.println("Nao tem acesso");
             // TODO usuario nao tem acesso
             }
         }       

}

void setup() {
	Serial.begin(9600);		// Initialize serial communications with the PC
	SPI.begin();			// Init SPI bus
	mfrc522.PCD_Init();		// Init MFRC522
	Serial.println("Scan tags...");

//PIN not required for galileo
  if (!SD.begin(0)) {
    Serial.println("SD initialization failed!");
    return;
  }
        system("mkdir /media/card/cartoes");
        system("mkdir /media/card/cartoes/liberados");
        system("mkdir /media/card/cartoes/bloqueados");

// Ids para  teste
        system("echo 1> /media/card/cartoes/liberados/00000000");        
        system("echo 1> /media/card/cartoes/bloqueados/00000001");
}

void loop() {
	// Look for new cards
	if ( mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    	  // Dump debug info about the card; PICC_HaltA() is automatically called
	  mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
          preparaIdBuffer();
          tratarCartao();
	}
        
        delay(5000);
        strcpy(idBuffer,"00000000");
        tratarCartao();
        
        strcpy(idBuffer,"00000001");
        tratarCartao();

        strcpy(idBuffer,"00000002");
        tratarCartao();
}




