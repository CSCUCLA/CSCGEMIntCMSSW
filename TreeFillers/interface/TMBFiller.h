#ifndef CSCGEMINT_TREEFILLERS_TMBFILLER_H
#define CSCGEMINT_TREEFILLERS_TMBFILLER_H

#include <DataFormats/CSCDigi/interface/CSCComparatorDigiCollection.h>
#include "BaseFiller.h"

namespace CSCGEMInt{
class TMBFiller : public BaseFiller {
public:
	TMBFiller(const edm::ParameterSet& cfg, edm::ConsumesCollector && cc);
	virtual ~TMBFiller() {};
	virtual void setup(TreeWrapper * wrapper);
	virtual void reset();
	virtual void processRun(const edm::Event& iEvent, const edm::EventSetup& iSetup);

	bool outputTMB(int station, int ring) const;

private:
edm::EDGetTokenT<CSCComparatorDigiCollection> t_comp;

int station_mask          = 0xF;
int ring_mask             = 0xF;

size i_comp_detID         = 0;
size i_comp_hs            = 0;
size i_comp_timeBinWord   = 0;
};

}

#endif
