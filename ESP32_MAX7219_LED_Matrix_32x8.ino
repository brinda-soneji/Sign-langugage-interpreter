//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 05_ESP32_MAX7219_LED_Matrix_32x8_Web_Server_SM
//---------------------------------------- Including Libraries.
#include <WiFi.h>
#include <WebServer.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <Preferences.h>
#include "PageIndex.h" 
//---------------------------------------- 

// hardware type.
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
//#define HARDWARE_TYPE MD_MAX72XX::GENERIC_HW

// Defining size, and output pins.
#define MAX_DEVICES 4
#define CS_PIN 5

//----------------------------------------Defining the key.
// "Key" functions like a password. In order to change the text on the MAX7219 LED Matrix 32x8, the user must know the "key".
#define key_Txt "uscore"
//----------------------------------------

//----------------------------------------Variable declaration for network credentials.
const char* ssid = "vivo V25 Pro";  //--> wifi name, you need to enter your ssid and password here to configure your esp32 
const char* password = "pass"; //--> wifi password
//----------------------------------------

int cnt_Text = 0;

char show_Text[101] = "";
char Text_1[101] = "";
char Text_2[101] = "";
char Text_3[101] = "";
char Text_4[101] = "";

// Initialization of "MD_Parola" library as "Display" and its settings.
MD_Parola Display = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

// Initialize the "Preferences" library as "preferences".
Preferences preferences;

// Server on port 80.
WebServer server(80);

//________________________________________________________________________________ handleRoot()
// This routine is executed when you open ESP32 IP Address in browser.
void handleRoot() {
  server.send(200, "text/html", MAIN_page); //Send web page
}
//________________________________________________________________________________

//________________________________________________________________________________ handleSettings().
// Subroutine to handle settings. The displayed text are set here.
void handleSettings() {
  String incoming_Settings;
  int incoming_Settings_Length;

  // Check for the key first
  incoming_Settings = server.arg("key");

  Serial.println();
  Serial.println("------------Settings");
  Serial.print("Key : ");
  Serial.println(incoming_Settings);

  // Conditions for checking keys
  if (incoming_Settings == key_Txt) {
    // Check if the incoming command is "set"
    incoming_Settings = server.arg("sta");
    
    if (incoming_Settings == "set") {
      //  code for handling 'Text1', 'Text2', 'Text3', 'Text4'
      
      // This part remains the same for saving settings
    }
    
    // Check if the incoming command is "word" for displaying the predicted word from Python
    if (server.hasArg("word")) {
      String word = server.arg("word");
      Serial.println("Received word: " + word);

      // Update the display with the predicted word
      strcpy(show_Text, word.c_str());  // Copy the word into the show_Text variable
      Display.displayReset();  // Reset the display before showing new text
      Display.displayScroll(show_Text, PA_RIGHT, PA_SCROLL_LEFT, 75);  // Scroll the predicted word

      server.send(200, "text/plain", "+OK");  // Send success response
    }
    else if (incoming_Settings == "get") {
      // Existing code to get texts
    }
  } else {
    // Handle incorrect key
    server.send(200, "text/plain", "+ERR");
  }
}

//________________________________________________________________________________

//________________________________________________________________________________ read_Texts_from_Flash_Memory()
void read_Texts_from_Flash_Memory() {
  Serial.println();
  Serial.println("------------Read texts from flash memory.");
    
  String saved_Text = "";
  int saved_Text_Length;

  saved_Text = preferences.getString("TXT_1", "");
  saved_Text_Length = saved_Text.length() + 1;
  saved_Text.toCharArray(Text_1, saved_Text_Length);

  saved_Text = preferences.getString("TXT_2", "");
  saved_Text_Length = saved_Text.length() + 1;
  saved_Text.toCharArray(Text_2, saved_Text_Length);

  saved_Text = preferences.getString("TXT_3", "");
  saved_Text_Length = saved_Text.length() + 1;
  saved_Text.toCharArray(Text_3, saved_Text_Length);

  saved_Text = preferences.getString("TXT_4", "");
  saved_Text_Length = saved_Text.length() + 1;
  saved_Text.toCharArray(Text_4, saved_Text_Length);

  Serial.print("Text_1 text   : "); Serial.println(Text_1);
  Serial.print("Text_1 length : "); Serial.println(strlen(Text_1));

  Serial.println();
  Serial.print("Text_2 text   : "); Serial.println(Text_2);
  Serial.print("Text_2 length : "); Serial.println(strlen(Text_2));

  Serial.println();
  Serial.print("Text_3 text   : "); Serial.println(Text_3);
  Serial.print("Text_3 length : "); Serial.println(strlen(Text_3));

  Serial.println();
  Serial.print("Text_4 text   : "); Serial.println(Text_4);
  Serial.print("Text_4 length : "); Serial.println(strlen(Text_4));
  Serial.println("------------");
}
//________________________________________________________________________________

//________________________________________________________________________________ save_Text_to_Flash_Memory()
void save_Text_to_Flash_Memory() {
  Serial.println();
  Serial.println("Save text to flash memory.");
  
  preferences.putString("TXT_1", Text_1);
  preferences.putString("TXT_2", Text_2);
  preferences.putString("TXT_3", Text_3);
  preferences.putString("TXT_4", Text_4);
}
//________________________________________________________________________________

//________________________________________________________________________________ set_Scrolling_Text_Display()
void set_Scrolling_Text_Display() {
  Serial.println();
  Serial.println("Set Scrolling Text Display.");
  
  Display.begin();
  delay(500);
  Display.setIntensity(0);
  Display.setInvert(false);
  Display.displayScroll(show_Text, PA_RIGHT, PA_SCROLL_LEFT, 75);
}
//________________________________________________________________________________ 

//________________________________________________________________________________ Scrolling_Text()
void Scrolling_Text() {
  if (Display.displayAnimate()) {
    cnt_Text++;
    if (cnt_Text > 4) {
      cnt_Text = 1;
    }
    
    if (cnt_Text == 1) {
      if (strlen(Text_1) > 0) {
        strcpy(show_Text, Text_1);
      } else {
        cnt_Text++;
      }
    }
    if (cnt_Text == 2) {
      if (strlen(Text_2) > 0) {
        strcpy(show_Text, Text_2);
      } else {
        cnt_Text++;
      }
    }
    if (cnt_Text == 3) {
      if (strlen(Text_3) > 0) {
        strcpy(show_Text, Text_3);
      } else {
        cnt_Text++;
      }
    }
    if (cnt_Text == 4) {
      if (strlen(Text_4) > 0) {
        strcpy(show_Text, Text_4);
      } else {
        cnt_Text++;
      }
    }
    if (cnt_Text > 4 || cnt_Text == 0) {
      return;
    }

    Serial.println();
    Serial.print("Scroll Text_");
    Serial.print(cnt_Text);
    Serial.println(".");
    
    delay(2000);
    Display.displayReset();
  }
}
//________________________________________________________________________________ 

//________________________________________________________________________________ VOID SETUP()
void setup() {
  // put the setup code here, to run once:
  
  Serial.begin(115200);
  Serial.println();
  delay(2000);

  preferences.begin("myText", false);

  delay(500);

  read_Texts_from_Flash_Memory();

  delay(500);

  // Calls the "set_Scrolling_Text_Display()" subroutine.
  set_Scrolling_Text_Display();
  
  delay(500);

  //----------------------------------------Set Wifi to STA mode.
  Serial.println();
  Serial.println("-------------");
  Serial.println("WIFI mode : STA");
  WiFi.mode(WIFI_STA);
  Serial.println("-------------");
  //---------------------------------------- 

  //----------------------------------------Connect to Wi-Fi (STA).
  Serial.println();
  Serial.println("------------");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  //:::::::::::::::::: The process of connecting ESP32 with WiFi Hotspot / WiFi Router.
  // The process timeout of connecting ESP32 with WiFi Hotspot / WiFi Router is 20 seconds.
  // If within 20 seconds the ESP32 has not been successfully connected to WiFi, the ESP32 will restart.
  // I made this condition because on my ESP32, there are times when it seems like it can't connect to WiFi, so it needs to be restarted to be able to connect to WiFi.
  
  int connecting_process_timed_out = 20; //--> 20 = 20 seconds.
  connecting_process_timed_out = connecting_process_timed_out * 2;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    if(connecting_process_timed_out > 0) connecting_process_timed_out--;
    if(connecting_process_timed_out == 0) {
      Serial.println();
      Serial.println("Failed to connect to WiFi. The ESP32 will be restarted.");
      Serial.println("------------");
      delay(1000);
      ESP.restart();
    }
  }
  
  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
  Serial.println("------------");
  //:::::::::::::::::: 
  //---------------------------------------- 

  delay(1000);

  //---------------------------------------- Setting the server.
  server.on("/", handleRoot); 
  server.on("/settings", handleSettings);
  //----------------------------------------

  //---------------------------------------- Start server.
  server.begin(); 
  Serial.println();
  Serial.println("HTTP server started");
  //----------------------------------------

  delay(500);

  // Open the IP address in the browser to open the interface page.
  // Make sure that the computer / mobile device and ESP32 are connected to the same WiFi network.
  Serial.println();
  Serial.print("IP address : ");
  Serial.println(WiFi.localIP());
  Serial.println("Open the IP address in your browser to open the interface page.");
  Serial.println("Make sure that your computer / mobile device and");
  Serial.println("ESP32 are connected to the same WiFi network.");

  delay(500);

  // Call the "set_Scrolling_Text_Display()" subroutine again.
  set_Scrolling_Text_Display();
  
  delay(500);
}
//________________________________________________________________________________

//________________________________________________________________________________ VOID LOOP()
void loop() {
  // put the main code here, to run repeatedly:

  // Handle client requests.
  server.handleClient();  

  Scrolling_Text();
}
//________________________________________________________________________________
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<Flash_Memory();

  delay(500);

  // Calls the "set_Scrolling_Text_Display()" subroutine.
  set_Scrolling_Text_Display();
  
  delay(500);

  //----------------------------------------Set Wifi to STA mode.
  Serial.println();
  Serial.println("-------------");
  Serial.println("WIFI mode : STA");
  WiFi.mode(WIFI_STA);
  Serial.println("-------------");
  //---------------------------------------- 

  //----------------------------------------Connect to Wi-Fi (STA).
  Serial.println();
  Serial.println("------------");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  //:::::::::::::::::: The process of connecting ESP32 with WiFi Hotspot / WiFi Router.
  // The process timeout of connecting ESP32 with WiFi Hotspot / WiFi Router is 20 seconds.
  // If within 20 seconds the ESP32 has not been successfully connected to WiFi, the ESP32 will restart.
  // I made this condition because on my ESP32, there are times when it seems like it can't connect to WiFi, so it needs to be restarted to be able to connect to WiFi.
  
  int connecting_process_timed_out = 20; //--> 20 = 20 seconds.
  connecting_process_timed_out = connecting_process_timed_out * 2;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    if(connecting_process_timed_out > 0) connecting_process_timed_out--;
    if(connecting_process_timed_out == 0) {
      Serial.println();
      Serial.println("Failed to connect to WiFi. The ESP32 will be restarted.");
      Serial.println("------------");
      delay(1000);
      ESP.restart();
    }
  }
  
  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
  Serial.println("------------");
  //:::::::::::::::::: 
  //---------------------------------------- 

  delay(1000);

  //---------------------------------------- Setting the server.
  server.on("/", handleRoot); 
  server.on("/settings", handleSettings);
  //----------------------------------------

  //---------------------------------------- Start server.
  server.begin(); 
  Serial.println();
  Serial.println("HTTP server started");
  //----------------------------------------

  delay(500);

  // Open the IP address in your browser to open the interface page.
  // Make sure that the computer / mobile device and ESP32 are connected to the same WiFi network.
  Serial.println();
  Serial.print("IP address : ");
  Serial.println(WiFi.localIP());
  Serial.println("Open the IP address in your browser to open the interface page.");
  Serial.println("Make sure that your computer / mobile device and");
  Serial.println("ESP32 are connected to the same WiFi network.");

  delay(500);

  // Call the "set_Scrolling_Text_Display()" subroutine again.
  set_Scrolling_Text_Display();
  
  delay(500);
}
//________________________________________________________________________________

//________________________________________________________________________________ VOID LOOP()
void loop() {
  // put the main code here, to run repeatedly:

  // Handle client requests.
  server.handleClient();  

  Scrolling_Text();
}
//________________________________________________________________________________
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
