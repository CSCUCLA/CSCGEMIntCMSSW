#ifndef CSCGEMINT_TREEFILLERS_BASEFILLER_H
#define CSCGEMINT_TREEFILLERS_BASEFILLER_H
#include<iostream>
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"

#include "AnalysisSupport/TreeInterface/interface/TreeWriterData.h"
#include "AnalysisSupport/Utilities/interface/Types.h"
using namespace ASTypes;

namespace CSCGEMInt{
class BaseFiller {
public:
	BaseFiller(std::string branchName) : branchName(branchName) {};
	virtual void setup(TreeWrapper * wrapper) = 0; //book tree
	virtual void processRun(const edm::Event& iEvent, const edm::EventSetup& iSetup) = 0; //process run to setup for filling
	virtual void execute() { };	   //do actual filling..for basic fillers no need to do anything

	virtual ~BaseFiller() {};
protected:
	void reset() {tdata.reset();}
  TreeWriterData tdata;
  std::string branchName = "";

};
}



#endif
