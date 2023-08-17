#!/bin/sh

# Parser setup

echo "Setting up virtual machine "

# Define the number of maximum retries
MAX_RETRIES=3

# Function to check command success and handle errors with retries
check_command_with_retries() {
    local retries=0
    local status=1
    while [ $retries -lt $MAX_RETRIES ] && [ $status -ne 0 ]; do
        "$@"
        status=$?
        if [ $status -ne 0 ]; then
            echo "Error occurred: Retrying..."
            sleep 5  # Add a delay between retries
            retries=$((retries + 1))
        fi
    done
    if [ $status -ne 0 ]; then
        echo "Max retries reached. Cannot continue. Exiting..."
        exit $status
    fi
}

check_command_with_retries sudo apt-get update -y 
check_command_with_retries sudo apt install python3-pip -y 
check_command_with_retries sudo apt-get install git -y 
check_command_with_retries pip3 install gdown 

git clone https://github.com/mjafferiqbal18/ShadeWatcher.git
cd ShadeWatcher/lib/
LIB_INSTALL_PATH=$PWD

# Parser Setup

#neo 4j
check_command_with_retries sudo add-apt-repository ppa:cleishm/neo4j -y 
check_command_with_retries sudo apt-get update -y 
check_command_with_retries sudo apt-get install libssl-dev neo4j-client libneo4j-client-dev -y 

check_command_with_retries sudo apt-get install libjsoncpp-dev -y
check_command_with_retries sudo apt install build-essential -y 
check_command_with_retries sudo apt-get install pkg-config libudev-dev libconfig++-dev libconfig-dev -y

# Add the repository for GCC 9
sudo add-apt-repository ppa:ubuntu-toolchain-r/test -y
check_command_with_retries sudo apt-get update -y 

# Install GCC 9 and G++ 9
check_command_with_retries sudo apt install gcc-9 g++-9 -y
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 90 --slave /usr/bin/g++ g++ /usr/bin/g++-9

#postgres
check_command_with_retries sudo apt-get install postgresql postgresql-contrib -y
check_command_with_retries sudo apt install libpq-dev
git clone https://github.com/jtv/libpqxx.git
cd libpqxx
git checkout master
./configure CXXFLAGS="-std=c++17" --disable-documentation --prefix=$LIB_INSTALL_PATH
make -j8
make install
cd ..

# boost
check_command_with_retries sudo apt-get install libboost-all-dev -y

# libconfig
wget https://hyperrealm.github.io/libconfig/dist/libconfig-1.7.2.tar.gz
sudo tar xzvf libconfig-1.7.2.tar.gz
cd /home/vagrant/ShadeWatcher/lib/libconfig-1.7.2/
sudo ./configure --prefix=$LIB_INSTALL_PATH
sudo make -j8
sudo make install
cd ../

#compilation
echo "Compiling ShadeWatcher"
cd /home/vagrant/ShadeWatcher/parse/

sudo make

sudo cp /home/vagrant/ShadeWatcher/lib/libconfig-1.7.2/lib/.libs/libconfig++.so.11 /usr/lib
sudo mkdir -p /home/vagrant/ShadeWatcher/data/encoding/e3_trace
chmod +x compiler.sh

# Recommendation Model Setup

# Install Python virtualenv
check_command_with_retries sudo apt-get install python3-venv -y

# Define the path for the virtual environment
VENV_PATH="/home/vagrant/.virtualenvs/shadewatcher"

# Check if the virtual environment exists
if [ ! -d "$VENV_PATH" ]; then
    # Create the virtual environment
    echo "Creating virtual environment"
    /usr/bin/python3.6 -m venv "$VENV_PATH"
fi

# Activate the virtual environment
echo "Activating virtual environment"
. "$VENV_PATH/bin/activate"

# Install required packages
echo "Installing packages..."

# TensorFlow
# Download tensorflow dependencies separately to avoid the gcrpio dependency.
pip install tensorflow_gpu==1.14 --no-deps
pip install protobuf==3.6.1
pip install wrapt
pip install termcolor
pip install keras_applications
pip install keras_preprocessing
pip install tensorflow_estimator==1.14.0
pip install dataclasses
pip install importlib-metadata
pip install Markdown
pip install absl-py
pip install gast
pip install astor

# Numpy
pip install numpy==1.19.2

# SciPy
pip install scipy==1.5.3

# scikit-learn
pip install scikit-learn==0.19.0

# tqdm
pip install tqdm

# colorlog
pip install colorlog

# Deactivate the virtual environment
deactivate

# Give vagrant user full access
cd /home/vagrant
sudo chown -R vagrant:vagrant /home/

echo "Provisioning completed."
