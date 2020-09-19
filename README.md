# FHNW-apsi
Welcome to the FHWN apsi course. This repo hosts all material used in the course. We suggest that you [clone this repo](https://docs.github.com/en/github/creating-cloning-and-archiving-repositories/cloning-a-repository) to your local machine. So you are up to date with the course material by pulling the repo.

## Demo Environment
In certain lectures, demonstrations will be given. In others, you will have to work on practical exercises. Therefore, you will have to set up a virtual **Demo Environment**. In the following we describe two ways to do that. The easy way and the manual way.

### Setup the easy way
> Note that having already installed another virtualization solution, e.g., VMware may cause problems with VirtualBox and you should set up the Demo Environment manually using your existing virtualization solution!

#### Prerequisites
In order to set up the Demo Environment the easy way, you first need to install:

- [VirtualBox](https://www.virtualbox.org/wiki/Downloads)
- [Vagrant](https://www.vagrantup.com/downloads)

> On **Mac**, make sure you enable VirtualBox's kernel extension within 30 minutes after installation: **System Preferences → Security & Privacy → General**

> On **Windows 10**, to avoid problems, you should clone the repo outside of your user folder or any system folder, e.g., you could use a new folder *C:\FHNW*.
You should also create a new folder for the Vagrant boxes, e.g., *C:\Vagrant* and set the **$VAGRANT_HOME** enviroment variable to it by running the following command as an administrator:
`setx VAGRANT_HOME "C:/Vagrant" /M`

#### Install the Demo Environment with Vagrant
After you installed the necessary tools you may point your command line to the folder containing the `Vagrantfile` and hit the following command:`vagrant up`

> Note that you need a fast internet connection, since the linux distribution (Debian) will now be downloaded and set up.

#### Access the Demo Environment
At the end of the installation, you will see the hints on how to access the Demo Environment using SSH.

### Setup manually
If you choose to set up the Demo Environment manually, you will have to do the following with whatever tools you have in place.

#### Virtual machine
1. Get the latest Debian 10 ISO file and set up a new virtual machine.
1. Configure the virtual network to forward the following ports from the guest to the host machine: **22 → 2222, 80 → 80, 443 → 443, 8080 → 8080, 8081 → 8081, 9090 → 9090**.
1. Access your virtual machine using SSH.
1. Install the following tools in the newly created virtual machine:
    - docker
    - build-essential
    - manpages-dev
    - gcc-multilib
    - gdb
    - git
1. Clone this repo within your guest system in order to have the exercise material right in your Demo Environment.

