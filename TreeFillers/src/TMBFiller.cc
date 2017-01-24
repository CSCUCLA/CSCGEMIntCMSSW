#include "../interface/TMBFiller.h"
namespace CSCGEMInt {
TMBFiller::TMBFiller(const edm::ParameterSet& cfg, edm::ConsumesCollector && cc) : BaseFiller("tmb") {
	t_comp = cc.consumes<CSCComparatorDigiCollection>( cfg.getParameter<edm::InputTag>("compDigiTag") );
	if(cfg.getParameter<bool>("tmb_onlyME11")){
		ring_mask = 0x9;
		station_mask = 0x1;
	}

	}
void TMBFiller::setup(TreeWrapper * wrapper){
	i_comp_detID           = tdata.addMulti<size>  (branchName,"comp_detID"      ,0);
	i_comp_hs              = tdata.addMulti<size16>(branchName,"comp_hs"         ,0);
	i_comp_timeBinWord     = tdata.addMulti<size16>(branchName,"comp_timeBinWord",0);
	tdata.book(wrapper);
}
void TMBFiller::reset() { BaseFiller::reset();}

void TMBFiller::processRun(const edm::Event& iEvent, const edm::EventSetup& iSetup){
	  reset();

	  edm::Handle<CSCComparatorDigiCollection> compDigi;
	  iEvent.getByToken(t_comp, compDigi);

	  for (CSCComparatorDigiCollection::DigiRangeIterator det=compDigi->begin(); det!=compDigi->end(); det++)
	  {
	    CSCDetId id = (*det).first;
	    if(!outputTMB(id.station(), id.ring())) continue;
	    size nFill = 0;
	    const CSCComparatorDigiCollection::Range& range =(*det).second;
	    for(CSCComparatorDigiCollection::const_iterator digiItr = range.first; digiItr != range.second; ++digiItr)
	    {
	    	nFill++;
	      tdata.fillMulti(i_comp_hs,size16(digiItr->getHalfStrip()));
	      tdata.fillMulti(i_comp_timeBinWord,size16(digiItr->getTimeBin()));
	    }
	    if(nFill){
		      tdata.fillMulti(i_comp_detID,id.rawId());
		      tdata.fillMulti(i_comp_detID,nFill);
	    }

	  }
}

bool TMBFiller::outputTMB(int station, int ring) const {
    if( ((1 << (station -1) ) & station_mask) == 0 ) return false;
    if( ((1 << (ring -1)) & ring_mask) == 0 ) return false;
    return true;
};

}
