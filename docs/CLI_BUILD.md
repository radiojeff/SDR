
# Goal

- [ ] What? From zero to 60 to build the whole SDT kit from scratch in the
command-line.

- [ ] Why?  The IDE for Arduino is a headache.


# Development Platform

It's feasible to run the arduino-cli in either a Windows or Linux
environment.

This primer will walk through the steps for each host platform.


# Command Line Build

## Windows

In Windows, there are options to get a `bash` shell.   The simplest
way is to get `git-bash` and install that. It's a fast download and
provides a `bash` shell and a small set of utilities.  It will
work for most situations.

In Windows, another approach is to run Cygwin and with that you get
a lot more than a `bash` shell.  Plus the Cygwin environment is easy to
update with new packages as the situation evolves.  (Just use the
original `setup.exe` you downloaded to revise your Cygwin provision on your
local Windows machine.  Also you're advised to keep the same settings
as far as where you downloaded the original Cygwin from.  Just let it
revert to the defaults since the last time you installed any package
for Cygwin).

## Linux, or Unix-like systems

If the system isn't Linux but some form of Unix (SunOS, Solaris, Ultrix,
AIX, NetBSD, FreeBSD, etc.. ) then you'll have to download the source
(probably) and recompile for your platform (likely).  That is not 
the scope of this document.  If you get stuck doing that, reach out.

On the other hand, for those running Linux (any variant) the solution is
easier:

In Linux things are a bit simpler.  You already will have a native Linux
environment and updating packages in Linux is fairly straight forward.

But the packages we need for Arduino-CLI development are not going to
come from the native Linux package manager.  Maybe in time that will be
the case -- or maybe your Linux actually has a package manager that is
aware of Arduino-CLI.  It's difficult to tell.  There are too many
Linux variants, but they fall into three main categories:  Those
that are RedHat based (like RH, and CentOS, etc..).  The second
category are those that are based on Debian (Debian, Ubuntu, etc..). 
And the third is everything else.  I'll throw MacOS into that category.

# Command Line

The command line to use will depend on the host operating system.


## Windows

In all of the examples below, when we use this form

```
$ arduino-cli.exe ....
```

We're in a Windows environment (the tell tale is the `.exe` extension).
Linux doesn't require files to have any extension to indicate what the 
file type is.  The file itself defines what it is, not the name of the
file. 

This applies to any git-bash or Cygwin bash environment hosted on Windows.

If your host system is Windows, look for the `arduino-cli.exe` syntax


## Linux (or Unix like systems)

For Linux invocations:

```
$ arduino-cli ....
```


# Arduino CLI

## Windows Host

(Again, for those running Cygwin or git-bash) on Windows this is the step
for getting the Arduino-CLI toolkit.

You do not ever need to download the Arduino GUI application.  You can if
you want to, but *it will not aid or assist you in configuring your
Arduino-CLI environment*

[Arduino CLI](https://arduino.github.io/arduino-cli/0.29/commands/arduino-cli_compile/)

In addition as part of the process to flash the image to the target, you'll need this loader:

[Teensy Loader](https://www.pjrc.com/teensy/loader_win10.html)

## Linux Host

Just as on Windows, there are two applications to get installed:

1.  The Teensy Loader
2.  The Arduino-CLI


### Teensy-Loader

Let's get the Teensy Loader out of the way first.

When the Arduino-CLI attempts to `upload` the image to the target, it will attempt to invoke the
Teensy Loader application.  This appears to be a GUI application (nothing we can do about that, I guess).

But the software needs to be installed nonetheless.

Here are the [Instructions for Installing the PJRC Teensy Loader on Linux](https://www.pjrc.com/teensy/loader_linux.html)

An area of research would be to investigate the use of SEGGER JLink debugger to flash the target, but that
will have to wait another day.   For now, rely on what has been given -- the Teensy Loader from PJRC.

### Arduino CLI

You have the choice as to whether or not you want to install the
arduino-cli for all users of your Linux system or just yourself.

Let's suppose you want to install it just for yourself.

1. Go to your home directory

```
$ cd
```

2. `cd` with no arguments will take you to your home directory
   Let's make a place to hold the Arduino-CLI tool

```
$ mkdir -p ard/bin
```

The `-p` flag tells `mkdir` to "make all the necessary directories" so this means it will make `~/ard` and then inside that make `~/ard/bin`.   `-p` makes `mkdir` do them all at once.

3.  Let's go into that directory and download the tar-ball

```
$ cd ard
$ wget -qO arduino-cli.tar.gz https://downloads.arduino.cc/arduino-cli/arduino-cli_latest_Linux_64bit.tar.gz
```

4. Unpack the tarball

```
$ tar xf arduino-cli.tar.gz -C bin
```


5. Set `PATH` (or not)

At this point you can invoke the `arduino-cli` application if you
know the path (`~/ard/bin/arduino-cli`)   OR you can add this yo your
`PATH` environment variable moving forward.  It's up to you.

To add it to your `PATH` environment variable then add this to your
`~/.bashrc` file:

```
export PATH=$PATH:~/ard/bin
```
Then re-run your bashrc (you only need to re-run it this one time).  If you
open a new shell from hence forth the `PATH` will be set and there is 
no need to "re-run" the `.bashrc`

```
$ . ~/.bashrc
```

That will "re-run" the `.bashrc` (again, you won't ever have to do this
again because the next time you open a terminal window with `bash` shell
that file will be sourced).

6.  Last thing is to re-assert that the Arduino config file for you is 
made.  Just run this to make sure:

```
$ arduino-cli config init
```

It may bark that you already have a config file.  Ignore that warning.

At this point your Arduino-CLI tool is mostly setup.  All that is
missing is the awareness of the Teensy 4.1 platform.  We'll get to that next.


# Installing the toolchain

## Windows

The process is pretty simple here.  We're going to run `arduino-cli`
to modify the toolchain.  

This will install the awareness of the Teensy hardware target:

```
$ arduino-cli.exe core install --additional-urls https://www.pjrc.com/teensy/package_teensy_index.json teensy:avr
```


## Linux

On Linux the command is the same as for Windows, but we need to grease the
rails a bit and download the `package_teensy_index.json` ourselves.

```
$ cd ~/.arduino15
$ wget https://www.pjrc.com/teensy/td_156/package_teensy_index.json
```

That will fetch `package_teensy_index.json` from PRJC and deposit it
in your own configuration directory for all of your Arduino work.

Now you can go ahead and run this command:

```
$ arduino-cli core install --additional-urls https://www.pjrc.com/teensy/package_teensy_index.json teensy:avr
```

# Get the libraries for the project

Fortunately some of the libraries we need can be installed with the
Arduino-CLI framework.  One in particular we need is the Adafruit GFX Library.

The double-quotes (") are needed because we're embedding spaces in the argument.


## Windows

```
$ arduino-cli.exe lib install "Adafruit GFX Library"
```

## Linux

```
$ arduino-cli lib install "Adafruit GFX Library"
```


# Other Libraries

The whole GUI mess of Arduino makes it rather difficult to 
bring in a precise library exactly where you want.  

Anyway, this application (T-41 SDT) has some dependencies and here's 
one way to bring them in.  It is NOT the ideal way, but it'll work for now.

We need to clone some repos and place them where `arduino-cli` will find them.

## Windows

Let's just temporarily go into that directory via the shell:
```
$ pushd ~/Documents/Arduino/libraries
```

Then just go clone the libraries

```
$ git clone https://github.com/chipaudette/OpenAudio_ArduinoLibrary
$ git clone https://github.com/etherkit/Si5351Arduino
$ git clone https://github.com/brianlow/Rotary
```

Go back to the directory you were.
```
$ popd
```

## Linux

The standard location for where Arduino installs libraries appears
to be `~/Arduino` and within that directory a `libraries` directory.

For instance if you look inside your freshly established Arduino
setup you'll see the Adafruit GFX Library

```
$ ls ~/Arduino/libraries
Adafruit_BusIO        Adafruit_GFX_Library
```

So it's not a stretch to think that we can simply drop new libraries 
into that directory.  So that is what we will do.

```
$ pushd ~/Ardruino/libraries
```

Now lets `git clone` the repositories that represent the libraries
needed by the SDR software here.

Here's the step to clone those repos:

```
$ git clone https://github.com/chipaudette/OpenAudio_ArduinoLibrary
$ git clone https://github.com/etherkit/Si5351Arduino
$ git clone https://github.com/brianlow/Rotary
```

Go back to the directory you were.
```
$ popd
```

### Why Clone and not download ZIP?

That's up to you.  If you download the ZIP files of the Libraries you're at liberty to do so.  Nothing
here depends on HOW you download the Libraries.

It makes sense to clone the repos only to preserve the opportunity to update them if the upstream Library
repo (remote) is changed (fixed? patched? etc..).    By downloading ZIP files the onus is on you to manage
the version of the Library and any fixes.   While it may be a bit safer to use the ZIP file only to use
packages that are well tested (why else would they release it if it wasn't tested *just a little?*)  That's
true.   Git sourced Libraries could be more "bleeding edge" for your comfort.  But you'are still at liberty
to choose which branch of those git-sourced libraries.

The choice is really up to you.  The author prefers to use git because of the extended flexibility later
if the libraries change.

#  Source Code

The big question is - build what?   The fact that the source code
is available a number of ways:

* From the Groups.io web site.
* From Github in a number of places
* Etc.

So this is where you need to think seriously about where you are
getting the source to build.  

The safest thing in terms of *official* source code is obviously
getting the source from the Groups.io site.  That's what all of the
prevailing posts/documentation indicate.

At the same time, there are a number of contributors who are
patching the software in important ways.  But the maintenance of
those patches into the main-line of the source is unclear.

To avoid the hassle - to avoid problems here is the advice from this
author:

1. Acquire the source code from the Groups.io site.
2. Create your own Github repo with that source code.
3. Clone YOUR repo locally and build from that.
4. Last resort clone an existing repo that already has the
  source.  The author recommends this Repository: [G0ORX Repo for SDT Software](https://github.com/g0orx/SDTVer042.git)



**This is a personal choice**  But it is also a choice that needs
to be tempered with some knowledge of what the impact is for you
as this software evolves.  (And if past is prologue, this
kind of software **will evolve** after the drop of HW comes out.)


If in the event the protagonists of the project spin the software
to a new version, you can always drop those changes into your repo.
The benefit to you, by using `git` is that you can track the changes
and even better be knowledgeable of the things that are changed and 
perhaps contribute meaningfully to the project.   *It is entirely
up to you*.


**However you do get the source, go get the source**

# Building

The way to build the software is really simple:

Suppose you placed the software in a directory: `~/projects/sdr`

So that means, inside the `~/projects/sdr` directory you will find
the directory that holds the software.

The Name of the Project is Important.

So when you acquired the project it had a name.  It had a directory
name like `SDTVer042` or something like that.  Inside that directory
is a file called `SDTVer042.ino`.   That NAME `SDTVer042` is what
we are going to need to know in the following steps for building.

Whatever the name is of the `.ino` file INSIDE the directory holding
the source code, that is the `PROJECT_NAME`.

The steps are virtually
the same for either Windows or Linux hosted systems so in the next
examples, the author is going to drop the `.exe`

- On Windows hosted systems, use `arduino-cli.exe`
- On Linux hosted systems, use `arduino-cli`

Further in these examples, the author is using `pushd` to move into
new directories rather than `cd` so that a mere `popd` will take you
back to where you were before the `pushd`.   Use `cd` if that makes
you more comfortable.


Let's build it. 

```
pushd ~/projects/sdr

```

```
$ arduino-cli compile -b teensy:avr:teensy41 PROJECT_NAME
```

## Verify Step

- On Windows hosted systems, use `arduino-cli.exe`
- On Linux hosted systems, use `arduino-cli`

```
$ arduino-cli compile -t -b teensy:avr:teensy41 PROJECT_NAME
```

## `--output-dir`

If you are hard-set on choosing where the compiler dumps the compiled image files then use
the `--output-dir OUTPUT_DIR` option as such:

```
$ arduino-cli compile -t -b teensy:avr:teensy41 --output-dir OUTPUT_DIR PROJECT_NAME
```

When the software compiles and produces the final `.elf` and `.hex` etc files they will be written
into the `OUTPUT_DIR` directory you specify.

(See also `--input-dir` when using `upload`)

## `--verbose`

If you want verbose output during the operation of compiling the software add the `--verbose` flag, as such:

```
$ arduino-cli compile -t -b teensy:avr:teensy41 --verbose PROJECT_NAME
```

This will cause all the verbose output to be printed to the console window (the shell).

And to capture that output for review later, just use `tee`

```
$ arduino-cli compile -t -b teensy:avr:teensy41 --verbose PROJECT_NAME | tee results.txt
```

## Other options

There are many other options to the tool, so running

```
$ arduino-cli help compile
```

will list those options.  You can stack as many options as needed on the single command line.


# Flash

For the these steps the same conditions apply:

- On Windows hosted systems, use `arduino-cli.exe`
- On Linux hosted systems, use `arduino-cli`

To "Flash" means to put the image (the compiled software is the 'image')

In order to instruct the `upload` command, the parameter it needs is
the port name to use.

`arduino-cli` provides a way to iterate the boards attached to the 
host system to find that port name.

```
$ arduino-cli board list
```

Example:

```
Port               Protocol Type              Board Name FQBN                Core
COM4               serial   Serial Port (USB) Unknown
COM9               serial   Serial Port (USB) Unknown
usb:0/140000/0/1/2 teensy   Teensy Ports      Teensy 4.1 teensy:avr:teensy41 teensy:avr
```

This is an example.  The author has other devices attached
to the host system (in this case Windows)

* `COM9` is not a Teensy board as it happens on my system. It has nothing to do with the Teensy development.
* `COM4` **is the debug port provisioned by the Teensy hardware.**  At least,
according to my Device Manager.  That's where SERIAL I/O happens.  Open that with your terminal program, like `PuTTY`.
* `usb:0/140000/0/1/2` --  this is the PORT we're interested in for flashing the image to the target.   We know so because the Type is not Serial Port, but instead **Teensy Ports**.  (That's what the Teensy developers decided to do, there's no hard-fast rule on the name of the Type).



Now you know the port, you can replace `PORT` and flash it.


```
$ arduino-cli upload -P PORT -fqbn teensy:avr:teensy41 PROJECT_NAME
```

For example using the situation described, the command would be:

```
$ arduino-cli upload -P usb:0/140000/0/1/2 -fqbn teensy:avr:teensy41 PROJECT_NAME
```


This will kick off the client-side of the load process, so the 
Teensy Loader on the Host side will start up.
You can use the Verbose Menu on the Teensy Loader to witness the gory details.

As you have the Serial Port opened (in this example, 
`COM4` was that debug serial port), the output shall spew forth on that 
port if there was any.

## `--input-dir`

If you had selected to place the result of the compile in a non-standard directory (`--output-dir PATH`)
then to `upload` that image you'll need the flag `--input-dir PATH`

```
$ arduino-cli upload -P usb:0/140000/0/1/2 --input-dir PATH -fqbn teensy:avr:teensy41 PROJECT_NAME
```


# Conclusion

That's it -- you can use Windows hosted or Linux hosted Arduino-CLI
to compile, verify and flash (upload) the image to the target.

There are options for getting the debug serial spew from the serial port.
`PuTTY` is recommended for that.

Good luck.

# Further Reading

- [Arduino CLI](https://arduino.github.io/arduino-cli/0.32/getting-started/)
- [Teensy for Linux](https://www.pjrc.com/teensy/loader_linux.html)
- [C++ Gotchas](https://www.powells.com/book/c-gotchas-avoiding-common-problems-in-coding-design-9780321125187)
- [Hacker's Delight](https://www.powells.com/book/hackers-delight-9780321842688)
- [The C Programming Language](https://www.powells.com/book/c-programming-language-9780131103627)
- [Embedded Software Primer](https://www.powells.com/book/embedded-software-primer-9780201615692)
- [So you want to be an embedded software engineer...?](https://rmbconsulting.us/publications/a-c-test-the-0x10-best-questions-for-would-be-embedded-programmers/)
- [Coding Style Standards](https://github.com/MaJerle/c-code-style)

***

Corrections/additions, I'm in good in QRZ.com.

Jeff, [W7BRS](https://blog.w7brs.com)

