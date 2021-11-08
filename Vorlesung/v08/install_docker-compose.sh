#!/bin/bash
echo "Installing docker-compose on the machine"
sudo curl -L "https://github.com/docker/compose/releases/download/v2.1.0/docker-compose-$(uname -s)-$(uname -m)" -o /usr/local/bin/docker-compose
sudo chmod +x /usr/local/bin/docker-compose
echo "Successfully installed, you may now run the docker-compose file using: docker-compose up"