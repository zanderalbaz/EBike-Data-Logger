import pandas as pd

df = pd.read_csv("04272025_trail_data.csv", header=None)

for i in range(1,50):
    print(f"{i}: {len(df[df[0] == i])}")