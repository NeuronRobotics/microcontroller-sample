/*
 * File:   main.c
 * Author: hephaestus
 *
 * Created on January 4, 2012, 5:48 PM
 */

/******************************************************************************
 * Software License Agreement
 *
 * Copyright � 2011 Microchip Technology Inc.  All rights reserved.
 * Microchip licenses to you the right to use, modify, copy and distribute
 * Software only when embedded on a Microchip microcontroller or digital
 * signal controller, which is integrated into your product or third party
 * product (pursuant to the sublicense terms in the accompanying license
 * agreement).
 *
 * You should refer to the license agreement accompanying this Software
 * for additional information regarding your rights and obligations.
 *
 * SOFTWARE AND DOCUMENTATION ARE PROVIDED ìAS ISî WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY
 * OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR
 * PURPOSE. IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR
 * OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,
 * BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT
 * DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL,
 * INDIRECT, PUNITIVE OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA,
 * COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY
 * CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF),
 * OR OTHER SIMILAR COSTS.
 *
 *****************************************************************************/


#include "UserApp.h"

#ifdef USB_A0_SILICON_WORK_AROUND
	#pragma config UPLLEN   = OFF       // USB PLL Enabled (A0 bit inverted)
#else
	#pragma config UPLLEN   = ON        // USB PLL Enabled
#endif

#pragma config FPLLMUL  = MUL_20        // PLL Multiplier
#pragma config UPLLIDIV = DIV_2         // USB PLL Input Divider
#pragma config FPLLIDIV = DIV_2         // PLL Input Divider
#pragma config FPLLODIV = DIV_1         // PLL Output Divider
#pragma config FPBDIV   = DIV_1         // Peripheral Clock divisor
#pragma config FWDTEN   = OFF           // Watchdog Timer
#pragma config WDTPS    = PS1           // Watchdog Timer Postscale
#pragma config FCKSM    = CSDCMD        // Clock Switching & Fail Safe Clock Monitor
#pragma config OSCIOFNC = OFF           // CLKO Enable
#pragma config POSCMOD  = HS            // Primary Oscillator
#pragma config IESO     = ON            // Internal/External Switch-over
#pragma config FSOSCEN  = ON            // Secondary Oscillator Enable (KLO was off)
#pragma config FNOSC    = PRIPLL        // Oscillator Selection
#pragma config CP       = OFF           // Code Protect
#pragma config BWP      = OFF           // Boot Flash Write Protect
#pragma config PWP      = OFF           // Program Flash Write Protect
#pragma config ICESEL   = ICS_PGx2      // ICE/ICD Comm Channel Select
#pragma config DEBUG    = OFF            // Background Debugger Enable

//These are the call backs. Fill these in to catch custom packets.
BYTE UserGetRPCs(BowlerPacket *Packet){
    return 0;
}
BYTE UserPostRPCs(BowlerPacket *Packet){
    return 0;
}
BYTE UserCriticalRPCs(BowlerPacket *Packet){
    return 0;
}

#define initLed()	_TRISD0 = OUTPUT;_TRISD1 = OUTPUT;_TRISD2 = OUTPUT;
#define initButton() 	_TRISD6  = INPUT;_TRISD7  = INPUT;_TRISD13 = INPUT;CNPUESET=0x00098000;
#define isPressed()	( _RD6==0 || _RD7==0 || _RD13==0)
#define setLed(a,b,c) 	_RD0=a;_RD1=b;_RD2=c;

#define server_mode

BYTE SPITransceve(BYTE b){
    SpiChnPutC(2, b);		// send data on the master channel, SPI1
    Delay1us(10);
    return SpiChnGetC(2);	// get the received data
}
BYTE get(BYTE b){
	BYTE back = SPITransceve(b);
        //Delay10us(60);
	return back;
}


int main(void) {
    Bowler_Init();
    char * dev = "NR Sample Serial Port";
    char * ser = "FF00FF00FF00";
    usb_CDC_Serial_Init(dev,ser,0x04D8,0x0001);
    //Declare a packet
    BowlerPacket Packet;
    //Declare a storage data for a periodic event
    //This will run every 1000 ms
    RunEveryData blink ={getMs(),1000};
    BOOL blinkState = TRUE;
    //Initialize the button and LED hardware
    initButton();
    initLed();
    const char * start = "Demo_Program!\r\n";

    addNamespaceToList((NAMESPACE_LIST *)getBcsSampleNamespace());

    
    while (1){
#if defined(server_mode)
                //This is how to run a bowler server on the USB and serial port
                //Keep checking the server in the main loop
                //You can add custom code here for co-operative operation
                Bowler_Server(&Packet, FALSE);
#else
        //Do something with the buttons
        if(!isPressed()){
            //For direct access to the USB, you can access the USB API directly
            int size = GetNumUSBBytes();
            if(size>0){
                //We have some data, read it in
                size = USBGetArray(Packet.stream, size);
                //then mirror it back out
                USBPutArray(Packet.stream, size);
                
            }
            //This will return false if it has not been enough time since last time it ran
            if ((RunEvery(&blink)>0)){
                //Set the LEDs to the oppsite state
                setLed(blinkState,blinkState,blinkState);
                blinkState = blinkState?FALSE:TRUE;
            }
        }else{
            if(!_RD6){
                int size=0;
                float sum=getAdcVoltage(0,10);// reads ADC channel 0 to a variable. Takes 10 samples
                OpenSPI2(   CLK_POL_ACTIVE_HIGH\
                            |SPI_MODE8_ON|ENABLE_SDO_PIN|SLAVE_ENABLE_OFF|SPI_CKE_OFF\
                            |MASTER_ENABLE_ON|SEC_PRESCAL_3_1|PRI_PRESCAL_4_1
                            , SPI_ENABLE); // set u master


                SPITransceve(0xff);


                while(start[size++]);//Calculates the length of the null terminated string
                USBPutArray((BYTE *)start, size);

            }
            if(!_RD7){
                //This is how to run a bowler server on the USB and serial port
                //Keep checking the server in the main loop
                //You can add custom code here for co-operative operation
                Bowler_Server(&Packet, FALSE);
            }
            //mirror the buttons on the LED's
            setLed(!_RD6,!_RD7,!_RD13);
        }
#endif
    }

}


