import matplotlib.pyplot as plt

# Dati da plottare

# Hidden Size
x = [10, 20, 30, 40, 50, 60]

# Speed
y_rt = [4.95, 1.78, 0.88, 0.55, 0.33, 0.23]
y_torch = [0.0068, 0.0047, 0.0051, 0.0056, 0.0059, 0.0056]

# Creazione del plot
plt.plot(x, y_rt, label='RTNeura Static')
plt.plot(x, y_torch, label='Torch')

# Aggiunta di etichette agli assi
plt.xlabel('Hidden Size')
plt.ylabel('Speed of Processing')

# Aggiunta di un titolo al plot
plt.title('Plot Cartesiano')

# Aggiunta della legenda
plt.legend()

# Mostra il plot
plt.grid(True)
plt.show()