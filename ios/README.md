# How we created this iOS XCode project 

## Overview

This lua1 project contains two XCode projects: one to build for physical devices and
another to build for iphone 3.5 emulation.  Two separate projects are useful because the
libraries need to be built differently for these two target environments.  I'm sure it's 
possible to put both in a single project but that's not what we did.

This README file is under the folder lua1/ios, which contains the Xcode project to build
for physical devices.  The folder lua1/iphone35 contains the Xcode project to build for
the emulator running iphone 3.5.  These instructions show how to create the project for
physical devices -- the process is similar for emulated devices.

## Pre-requisites

These instructions assume you have an iOS device and an iOS developer account.

The Lua source code is located under lua1/lua. These include all the Lua files except
for lua.c, luac.c, lua.hpp.  

The SDL header files, including SDL2_ttf.h and SDL2_image.h, are stored in lua1/include.

The SDL2, SDL_ttf and SDL_image source distributions are stored outside of lua1.
This project was build with the unstable snapshot SDL-2.0.3-8795.

## Copy the SDL2 project template

The SDL application template is located at "SDL2/Xcode-iOS/Template/SDL iOS Application". 
Copy this and rename to lua1/ios/proj.  We renamed to proj for simplicity.

Make an additional copy of the template to lua1/iphone35/proj.
The remaining instructions focus on completing lua1/ios/proj.

Open the xcode project and do the following:

	a) Set the user-defined include search path to ../../include
	b) Delete libsdl.a from the SDL/lib group.
	c) Create a group named _lua_ and add the Lua source code in lua1/lua to it.
	d) Rename the project so that the name does not include underscores.
	e) Delete main.c.
	f) Under Sources, add the files in lua1/src/.
	g) Under Frameworks, add MobileCoreServices.framework and ImageIO.framework. 
	h) Under Resources, add the resource foldersand files (with "create folder references" checked).

Close Xcode for the time being.

Create folder lua1/ios/proj/lib to contain the SDL libraries that we will create in
the next steps.

## Build SDL2

Open the Xcode project inside the Xcode-iOS/SDL folder of the SDL2 source distribution.

Connect an iOS device to your computer through the USB port.  Perform whatever 
configuration is needed.

Inside of Xcode, adjust the project scheme as follows.

Go into Product > Scheme > Edit Scheme.
Make sure the Scheme is set to libSDL.  For the Destination, select your connected 
device and set configuration to Release.

Build the target. (Select Product > Build.)

Expand the Products group and select libSDL2.a. 
Determine the full pathname of the file by looking in the inspector panel.
Copy the libSDL2.a file to lua1/ios/proj/lib/. 

## Build SDL_image

Perform steps similar to building SDL2.
Copy result to lua1/ios/proj/lib/.

## Build SDL_ttf

Perform steps similar to building SDL2.
Copy result to lua1/ios/proj/lib/.

## Finish 

Open the ios lua1 Xcode project.

In the SDL/lib group folder add libSDL2.a, libSDL2_image.a and libSDL2_ttf.a.

Build and run.

