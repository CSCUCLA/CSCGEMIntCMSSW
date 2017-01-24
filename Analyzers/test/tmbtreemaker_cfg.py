import FWCore.ParameterSet.Config as cms

process = cms.Process("TEST")

from FWCore.ParameterSet.VarParsing import VarParsing
options = VarParsing('analysis')
options.parseArguments()

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
process.options = cms.untracked.PSet( SkipEvent =
cms.untracked.vstring('ProductNotFound') )

process.source = cms.Source ("PoolSource",
        fileNames = cms.untracked.vstring(options.inputFiles )        

)
process.MessageLogger = cms.Service("MessageLogger",
    cout = cms.untracked.PSet(
        default = cms.untracked.PSet( limit = cms.untracked.int32(100) ),
        FwkJob = cms.untracked.PSet( limit = cms.untracked.int32(0) )
    ),
    categories = cms.untracked.vstring('FwkJob'),
    destinations = cms.untracked.vstring('cout')
)



process.load('Configuration.Geometry.GeometryExtended2023D4Reco_cff')

process.analyze = cms.EDAnalyzer("TMBTreeMaker",
        output       = cms.untracked.string(options.outputFile),        
        
        compDigiTag        = cms.InputTag("simMuonCSCDigis", "MuonCSCComparatorDigi"),       
        tmb_onlyME11       = cms.bool(True),
        
        genParticleTag     = cms.InputTag("genParticles"),   
        trackingParticleTag= cms.InputTag("mix","MergedTrackTruth"),
        cscSimHitTag       = cms.InputTag("g4SimHits","MuonCSCHits"),   
        CSCStripDigiSimLink       = cms.InputTag("simMuonCSCDigis","MuonCSCStripDigiSimLinks"),   
        CSCWireDigiSimLink       = cms.InputTag("simMuonCSCDigis","MuonCSCWireDigiSimLinks"),   
        
        )


process.p = cms.Path(process.analyze)

