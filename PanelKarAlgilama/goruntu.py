import cv2
import numpy as np
import serial
import time

# Arduino ile seri iletişim için bağlantı ayarları
ser = serial.Serial('COM3', 9600)   

def detect_snow(frame):
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    blurred = cv2.GaussianBlur(gray, (5, 5), 0)
    _, thresh = cv2.threshold(blurred, 200, 255, cv2.THRESH_BINARY)

    kernel = np.ones((5, 5), np.uint8)
    eroded = cv2.erode(thresh, kernel, iterations=1)
    dilated = cv2.dilate(eroded, kernel, iterations=1)
    
    edges = cv2.Canny(dilated, 50, 150, apertureSize=3)
    contours, _ = cv2.findContours(edges, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    
    snow_detected = False
    for contour in contours:
        area = cv2.contourArea(contour)
        
        # Eğer kontur alanı belirli bir eşik değerinden büyükse, kar olarak kabul et
        if area > 1000: 
            x, y, w, h = cv2.boundingRect(contour)
            cv2.rectangle(frame, (x, y), (x+w, y+h), (0, 255, 0), 2)
            snow_detected = True

    time.sleep(2)
    if snow_detected:
        cv2.putText(frame, 'Kar Algilandi', (50, 50), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2, cv2.LINE_AA)
        # Arduino'ya veri gönder
        ser.write(b'1')  # Kar algılandığını belirtmek için '1' gönder
    else:
        ser.write(b'2')
    cv2.imshow('Snow Detection', frame)

# Bilgisayar kamerasını başlat
cap = cv2.VideoCapture(0)

while True:
    ret, frame = cap.read()
    detect_snow(frame)
    if cv2.waitKey(1) & 0xFF == ord('q'): #cikis
        ser.write(b"3")
        break

cap.release()
cv2.destroyAllWindows()