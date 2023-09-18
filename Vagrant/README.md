# ShadeWatcher Vagrant Setup

This directory contains the Vagrant file for automatically setting up ShadeWatcher.

## Usage

To use this Vagrant file, follow the instructions below:

1. Ensure you have [Vagrant](https://www.vagrantup.com/) installed on your system.
2. Ensure you have [svn](https://subversion.apache.org/packages.html#ubuntu) installed on your system.
2. Setup the VM

```
svn checkout https://github.com/mjafferiqbal18/ShadeWatcher/trunk/Vagrant
cd ./Vagrant
vagrant up --provision
```
