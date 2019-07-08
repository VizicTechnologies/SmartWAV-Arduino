/*********************************************************
VIZIC TECHNOLOGIES. COPYRIGHT 2012.
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
 IMPORTANT : This Example is created for the Arduino 1.0 Software IDE
********************************************************/

#include <SMARTWAV.h>     //include the SMARTWAV library!

SMARTWAV sWav;            //create our object called sWAV (smartWAV)

//Global Variables
char playList[100];     //an array for storing the retrieved list of Audio files
char songName[9];       // 8 characters + null character
unsigned char audioItems=0;      //variable storing the total of audio files on the 'playlist' array

/************************************************************************************************/
//Simple function to count the number of items on a list
/************************************************************************************************/
unsigned char getListItems(char *list){  // returns the number of audio files on current folder/Dir, if none or fails returns 0
    unsigned int i=0;
    unsigned char counter=0;
    
    while(1){
        if(list[i]==','){
            counter ++;
        }
        if(list[i]==0x00){
           return counter;
        }
        i++;
    }
}

/************************************************************************************************/
//Simple function that returns the name of a given item number of a list
/************************************************************************************************/
unsigned char getListNameItem(char *list, unsigned char itemNo,  char *name){  // returns the name of the audio file number 'Item' on the given 'name' buffer; on current buffer, fails or end of items return 0, succesfull returns 1;
    unsigned int i=0;
    unsigned char counter=0,x=0;
    
    while(1){
        if(counter==itemNo){
            break;        
        }
        while(1){
            if(list[i]==','){                
                i++;
                counter ++;
                break;
            }
            if(list[i]==0x00){
                return 0;
            }
            i++;
        }
    }
    while(1){
        name[x]=list[i];
        i++;
        x++;
        if(list[i]==','){
            name[x]=0x00;
            return 1;
        }
        if(x>7){
            return 0;
        }    
    }                
}



/**************************************************************************************/
/**************************************************************************************/
/**************************************************************************************/
void setup() { //initial setup
  //This function must always be called for SMARTWAV support
  sWav.init();  //configure the serial and pinout of arduino board for SMARTWAV support
  sWav.reset(); //perform an initial reset

  pinMode(1,OUTPUT);
  pinMode(0,INPUT);  
  Serial.begin(9600);
}

void loop() { //main loop
  unsigned char x=0;

  Serial.write('E');  
          
  delay(1000);

   Serial.write(sWav.playTracks());    
  
  delay(2000);

  Serial.write(sWav.volume(0xF0));

  delay(5000);
   Serial.write(sWav.stopTrack());
  delay(500);

  
    sWav.getFileList(playList);  //get the list of current songs on the root path.  
    if(getListItems(playList)>0){ //if the number of items on the list is bigger than 0.
        getListNameItem(playList,2,songName); //we take the name of item number 2 of this list.
        Serial.write(sWav.playTrackName(songName));        //call the song by it's name.
        while(1){
          delay(1000);
          Serial.write(sWav.getStatus());            
        }

        //delay(10000);                          //we wait 10 seconds.
    }
            
    sWav.stopTrack();            //stop the track.      
    delay(100);
    sWav.playTracks();            //Start playing again any found songs on root path.
    delay(100);
    sWav.continuousPlay(ENABLE);  //Enable continuous play forever. 


 
 
  while(1);//loop forever 
}
