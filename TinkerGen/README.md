# WIO Catan-Die Roll on *TinkerGen IDE*

Machine Learning on **TinkerGen IDE** in WIO Terminal (Seeed Studio) to recognize shake and roll two die

## Hardware

- [WIO Terminal (Seeed Studio)](https://www.seeedstudio.com/Wio-Terminal-p-4509.html)
- USB-C cable

## Software

- On Browser: [TinkerGen](https://ide.tinkergen.com/)
- Install [Codecraft Assistant](https://ide.tinkergen.com/download/en/#:~:text=Mac%20v2.6.4.25-,Codecraft%20Assistant,-Codecraft%20Assistant%20is)

## TinkerGen IDE

1. Navigate to [TinkerGen IDE](https://ide.tinkergen.com/) and create a free account.

2. Connect WIO terminal using USB-C cable and run [CodeCraft Assistant](https://ide.tinkergen.com/download/en/#:~:text=Mac%20v2.6.4.25-,Codecraft%20Assistant,-Codecraft%20Assistant%20is)

3. Navigate to **Model Creation** and create a new model using accelerometer data.

![ide_tinker_model_create](https://github.com/debsahu/WIOCatanDieRoll/blob/main/docs/ide_tinker_model_create.png)

4. Start obtaining accelerometer data from WIO terminal for various labels by clicking on **Data Acquisition**. Note that the accelerometer data is collected for at least 2s which makes up 128 points (62.5Hz).

![ide_tinker_data_acqu](https://github.com/debsahu/WIOCatanDieRoll/blob/main/docs/ide_tinker_data_acqu.png)

5. Collect lots of data, more data is better. Yes, it takes time. Do not skip or skimp on this step

6. Train your model using the collected data by clicking on **Training and Deployment**.

![ide_tinker_training_deploy](https://github.com/debsahu/WIOCatanDieRoll/blob/main/docs/ide_tinker_training_deploy.png)

Look for accuracy of the model generated.

7. Deploy your model by clicking on **Programming**

![ide_tinker_deploycode](https://github.com/debsahu/WIOCatanDieRoll/blob/main/docs/ide_tinker_deploycode.png)

