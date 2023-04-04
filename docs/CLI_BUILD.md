
# Goal

- [ ] What? From zero to 60 to build the whole SDT kit from scratch in the
commandline.

- [ ] Why?  The IDE for Arduino is a headache.

- [ ] Again?   Yes, for those of us who write software for a living, the Arduino IDE is a complete headache to use.


## TODO

- [ ] Integrate the libraries into the local environment using submodules
of the main project.
- [ ] Convert the whole thing to use `cmake`  (which means adding
the steps to get the `arm-none-eabi-*` toolchain setup.
- [ ] Integate the CI/CD pipeline to build this in the repo and release
- [ ] A bit more documentation about the precise HW needed.  The information
appears to be here and there, but not in the same place.  For example
there are two main Groups.IO forums for the same project.


# Command Line Build

For those who do not jive with the Arduino IDE because it is an experience much like a root-canal for those
who have used other development environments, there is hope.

Use the CLI (command line interface) version.  Otherwise known as, *run from the shell*.

In all the examples here, the `arduino-cli` is invoked from `bash` on Windows. (Git-Bash, actually -- close
enough).


## Arduino CLI

The CLI version of the Arduino framework (not the compiler per se, but the build framework):

[Arduino CLI](https://arduino.github.io/arduino-cli/0.29/commands/arduino-cli_compile/)

In addition as part of the process to flash the image to the target, you'll need this loader:

[Teensy Loader](https://www.pjrc.com/teensy/loader_win10.html)


## Installing the toolchain

```
arduino-cli.exe core install --additional-urls https://www.pjrc.com/teensy/package_teensy_index.json teensy:avr
```

## Get the libraries for the project

```
arduino-cli lib install "Adafruit GFX Library"
```

### Other Libraries

The whole GUI mess of Arduino makes it rather difficult to bring in a precise library
exactly where you want.  

Anway, this application (T-41 SDT) has some dependencies and here's one way to bring them
in.  It is NOT the ideal way, but it'll work for now.

We need to clone some repos and place them where `arduino-cli` will find them.

Again, assuming a `git-bash` environment on Windows.  On Linux the process is going
to be rather similiar -- just where precisely `arduino-cli` expects to find
third party libraries is another matter.  Let's assume that is as shown:


Let's just temporarily go into that directory via the shell:
```
pushd ~/Documents/Arduino/libraries
```

Then just go clone the libraries
```
git clone https://github.com/chipaudette/OpenAudio_ArduinoLibrary
git clone https://github.com/etherkit/Si5351Arduino
git clone https://github.com/brianlow/Rotary
```

Now get out of there...

```
popd
```

Why clone and not download ZIP?  Because if the upstream source repo changes (gets
patched, fixed, whatever then it's rather simple to upgrade your local repo.) 

Further more, we may find out along the way we need to branch and modify the upstream
library for **this** application.  Managing drops via ZIP files is just nonsense.  Use
what the pros use.  Just use git.


## Building 

We need to get away from naming this repo `SDTVer042`

Git is really good at keeping track of versions!   Just tag it or make a branch.

Now you have the choice.

Are you going to contribute to the repo or just use it?


If you are going to contribute, then build from your own fork.

Recommended steps (if you intend to contribute)

1.  Fork the repo.  Go to [Github](https://github.com/g0orx/SDTVer042.git) and fork the repo.

2.  Now you have your own fork.  **Clone the fork.**  I won't detail that since if you are forking the repo, you already know what you're doing.


If you aren't going to contribute to the repo, then just clone the repo:

(TODO: We need to rename this to SDT-something-else not a name
with the version.  For example: `SDT` rather than `SDTVer042`
 Git is good at dealing with branches... etc..)


URL is going to be either the upstream repo or your fork.

```
pushd ~/wherever/you/put/your/SDT
# git clone URL
git clone https://github.com/g0orx/SDTVer042.git
```

It wouldn't hurt to make a branch in case you want to patch 
something and offer it up.  This is not going to affect your build.

(You don't need to make a branch unless you're intending to change things.)

```
# If you want to make a branch, use a syntax that makes it easier to see
# who's branch it is -- this will help if PR's are done against your
# fine patches.
git checkout -b CALLSIGN/something-special
```



Now build it.


```
arduino-cli.exe compile -b teensy:avr:teensy41 SDTVerNNN/SDTVerNNN.ino
```

## Verify Step
(for those who do this)

```
arduino-cli.exe compile -t -b teensy:avr:teensy41 SDTVerNNN/SDTVerNNN.ino
```

## Flash

To get the PORT, use

```
$ arduino-cli.exe board list
```

Example:

```
Port               Protocol Type              Board Name FQBN                Core
COM4               serial   Serial Port (USB) Unknown
COM9               serial   Serial Port (USB) Unknown
usb:0/140000/0/1/2 teensy   Teensy Ports      Teensy 4.1 teensy:avr:teensy41 teensy:avr
```

`COM9` is not a Teensy board as it happens on my system, 
but `COM4` **is the debug port provisioned by the Teensy hardware.**  At least,
according to my Device Manager.  
That's where SERIAL I/O happens.  Open that with your terminal program.

(On Linux, you can get the device from `lsusb`)


As far as the port on which to upload, that would be `usb:0/140000/0/1/2` and it is so because it's a 
type `Teensy Ports`.  That's the PORT to the bootloader that is already present on the Teensy hardware.

(A subject seldom discussed -- where is the bootloader?  It's on the target already.  This is not quite
bare-metal programming.  But it's close.  But not bare-metal.)

Now you know the port, you can replace `PORT` and flash it.


```
arduino-cli.exe upload -P  PORT -fqbn teensy:avr:teensy41 SDTVerNNN
```

This will kick off the client-side of the load process, so the Teensy Loader on the Host side will start up.
You can use the Verbose Menu on the Teensy Loader to witness the gory details.

As you have the Serial Port opened (in this example, `COM4` was that debug serial port), the output shall
spew forth on that port if there was any.


####
Corrections/additions, I'm in good in QRZ.com.

Jeff, W7BRS

