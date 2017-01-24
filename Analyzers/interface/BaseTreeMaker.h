#ifndef CSCGEMINT_ANALYZERS_BASETREEMAKER_H
#define CSCGEMINT_ANALYZERS_BASETREEMAKER_H

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

class TreeWrapper;
namespace CSCGEMInt {
class BaseFiller;
}
namespace ROOT{
	class TFile;
}

namespace CSCGEMInt {

class BaseTreeMaker : public edm::EDAnalyzer {
    public:
        explicit BaseTreeMaker(const edm::ParameterSet&);
        virtual ~BaseTreeMaker();


    protected:
        virtual void beginJob() {};

        virtual void addFiller(BaseFiller * filler);

        virtual void setup();
        virtual void processRun(const edm::Event&, const edm::EventSetup&);
        virtual void execute();

        virtual void analyze(const edm::Event&, const edm::EventSetup&);
        virtual void endJob() {};

        TFile * file;
        TreeWrapper * tree;
        std::vector<BaseFiller*> fillers;
};

}




#endif



