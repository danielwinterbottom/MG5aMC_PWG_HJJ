# MG5aMC_PWG_HJJ
a repository with instructions and scripts for producing HJJ CP samples

## download and setup madgraph and powheg-box

## download madgraph and untar
wget http://madgraph.physics.illinois.edu/Downloads/MG5_aMC_v2.7.0.tar.gz
tar -xvf MG5_aMC_v2.7.0.tar.gz

#setup a CMSSW environment
cmsrel CMSSW_10_2_19
cd CMSSW_10_2_19/src/; cmsenv; cd ../../

##interface with powheg box

get plugin:
bzr branch lp:~mg5amc-pwg-team/mg5amc-pwg/v0

##copy MG5aMC_PWG to PLUGIN directory

download powheg box
svn checkout --username anonymous --password anonymous svn://powhegbox.mib.infn.it/trunk/POWHEG-BOX-V2

## note you seem to need to run the bin/mg5_aMC once without the --mode=MG5aMC_PWG option to ensure everything gets installed properly

python ./bin/mg5_aMC --mode=MG5aMC_PWG
import model HC_NLO_X0_UFO-heft
set_powhegbox_path /vols/build/cms/dw515/POWHEG-BOX-V2
set fastjet /cvmfs/cms.cern.ch/slc7_amd64_gcc700/external/fastjet/3.3.0-pafccj/bin/fastjet-config 
set lhapdf /vols/build/cms/dw515/LHAPDF/bin/lhapdf-config
# note for different ddf's might need to use: set lhapdf /PATH/TO/lhapdf-config 
generate p p > x0 j j / t [QCD]
output gghjj
exit

cd gghjj

# svn checkout the X0jj directory from the User-Processes-V2 repository:

svn co --username anonymous --password anonymous svn://powhegbox.mib.infn.it/trunk/User-Processes-V2/X0jj  ./"X0jj_svn_directory"

# replace all the files in the "directory_where_the_code_by_MG_is_created"
with the files from the svn directory "X0jj_svn_directory"

# modify params card
depending on the process you want

#CP mixed Higgs (old 2*SM cross section)
       2 0.707000e+00 # cosa
      14 1.414000e+00 # kHgg
      15 0.943000e+00 # kAgg
#CP mixed Higgs
       2 0.707000e+00 # cosa
      14 1.000000e+00 # kHgg
      15 0.666667e+00 # kAgg
#Scalar H
       2 1.000000e+00 # cosa
      14 1.000000e+00 # kHgg
      15 1.000000e+00 # kAgg
#Pseudoscalar A
        2 0.000000e+00 # cosa
      14 1.000000e+00 # kHgg
      15 0.666667e+00 # kAgg

#open Makefile and set path to POWHEG-BOX-V2
BOX=/vols/build/cms/dw515/POWHEG-BOX-V2

#also change fastjet-config to correct path (note appears several times in makefile, change all occurances) 
FASTJET_CONFIG=$(shell which /cvmfs/cms.cern.ch/slc7_amd64_gcc700/external/fastjet/3.3.0-pafccj/bin/fastjet-config)

#set correct pdf sets in powheg.input

lhans1 306000      ! pdf set for hadron 1 (LHA numbering)
lhans2 306000      ! pdf set for hadron 2 (LHA numbering) 

#and put number of events to: 
numevts 100

# make a copy of this input file for reference

cp powheg.input powheg.input-origional

#then make:

make

# to setup generation, produce event and do reweighting

# note the modifications to powheg.inputs with the exception of the MGcosa option are done already in the version included in this repository so no need to do them again!

# modify powheg.input as follows:
numevts NEVENTS
iseed    SEED

# in params_card.dat set the following options
    2 0.000000e+00 # cosa 
   14 1.000000e+00 # kHgg  
   15 0.666667e+00 # kAgg 

# cosa = 0.0 corresponds to CP-odd, cosa=1 is CP-even, cosa=-1/sqrt(2) = max-mix with so-called JHUGen sign convention for the interference.
# This parameter gets overwritten by MGcosa in powheg.inputs so make sure you change it in there as well

# also add the following to ensure reweighting will work properly later:
lhrwgt_id 'nominal'

# in powheg.input make sure MGcosa is set to the value you want as this overwrites the value set in params_card.dat

# use these settings to reduce the number of negative weights

foldcsi   2    ! number of folds on csi integration
foldy     5    ! number of folds on  y  integration
foldphi   2    ! number of folds on phi integration

# to ensure the integration grid etc is accurate enough the following parameters are used with the instructions below. Since these are run as parallel jobs you would need to adjust these parameters if you use a different number of jobs:

ncall1  2500  ! number of calls for initializing the integration grid
itmx1    1     ! number of iterations for initializing the integration grid
ncall2  1250  ! number of calls for computing the integral and finding upper bound
itmx2     5    ! number of iterations for computing the integral and finding upper bound
nubound 5000  ! number of bbarra calls to setup norm of upper bounding function

# if you use a different jobs for stage 1 (-p 1 in batch_sub.py script) or you use a different number of submissions with option -x (different from 5) you need to change ncall1 - the total number of calls should remain the same

# if you use a different number of jobs for stage 2 (-p 2) then change ncall2 so the total number of calls is the same 

# if you use a different number of jobs for stage 3 (-p 3) then change nubound so the total nubound*Njobs is the same

# if you are running parallel jobs using batch_sub.py script then make a copy of powheg.input named posheg.input-base

# prepare run directory using:


cp powheg.input powheg.input-base

./prepare_run_dir RUN1 # if you are submitting jobs using batch_sub.py then you need to make sure you use the version of prepare_run_dir in the repository not the one that you get when setting up the powheg-box generator!

cd RUN1
 
#run stage 1 5 times with 400 cores:

python ../batch_sub.py -p 1 -j 400

python ../batch_sub.py -p 1 -x 2 -j 400

python ../batch_sub.py -p 1 -x 3 -j 400

python ../batch_sub.py -p 1 -x 4 -j 400

python ../batch_sub.py -p 1 -x 5 -j 400

#stage 2 run once:

python ../batch_sub.py -p 2 -j 800


#stage 3 run once:

python ../batch_sub.py -p 3 -j 400

#run events with 

python ../batch_sub.py -p 4 -j 1000 -n 200

# run reweighting with replacing "-p 5" which is sm weight with "-p 6" for ps and "-p 7" for mm
python ../batch_sub.py -p 5 -j 1000
