import FWCore.ParameterSet.Config as cms

MssmHbbNtuplizerTriggerPaths = cms.PSet(
    TriggerPaths    = cms.vstring  (
    ## I recommend using the version number explicitly to be able to compare 
    ## however for production one has to be careful that all versions are included.
    ## Thinking of a better solution...
# physics triggers
                                     'HLT_DoublePFJets100MaxDeta1p6_DoubleCaloBTagCSV_p33_v', 
                                     'HLT_Mu12_DoublePFJets40MaxDeta1p6_DoubleCaloBTagCSV_p33_v',
# backup triggers                                     
                                     'HLT_DoublePFJets116MaxDeta1p6_DoubleCaloBTagCSV_p33_v', 
                                     'HLT_DoublePFJets128MaxDeta1p6_DoubleCaloBTagCSV_p33_v', 
                                     'HLT_Mu12_DoublePFJets54MaxDeta1p6_DoubleCaloBTagCSV_p33_v', 
                                     'HLT_Mu12_DoublePFJets62MaxDeta1p6_DoubleCaloBTagCSV_p33_v',
# control triggers
### btag                             
                                     'HLT_SingleJet30_Mu12_SinglePFJet40_v', 
                                     'HLT_DoublePFJets40_CaloBTagCSV_p33_v', 
                                     'HLT_DoublePFJets100_CaloBTagCSV_p33_v', 
                                     'HLT_DoublePFJets200_CaloBTagCSV_p33_v', 
                                     'HLT_DoublePFJets350_CaloBTagCSV_p33_v', 
                                     'HLT_Mu12_DoublePFJets40_CaloBTagCSV_p33_v', 
                                     'HLT_Mu12_DoublePFJets100_CaloBTagCSV_p33_v', 
                                     'HLT_Mu12_DoublePFJets200_CaloBTagCSV_p33_v', 
                                     'HLT_Mu12_DoublePFJets350_CaloBTagCSV_p33_v',
### jet triggers
                                     'HLT_PFJet40_v', 
                                     'HLT_PFJet60_v', 
                                     'HLT_PFJet80_v', 
                                     'HLT_PFJet140_v', 
                                     'HLT_PFJet200_v', 
                                     'HLT_PFJet260_v', 
                                     'HLT_PFJet320_v', 
                                     'HLT_PFJet400_v', 
                                     'HLT_PFJet450_v', 
                                     'HLT_PFJet500_v', 
                                     'HLT_PFJet550_v',
                                     'HLT_DiPFJetAve40_v',
                                     'HLT_DiPFJetAve80_v',
                                     'HLT_DiPFJetAve200_v',
                                     'HLT_DiPFJetAve320_v',
                                     'HLT_DiPFJetAve500_v',
                                      

### muon triggers
                                     'HLT_Mu8_v', 
                                     'HLT_Mu3_PFJet40_v', 
### zerobias trigger                                     
                                     'HLT_ZeroBias_v',
    ),
)

