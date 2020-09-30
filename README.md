# MG5aMC_PWG_HJJ
a repository with instructions and scripts for producing HJJ CP samples

## Download and setup madgraph and powheg-box

Download madgraph and untar:

              wget http://madgraph.physics.illinois.edu/Downloads/MG5_aMC_v2.7.0.tar.gz
              tar -xvf MG5_aMC_v2.7.0.tar.gz

setup a CMSSW environment:

              cmsrel CMSSW_10_2_19
              cd CMSSW_10_2_19/src/; cmsenv; cd ../../

## Interface with powheg box v2

Get plugin:

              bzr branch lp:~mg5amc-pwg-team/mg5amc-pwg/v0

Then copy MG5aMC_PWG to PLUGIN directory

Download powheg box:

              svn checkout --username anonymous --password anonymous svn://powhegbox.mib.infn.it/trunk/POWHEG-BOX-V2


Run mg5_aMC (Note you seem to need to run the bin/mg5_aMC once first without the --mode=MG5aMC_PWG option to ensure everything gets installed properly):

              python ./bin/mg5_aMC --mode=MG5aMC_PWG
              import model HC_NLO_X0_UFO-heft
              set_powhegbox_path /vols/build/cms/dw515/POWHEG-BOX-V2
              set fastjet /cvmfs/cms.cern.ch/slc7_amd64_gcc700/external/fastjet/3.3.0-pafccj/bin/fastjet-config 
              set lhapdf /vols/build/cms/dw515/LHAPDF/bin/lhapdf-config
              generate p p > x0 j j / t [QCD]
              output gghjj
              exit

              cd gghjj

svn checkout the X0jj directory from the User-Processes-V2 repository:

              svn co --username anonymous --password anonymous svn://powhegbox.mib.infn.it/trunk/User-Processes-V2/X0jj  ./"X0jj_svn_directory"

replace all the files in the "directory_where_the_code_by_MG_is_created" with the files from the svn directory "X0jj_svn_directory"

#open Makefile and set path to POWHEG-BOX-V2

              BOX=/vols/build/cms/dw515/POWHEG-BOX-V2

#also change fastjet-config to correct path (note appears several times in makefile, change all occurances) 

              FASTJET_CONFIG=$(shell which /cvmfs/cms.cern.ch/slc7_amd64_gcc700/external/fastjet/3.3.0-pafccj/bin/fastjet-config)

#set correct pdf sets in powheg.input

              lhans1 306000      ! pdf set for hadron 1 (LHA numbering)
              lhans2 306000      ! pdf set for hadron 2 (LHA numbering) 

make a copy of this input file for reference

              cp powheg.input powheg.input-origional

modify Cards/params_card.dat as desired then compile:

              make -j8

# Setup generation, produce events, and do reweighting:

Hote the modifications to powheg.inputs with the exception of the MGcosa option are done already in the version included in this repository so no need to do them again!

modify powheg.input as follows:

              numevts NEVENTS

Make sure MGcosa is set to the value you want as this overwrites the value set in params_card.dat:

for SM:

              MGcosa    1d0
for PS:

              MGcosa    0d0
for MM:

              MGcosa    -0.707107d0
              
Also add the following to ensure reweighting will work properly later:
              lhrwgt_id 'nominal'



Use these settings to reduce the number of negative weights to ~10%

              foldcsi   2    ! number of folds on csi integration
              foldy     5    ! number of folds on  y  integration
              foldphi   2    ! number of folds on phi integration

To ensure the integration grid etc is accurate enough the following parameters are used with the instructions below. Since these are run as parallel jobs you would need to adjust these parameters if you use a different number of jobs. These were chosen based off the recomendations in https://twiki.cern.ch/twiki/bin/viewauth/CMS/PowhegBOXPrecompiled :

              ncall1  2500  ! number of calls for initializing the integration grid
              itmx1    1     ! number of iterations for initializing the integration grid
              ncall2  1250  ! number of calls for computing the integral and finding upper bound
              itmx2     5    ! number of iterations for computing the integral and finding upper bound
              nubound 5000  ! number of bbarra calls to setup norm of upper bounding function

If you use a different jobs for stage 1 (-p 1 in batch_sub.py script) or you use a different number of submissions with option -x (different from 5) you need to change ncall1 - the total number of calls should remain the same. The total number of calls used = ncall1*(number of -x itterations)*(number of jobs) 2500 * 5 * 400 = 5000000

If you use a different number of jobs for stage 2 (-p 2) then change ncall2 so the total number of calls is the same. The total number of calls is ncall2*itmx2*(number of jobs) 1250 * 5 * 800 = 5000000

If you use a different number of jobs for stage 3 (-p 3) then change nubound so the total nubound*(number of jobs) is the same. nubound*(number of jobs) = 5000 * 400 = 2000000 

In Cards/params_card.dat set the following options

              2 0.000000e+00 # cosa 
              14 1.000000e+00 # kHgg  
              15 0.666667e+00 # kAgg 

cosa = 0.0 corresponds to CP-odd, cosa=1 is CP-even, cosa=-1/sqrt(2) = max-mix with so-called JHUGen sign convention for the interference.
This parameter gets overwritten by MGcosa in powheg.inputs so make sure you change it in there as well

Build again as you have changed params_card.dat:
              
              make -j8


If you are running parallel jobs using batch_sub.py script then make a copy of powheg.input named powheg.input-base

              cp powheg.input powheg.input-base

Prepare run directory:

              ./prepare_run_dir RUN1 # if you are submitting jobs using batch_sub.py then you need to make sure you use the version of prepare_run_dir in the repository not the one that you get when setting up the powheg-box generator!

              cd RUN1

Run jobs in parallel on the IC batch system (can modify batch_sub.py to work on other batch systems if needed), note you need to wait for all jobs to finish after each batch_sub.py run before submitting the next step:

run stage 1 5 times with 400 cores:

              python ../batch_sub.py -p 1 -j 400

              python ../batch_sub.py -p 1 -x 2 -j 400

              python ../batch_sub.py -p 1 -x 3 -j 400

              python ../batch_sub.py -p 1 -x 4 -j 400

              python ../batch_sub.py -p 1 -x 5 -j 400

stage 2 run once:

              python ../batch_sub.py -p 2 -j 800

stage 3 run once:

              python ../batch_sub.py -p 3 -j 400

run events with, change -n option to produce a different number of jobs with each run 

              python ../batch_sub.py -p 4 -j 1000 -n 200

It is useful to run the ... script at this point to fix the produced lhe files in case the jobs times out and a partical lhe event is written at the end

              ../fix_lhe.sh

run reweighting with replacing "-p 5" which is sm weight with "-p 6" for ps and "-p 7" for mm:

              python ../batch_sub.py -p 5 -j 1000 # add sm weight
              python ../batch_sub.py -p 5 -j 1000 # add ps weight
              python ../batch_sub.py -p 5 -j 1000 # add mm weight
              
# Other useful tools

To combine together LHE files into 1 big file use mergeLheFiles
Build:

              g++ -Wall -o mergeLheFiles mergeLheFiles.cpp
          
Run:
    
              ./mergeLheFiles pwgevents-0001.lhe pwgevents-0002.lhe pwgevents-0003.lhe ...
