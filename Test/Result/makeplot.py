#%%
import matplotlib.pyplot as plt

# Data to plot
# Data is manually input based on obtained values. The speed value is calculated as number_of_samples/processing_time
# In our case, we consider 48000 samples

# Hidden Size
x = [10, 20, 30, 40, 50, 60, 70]

# Speed
y_rt = [69666, 69064, 44444, 36923,30000 ,24489,20600]
y_torch = [2962,2526,2513, 2388,2285, 2253,2217]

# Create the plot
plt.plot(x, y_rt, label='RTNeural Static')
plt.plot(x, y_torch, label='Torch')

# Add labels to the axes
plt.xlabel('Hidden Size')
plt.ylabel('Speed of Processing')

# Add a title to the plot
plt.title('Cartesian Plot')

# Add a legend
plt.legend()

# Display the grid
plt.grid(True)

# Save the plot in the 'Plot' folder
plt.savefig('./Plot/conv.png')
plt.show()
# %%
