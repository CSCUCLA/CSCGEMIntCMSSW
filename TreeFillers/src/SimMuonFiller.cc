#include "../interface/SimMuonFiller.h"
#include "../interface/TMBFiller.h"
#include "AnalysisSupport/CMSSWUtilities/interface/ParticleUtilities.h"

#include "Geometry/CSCGeometry/interface/CSCGeometry.h"
#include "Geometry/CSCGeometry/interface/CSCLayerGeometry.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"

#include "SimDataFormats/TrackerDigiSimLink/interface/StripDigiSimLink.h"
#include "DataFormats/Common/interface/DetSetVector.h"

namespace CSCGEMInt {
SimMuonFiller::SimMuonFiller(const edm::ParameterSet& cfg, edm::ConsumesCollector && cc, const TMBFiller * tmbFillerPtr, bool addSimDigiLinks) : BaseFiller("simmuon") {
	if(tmbFillerPtr) options |= TMB_TRUTH;
	if(addSimDigiLinks) options |= SIMDIGI_LINK;
	t_genParticles = cc.consumes<reco::GenParticleCollection>( cfg.getParameter<edm::InputTag>("genParticleTag") );
	if(options & TMB_TRUTH){
		tmbFiller =tmbFillerPtr;
		t_trackingParticles = cc.consumes<TrackingParticleCollection>( cfg.getParameter<edm::InputTag>("trackingParticleTag") );
		t_csc_sh = cc.consumes<std::vector<PSimHit>>( cfg.getParameter<edm::InputTag>("cscSimHitTag") );
	}
	if(options & SIMDIGI_LINK){
		t_csc_stlink = cc.consumes<edm::DetSetVector<StripDigiSimLink>>( cfg.getParameter<edm::InputTag>("CSCStripDigiSimLink") );
		t_csc_wrlink = cc.consumes<edm::DetSetVector<StripDigiSimLink>>( cfg.getParameter<edm::InputTag>("CSCWireDigiSimLink") );
	}
	}
SimMuonFiller::~SimMuonFiller() {}

void SimMuonFiller::setup(TreeWrapper * wrapper){
	i_pt        = tdata.addMulti<float>(branchName,"pt"    ,0);
	i_eta       = tdata.addMulti<float>(branchName,"eta"   ,0);
	i_phi       = tdata.addMulti<float>(branchName,"phi"   ,0);
	i_charge    = tdata.addMulti<int8> (branchName,"charge",0);

	if(options & TMB_TRUTH){
		i_csc_nHits        = tdata.addMulti<size8>(branchName,"csc_nHits"   ,0);
		i_csc_tpNumber     = tdata.addMulti<size8>(branchName,"csc_tpNumber",0);
		i_csc_detID        = tdata.addMulti<size> (branchName,"csc_detID"   ,0);
		i_csc_strip        = tdata.addMulti<float>(branchName,"csc_strip"   ,0);
		i_csc_wg           = tdata.addMulti<size8>(branchName,"csc_wg"      ,0);
		i_csc_w            = tdata.addMulti<size> (branchName,"csc_w"       ,0);
	}
	if(options & SIMDIGI_LINK){
			i_csc_stLink_nHits        = tdata.addMulti<size8>(branchName,"csc_stLink_nHits"   ,0);
			i_csc_stLink_detId        = tdata.addMulti<size> (branchName,"csc_stLink_detId",0);
			i_csc_stLink_strip        = tdata.addMulti<size8>(branchName,"csc_stLink_strip",0);
			i_csc_wrLink_nHits        = tdata.addMulti<size8>(branchName,"csc_wrLink_nHits"   ,0);
			i_csc_wrLink_detId        = tdata.addMulti<size> (branchName,"csc_wrLink_detId",0);
			i_csc_wrLink_wire         = tdata.addMulti<size8>(branchName,"csc_wrLink_wire" ,0);
	}

	tdata.book(wrapper);
}
void SimMuonFiller::reset() { BaseFiller::reset();}

void SimMuonFiller::processRun(const edm::Event& iEvent, const edm::EventSetup& iSetup){
	  reset();

	  edm::Handle<reco::GenParticleCollection>  genParticles ;
	  iEvent.getByToken(t_genParticles,genParticles);

	  edm::Handle<TrackingParticleCollection>  TPCollectionH ;
	  edm::Handle<std::vector<PSimHit> >  csc_SH ;
	  if(options & TMB_TRUTH){
		  iEvent.getByToken(t_trackingParticles,TPCollectionH);
		  iEvent.getByToken(t_csc_sh,csc_SH);
		    edm::ESHandle<CSCGeometry> hGeom;
		    iSetup.get<MuonGeometryRecord>().get("idealForDigi",hGeom);
		    theCSCGeometry  = &*hGeom;
	  }

	  edm::Handle<edm::DetSetVector<StripDigiSimLink>>  strip_simLink ;
	  edm::Handle<edm::DetSetVector<StripDigiSimLink> >  wire_simLink ;
	  if(options & SIMDIGI_LINK){
		  iEvent.getByToken(t_csc_stlink,strip_simLink);
		  iEvent.getByToken(t_csc_wrlink,wire_simLink);
	  }

//	  ParticleInfo::printGenInfo(*genParticles);
	  for(unsigned int iP = 0; iP < genParticles->size(); ++iP){
		const auto& p = genParticles->at(iP);
//		std::cout <<p.pt() <<","<<p.eta() <<" : "<<p.pdgId()<<","<<p.status();
		const int pdgid = TMath::Abs(p.pdgId());
		if(ParticleInfo::p_muminus != pdgid) continue;
		if(!ParticleInfo::isFinal(p.status())) continue;
//		const auto * firstMuon = ParticleUtilities::getOriginal(&p);
//		if(! ParticleInfo::isDoc(firstMuon->status()) ) continue;
	      tdata.fillMulti(i_pt,float(p.pt()));
	      tdata.fillMulti(i_eta,float(p.eta()));
	      tdata.fillMulti(i_phi,float(p.phi()));
	      tdata.fillMulti(i_charge,int8(p.charge()));

	      std::vector<const SimTrack*> strks;
	      std::map<size,std::vector<const SimTrack*>> detIDMap;
	      if((options & TMB_TRUTH) || (options & SIMDIGI_LINK)){
	    	  strks = getSimTracks(TPCollectionH,&p);
	    	  int nTracks = 0;
	    	  int nSimHits = 0;

	    	  for(const auto* strk :strks){
	    		  for (const auto& simHit : (*csc_SH)) {
	    		    if(simHit.eventId() != strk->eventId()) continue;
	    		    if(simHit.trackId() != strk->trackId()) continue;
	    		    const CSCDetId detID = simHit.detUnitId();
	    		    if(!tmbFiller->outputTMB(detID.station(),detID.ring())) continue;
	    		    detIDMap[detID].push_back(strk);
	    		    if((options & TMB_TRUTH) && nSimHits < 255){
		    		    const CSCLayerGeometry * geom = getLayerGeometry(detID);
		    		    auto lp = simHit.localPosition();
		    		    int nearestWire=geom->nearestWire(lp);
		    		    int wiregroup = geom->wireGroup(nearestWire);
		    		    float strip = geom->strip(lp);
		    		    tdata.fillMulti(i_csc_tpNumber,convertTo<size8>(nTracks,"i_csc_tpNumber"));
		    		    tdata.fillMulti(i_csc_detID,size(detID.rawId()));
		    		    tdata.fillMulti(i_csc_strip,strip);
		    		    tdata.fillMulti(i_csc_wg,convertTo<size8>(wiregroup,"i_csc_wg"));
		    		    tdata.fillMulti(i_csc_w,size(nearestWire));
		    		    nSimHits++;
	    		    }
	    		  }
	    		  if(nSimHits) nTracks++;
	    	  }
	    	  if(options & TMB_TRUTH)  {tdata.fillMulti(i_csc_nHits,size8(nSimHits));}
	      }

	      if((options & SIMDIGI_LINK)){
	    	  int nStripHits = 0;
	    	  int nWireHits = 0;
	    	  for(std::map<size,std::vector<const SimTrack*>>::const_iterator detIDIt = detIDMap.begin(); detIDIt != detIDMap.end(); detIDIt++) {
	    		  edm::DetSetVector<StripDigiSimLink>::const_iterator st_layerLinks = strip_simLink->find(detIDIt->first);
	    		  if(st_layerLinks != strip_simLink->end()){
	    			  for (edm::DetSet<StripDigiSimLink>::const_iterator link=st_layerLinks->begin(); link!=st_layerLinks->end(); ++link) {
	    				  bool found = false;
	    				  for(unsigned int iT = 0; iT < detIDIt->second.size(); ++iT ){
	    		    		    if(link->eventId() != detIDIt->second[iT]->eventId()) continue;
	    		    		    if(link->SimTrackId() != detIDIt->second[iT]->trackId()) continue;
	    		    		    found = true;
	    		    		    break;
	    				  }
	    				  if(found && nStripHits < 255){
				    		    tdata.fillMulti(i_csc_stLink_detId,size(detIDIt->first));
				    		    tdata.fillMulti(i_csc_stLink_strip,convertTo<size8>(link->channel(),"csc_stLink_strip"));
				    		    nStripHits++;
	    				  }
	    			  }
	    		  }

	    		  edm::DetSetVector<StripDigiSimLink>::const_iterator wr_layerLinks = wire_simLink->find(detIDIt->first);
	    		  if(wr_layerLinks != wire_simLink->end()){
	    			  for (edm::DetSet<StripDigiSimLink>::const_iterator link=wr_layerLinks->begin(); link!=wr_layerLinks->end(); ++link) {
	    				  bool found = false;
	    				  for(unsigned int iT = 0; iT < detIDIt->second.size(); ++iT ){
	    		    		    if(link->eventId() != detIDIt->second[iT]->eventId()) continue;
	    		    		    if(link->SimTrackId() != detIDIt->second[iT]->trackId()) continue;
	    		    		    found = true;
	    		    		    break;
	    				  }
	    				  if(found && nWireHits < 255){
				    		    tdata.fillMulti(i_csc_wrLink_detId,size(detIDIt->first));
				    		    tdata.fillMulti(i_csc_wrLink_wire,convertTo<size8>(link->channel() - 100 ,"csc_wrLink_wire"));
				    		    nWireHits++;
	    				  }
	    			  }
	    		  }

	    	  }
	    	  tdata.fillMulti(i_csc_stLink_nHits,size8(nStripHits));
	    	  tdata.fillMulti(i_csc_wrLink_nHits,size8(nWireHits));
	      }




//	      if(options & TMB_TRUTH){
//	    	  int nTracks = 0;
//	    	  int nHits = 0;
//	    	  std::vector<const SimTrack*> strks = getSimTracks(TPCollectionH,&p);
//	    	  for(const auto* strk :strks){
//	    		  for (const auto& simHit : (*csc_SH)) {
//	    		    if(simHit.eventId() != strk->eventId()) continue;
//	    		    if(simHit.trackId() != strk->trackId()) continue;
//	    		    const CSCDetId detID = simHit.detUnitId();
//	    		    if(!tmbFiller->outputTMB(detID.station(),detID.ring())) continue;
//	    		    const CSCLayerGeometry * geom = getLayerGeometry(detID);
//	    		    auto lp = simHit.localPosition();
//	    		    int nearestWire=geom->nearestWire(lp);
//	    		    int wiregroup = geom->wireGroup(nearestWire);
//	    		    float strip = geom->strip(lp);
//	    		    tdata.fillMulti(i_csc_tpNumber,size8(nTracks));
//	    		    tdata.fillMulti(i_csc_detID,size(detID.rawId()));
//	    		    tdata.fillMulti(i_csc_strip,strip);
//	    		    tdata.fillMulti(i_csc_wg,size8(wiregroup));
//	    		    tdata.fillMulti(i_csc_w,size(nearestWire));
//	    		    nHits++;
//	    		    if(nHits > 254) break;
//	    		  }
//	    		  if(nHits) nTracks++;
//	    		  if(nHits > 254) break;
//	    	  }
//	    	  tdata.fillMulti(i_csc_nHits,size8(nHits));
//	      }
	  }
}

std::vector<const SimTrack*> SimMuonFiller::getSimTracks(edm::Handle<TrackingParticleCollection>  TPCollectionH, const reco::GenParticle * p) const  {
	std::vector<const SimTrack*> trks;
	  for (TrackingParticleCollection::size_type i=0; i<TPCollectionH->size(); i++) {
		    TrackingParticleRef trpart(TPCollectionH, i);
		    if(!trpart->genParticles().size() ||  !trpart->g4Tracks().size())continue;
		    bool isGood = false;
		    for(auto g : trpart->genParticles() )
		    	if(TMath::Abs(g->pdgId()) == ParticleInfo::p_muminus && ParticleUtilities::isAncestor(&(*g), p) ){
		    		isGood = true;
		    		break;
		    	}
		    if(!isGood) continue;
		    for(unsigned int iST = 0; iST <  trpart->g4Tracks().size(); ++iST){
		    	const auto & strack = trpart->g4Tracks()[iST];
		    	if(TMath::Abs(strack.type()) != ParticleInfo::p_muminus) continue;
		    	trks.push_back(&strack);
		    }
	  }
	return trks;
}

const CSCLayerGeometry * SimMuonFiller::getLayerGeometry(int detId) const {
  assert(theCSCGeometry != 0);
  const GeomDetUnit* detUnit = theCSCGeometry->idToDetUnit(CSCDetId(detId));
  if(detUnit == 0)
  {
    throw cms::Exception("CSCDigiProducer") << "Invalid DetUnit: " << CSCDetId(detId)
      << "\nPerhaps your signal or pileup dataset are not compatible with the current release?";
  }
  return dynamic_cast<const CSCLayer *>(detUnit)->geometry();
}


}
