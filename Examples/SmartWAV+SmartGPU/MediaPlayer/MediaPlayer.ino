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

#include <SMARTGPU.h>     //include the SMARTGPU library!
#include <SMARTWAV.h>     //include the SMARTWAV library!

SMARTGPU lcd;             //create our object called LCD
SMARTWAV sWav;            //create our object called sWAV (smartWAV)

//Each time we use the touchscreen we must define a int array that stores the X and Y readed or touched coordinates.
int touch[2];
//Each time we use the touchicon we must define a char array that stores the name of the touched icon.
char icon[3];

//Global Variables
char playList[100];     //an array for storing the retrieved list of Audio files
char dirList[100];     //an array for storing the retrieved list of Folders/Dirs
char songName[9];       // 8 characters + null character
char time[6]={0};         //array to store the time
unsigned char audioItems=0;      //variable storing the total of audio files on the 'playlist' array
unsigned char dirItems=0;      //variable storing the total of Folders/Dirs on the 'dirList' array
unsigned char totalAudioItems=0;
unsigned char songPointer=0;
unsigned char minutes=0,seconds=0;
  
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

/************************************************************************************************/
/************************************************************************************************/
//This function gets the existing folders and songs on the microSD root, and displays them
void getData(){
  unsigned char x,y;
  char name[9]={0}; 
  
  delay(50);
  sWav.getFolderList(dirList); 
  delay(50);
  sWav.getFileList(playList);
  audioItems=getListItems(playList);
  dirItems=getListItems(dirList);
  
  if(dirItems>8){  //Show up to 8 Folders/Dirs
    dirItems=8;  
  }

  if(audioItems>8){  //Show up to 8 audio Items
    audioItems=8;  
  }
  //display the folder items  
  for(x=0;x<dirItems;x++){
    getListNameItem(dirList,x,name);
    lcd.string(15,70+(x*20),319,239,YELLOW,FONT3,UNFILL,name);    
  }
  //display the audio items
  for(x=0;x<audioItems;x++){
    getListNameItem(playList,x,name);
    lcd.string(170,70+(x*20),319,239,YELLOW,FONT3,UNFILL,name);    
  }
  //get true total items
  audioItems=getListItems(playList);
  totalAudioItems=audioItems;
  dirItems=getListItems(dirList);  
}

/************************************************************************************************/
/************************************************************************************************/
//This function search for a song and play it, also displays the song artwork.
void searchSong(){
  char name[9]={0};
  
  if(songPointer>=totalAudioItems){//if we reached the end of items
    songPointer=0;  
  }
  while(1){
    getListNameItem(playList,songPointer,name); //get a song file name
    if(sWav.playTrackName(name)=='O'){
      lcd.imageSD(80,25,name);
      lcd.drawRectangle(10,8,75,22,0x0926,FILL);       //erase previous song name
      lcd.string(10,8,319,239,WHITE,FONT3,FILL,name);  //draw song name
      lcd.string(265,8,319,239,WHITE,FONT2,FILL,"00:00"); //draw the time 
      lcd.imageSD(146,208,"pause");        
      minutes=0;
      seconds=0; 
      songPointer++;                                     //increase song pointer
      break;
    }else{
      songPointer++;                                     //increase song pointer
      delay(100);      
    }
  }
}

/**************************************************/
/**************************************************/
//Initial setup
void setup() { //initial setup
  //Those two functions must always be called for SMARTGPU support
  lcd.init();  //configure the serial and pinout of arduino board for SMARTGPU support
  lcd.start(); //initialize the SMARTGPU processor
  //Those two functions must always be called for SMARTWAV support
  sWav.init();  //configure the serial and pinout of arduino board for SMARTWAV support
  sWav.reset(); //perform an initial reset  

  //those next instructions are never needed...
  pinMode(5,OUTPUT);        //we set pin5 to be our GND reference for SmartWAV + SmartGPU connection
  digitalWrite(5, LOW);     //set the pin low to use as GND reference for SmartWAV 
}

/**************************************************/
/****************** MAIN LOOP *********************/
/**************************************************/
/**************************************************/
void loop() { //main loop
  unsigned int tick=0;   //counter to advance seconds
  
  lcd.baudChange(2000000);         //set high baud for advanced applications
  
  //load main playlist 
  lcd.imageSD(0,0,"List");    
  lcd.string(5,5,239,239,GREEN,FONT3,UNFILL,"Library Play List:");
  lcd.string(5,40,239,239,GREEN,FONT3,UNFILL,"Folders/Dirs:");    
  lcd.string(160,40,319,239,GREEN,FONT3,UNFILL,"Songs/Audio Files:");  
  getData();  //get folders and songs and display them
  
  //wait for a touch on screen to start playing songs
  while(lcd.touchScreen(touch)==0);      
  lcd.imageSD(0,0,"player");
  lcd.drawLine(230,222,271,222,YELLOW);
  lcd.imageSD(266,217,"ball");
  lcd.setTextBackground(0x0926);
  
  while(1){
    // search for a song and play it + show artwork
    searchSong();
    
    while(1){
      //wait for a touch on screen to perform something
      while(lcd.touchScreen(touch)==0 | touch[YCOORD]<200){
        tick++;  
        if(tick > 500){   //if about 1 second have passed- this isn't accurate, the best way to achieve 1 second is by timer interrupts
          tick=0;              //reset counter
          seconds++;
          if(seconds>59){
            minutes++;  
            seconds=0;            
          }
          time[0]=(minutes/10)+0x30;  //get the tens of the minutes and convert them to ascii
          time[1]=(minutes%10)+0x30;  //get the ones of the minutes and convert them to ascii
          time[2]=':';
          time[3]=(seconds/10)+0x30;  //get the tens of the seconds and convert them to ascii
          time[4]=(seconds%10)+0x30;  //get the ones of the seconds and convert them to ascii
          lcd.string(265,8,319,239,WHITE,FONT2,FILL,time); //draw the time             

          if(sWav.getStatus()!=0x01){ //if no song is being played-means end of song
            delay(500);     
            sWav.stopTrack();
            delay(1000);              
            searchSong();         //play next songpointer song
          }
        }
      } 

      //decode touch into buttons
      if(touch[XCOORD]>90 & touch[XCOORD]<117){//touch on STOP button
        sWav.stopTrack();
        lcd.imageSD(146,208,"play");
        lcd.string(265,8,319,239,WHITE,FONT2,FILL,"00:00"); //draw the time         
        delay(1000);
        //wait for a touch on play button to resume song play-back
        while(lcd.touchScreen(touch)==0 | !(touch[XCOORD]>144 & touch[XCOORD]<173)); 
        break;        
      }else if(touch[XCOORD]>116 & touch[XCOORD]<145){//touch on REWIND button
        sWav.rewindTrack();  
        lcd.string(265,8,319,239,WHITE,FONT2,FILL,"00:00"); //draw the time         
        minutes=0;
        seconds=0;        
      }else if(touch[XCOORD]>144 & touch[XCOORD]<173){//touch on PAUSE/PLAY button
        sWav.pausePlay();
        lcd.imageSD(146,208,"play");
        delay(1000);
        //wait for a touch on pausePlay button to resume song play-back
        while(lcd.touchScreen(touch)==0 | !(touch[XCOORD]>144 & touch[XCOORD]<173));
        sWav.pausePlay();
        lcd.imageSD(146,208,"pause");         
      }else if(touch[XCOORD]>172 & touch[XCOORD]<202){//touch on NEXT button
        sWav.stopTrack();
        delay(200);
        break;
      }else if(touch[XCOORD]>229 & touch[XCOORD]<272){//touch on VOLUME
        lcd.drawLine(touch[XCOORD],222,271,222,BLACK);
        lcd.drawLine(230,222,touch[XCOORD],222,YELLOW);
        lcd.imageSD(touch[XCOORD]-4,217,"ball");
        sWav.volume(214+ (touch[XCOORD]-230)); 
      }
      delay(200);  
   }
  } 
}
