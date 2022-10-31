print("gpio18/23 LCD will turn on/off 4 times")
import RPi.GPIO as gpio   # RPi.GPIO 라이브러리를 gpio로 import
import time                     # time 라이브러리 import

gpio.setmode(gpio.BCM)  # Broadcom GPIO를 제어로 설정함
gpio.setup(18, gpio.OUT) # GPIO 18을 output으로 설정함
gpio.setup(23, gpio.OUT) # GPIO 23을 output으로 설정함
gpio.setup(25, gpio.IN) # GPIO 25을 output으로 설정함

for i in range(1, 5):         # 아래 for loop 4번 반복함
	gpio.output(18, gpio.HIGH)    # GPIO18에 HIGH 입력 (LCD ON)
	gpio.output(23, gpio.HIGH)    # GPIO23에 HIGH 입력 (LCD ON)
	time.sleep(1)               # 1초 sleep
	gpio.output(18, gpio.LOW)   # GPIO18에 LOW 입력 (LCD OFF)
	gpio.output(23, gpio.LOW)   # GPIO23에 LOW 입력 (LCD OFF)
	time.sleep(1)               # 1초 sleep

try:
	while True:
		inputIO = gpio.input(25)  # GPIO25의 Input 값
		if inputIO == False:
			gpio.output(18, gpio.HIGH)
			gpio.output(23, gpio.HIGH)
		else: 
			gpio.output(18, gpio.LOW)
			gpio.output(23, gpio.LOW)
except KeyboardInterrupt:   # Ctrl + C 누르면, 종료됨
	print("Force quit")

gpio.output(18, gpio.LOW)
gpio.output(23, gpio.LOW)

gpio.cleanup()                 # cleanup all GPIO
