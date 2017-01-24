#include "../interface/BaseTreeMaker.h"

#include "AnalysisSupport/TreeInterface/interface/TreeWrapper.h"
#include "CSCGEMIntCMSSW/TreeFillers/interface/BaseFiller.h"
#include <TFile.h>

namespace CSCGEMInt {

BaseTreeMaker::BaseTreeMaker(const edm::ParameterSet& iConfig) :
		file (new TFile(iConfig.getUntrackedParameter<std::string>("output").c_str(), "RECREATE")),
		tree (new TreeWrapper(new TTree("Events",""),"Events"))
		{}
BaseTreeMaker::~BaseTreeMaker() {
	  file->cd();
	  tree->getTree()->Write();
	  file->Close();
	  delete file;
	for(auto* f :fillers) delete f;
}

void BaseTreeMaker::addFiller(BaseFiller * filler) {fillers.push_back(filler);}
void BaseTreeMaker::setup() { for(auto * f : fillers) f->setup(tree);}
void BaseTreeMaker::processRun(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
	for(auto * f : fillers) f->processRun(iEvent,iSetup);
}
void BaseTreeMaker::execute() { for(auto * f : fillers) f->execute();}
void BaseTreeMaker::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
	processRun(iEvent,iSetup);
	for(auto * f : fillers) f->execute();
	tree->fill();
}
}
