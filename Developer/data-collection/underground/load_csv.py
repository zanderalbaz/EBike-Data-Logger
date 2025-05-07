import pandas as pd

df = pd.read_csv('trail_data.csv', header=None)

print("Neither:", len(df[df[19] == 0])/49)
print("Bike:", len(df[df[19] == 2])/49)
print("E-Bike:", len(df[df[19] == 1])/49)