import requests
import json
import numpy
import tensorflow as tf
import tensorflowjs as tfjs

#model=tfjs.converters.load_keras_model("http://localhost:5984/pnf/tf-ar-a/model.json")

# there is inbuilt json() constructor for requests.get() method
model_data = requests.get("http://localhost:5984/pnf/tf-ar-a/model.json").json()
weights_data = requests.get("http://localhost:5984/pnf/tf-ar-a/model.weights.bin")
print(model_data)

# To actually write the data to the file, we just call the dump() function from json library
with open('/home/simon/tf-ar-a/model.json', 'w') as json_file:
    json.dump(model_data, json_file)

with open('/home/simon/tf-ar-a/model.weights.bin', 'wb') as bin_file:
    bin_file.write(bytes(weights_data.content))

model=tfjs.converters.load_keras_model("/home/simon/tf-ar-a/model.json")

# model.predict([[numpy.tanh(x/10) for x in [-21,15,-22,8,-4,5,-11,22]]])

model.save("/tmp/tf-ar-a")
