# Introduction

This repository hosts the Monte Carlo simulation for CYGNO experiment.
The prerequisite to run this simulations is to have ROOT (v6.X.X), GEANT4 (v10.05.X) and CADMesh (v1.1) software installed.
For more informations about the software see:
* ROOT:  https://root.cern.ch/ 
* GEANT4: http://geant4.cern.ch/
* CADMesh: https://github.com/christopherpoole/CADMesh


# Setup ROOT, GEANT4 and CADMesh

Setup all the environment variables of ROOT and GEANT4.

## Instructions for Roma 1 cluster

In `farm-login.roma1.infn.it` you can do
```
###for ROOT
alias cmake="/ua9/soft/cmake-3.14.4-install/bin/cmake"
source /ua9/soft/root-v6-12-06-install/bin/thisroot.sh
## for pyROOT
export LD_LIBRARY_PATH=$PYTHONDIR/lib:$LD_LIBRARY_PATH
export PYTHONPATH=$ROOTSYS/lib:$PYTHONPATH
## for geant4
source /ua9/soft/geant4.10.05.p01-install/bin/geant4.sh 
alias g4cmake="cmake -DGeant4_DIR=/ua9/soft/geant4.10.05.p01-install/lib64/Geant4-10.5.1/ -Dcadmesh_DIR=/ua9/soft/CADMesh-install/lib/cmake/cadmesh-1.1.0/"

## CADMesh
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/ua9/soft/CADMesh-install/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/ua9/soft/CADMesh-install-geant10.4.2/lib
```

## Instructions for Roma 3 cluster

In `ui7-01.roma3.infn.it` you can do
```
#####for ROOT
alias cmake="/storage/local/exp_soft/cygnorm3/cmake-3.14.6-install/bin/cmake"
source /storage/local/exp_soft/cygnorm3/root-v6-12-06-install/bin/thisroot.sh
### for pyROOT
export LD_LIBRARY_PATH=$PYTHONDIR/lib:$LD_LIBRARY_PATH
export PYTHONPATH=$ROOTSYS/lib:/storage/local/exp_soft/cygnorm3/python2.7-local/lib/python2.7/site-packages:/storage/local/exp_soft/cygnorm3/python2.7-local/python2.7-local/lib64/python2.7/site-packages:$PYTHONPATH
#### for geant4
source /storage/local/exp_soft/cygnorm3/geant4-v10.5.1-install/bin/geant4.sh
alias g4cmake="cmake -DGeant4_DIR=/storage/local/exp_soft/cygnorm3/geant4-v10.5.1-install/lib64/Geant4-10.5.1/ -Dcadmesh_DIR=/storage/local/exp_soft/cygnorm3/CADMesh-install/lib/cmake/cadmesh-1.1.0/"

#### CADMesh
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/storage/local/exp_soft/cygnorm3/CADMesh-install/lib/
```

In general:

```
source path-to-root-install/bin/thisroot.sh
source path-to-geant-install/bin/geant.sh
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:path-to-CADmesh-install/lib
```


You can put these commands in your `.bashrc` to execute them automatically every time you open a bash shell.

# Download CYGNO-MC repository

Download CYGNO and LIME geometry files
```
git clone git@github.com:CYGNUS-RD/geometry.git
```

Download Geant4 code 
```
git clone git@github.com:CYGNUS-RD/CYGNO-MC.git
```
or, if you don't want to configure a ssh key in gitlab use https protocol
```
git clone https://github.com/CYGNUS-RD/CYGNO-MC.git
```

Now you have downloaded the code in `CYGNO-MC/` directory

# Setup CYGNO-MC code

Create a build directory 
```
mkdir CYGNO-MC-build
```
and compile CYGNO-MC code
```
cd CYGNO-MC-build
g4cmake ../CYGNOMC
make -j`nproc`
```

Now you have the CYGNO executable in the build directory.
You can run it in graphic mode:
```
./CYGNO
```
or specify a macro to run instructions
```
./CYGNO macro.mac
```
Some example macros are available in the `macro` directory.


A guide of CYGNO commands is in  CYGNOCommandsREADME file.


# Split jobs in the batch system

A set of scripts to split the simulation in multiple jobs is provided in the folder `scripts`.
Some command examples are provided for roma3 cluster, where PBS batch system is used.
Examples of basic commands of PBS can be found in https://www.bo.infn.it/alice/introgrd/pbsabout/node17.html (or simply google).

## Submit and check status of single job

Submit job:
```
qsub scripts/examplejob.sh
```
Check status:
```
qstat -u $USER
```


## Split jobs

Example to split 100M events into 100 jobs:
```
python scripts/submit_jobs_rm3.py -m CYGNOtest_surface_gamma --tag ext_gamma -n 100000000 -e 1000000 --builddir /storage/local/home/cygnorm3/dimperio/CYGNO/CYGNO-MC-build/
```

Options meaning:

* `-m` name of the macro template. The script looks for the macro in the directory `macro/` (use macro name without `.mac` extension)
* `-n` total number of events to generate
* `-e` events per job
* `--tag` useful tag to identify the simulation. Output will be saved in a directory with this name
* `--builddir` path to dir containing CYGNO executable

By default the output is saved in `/storage/DATA-03/cygnorm3/CYGNO-MC-data/pbs_outputs/<tag-string>`
Also other directories will be created in `pbs_logs` and  `pbs_workdir` folders, containing respectively the logs and the copy of the macro for each job.


## Submit jobs with multiple macro configurations (for radioactive decays)

Example to send on batch multiple radioactive isotopes simulations:
```
python scripts/submit_jobs_rm3.py -m CYGNOtest --tag PbShieldRadioactivity -f U238Activity_Shield2Pb_10Pb2Cu.txt -e 1000000  --builddir /storage/local/home/cygnorm3/dimperio/CYGNO/CYGNO-MC-build/
```
Options meaning:
* `-f` configuration file for isotopes to be simulated. The script looks for the file in the `background/` directory. The file contains 4 columns: Name, Z, A, NEvents
* the other options are the same. Note that `-n` option is not necessary when `-f` option is used (and will be ignored), since the total number of events is in the configuration file
