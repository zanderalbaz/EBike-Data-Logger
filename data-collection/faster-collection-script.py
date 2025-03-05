import serial
import pandas as pd
import os
import numpy as np
import threading

WINDOW_SIZE = 50
CALIBRATE = 'c'
REINITIALIZE = 'r'
SAVE = 's'
CLASS = 'E-Bike'
IDLE = 'i'
FILENAME = 'rady'

data_dict = {}

def input_thread():
    while True:
        user_input = input("Enter input: ")
        if(user_input == CALIBRATE):
            calibrate_sensors()
        elif(user_input == REINITIALIZE):
            reinitialize_sensors()
        elif(user_input == SAVE):
            save_data()
        elif(user_input == IDLE):
            go_to_idle()
        elif (user_input.split(' ')[0] == 'e'):
            changeClass(user_input.split(' ')[1])
        elif (user_input.split(' ')[0] == 'f'):
            changeFileName(user_input.split(' ')[1])
def go_to_idle():
    ser.write(IDLE.encode())

def calibrate_sensors():
    ser.write(CALIBRATE.encode())
    
def reinitialize_sensors():
    ser.write(REINITIALIZE.encode())

def changeClass(inputClass):
    global CLASS
    CLASS = inputClass
def changeFileName(inputFileName):
    global FILENAME
    FILENAME = inputFileName
    


def save_data():
    magX1 = ([num for num in data_dict["MagX1"]])
    magY1 = ([num for num in data_dict["MagY1"]])
    magZ1 = ([num for num in data_dict["MagZ1"]])
    accX1 = ([num for num in data_dict["AccX1"]])
    accY1 = ([num for num in data_dict["AccY1"]])
    accZ1 = ([num for num in data_dict["AccZ1"]])
    
    magX2 = ([num for num in data_dict["MagX2"]])
    magY2 = ([num for num in data_dict["MagY2"]])
    magZ2 = ([num for num in data_dict["MagZ2"]])
    accX2 = ([num for num in data_dict["AccX2"]])
    accY2 = ([num for num in data_dict["AccY2"]])
    accZ2 = ([num for num in data_dict["AccZ2"]])

    magX3 = ([num for num in data_dict["MagX3"]])
    magY3 = ([num for num in data_dict["MagY3"]])
    magZ3 = ([num for num in data_dict["MagZ3"]])
    accX3 = ([num for num in data_dict["AccX3"]])
    accY3 = ([num for num in data_dict["AccY3"]])
    accZ3 = ([num for num in data_dict["AccZ3"]])
    label = -1
    if 'E-Bike' == CLASS:
        label = 1
    elif 'Bike' == CLASS:
        label = 2
    elif 'Neither' == CLASS:
        label = 0

    class_label = ([label for _ in range(WINDOW_SIZE)])

    new_data = pd.DataFrame({'MagX1': magX1, 'MagY1': magY1, 'MagZ1': magZ1,
                             'AccX1': accX1, 'AccY1': accY1, 'AccZ1': accZ1,
                             'MagX2': magX2, 'MagY2': magY2, 'MagZ2': magZ2,
                             'AccX2': accX2, 'AccY2': accY2, 'AccZ2': accZ2,
                             'MagX3': magX3, 'MagY3': magY3, 'MagZ3': magZ3,
                             'AccX3': accX3, 'AccY3': accY3, 'AccZ3': accZ3,
                             'Class': class_label})
    # print(new_data)
    header = True
    if os.path.exists(f"{FILENAME}.csv"):
        header = None
    new_data.to_csv(FILENAME + '.csv', mode='a', header=header, index=False)
    # will re-write header everytime?? -> yes
    # can I have header creation when user names the file
    # does it save current view or all data collected to that point?
def worker_thread():
    while(True):
        line = ser.readline().strip()
        data = str(line).strip("b'").split(",")
        for data_point in data:
            # Ensure we have clean data to work with
            if (data_point == '' or data_point.find("Initialization") != -1 or data_point.find("Sensor") != -1 or data_point.find("Trying") != -1):
                continue
            data_point = data_point.replace(" ", "").split(":")
            #print(data_point)
            try:
                if len(data_dict[data_point[0]]) >= WINDOW_SIZE:
                    data_dict[data_point[0]].pop()
                    data_dict[data_point[0]].insert(0, float(data_point[1]))
                else:
                    data_dict[data_point[0]].insert(0, float(data_point[1]))
            except KeyError:
                try:
                    data_dict[data_point[0]] = [0.0] * (WINDOW_SIZE - 1)
                    data_dict[data_point[0]].insert(0, float(data_point[1]))
                except IndexError:
                    print("incorrect parsing when adding to data dict")
                except ValueError:
                    continue
            except ValueError:
                print("Parsed incorrectly")
            except IndexError:
                continue
        try:
            print(data_dict['MagX1'][0])
        except KeyError:
            continue
        except IndexError:
            continue
    
ser = serial.Serial('COM7', 115200, timeout=3)


input_thread = threading.Thread(target=input_thread)
input_thread.daemon = True
input_thread.start()
    
worker_thread = threading.Thread(target=worker_thread)
worker_thread.daemon = True
worker_thread.start()


input_thread.join()

