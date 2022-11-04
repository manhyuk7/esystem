print("gpio18/23 LED will turn on/off 4 times")
import RPi.GPIO as gpio   # RPi.GPIO ���̺귯���� gpio�� import
import time                     # time ���̺귯�� import

gpio.setmode(gpio.BCM)  # Broadcom GPIO�� ����� ������
gpio.setup(18, gpio.OUT) # GPIO 18�� output���� ������
gpio.setup(23, gpio.OUT) # GPIO 23�� output���� ������
gpio.setup(25, gpio.IN) # GPIO 25�� output���� ������

for i in range(1, 5):         # �Ʒ� for loop 4�� �ݺ���
	gpio.output(18, gpio.HIGH)    # GPIO18�� HIGH �Է� (LED ON)
	gpio.output(23, gpio.HIGH)    # GPIO23�� HIGH �Է� (LED ON)
	time.sleep(1)               # 1�� sleep
	gpio.output(18, gpio.LOW)   # GPIO18�� LOW �Է� (LED OFF)
	gpio.output(23, gpio.LOW)   # GPIO23�� LOW �Է� (LED OFF)
	time.sleep(1)               # 1�� sleep

try:
	while True:
		inputIO = gpio.input(25)  # GPIO25�� Input ��
		if inputIO == False:
			gpio.output(18, gpio.HIGH)
			gpio.output(23, gpio.HIGH)
		else: 
			gpio.output(18, gpio.LOW)
			gpio.output(23, gpio.LOW)
except KeyboardInterrupt:   # Ctrl + C ������, �����
	print("Force quit")

gpio.output(18, gpio.LOW)
gpio.output(23, gpio.LOW)

gpio.cleanup()                 # cleanup all GPIO
