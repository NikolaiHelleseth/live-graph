from flask import Flask, render_template
from threading import Thread
from flask_socketio import SocketIO
import serial


app = Flask(__name__, static_folder='static')
socketio = SocketIO(app)


sensor_name = ["temperature", "turbiditet", "ph", "trykk"]
x_val = []
temperature = []
turbiditet = []
ph = []
trykk = []
arduino_port = "COM3"
baud = 9600
Arduino = serial.Serial(port=arduino_port, baudrate=baud, timeout=1)


def append_data(data):
    try:
        for n in range(len(data)):
            if n == 0:
                turbiditet.append(float(data[n]))
            elif n == 1:
                temperature.append(float(data[n]))
            elif n == 2:
                trykk.append(float(data[n]))
            elif n == 3:
                ph.append(float(data[n]))
        x_val.append(len(temperature))
    except ValueError:
        print("error")

def antall():
    if len(temperature) >= 100:
        temperature.pop(0)
        turbiditet.pop(0)
        ph.pop(0)
        trykk.pop(0)

def read_data():
        data = Arduino.readline().decode("utf-8").strip().split(",")

        append_data(data)
        antall()
      

@app.route('/')
def index():
    return render_template('plot.html', sensorer=sensor_name)


@app.route("/sensor/<sensor>")
def individ(sensor):
  return render_template("individuell.html", sensorer = sensor_name, header=sensor)

@app.route("/about")
def about(): 
    return render_template("about.html", sensorer = sensor_name)

@socketio.on('get_plot_data')
def handle_connect():
    print('Client connected')
    emit_plot_data()

def emit_plot_data():
    read_data()
    socketio.emit('plot_data', {'x_val': x_val, 'temperature': temperature, 'turbiditet': turbiditet,
                                'ph': ph, 'trykk': trykk})
    

if __name__ == '__main__':
    Thread(target=read_data).start()
    socketio.run(app, debug=True, port=5000)
