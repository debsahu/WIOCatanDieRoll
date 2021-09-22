# WIO Catan-Die Roll

Machine Learning in WIO Terminal (Seeed Studio) to recognize shake and roll two die using True Random Number Generator (TRNG)

## Hardware

- [WIO Terminal (Seeed Studio)](https://www.seeedstudio.com/Wio-Terminal-p-4509.html)
- USB-C cable

## Software

- **WIOImuTap**: Uses in-built IMU to look for double tap to roll 2 die using TRNG 
- **EdgeImpulse**: Upload and train NN using sensor data on [Edge Impulse](https://www.edgeimpulse.com/), deployed on WIO with live classification on serial port. [Model is included](https://studio.edgeimpulse.com/public/48805/latest)
- **AIShakeDie**: Uses in-built IMU to recognize shake using NN from **Edge Impulse** and rolls 2 die using TRNG

## Collecting data from WIO & storing on Edge Impulse, training Neural Network on Edge Impulse, Export and deploy on WIO

### 1. Collecting data from WIO & storing on Edge Impulse

#### Installing dependencies (Windows)

1. Install [Python 3](https://www.python.org/)
2. Install [Node.js v14 or higher](https://nodejs.org/en/) - install additional Node.js tools or-else install [Microsoft Visual Sudio 2015](https://visualstudio.microsoft.com/vs/older-downloads/)
3. Open powershell as admin, install *edge-impulse-cli*
```
npm install -g edge-impulse-cli --force
```

For other OS please follow instructions from [here](https://docs.edgeimpulse.com/docs/cli-installation).

#### Connecting WIO to Edge Impulse

1. Connect WIO to computer using USB-C cable
2. Enter bootloader byt sliding power switch down twice in quick succession.
![enter_bootloader_wio](https://files.seeedstudio.com/wiki/Wio-Terminal/img/Wio-Terminal-Bootloader.png)
3. WIO will show up as a new USB drive named `Arduino`, copy [latest released precompiled UF2 firmware](https://github.com/Seeed-Studio/Seeed_Arduino_edgeimpulse/releases) to `Arduino` drive
4. Start sending data to Edge Impulse, open a powershell and type
```
C:\> edge-impulse-daemon
```
Follow instructions on screen by logging into your Edge Impulse account and selecting a project.
5. Open the [Edge Impulse project](https://studio.edgeimpulse.com/studio/select-project?autoredirect=1) on a browser and see if WIO is connected

![EI-WIO-Connected](https://github.com/debsahu/WIOCatanDieRoll/blob/main/docs/device_ei_connected.png)

### 2. Storing Accelerometer data on Edge Impulse

1. Start collecting data from device:

![EI-WIO-Accelerometer](https://github.com/debsahu/WIOCatanDieRoll/blob/main/docs/training_data.png)

2. Generate appropiate labels for each data point, click on **start sampling**. When the countdown starts, perform appropiate action on the WIO based on the label given
3. Once a 10s data collection is complete, check the collected data section and click on **hamberger icon** and split sample

![Split-Sample](https://github.com/debsahu/WIOCatanDieRoll/blob/main/docs/split_sample.png)

4. Split the sample into 1s chunks and click **split**

![Split-Sample-2](https://github.com/debsahu/WIOCatanDieRoll/blob/main/docs/split_sample_2.png)

5. Repeat to have lots of data for each label
6. Do the same for **Test data** (top-left) for each label

### 3. Create Impulse & Process/Clean up data & training NN

1. Create a new impulse (overall design of the project)

![Create-Impulse](https://github.com/debsahu/WIOCatanDieRoll/blob/main/docs/create_impulse.png)

2. Generate Spectral Features - clean up your raw accelerometer data

![Spectral-Features](https://github.com/debsahu/WIOCatanDieRoll/blob/main/docs/spectral_features.png)

3. Generate Features

![Generate-Features](https://github.com/debsahu/WIOCatanDieRoll/blob/main/docs/generate_features.png)

4. Train your neural network using cleaned-up training data features from previous step

![NN-Train](https://github.com/debsahu/WIOCatanDieRoll/blob/main/docs/nn_train.png)

### 4. Testing your NN performance

Click on **Model Testing** > **Classify All**

![Model-Testing](https://github.com/debsahu/WIOCatanDieRoll/blob/main/docs/model_testing.png)

### 5. Deploying your NN on WIO

1. Click on **Deployment**
2. Select **Arduino library**

![Deploy-Arduino-Library](https://github.com/debsahu/WIOCatanDieRoll/blob/main/docs/deploy_arduino_lib.png)

3. Build your Arduino library

![Build-Library](https://github.com/debsahu/WIOCatanDieRoll/blob/main/docs/build_lib.png)

4. Copy contents from **src** directory into your project
5. See implementation in **EdgeImpulse** folder