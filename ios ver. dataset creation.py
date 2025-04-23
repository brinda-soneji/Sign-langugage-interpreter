import cv2
import csv
import itertools
import mediapipe as mp
import os

# Initialize MediaPipe Hands
mp_hands = mp.solutions.hands
hands = mp_hands.Hands()
mp_draw = mp.solutions.drawing_utils

# Create dataset file if it doesn't exist
csv_file_path = 'hand_gesture_data.csv'
if not os.path.exists(csv_file_path):
    with open(csv_file_path, mode='w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(["Gesture_ASCII"] + [f"Landmark_{i}_{axis}" for i in range(21) for axis in ["x", "y"]])

# Open CSV file in append mode
csv_file = open(csv_file_path, mode='a', newline='')
csv_writer = csv.writer(csv_file)

def pre_process_landmark(landmark_list):
    """Normalize landmark positions relative to the wrist (first landmark)."""
    temp_landmark_list = []
    base_x, base_y = landmark_list[0][0], landmark_list[0][1]  # Wrist as reference

    for x, y in landmark_list:
        temp_landmark_list.append((x - base_x, y - base_y))  # Normalize positions

    return list(itertools.chain.from_iterable(temp_landmark_list))  # Flatten list

cap = cv2.VideoCapture(0, cv2.CAP_AVFOUNDATION)  # Use AVFoundation for macOS

gesture_number = None  # Store user input label

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        break
    frame = cv2.flip(frame, 1)
    
    # Convert to RGB for MediaPipe
    rgb_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    results = hands.process(rgb_frame)

    # Detect hand landmarks
    if results.multi_hand_landmarks:
        for hand_landmarks in results.multi_hand_landmarks:
            landmark_list = [(lm.x, lm.y) for lm in hand_landmarks.landmark]

            if gesture_number is not None:  # Only save if a key was pressed
                processed_landmarks = pre_process_landmark(landmark_list)
                row = [gesture_number] + processed_landmarks
                csv_writer.writerow(row)
                print(f"Gesture ASCII '{gesture_number}' recorded!")
                gesture_number = None  # Reset after one input

            # Draw landmarks on the frame
            mp_draw.draw_landmarks(frame, hand_landmarks, mp_hands.HAND_CONNECTIONS)

    # Display frame
    cv2.imshow("Hand Gesture Capture", frame)
    cv2.setWindowProperty("Hand Gesture Capture", cv2.WND_PROP_TOPMOST, 1)  # Ensure window stays interactive

    # Capture user input (Only once per press)
    key = cv2.waitKey(1) & 0xFF
    if key == 27:  # Exit on ESC key
        break
    elif key != 255:  # Capture only actual key presses
        gesture_number = key
        print(f"Set ASCII label: {gesture_number}")

cap.release()
cv2.destroyAllWindows()
csv_file.close()
