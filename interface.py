import cv2
import mediapipe as mp
import numpy as np
from tensorflow.keras.models import load_model
import joblib
import time
import requests
import threading

# Load model and label encoder
model = load_model('/home/brinda/Documents/hand gesture recognition/sign_language_model.keras')
label_encoder = joblib.load('/home/brinda/Documents/hand gesture recognition/sign_label_encoder.pkl')

# Setup MediaPipe
mp_hands = mp.solutions.hands
hands = mp_hands.Hands(static_image_mode=False, max_num_hands=1)
mp_drawing = mp.solutions.drawing_utils

# ESP32 config
esp32_url = 'http://192.168.2.70/settings'

# Globals
predicted_text = ""
last_predicted_char = None
last_prediction_time = 0
last_hand_seen_time = time.time()
word_sent = False
show_button = False
last_sent_word = ""
meaning_text = ""
button_width, button_height = 200, 40
button_coords = (0, 0, 0, 0)
last_click_time = 0  # For debouncing mouse clicks
ignore_prediction_count = 0  # <-- NEW:frame count to ignore

def send_word_to_esp(word):
    print("Sending word to ESP32:", word)
    try:
        params = {'key': 'uscore', 'sta': 'set', 'word': word}
        response = requests.get(esp32_url, params=params, timeout=2)
        if response.status_code == 200:
            print("Word sent successfully!")
        else:
            print("Failed to send word. Status code:", response.status_code)
    except Exception as e:
        print("Error sending word:", e)

def fetch_meaning(word):
    global meaning_text
    print(f"Fetching meaning for word: {word}")
    try:
        response = requests.get(f"https://api.dictionaryapi.dev/api/v2/entries/en/{word.lower()}")
        if response.status_code == 200:
            data = response.json()
            meaning_text = data[0]['meanings'][0]['definitions'][0]['definition']
            print(f"Meaning fetched: {meaning_text}")
        else:
            meaning_text = "Definition not found."
            print("Definition not found.")
    except Exception as e:
        meaning_text = f"Error fetching meaning: {str(e)}"
        print(meaning_text)

def mouse_click(event, x, y, flags, param):
    global meaning_text, last_sent_word, show_button, last_click_time

    current_time = time.time()
    if current_time - last_click_time < 1:  # 1 second debounce
        return
    last_click_time = current_time

    y_offset = cap.get(cv2.CAP_PROP_FRAME_HEIGHT)
    y_adjusted = y - int(y_offset)

    print(f"Mouse clicked at: ({x}, {y}) [adjusted y: {y_adjusted}]")
    x1, y1, x2, y2 = button_coords

    if show_button and x1 <= x <= x2 and y1 <= y_adjusted <= y2:
        threading.Thread(target=fetch_meaning, args=(last_sent_word,)).start()

# Webcam setup
cap = cv2.VideoCapture(0)
cv2.namedWindow("Hand Gesture Recognition", cv2.WINDOW_NORMAL)
cv2.setWindowProperty("Hand Gesture Recognition", cv2.WND_PROP_FULLSCREEN, cv2.WINDOW_FULLSCREEN)
cv2.setMouseCallback("Hand Gesture Recognition", mouse_click)

while True:
    ret, frame = cap.read()
    if not ret:
        break

    frame = cv2.flip(frame, 1)
    rgb_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    results = hands.process(rgb_frame)
    frame_output = frame.copy()
    frame_height, frame_width, _ = frame_output.shape

    extra_height = 200
    bottom_panel = np.zeros((extra_height, frame_width, 3), dtype=np.uint8)

    x1 = frame_width - button_width - 20
    y1 = 20
    x2 = x1 + button_width
    y2 = y1 + button_height
    button_coords = (x1, y1, x2, y2)

    current_time = time.time()

    if results.multi_hand_landmarks:
        if ignore_prediction_count > 0:
            ignore_prediction_count -= 1
            print(f"Ignoring prediction... {ignore_prediction_count} frames left to skip.")
        else:
            last_hand_seen_time = current_time
            word_sent = False

            hand_landmarks = results.multi_hand_landmarks[0]
            coords = [(lm.x, lm.y) for lm in hand_landmarks.landmark]
            coords = np.array(coords).flatten()

            base_x, base_y = coords[0], coords[1]
            coords[::2] -= base_x
            coords[1::2] -= base_y
            normalized = coords[2:]

            if current_time - last_prediction_time > 2:
                prediction = model.predict(np.array([normalized]), verbose=0)
                predicted_class = np.argmax(prediction)
                char = label_encoder.inverse_transform([predicted_class])[0]
                predicted_text += char
                last_predicted_char = char
                last_prediction_time = current_time

            mp_drawing.draw_landmarks(frame_output, hand_landmarks, mp_hands.HAND_CONNECTIONS)

    else:
        if not word_sent and predicted_text and (current_time - last_hand_seen_time > 5):
            last_sent_word = predicted_text
            threading.Thread(target=send_word_to_esp, args=(predicted_text,)).start()
            predicted_text = ""
            word_sent = True
            show_button = True
            last_predicted_char = None
            meaning_text = ""
            ignore_prediction_count = 5 

    # Draw text
    cv2.putText(bottom_panel, f"Text: {predicted_text}", (10, 40),
                cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)

    # Show "Meaning" button
    if show_button:
        cv2.rectangle(bottom_panel, (x1, y1), (x2, y2), (0, 0, 0), -1)
        cv2.rectangle(bottom_panel, (x1, y1), (x2, y2), (0, 0, 255), 2)
        cv2.putText(bottom_panel, "Meaning", (x1 + 20, y1 + 28),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.8, (0, 0, 255), 2)

    # Show meaning text
    if meaning_text:
        lines = []
        words = meaning_text.split()
        line = ""
        for word in words:
            if len(line + word) < 70:
                line += word + " "
            else:
                lines.append(line.strip())
                line = word + " "
        lines.append(line.strip())
        for i, line in enumerate(lines[:3]):
            cv2.putText(bottom_panel, line, (10, 80 + i*25),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 255, 0), 2)

    # Combine frames
    combined_frame = np.vstack((frame_output, bottom_panel))
    cv2.imshow("Hand Gesture Recognition", combined_frame)

    if cv2.waitKey(1) & 0xFF == 27:
        break

cap.release()
cv2.destroyAllWindows()

