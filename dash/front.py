import streamlit as st
import json

# Define the location of the JSON file
json_file = 'data.json'

# Define the function to load JSON data
@st.cache
def load_data(json_file):
    with open(json_file) as f:
        data = json.load(f)
    return data

# Load the JSON data
data = load_data(json_file)

# Display the dashboard
st.title('Live JSON Data Dashboard')

# Display the data in a table
st.write('## Data Table')
st.table(data)

# Display a line chart of the data
st.write('## Line Chart')
st.line_chart(data)

# Display a bar chart of the data
st.write('## Bar Chart')
st.bar_chart(data)
