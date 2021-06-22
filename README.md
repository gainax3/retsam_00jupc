Installation
------------

### Windows 10 (Windows Subsystem for Linux)

Install WSL as [explained here](https://docs.microsoft.com/en-us/windows/wsl/install-win10).

If using Debian or Ubuntu, install the packages as follows:
```
sudo apt-get install git gcc make bison flex bzip2
```
Or install the equivalent of these packages in the distribution of your choice.

Change to a directory accessible from Windows where you'll store the files, for example:
```
cd /mnt/c/Users/$USER/Desktop
```

Continue with the [building instructions](#building)


### Windows (Cygwin)

Get the installer ([64 bit](https://www.cygwin.com/setup-x86_64.exe) or [32 bit](https://www.cygwin.com/setup-x86.exe)). Grab the setup script ([64 bit](tools/cygwin_setup-x86.bat) or [32 bit](tools/cygwin_setup-x86_64.bat)).  
Place these files next to eachother and run the batch file. Follow through the installation, there's no need to select any additional packages.

Change to a directory accessible from Windows where you'll store the files, for example:
```
cd /cygdrive/c/Users/$USER/Desktop
```

Continue with the [building instructions](#building)


### Linux

You need to install the following software:
* git
* gcc
* make
* bison
* flex
* wine (with 32-bit support!)

On Debian or Ubuntu you can do this by running:
```
[TODO]
sudo dpkg --add-architecture i386
sudo apt-get update
sudo apt-get install git gcc make bison flex wine32
```

Continue with the [building instructions](#building)


### Building

Clone the repository:
```
git clone <repo url> pokemon_dp
cd pokemon_dp
```

To build everything (necessary the first time around!):
```
./build_all.sh
```
You might have to run this multiple times as the build system is prone to breaking.

After this, you can build any changes to only the game itself by running:
```
source ./env
make
```
