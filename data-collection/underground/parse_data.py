rawData = ""
with open("04272025_trail_data.txt") as rawDataFile:
    rawData = rawDataFile.read()
    rawDataFile.close()
dataChunks = rawData.split(":start:")[1].split(":end:")[0].replace("\r", "\n").split("\n\n\n\n")

with open("04272025_trail_data.csv", "w") as processedDataFile:
    for dataChunk in dataChunks:
        try:
            processedDataChunk = dataChunk.split(":data:")[1]
            processedDataFile.write(processedDataChunk)
        except IndexError:
            print("#"*20)
            print(dataChunk)        
            print("#"*20)
    processedDataFile.close()
