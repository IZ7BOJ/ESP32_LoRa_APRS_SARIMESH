This SW package is the porting under PlatformIO of an original work developped under arduino as part of an experimentation promoted by the Radio Amateur Group of the Penisola Sorrentina... ( near Napoli , Southern Italy )

The initial target of this activity was to stimulate local OMs to jump over the new technologies  that represent the "radio Amateur toolset of the 2000 years"...  

We created a Projet ( in 2016 ) named SARIMESH (http://www.sarimesh.net) that stands for "Sorrento Amateur Radio Internode MESH" aiming to create  an emergency oriented WiFi network and to experiment how this network can be created, deploied and managed...

Inside this main activity we then introduced the new LoRa technology as a possible access network to integrate with the MESH network and allow several possibile new services...

All the activities was conceived to be easily accessible to "non professionals" in order to make as low as possible the access gap for newcomers...

This approach leed to make implementation  coiches  that could be nearly always be sub-optimal both in terms of tools and specialistic knowledges....

The SW and HW produced during these activities was never intended to be made available outside our local focus just for these intrinsic limitation...

Over the time we then discovered that in particular the LoRa components was getting some interest from new OMs outside our oruginal focus and it was evident that some of our outcomes could be of more general interest... so we was starting to make available these components  over GITHUB so that more people could possibly benefit of our work....

With the advent of PlatformIO it was clear that the right way to present over internet our work was to just join this new powerfull development environment..  obviously it was out of scope to rework all the code to make it very nice looking by using all the latest features of SW programming just to make it look nice... so if you will decide to read our code you will find a very old coding style ... we make our code available as it was originally organized  (i.e. to be readable by  OMs absolutely newcomers to SW programming...) and with the explicit advice , if somebody likes it more modern, to just contribute to make it better ....

Our LoRa project was never intended to generate "products" but conversely to make available a "development" environment HW-SW to make experimentations and to possibly create ancillary applications...  so the name we are now giving to our project is  "Sarimesh Core" just to say that it is something not strictly LoRa oriented but in some form over-engineered in order to implement new features..

So the project includes a number of funtionalities like management and debug features, time sincronizzation, SW architecture that could be used also for some applications with tight realtime constrains... we decided to use ESP32 and its FreeRTOS  executive environment making explicitly and clearly visible to the user some basic FreRTOS capabilities to stimulate a programming style not strictly of High Level SW type but also oriented to possible realtime low level applications.

Sorrento, 2023

Michele, I8FUC



