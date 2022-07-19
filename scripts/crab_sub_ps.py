from CRABClient.UserUtilities import config
config = config()

config.General.requestName     = 'ggHjj_minlo_Jun15_PS'
config.General.workArea        = 'ggHjj_minlo_Jun15_PS'
config.General.transferOutputs = True
config.General.transferLogs    = True
config.JobType.numCores = 4
config.JobType.maxMemoryMB = 8000
config.JobType.pluginName = 'PrivateMC'
config.JobType.psetName   = 'gen_gghjj_filter_PS_cfg.py'

config.JobType.allowUndistributedCMSSW = True
config.Data.outputPrimaryDataset = 'ggHjj_minlo_Jun15_PS'
config.Data.inputDBS             = 'global'
config.Data.splitting            = 'EventBased'
config.Data.unitsPerJob          = 500
config.Data.totalUnits           = 3000000
config.Data.outLFNDirBase        = '/store/user/dwinterb/ggHjj_minlo_Jun15_PS/'
config.Data.publication          = True
config.Data.outputDatasetTag     = 'ggHjj_minlo_Jun15_PS'
config.JobType.inputFiles = ['X0jj_slc7_amd64_gcc820_CMSSW_10_6_7_X0jj_PS_HTT_v3.tgz']


config.Site.storageSite = 'T2_UK_London_IC'
