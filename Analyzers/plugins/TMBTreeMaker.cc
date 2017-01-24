#include "FWCore/Framework/interface/MakerMacros.h"

#include "../interface/BaseTreeMaker.h"
#include "CSCGEMIntCMSSW/TreeFillers/interface/TMBFiller.h"
#include "CSCGEMIntCMSSW/TreeFillers/interface/SimMuonFiller.h"
using namespace CSCGEMInt;

class TMBTreeMaker : public BaseTreeMaker {
public:
    explicit TMBTreeMaker(const edm::ParameterSet& cfg) : BaseTreeMaker(cfg){
    	addFiller(new TMBFiller(cfg,consumesCollector()));
    	addFiller(new SimMuonFiller(cfg,consumesCollector(),(TMBFiller*)fillers.back(),true));
    	setup();
    }
    ~TMBTreeMaker() {};

};

DEFINE_FWK_MODULE(TMBTreeMaker);
