from flask import Flask, request, jsonify
import pandas as pd
import pickle

app = Flask(__name__)

# Load the trained machine learning model
with open('model.pkl', 'rb') as model_file:
    model = pickle.load(model_file)

@app.route('/predict', methods=['POST'])
def predict():
    # Get incoming JSON data
    data = request.get_json()
    
    # Extract features from the incoming data
    # Assuming the data is in the format:
    # {"vibration": value, "noise": value, "voltage": value, "current": value, "RPM": value, "temperature": value, "efficiency": value}
    try:
        features = [
            data['vibration'],
            data['noise'],
            data['voltage'],
            data['current'],
            data['RPM'],
            data['temperature'],
            data['efficiency']
        ]
        
        # Convert features into DataFrame
        input_data = pd.DataFrame([features], columns=['vibration', 'noise', 'voltage', 'current', 'RPM', 'temperature', 'efficiency'])
        
        # Make prediction
        prediction = model.predict(input_data)[0]
        
        # Create response based on prediction
        if prediction == 0:  # Assuming 0 is normal and 1 is abnormal
            result = {"status": "Normal"}
        else:
            result = {"status": "Abnormal"}
        
        return jsonify(result), 200
    
    except Exception as e:
        return jsonify({"error": str(e)}), 400

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
