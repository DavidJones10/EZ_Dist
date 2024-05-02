# EZ_Dist
This is a distortion and overdrive plugin that uses physical modeling to produce accurate simulations of the circuits that are in many distortion and overdrive pedals. Most of the math and circuit analysis comes from this paper: [SIMPLIFIED, PHYSICALLY-INFORMED MODELS OF DISTORTION AND OVERDRIVE
GUITAR EFFECTS PEDALS](https://ccrma.stanford.edu/~dtyeh/papers/yeh07_dafx_distortion.pdf).

# Build instructions
### If you would like to build this plugin, you will need the [Juce Framework and Projucer](https://juce.com/download/) as well as either [XCode](https://developer.apple.com/xcode/) if you are on Mac or [Visual Studio 2022](https://visualstudio.microsoft.com/downloads/) if you are on Windows. If you're on linux then I'm sure you'll figure it out lol. 
* Once you have Juce, the Projucer application, and your preferred Juce-compatible IDE, you should clone this repository or download as a zip file.
* Once you have all of the source files, open the EZ_Dist.jucer file in the Projucer application.
* Then, navigate to the Exporters tab and select your desired export target if it is not already selected.
* Then, click the icon of your selected exporter at the top of the Jucer window.
* Once you are in your IDE, select your desired export target file type. The options are: Standalone Plugin, AU, VST3 Manifest Helper, VST3, All, and Shared Code. If you are unsure what file type you need, select All, as it will build all of the different options.
* If you are building as a VST3 or AU, you will need to place the EZ_Dist.VST3 or the EZ_DIST.component in the folders in which your DAW will look for them. On Mac this folder will be in a location that looks something like this: "Library/Audio/Plug-Ins/VST3" or "Library/Audio/Plug-Ins/Components." Each time you build the files, they will be in a directory that looks like this: "EZ_Dist/Builds/MacOSX/build/Debug/."
* Once you have placed your built files into their proper directories, your DAW should scan them, and then that's it! You have successfully build the EZ_Dist plugin!

#### There is also a short video showing how to build and demoing the plugin [here](https://youtu.be/MwwqMA6-xPY).

# UI 
![plot](./Assets/EZ_DIST_UI.png)
