# ESP32-guitar-tuner
ESP32 Based guitar tuner

![PCB layout](https://user-images.githubusercontent.com/102794563/161469802-59e867ad-581b-4489-9825-b58ddc6e31e4.PNG)



## Background
Each note played on the guitar creates a signal with a certain fundamental frequency
![image](https://user-images.githubusercontent.com/102794563/161469939-41ff7dc8-fb4e-496f-9ce2-b11f3faa0bed.png)

We can determine the note being played by working out the fundamental frequency of the signal taken from the guitar
![image](https://user-images.githubusercontent.com/102794563/161470536-df2d525c-4d45-43ca-b654-434e09f26a1b.png)

## Circuit board
The signal coming from an electric guitar is small (approximately +-100->300mV). For a microcontoller to distinguish between samples of similar magnitude, we should amplify the signal to maximize these differences. A typical microcontroller accepts voltages in the range of 0-3.3V or 0.5V, and the analog pins can be damaged by the negative part of an incoming AC signal. 
![image](https://user-images.githubusercontent.com/102794563/161470953-bfb4b4d2-8bee-409f-9ee6-b38120fb463a.png)

From the simulation results below we can see that the circuit can offset and amplify the incoming signal to fit the desired range of the microcontroller analog pin
![image](https://user-images.githubusercontent.com/102794563/161471068-3d126e1d-eb89-44ff-972c-c3f167e40500.png)

