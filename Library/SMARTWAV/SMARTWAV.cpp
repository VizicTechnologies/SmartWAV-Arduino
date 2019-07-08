/*********************************************************
VIZIC TECHNOLOGIES. COPYRIGHT 2019.
THE DATASHEETS, SOFTWARE AND LIBRARIES ARE PROVIDED "AS IS." 
VIZIC EXPRESSLY DISCLAIM ANY WARRANTY OF ANY KIND, WHETHER 
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
OR NONINFRINGEMENT. IN NO EVENT SHALL VIZIC BE LIABLE FOR 
ANY INCIDENTAL, SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES, 
LOST PROFITS OR LOST DATA, HARM TO YOUR EQUIPMENT, COST OF 
PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, 
ANY CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO 
ANY DEFENCE THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION,
OR OTHER SIMILAR COSTS.
*********************************************************/

/********************************************************
 IMPORTANT : This library is created for the Arduino 1.8.5 Software IDE
********************************************************/

#include <avr/interrupt.h>
#include <Arduino.h> 

#include "SMARTWAV.h"
#include <SoftwareSerial.h>

// SMART WAV DEFAULT BAUD RATE: 9600bps
// Be sure to assign correct pins to TXPIN, RXPIN and Reset in SMARTWAV.h file
/*default pin assignation in SMARTWAV.h:
#define TX_PIN     3     //software UART Transmit pin - Connect this pin to SMARTWAV RX pin
#define RX_PIN     2     //software UART Receive pin - Connect this pin to SMARTWAV TX pin
#define RST_PIN    4     //reset - Connect this pin to SmartWAV reset pin
*/

//Declare software serial object
SoftwareSerial softSerial(RX_PIN, TX_PIN);
	
/****************************************************************/
//Communication Functions(PLATFORM DEPENDENT) - MODIFY THE NEXT 
// FUNCTIONS TO FIT YOUR PLATFORM IF DIFFERENT THAN ARDUINO
/****************************************************************/
//Sends a single character through the serial port(USART)
void putcharTX(char data){
	softSerial.write(data);
}

//returns the available received chars in the RX pin
unsigned char availableRX(void){
	return softSerial.available();	
}

//Returns a single character obtained from the serial port(USART)
char getcharRX(void){
	while(availableRX() == 0);
	return softSerial.read();
}

//Performs a Hardware Reset on smartWAV RESET pin
void resetSmartWAV(void){           //Reset the SMARTWAV board
	digitalWrite(RST_PIN, LOW);    //Set the pin to GND to reset 
	delay(500);
	digitalWrite(RST_PIN, HIGH);   //Set the pin to 5v to end reset
	delay(500);	
}

//Arduino Exclusive function - Configures the Hardware
void initBoard(void){                  //configure the arduino board for SMARTWAV board support
	softSerial.begin(9600);            //Default Arduino-SMARTWAV communication Baudrate is 9600
	pinMode(RST_PIN, OUTPUT);          //Configure pin as output	
}


/****************************************************************/
//Arduino exclusive Functions
/****************************************************************/
SMARTWAV::SMARTWAV(void){

}

void SMARTWAV::init(){           //configure the arduino board for SMARTWAV board
	initBoard();
	resetSmartWAV();
}

void SMARTWAV::reset(){          //Reset the SMARTWAV board
  init();
} 

uint8_t SMARTWAV::sleep(){       //Send SMARTWAV to sleep mode, if already on sleep mode, then awake SmartWAV
  putcharTX('Z'); 
  return getcharRX();  
}	

uint8_t SMARTWAV::getStatus(){   //Asks for active/play status
  unsigned char x;
  putcharTX('A');  
  x=getcharRX();
  getcharRX();
  return x;
}
 
uint8_t SMARTWAV::playTracks(){       //Start playing any song on current Folder/Dir
  putcharTX('T'); 
  return getcharRX();
}	

uint8_t SMARTWAV::pausePlay(){       //Pause current track, if already paused, resumes playing. If called when no playing, returns NAK 'F'
  putcharTX('P'); 
  return getcharRX();
}	

uint8_t SMARTWAV::rewindTrack(){       //Rewind current track. If called when no playing, returns NAK 'F'
  putcharTX('R'); 
  return getcharRX();
}	

uint8_t SMARTWAV::nextTrack(){       //Switch to next track. If called when no playing, returns NAK 'F'
  putcharTX('N'); 
  return getcharRX();
}

uint8_t SMARTWAV::playTrackName(char name[]){       //play an Audio file contained on the micro SD card with the given name.
  uint8_t counter=0;
  
  putcharTX('F'); 
  while(1){
	putcharTX(name[counter]);
	delayMicroseconds(98);                //101us plus instructions time = 104us
    if(name[counter]==0x00){
      break;
	}	
	counter++;
  }
  return getcharRX();
}
		
uint8_t SMARTWAV::stopTrack(){       //Stops current track and all audio, Clears "Continue Play Flag". If called when no playing, returns NAK 'F'
  putcharTX('S');  
  return getcharRX();
}		

uint8_t SMARTWAV::continuousPlay(uint8_t enable){       //Enables/Disables Continuous play flag
  unsigned char aux;
  putcharTX('C'); 
  delayMicroseconds(98);                //101us plus instructions time = 104us  
  putcharTX(enable); // Enable-Disable
  aux=getcharRX();
  getcharRX();
  return aux;
}

uint8_t SMARTWAV::volume(uint8_t vol){       //Change current audio Volume
  putcharTX('V'); 
  delayMicroseconds(98);                //101us plus instructions time = 104us  
  putcharTX(vol);
  return getcharRX();
}	

uint8_t SMARTWAV::setFolder(char name[]){        //Set/Enters inside a folder/path on the micro SD card with the given name.
  uint8_t counter=0;
  
  putcharTX('D'); 
  while(1){
	putcharTX(name[counter]);
	delayMicroseconds(98);                //101us plus instructions time = 104us	
    if(name[counter]==0x00){
      break;
	}	
	counter++;
  }
  return getcharRX();
}

uint8_t SMARTWAV::getFileName(char name[]){ //Reads the name of the current/ last audio file being played on the SMARTWAV, and stores it on the name[] buffer.
  uint8_t counter=0;
  
  putcharTX('I');
  delayMicroseconds(98);                //101us plus instructions time = 104us  
  putcharTX('S');  
   //receive all the file name
  while(1){
	name[counter]=getcharRX();
    if(name[counter]==0x00){
		break;
	}
	counter++;
  }    
  return getcharRX();
}

uint8_t SMARTWAV::getFolderName(char name[]){ //Reads the name of the current folderDir/path name the SMARTWAV, and stores it on the name[] buffer.
  uint8_t counter=0;
  
  putcharTX('I');
  delayMicroseconds(98);                //101us plus instructions time = 104us  
  putcharTX('D');  
   //receive all the folder name
  while(1){
	name[counter]=getcharRX();
    if(name[counter]==0x00){
		break;
	}
	counter++;
  }    
  return getcharRX();
}

uint8_t SMARTWAV::getFileList(char list[]){ //Reads all the names of the .WAV files on the current folder/dir on the SMARTWAV, and stores it on the list[] buffer separated by comma ','.
  uint8_t counter=0;
  
  putcharTX('I');
  delayMicroseconds(98);                //101us plus instructions time = 104us  
  putcharTX('L');  
   //receive all the file names
  while(1){
	list[counter]=getcharRX();
    if(list[counter]==0x00){
		break;
	}
	counter++;
  }    
  return getcharRX();
}

uint8_t SMARTWAV::getFolderList(char list[]){ //Reads all the folders on the current folder/dir on the SMARTWAV, and stores it on the list[] buffer separated by comma ','.
  uint8_t counter=0;
  
  putcharTX('I');
  delayMicroseconds(98);                //101us plus instructions time = 104us  
  putcharTX('X');  
   //receive all the folder names
  while(1){
	list[counter]=getcharRX();
    if(list[counter]==0x00){
		break;
	}
	counter++;
  }    
  return getcharRX();
}

uint8_t SMARTWAV::playSpeed(uint8_t speed){     //Change current Play Speed: X0.5, X1, X1.5, X2, if track ends, play speed returns to 1.0X
  putcharTX('M');
  delayMicroseconds(98);                //101us plus instructions time = 104us
  putcharTX(speed);
  getcharRX();
  return getcharRX();
}	



