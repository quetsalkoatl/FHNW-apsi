#!/bin/bash
echo "Installing docker-compose on the machine"
sudo curl -L "https://github.com/docker/compose/releases/download/1.27.4/docker-compose-$(uname -s)-$(uname -m)" -o /usr/local/bin/docker-compose
sudo chmod +x /usr/local/bin/docker-compose
echo "Successfully installed, you may now run the docker-compose file using: docker-compose up"