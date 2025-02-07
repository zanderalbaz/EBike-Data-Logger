import serial
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import tkinter as tk
from tkinter import *
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import re
import pandas as pd
import os
import numpy as np

# Open the serial port

WINDOW_SIZE = 50
CALIBRATE = 'c'
ACC_DETECTION_THRESHOLD = 100
MAG1_DETECTION_THRESHOLD = 50
MAG2_DETECTION_THRESHOLD = 50
mag1_avg_std = 0
mag2_avg_std = 0
acc_avg_std = 0
data_dict = {}
data_stats = {}
data_offsets = {"AccX1": 1200,
                "AccY1": 1000,
                "AccZ1": 800,
                "MagX1": 600,
                "MagY1": 400,
                "MagZ1": 200,
                "MagX2": -200,
                "MagY2": -400,
                "MagZ2": -600}


def on_close():
    if ser.isOpen():
        ser.close()
    root.destroy()
    exit()


def calibrate_sensors():
    ser.write(CALIBRATE.encode())


def save_data():
    col1 = ([num for num in data_dict["MagX1"]])
    col2 = ([num for num in data_dict["MagY1"]])
    col3 = ([num for num in data_dict["MagZ1"]])
    acc1 = ([num for num in data_dict["AccX1"]])
    acc2 = ([num for num in data_dict["AccY1"]])
    acc3 = ([num for num in data_dict["AccZ1"]])
    mag1 = ([num for num in data_dict["MagX2"]])
    mag2 = ([num for num in data_dict["MagY2"]])
    mag3 = ([num for num in data_dict["MagZ2"]])
    label = -1
    if 'E-Bike' == clicked.get():
        label = 1
    elif 'Bike' == clicked.get():
        label = 2
    elif 'Neither' == clicked.get():
        label = 0

    class_label = ([label for _ in range(WINDOW_SIZE)])

    new_data = pd.DataFrame({'MagX1': col1, 'MagY1': col2, 'MagZ1': col3, 'AccX1': acc1,
                             'AccY1': acc2, 'AccZ1': acc3, 'MagX2': mag1, 'MagY2': mag2,
                             'MagZ2': mag3, 'Class': class_label})
    # print(new_data)
    header = True
    if os.path.exists(f"{get_name()}.csv"):
        header = None
    new_data.to_csv(get_name() + '.csv', mode='a', header=header, index=False)
    # will re-write header everytime?? -> yes
    # can I have header creation when user names the file
    # does it save current view or all data collected to that point?


# def set_window_size():
#   global WINDOW_SIZE =

def get_name():
    csvName = entry.get()
    return csvName


def classification_selected(selected):
    print(clicked.get())
    clicked.set(selected)


def update_detection_text(status):
    detection_status.set(status)


def get_detection_status():
    return detection_status.get()

def update_stdev_labels():
    Mag1_text.set(f"Mag1 StDev: {mag1_avg_std:.2f}")
    Mag2_text.set(f"Mag2 StDev: {mag2_avg_std:.2f}")
    Acc_text.set(f"Acc StDev: {acc_avg_std:.2f}")

    root.after(100, update_stdev_labels)

def update(frameNum, magX1, magY1, magZ1, accX1, accY1, accZ1, magX2, magY2, magZ2):
    global mag1_avg_std, mag2_avg_std, acc_avg_std
    line = ser.readline().strip()
    data = str(line).strip("b'").split(",")
    for data_point in data:
        # Ensure we have clean data to work with
        if (data_point == '' or data_point.find("Initialization") != -1 or data_point.find("Sensor") != -1 or data_point.find("Trying") != -1):
            continue
        data_point = data_point.replace(" ", "").split(":")
        try:
            if len(data_dict[data_point[0]]) >= WINDOW_SIZE:
                data_dict[data_point[0]].pop()
                data_dict[data_point[0]].insert(0, float(data_point[1]))
                magX1.set_data(range(WINDOW_SIZE), data_dict["MagX1"])
                magY1.set_data(range(WINDOW_SIZE), data_dict["MagY1"])
                magZ1.set_data(range(WINDOW_SIZE), data_dict["MagZ1"])
                accX1.set_data(range(WINDOW_SIZE), data_dict["AccX1"])
                accY1.set_data(range(WINDOW_SIZE), data_dict["AccY1"])
                accZ1.set_data(range(WINDOW_SIZE), data_dict["AccZ1"])
                magX2.set_data(range(WINDOW_SIZE), data_dict["MagX2"])
                magY2.set_data(range(WINDOW_SIZE), data_dict["MagY2"])
                magZ2.set_data(range(WINDOW_SIZE), data_dict["MagZ2"])
                acc_avg_std = np.average([np.std(data_dict["AccX1"]), np.std(data_dict["AccY1"]), np.std(data_dict["AccZ1"])])
                mag1_avg_std = np.average([np.std(data_dict["MagX1"]), np.std(data_dict["MagY1"]), np.std(data_dict["MagZ1"])])
                mag2_avg_std = np.average([np.std(data_dict["MagX2"]), np.std(data_dict["MagY2"]), np.std(data_dict["MagZ2"])])
                if acc_avg_std > ACC_DETECTION_THRESHOLD:
                    if get_detection_status() != "Bike Detected":
                        update_detection_text("Bike Detected")
                else:
                    if get_detection_status() != "No Bike Detected":
                        update_detection_text("No Bike Detected")
            else:
                data_dict[data_point[0]].insert(0, float(data_point[1]))
        except KeyError:
            data_dict[data_point[0]] = [0.0] * (WINDOW_SIZE - 1)
            data_dict[data_point[0]].insert(0, float(data_point[1]))
    return magX1,


ser = serial.Serial('COM4', 115200, timeout=3)

# Visualize data
fig = plt.figure()
ax = plt.axes(xlim=(0, WINDOW_SIZE), ylim=(-100, 100))
MagX1, = ax.plot([], [])
MagY1, = ax.plot([], [])
MagZ1, = ax.plot([], [])
MagX2, = ax.plot([], [])
MagY2, = ax.plot([], [])
MagZ2, = ax.plot([], [])
AccX1, = ax.plot([], [])
AccY1, = ax.plot([], [])
AccZ1, = ax.plot([], [])
anim = animation.FuncAnimation(fig, update,
                               fargs=(MagX1, MagY1, MagZ1, AccX1, AccY1, AccZ1, MagX2, MagY2, MagZ2),
                               frames=200,
                               interval=25)
ax.legend(['MagX1', 'MagY1', 'MagZ1', 'MagX2', 'MagY2', 'MagZ2', 'AccX1', 'AccY1', 'AccZ1'], ncol=3, loc="upper right")

# graph & save button
root = tk.Tk()

# Detection Status
detection_status = StringVar(root)
detection_status.set("No Bike Detected")
detection_text = Label(root, textvariable=detection_status, font=24)
detection_text.pack(pady=10)

# Matplotlib Graph
canvas = FigureCanvasTkAgg(fig, master=root)
canvas.draw()
canvas.get_tk_widget().pack(side=tk.TOP, fill=tk.BOTH, expand=1)

# Save Data Button
save_button = tk.Button(root, text="Save Data", command=save_data)
save_button.pack(padx=20, side="right")

# Calibration Button
calibration_button = tk.Button(root, text="Calibrate Sensors", command=calibrate_sensors)
calibration_button.pack(padx=(20, 0), side="right")

# Filename textbox entry
entry_label = tk.Label(root, text="CSV Filename: ")
entry_label.pack(padx=(20, 0), side="left")
entry = tk.Entry(root)
entry.pack(pady=20, side="left")

# Dropdown + Label
options = ["E-Bike", "Bike", "Neither"]
clicked = StringVar(root)
clicked.set(options[0])
dropdown_label = tk.Label(root, text="Classification: ")
dropdown_label.pack(padx=(20, 0), side="left")
dropdown = OptionMenu(root, clicked, *options, command=classification_selected)
dropdown.pack(side="left")

# Std Dev info
Mag1_text = StringVar(root)
Mag1_text.set("Mag1 StDev: ")
Mag1_label = tk.Label(root, textvariable=Mag1_text, font=24)
Mag1_label.pack(side="bottom")

Mag2_text = StringVar(root)
Mag2_text.set("Mag2 StDev: ")
Mag2_label = tk.Label(root, textvariable=Mag2_text, font=24)
Mag2_label.pack(side="bottom")

Acc_text = StringVar(root)
Acc_text.set("Acc StDev: ")
Acc_label = tk.Label(root, textvariable=Acc_text, font=24)
Acc_label.pack(side="bottom")

# Create Window
update_stdev_labels()
root.protocol("WM_DELETE_WINDOW", on_close)
root.mainloop()
plt.show()

ser.close()
