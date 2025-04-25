import cv2
import csv
import itertools
import mediapipe as mp
import os

mp_hands = mp.solutions.hands
hands = mp_hands.Hands()
mp_draw = mp.solutions.drawing_utils

csv_file_path = 'hand_gesture_data.csv'
if not os.path.exists(csv_file_path):
    with open(csv_file_path, mode='w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(["Gesture_ASCII"] + [f"Landmark_{i}_{axis}" for i in range(21) for axis in ["x", "y"]])

csv_file = open(csv_file_path, mode='a', newline='')
csv_writer = csv.writer(csv_file)

def pre_process_landmark(landmark_list):
    """Normalize landmark positions relative to the wrist (first landmark)."""
    temp_landmark_list = []
    base_x, base_y = landmark_list[0][0], landmark_list[0][1]  

    for x, y in landmark_list:
        temp_landmark_list.append((x - base_x, y - base_y)) 

    return list(itertools.chain.from_iterable(temp_landmark_list))  

cap = cv2.VideoCapture(0, cv2.CAP_AVFOUNDATION)  

gesture_number = None 

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        break
    frame = cv2.flip(frame, 1)
       
    rgb_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    results = hands.process(rgb_frame)

    if results.multi_hand_landmarks:
        for hand_landmarks in results.multi_hand_landmarks:
            landmark_list = [(lm.x, lm.y) for lm in hand_landmarks.landmark]

            if gesture_number is not None: 
                processed_landmarks = pre_process_landmark(landmark_list)
                row = [gesture_number] + processed_landmarks
                csv_writer.writerow(row)
                print(f"Gesture ASCII '{gesture_number}' recorded!")
                gesture_number = None  

            mp_draw.draw_landmarks(frame, hand_landmarks, mp_hands.HAND_CONNECTIONS)

    cv2.imshow("Hand Gesture Capture", frame)
    cv2.setWindowProperty("Hand Gesture Capture", cv2.WND_PROP_TOPMOST, 1)  

    key = cv2.waitKey(1) & 0xFF
    if key == 27: 
        break
    elif key != 255: 
        gesture_number = key
        print(f"Set ASCII label: {gesture_number}")

cap.release()
cv2.destroyAllWindows()
csv_file.close()
