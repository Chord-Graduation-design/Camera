
import cv2
# RTSP 地址
rtsp_url = "rtsp://192.168.31.12:8554/mjpeg/1"
 
# 打开 RTSP 视频流
cap = cv2.VideoCapture(rtsp_url)
 
# 检查视频是否成功打开

if not cap.isOpened():
    print("Failed to open RTSP stream")
    exit()
 
# 循环读取视频帧
while True:
    # 读取视频帧
    ret, frame = cap.read()
 
    # 检查是否成功读取视频帧
    if not ret:
        break
 
    # 显示视频帧
    cv2.imshow("RTSP Stream", frame)
 
    # 按 'q' 键退出循环
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
 
# 释放资源
cap.release()
cv2.destroyAllWindows()