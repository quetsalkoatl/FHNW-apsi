# The WebGoat exercise environment
In the following two lectures you will get hands-on experience using a vulnerable system (*WebGoat*). You will learn to hack the most common vulnerabilities with this respect.

# Setup
The setup is pretty simple. The OWASP offers docker images for *WebGoat* and *WebWolf* (which you as an attacker will use for certain exercises). 

## Install docker-compose on the apsi-host
To start the environment, the OWASP offers a simple *docker-compose.yml* file, which you will in this folder. However, to use `docker-compose`, you will first have to install it. You will find a simple script in this folder which will do the job for you. First make the *install_docker-compose.yml* file executable:

`vagrant@apsi-host:~/FHNW-apsi/Vorlesung/v06$ sudo chmod u+x install_docker-compose.sh`

and then execute it:

`vagrant@apsi-host:~/FHNW-apsi/Vorlesung/v06$ ./install_docker-compose.sh`

After installation you may test whether or not `docker-compose` is correctly installed on the demo-machine:

`vagrant@apsi-host:~/FHNW-apsi/Vorlesung/v06$ docker-compose -v`

You should now see the version of your `docker-compose` installation.

## Start WebGoat and WebWolf
Now, that you have `docker-compose` installed, you may just have to execute it with the given *docker-compose.yml* file:

`vagrant@apsi-host:~/FHNW-apsi/Vorlesung/v06$ docker-compose up`

# Using WebGoat
After *WebGoat* and *WebWolf* are running properly, you may just use your favorite browser on your host system and point it to:

`http://localhost:8080/WebGoat`

or

`http://localhost:9090/WebWolf`

respectively.

If you do so, just register a new user first. You will use these credentials further on to authenticate yourself.

*Happy hacking*