lines = []
with open("collection1_chocie_bike.txt", 'r') as file:
	line = file.readline()
	while(line):
		line = line.split("->")[1]
		lines.append(line)
		line = file.readline()
	file.close()
	
with open("trail_data.csv", 'w') as writeFile:
	writeFile.writelines(lines)
	writeFile.close()
