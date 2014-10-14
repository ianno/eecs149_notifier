/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
 /**
 *  \brief EECS149 demo
 *  \author Antonio Iannopollo
 *
 * This demo illustrates how to connect the mbed KL25Z platform to internet 
 * thorugh the CC3000 wifi breakout board (http://www.adafruit.com/product/1469).
 * Connections between the KL25Z and the CC3000 are made according to the 
 * guide (https://learn.adafruit.com/adafruit-cc3000-wifi) -- KL25Z and arduino
 * UNO are pin to pin compatible --
 *
 * This application will read a number from a remote text file and change the 
 * color of the onboard LED from green to red if the number is >0, and vice 
 * versa.
 *
 * The number in the remote file represents the number of unread emails with
 * subject equal to [EECS 149/249] in Antonio's account.
 * The remote text file is written by a server script 
 * (https://github.com/ianno/eecs149_notifier).
 *
 * This application uses the following libraries:
 * - cc3000_hostdriver_mbedsocket 
 *   (http://developer.mbed.org/users/Kojto/code/cc3000_hostdriver_mbedsocket/)
 * - HTTPClient (http://developer.mbed.org/users/donatien/code/HTTPClient/)
 */
 
#include "mbed.h"
#include "cc3000.h"
#include "main.h"

#include "HTTPClient.h"

#define MAIL_URL "http://www.eecs.berkeley.edu/~antonio/149_count.txt"


using namespace mbed_cc3000;

//KL25Z wifi connection
//we need to define connection pins for:
//- IRQ             (pin D3)
//- Enable          (pin D5)
//- SPI CS (pin D10) 
//- SPI configuration:
//  - MOSI (pin D11)
//  - MISO (pin D12)
//  - SCLK (pin D13)
//plus wifi network SSID, password, security level and smart-configuration flag.
cc3000 wifi(D3, D5, D10, SPI(D11, D12, D13), "antonio", "0123456789", WPA2, false);

//setup the serial connection to the host machine (used to print debug info)
Serial pc(USBTX, USBRX);

//global vars
HTTPClient http;
char str[10];
int num_emails;

//directly control the green and red onboard LEDs
//LEDs are internally pulled-up. To turn on a LED, set its pin to 0.
DigitalOut led_red(LED_RED);
DigitalOut led_green(LED_GREEN); 


int main() {
    
    //initially turn off both the leds.
    //Blue LED will be always on because it shares the connection with the SPI clock
    led_red = 1;
    led_green = 1;

    printf("EECS149 email notifier. \r\n");
    
    wifi.init();
    
    while(1) {
        
        //continuosly check connection status
        //If not connected, try to set up a connection
        if(wifi.is_connected() == false) {
            
            //try to connect
            if (wifi.connect() == -1) {
                printf("Failed to connect. Please verify connection details and try again. \r\n");
            } else {
                printf("IP address: %s \r\n",wifi.getIPAddress());
                
                //once connected, turn green LED on and red LED off
                led_red = 1;
                led_green = 0;  
            }
        } else {
            //if the board is connected, fetch the remote file
            
            printf("\r\nTrying to fetch mail info... \r\n");
            
            //GET remote file data
            int ret = http.get(MAIL_URL, str, 128);
            //analyze ret code
            if (!ret) {
                printf("Page fetched successfully - read %d characters \r\n", strlen(str));
                printf("Result: %s \r\n", str);
                
                num_emails = atoi(str);
                
                if(num_emails == 0) {
                    //no unread emails, turn the green LED on and red LED off
                    led_red = 1;
                    led_green = 0;    
                } else {
                    //unread emails. Turn the green LED off and the red one on
                    led_red = 0;
                    led_green = 1;    
                }
                
                
            } else {
                //error fetching remote file
                printf("Error - ret = %d - HTTP return code = %d \r\n", ret, http.getHTTPResponseCode());
            }
        }
        //poll the remote file every 5 sec
        wait(5.0);
    }
            

}
