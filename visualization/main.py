import matplotlib.pyplot as plt
import pandas as pd

inside = pd.read_csv("in.txt", sep=" ")
outside = pd.read_csv("out.txt", sep=" ")

#adding first row (for lines chart) 
first_row = outside[0:1]
new_df = pd.DataFrame(first_row)
outside = pd.concat([outside, new_df], ignore_index=True)

plt.scatter(x=inside["x"], y=inside["y"], c="blue", s=1)
plt.plot(outside.x, outside.y, marker='o', linestyle='-', color="red", ms=3)
plt.show()