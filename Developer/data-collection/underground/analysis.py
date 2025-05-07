import pandas as pd

data = []
i = 0
with open('underground_collection.txt', 'r') as f:
    for line in f.read().split('\n'):
        i += 1
        lineData = line.split('->')[1].split(', ')
        output = []
        for item in lineData:
            output.append(float(item))
        data.append(output)
print(len(data))

df = pd.DataFrame(data)
print(df)

ebike_count = sum((df[19] == 1) & (df[0] == 50.0))
bike_count = sum((df[19] == 2) & (df[0] == 50.0))
neither_count = sum((df[19] == 0) & (df[0] == 50.0))

print("E:", ebike_count)        
print("B:", bike_count)        
print("N:", neither_count)
        