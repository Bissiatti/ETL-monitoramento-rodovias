import json
import sys

placas = open('API/placas2.json', 'r') 
plate_code = sys.argv[1]
data = json.load(placas)  # convert JSON string to Python list of dictionaries

# Search for plate code
for plate_data in data:
    if plate_data["placa"] == plate_code:
        # Print name, year, and model
        print(plate_data["Nome"], plate_data["Ano"], plate_data["model"])
        # Exit script
        sys.exit(0)

print("Plate code not found.")
sys.exit(1)



