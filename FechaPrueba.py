import serial
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
from datetime import datetime, timedelta

# Configura el puerto serial (ajusta el puerto y la velocidad según tu Arduino)
puerto_serial = serial.Serial('/dev/ttyUSB0', 115200)  # Cambia '/dev/ttyUSB0' al puerto que corresponda

# Inicializa listas para almacenar los datos
fechas = []
valores = []

# Hora de inicio
inicio = datetime.strptime("11:00:00", "%H:%M:%S")

tiempo_actual = inicio

try:
    # Envia el comando "soil1" al Arduino
    puerto_serial.write("soil1\n".encode())
    print("Comando 'soil1' enviado al Arduino.")

    while True:
        linea = puerto_serial.readline().decode('utf-8').strip()

        # Comprueba si la línea contiene el comando de finalización
        if linea == "1122":
            print("Final de la transmisión.")
            break

        # Intenta convertir la línea a un valor float
        try:
            valor = float(linea)
            valores.append(valor)
            fechas.append(tiempo_actual)
        except ValueError:
            # Ignora líneas que no se pueden convertir a float
            continue

        # Incrementa el tiempo en 3 minutos (180 segundos)
        tiempo_actual += timedelta(seconds=180)

except KeyboardInterrupt:
    # Ctrl+C para detener la lectura del puerto serial
    print("Lectura del puerto serial detenida.")

finally:
    # Cierra el puerto serial al finalizar
    puerto_serial.close()

# Grafica los valores recopilados
fig, ax = plt.subplots()
ax.xaxis.set_major_formatter(mdates.DateFormatter("%H:%M:%S"))
ax.plot(fechas, valores)
ax.set_xlabel('Hora desde las 11:00 AM')
ax.set_ylabel('Valor')
ax.set_title('Gráfico de Valores Recopilados Soil1')

# Muestra el gráfico
plt.xticks(rotation=45)  # Rota las etiquetas del eje x para una mejor visualización
plt.tight_layout()
plt.show()
