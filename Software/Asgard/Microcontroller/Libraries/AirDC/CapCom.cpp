/**
 * CapCom.cpp - Library for Basic Air Data Communications
 * Under renew to fit the new requirements 18-04-2017
 * Created by J. Larragueta, December 3, 2015.
 * https://github.com/BasicAirData/AirDataComputer/wiki/Communication
 * http://www.basicairdata.eu/
*/
#include "CapCom.h"
#include <stdio.h>
#include <string.h>

CapCom::CapCom(int pid)
{
    /** CapCom Default
    constructor*/
    //Default parameters values
    _pid = pid;
}
/** Handles the message to the ADC
* @param  *airdata Pointer  to the AirDataComputer to use to gather data
* @param  *inmsg Pointer to message to process
* @param  *outstr Pointer to reply message
* @return Void
 */
void CapCom::HandleMessage(AirDC *airdata,char *inmsg, char*outstr)
{
    int counter = 0;
    int value = 0;
    char uDELIMITER[2];  //Delimiter string
    uDELIMITER[0]=DELIMITER;
    uDELIMITER[1]='\0';
    char workbuff[32]; //General purpose buffer, used for itoa conversion
    /*  if (Serial.available()) // If the bluetooth has received any character
      {
          while (Serial.available() && (!endmsg))   // until (end of buffer) or (newline)
          {
              *ch = Serial.read();                    // read char from serial
              if (*ch == DELIMITER)
              {
                  endmsg = true;                        // found DELIMITER
                  *ch = 0;
              }
              else ++ch;                              // increment index
          }

          if ((endmsg) && (ch != &input[0]))        // end of (non empty) message !!!
          {
    */
//Here we trap each message and react
//This section should be reordered with the most used message first
    char *command = strtok(inmsg,SEPARATOR);
//#0 - HBQ - HEARTBEAT_REQ
    if (!strcmp(command, "$HBQ"))           // Received a command, for example "$STR,1"
    {
//Receive the first field, null char added manually
        command = strtok (NULL, SEPARATOR);
        if (strlen(command)<1)
        {
            goto furout;
        }
        strncpy(_PeerDevice,command, PEERDEVICE_SIZE);
        _PeerDevice[PEERDEVICE_SIZE - 1] = '\0';
//Receive the second field, null char added manually
        command = strtok (NULL, SEPARATOR);
        if (strlen(command)<1)
        {
            goto furout;
        }
        strncpy(_PeerDeviceVer,command, PEERDEVICE_SIZE_VER);
        _PeerDeviceVer[PEERDEVICE_SIZE_VER - 1] = '\0';
        /* Answer with the #1 message HBA
                        Serial.print("$HBA,");
                        Serial.print(ADC_NAME);// Send out ADC name/Description
                        Serial.print(SEPARATOR);// Send out ADC name/Description
                        Serial.print(FIRMWARE_V);// Firmware version
                        Serial.print(SEPARATOR);// Send out ADC name/Description
                        Serial.print(_PeerDeviceVer);// Firmware version
                        Serial.write(DELIMITER);// Standard string delimiter
                        */
        //Attention, sizes are not checked
        strcpy (outstr,"$HBA,");
        strcat (outstr,ADC_NAME);
        strcat (outstr,SEPARATOR);
        strcat (outstr,FIRMWARE_V);
        strcat (outstr,SEPARATOR);
        strcat (outstr,_PeerDeviceVer);
        strcat (outstr,uDELIMITER);
    }


//Service message
    if (!strcmp(command, "$STR"))           // Received a command, for example "$STR,1"
    {
        command = strtok (NULL, SEPARATOR);
        value = atoi(command);                // Read the value after the comma, for example "1". Integer conversion

        // If we receive "$STR,1" the ADC switch to sensor test mode. Starts tests and reports.
        // if (value == 1) testme();

        counter = counter + value;
        if (counter > 1000) counter = 0;      // A reset, for the serial gamers :)
        //--------------------------------------------------------------------------

        /*          // Answer with a string
                  Serial.print("$ANS,");
                  Serial.print(counter);
                  Serial.write(DELIMITER);
                  */
        strcpy (outstr,"$ANS,");
        itoa(counter,workbuff,10);
        strcat (outstr,workbuff);
        strcat (outstr,uDELIMITER);
    }
//strncopy(outstr,inmsg,OUTPUT_SIZE);  //For test purposes the output is equal to input
furout:;
}

/*
        if (endmsg)
        {
            endmsg = false;
            *ch = 0;
            ch = &input[0];                         // Return to first index, ready for the new message;
        }


    }

  */

