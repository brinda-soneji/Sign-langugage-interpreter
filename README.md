# Sign Langugage Interpreter Using MediaPipe And ANN
This project is a real-time sign language interpreter(asl) that uses Mediapipe to extract 2D hand landmark coordinates and classifies them using an Artificial Neural Network (ANN). The output is displayed on a dot matrix, and users can also look up the meanings of the predicted words through a built-in dictionary.

## Features
. Real-time hand gesture recognition  
. ANN-based classification using only hand landmarks  
. Highly inclusive — does not rely on skin color, texture, size, or lighting  
. Mediapipe-powered hand tracking for precise 2D landmark extraction  
. Dot Matrix output for visual feedback  
. Integrated dictionary for meaning lookup of recognized signs  
. Multithreaded interface for faster, smoother, real-time performance    

## Tech Stack
### Software:
. Python  
. OpenCV(CV2), Mediapipe (Hand Landmarks)  
. NumPy, Pandas, Joblib,  TensorFlow, Keras     
. Multithreading  

### Hardware:
.ESP32 Microcontroller  
.MAX7219 LED Matrix Display (32x8)  
.Libraries Used in Arduino (.ino) Code:MD_MAX72XX – for controlling the LED matrix, MD_Parola – for scrolling and text effects, WiFi.h – to connect ESP32 to Wi-Fi

## How It Works
. Mediapipe detects and tracks the hand.  
. 2D landmark coordinates are extracted (21 points).  
. These points are passed through a trained ANN classifier.  
. The predicted word is shown on-screen and printed via a dot matrix.  
. Users can click to look up the meaning of the predicted word.

