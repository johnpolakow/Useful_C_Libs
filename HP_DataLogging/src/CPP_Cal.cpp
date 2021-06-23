#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>     //UNIX Standard function definitions
#include <fcntl.h>      //File control
#include <errno.h>      //Error number def
#include <termios.h>    //POSIX terminal control
#include <termio.h>
#include <sys/ioctl.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <net/if.h>

#ifdef __arm__
    #include <bcm2835.h>
#endif



#include "HP34401.h"
//#include "Calibrate.h"

#include "Write_Log.h"
#include "DataLog.h"
#include "E3648.h"
#include "TStamp.h"
#include "Menu.h"
#include "USB_Helper.h"
#include "Cal_Parameters.h"

extern const char* HP_A_ID;
extern const char* HP_B_ID;




#define LABEL1 "COOLER_DRV"     // short text describing measurements on HP_A
#define LABEL2 "IDCABOT"        // short text describing measurements on HP_A

const meas_t HP_A_measuremnt = AC_AMPS;
const meas_t HP_B_measuremnt = AC_VOLTS;

static const char* FILENAME = "IDCABOT_P1";
static const char* FILE_COMMENT = "CCC Pos #1";


void DataLog(HP34401* HP_A, HP34401* HP_B, E3648* E3648A, Write_Log* FileWrite)
{
    //Menu::Configure_Logging_Options
    HP_B->Set_Description(LABEL2);
    HP_B->Set_Measurement_Type(HP_B_measuremnt);
    HP_B->Enable_Logging();
    //HP_A->Disable_Logging();

    HP_A->Set_Description(LABEL1);
    HP_A->Set_Measurement_Type(HP_A_measuremnt);
    HP_A->Enable_Logging();
    //HP_B->Disable_Logging();

    E3648A->Enable_Control(true);

    DataLog::Setup_Instruments(HP_A, HP_B, E3648A);   
    DataLog::Start_Logging(HP_A, HP_B, E3648A, FileWrite);
}


int main( int argc, char** argv )
{

  Timestamp* tstamp = new Timestamp();
  char* timestamp_now = tstamp->toString();
  Write_Log* FileWrite = new Write_Log(FILENAME, FILE_COMMENT);
  free(timestamp_now);
  delete tstamp;


  E3648* E3648A = NULL;
  printf("\n---------------------------------\n\n");

  USB_Device** dev_list = USB::Search_ttyUSB_Devices(); // searches the linux device tree for ttyUSB[x] using shell command: "ls /dev | grep ttyUSB"
                                                        // opens serial port to device, and queries it for mfr, ID string, and Model

  if( dev_list == NULL)
  {
      printf(COLOR_BOLD_RED "CANNOT DATALOG w/ NO METERS,  exiting\n" COLOR_RESET);
      exit(0);
  }

  Meter* meter1 = NULL;
  Meter* meter2 = NULL;

  HP34401* HP_A = NULL;
  HP34401* HP_B = NULL;

  int index = 0;
  while( dev_list[index] != NULL)   // process each device returned from the 'Search_ttyUSB_Devices()' command
  {                                 // if the device ID string matches one in our pre-defined list, we assign that device to its proper class object
                                    // possible models are HP34401A, HP34401B, and E3648A
      USB_Device* dev = dev_list[index];
      char* ID = dev->ID_string;
      char* mfr = dev->manufacturer;
      char* model = dev->model;

      if( StringH::String_Contains_Ignore_Case(mfr, "HEWLETT-PACKARD"))
      {
          // device must match one of two IDs. We know the IDs of the HP meters already
          if( StringH::String_Contains_Ignore_Case(ID, HP_A_ID))        // if the USB device matches the HP_A ID, which is "0,11-5-2"
          {                   
              meter1 = new Meter(dev->file_descriptor, dev->device_path, ID, "HP34401A", dev->manufacturer, dev->model); 
              HP_A = new HP34401(meter1);
          }
          else if( StringH::String_Contains_Ignore_Case(ID, HP_B_ID))   // if the USB device matches the HP_B ID, which is "0,2-1-1"
          {
              meter2 = new Meter(dev->file_descriptor, dev->device_path, ID, "HP34401B", dev->manufacturer, dev->model); 
              HP_B = new HP34401(meter2);
          }
      }
      else if( StringH::String_Contains_Ignore_Case(mfr, "AGILENT"))
      {
        // assign info from this device to E3648A
        E3648A = new E3648(dev->file_descriptor, dev->device_path, dev->manufacturer, dev->model, dev->ID_string);
      }
      ++index;
  }

  USB::Free_Mem_Device_List(dev_list);  // free memory dynamically allocated to hold objects from the 'Search_ttyUSB_Devices()' command

  if( !( HP_A->HP_meter->file_descriptor > 0 && HP_B->HP_meter->file_descriptor > 0 ) )   // if file descriptors haven't been assigned to BOTH HP meters, throw an error
  {
      if( HP_A->HP_meter->file_descriptor < 0)
         printf("\t(" COLOR_BOLD_RED "HP_A" COLOR_RESET " not detected)\n");
      if( HP_B->HP_meter->file_descriptor < 0)
         printf("\t(" COLOR_BOLD_RED "HP_B" COLOR_RESET " not detected)\n");
      printf("\n CANNOT CONTINUE\n");
  }
  if( E3648A == NULL || !(E3648A->file_descriptor > 0) )                       // if E3648A file descriptor hasnt been assigned (could not connect to the device), throw an error
  {
      printf("\nDID NOT DETECT AGILENT POWER SUPPLY\n\n" COLOR_RESET);
  }

    Menu* menu = new Menu();
    menu->Configure_Logging_Options(HP_A, HP_B, E3648A);
    DataLog(HP_A, HP_B, E3648A, FileWrite);


  //HP34401_Display_Text(&HP34401A, "  --DONE--");
  //HP34401_Display_Text(&HP34401B, "  --DONE--");

  delete menu;
  
  

  // Cleanup Allocated Memory
  if( HP_A != NULL){ close(HP_A->HP_meter->file_descriptor); delete HP_A;}
  if( HP_B != NULL){ close(HP_B->HP_meter->file_descriptor); delete HP_B; }
  if( meter1 != NULL) delete meter1;
  if( meter2 != NULL) delete meter2;
  if( E3648A != NULL) { close(E3648A->file_descriptor); delete E3648A; }
  if( FileWrite != NULL) delete FileWrite;

  return 0;
}
