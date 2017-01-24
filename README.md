# CSCGEMIntCMSSW
`cmsrel CMSSW_9_0_0_pre2`  
`cd CMSSW_9_0_0_pre2/src`  
`cmsenv`   
`git clone git@github.com:{USER}/CSCGEMIntCMSSW.git`   
`git clone git@github.com:{USER}/AnalysisSupport.git`   
`scram b -j20`   
`cmsRun CSCGEMIntCMSSW/Analyzers/test/tmbtreemaker_cfg.py inputFiles="file:step2.root"`   
