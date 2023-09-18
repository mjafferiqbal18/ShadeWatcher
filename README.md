# ShadeWatcher

## Installation

Please use this [vagrant script](Vagrant) to setup the Ubuntu VM to use ShadeWatcher

## Demo

The parser is setup such that it expects to receive a path to the folder containing the TRACE files and a path to a folder that contain the ground_truth files. The g.t files are only needed for corresponding attack files (so, if parsing the whole TRACE E3 dataset, the g.t folder should contain 3 files with the following naming convention: ta1-trace-e3-official.json.XXX_malicious_truth.txt). For context and testing purposes, a (partially accurate) ground truth file has been provided [here](ground_truth) (Try and run a single file, ta1-trace-e3-official.json.125, along with this)

Inside the VM:
Ensure the encoding directory (ShadeWatcher/data/encoding/e3_trace) is clean.
Run the parser, adjust the paths to the dataset, the ground truth, and the number of threads as necessary.

```
cd ShadeWatcher/parse
./driverdar -dataset e3_trace -trace ../trace_jsons/ -malicious_truth ../ground_truth/ -multithread 20
```

Run the model.

```
cd ../recommend
source /home/vagrant/.virtualenvs/shadewatcher/bin/activate
python driver.py --dataset e3_trace --epoch 5 --ground_truth_given --show_test --show_val
```

More details on what the setup is based on can be found in the readme of the original repository [here](https://github.com/jun-zeng/ShadeWatcher)
