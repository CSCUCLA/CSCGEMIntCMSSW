#include "../interface/SimMuonFiller.h"
#include "../interface/TMBFiller.h"
#include "../../AnalysisSupport/interface/ParticleUtilities.h"

#include "Geometry/CSCGeometry/interface/CSCGeometry.h"
#include "Geometry/CSCGeometry/interface/CSCLayerGeometry.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"

namespace CSCGEMInt {
SimMuonFiller::SimMuonFiller(const edm::ParameterSet& cfg, edm::ConsumesCollector && cc, const TMBFiller * tmbFillerPtr) : BaseFiller("simmuon") {
	if(tmbFillerPtr) options |= TMB_TRUTH;
	t_genParticles = cc.consumes<reco::GenParticleCollection>( cfg.getParameter<edm::InputTag>("genParticleTag") );
	if(options & TMB_TRUTH){
		tmbFiller =tmbFillerPtr;
		t_trackingParticles = cc.consumes<TrackingParticleCollection>( cfg.getParameter<edm::InputTag>("trackingParticleTag") );
		t_csc_sh = cc.consumes<std::vector<PSimHit>>( cfg.getParameter<edm::InputTag>("cscSimHitTag") );
	}
	}
SimMuonFiller::~SimMuonFiller() {}

void SimMuonFiller::setup(TreeWrapper * wrapper){
	i_pt        = tdata.addMulti<float>(branchName,"pt",0);
	i_eta       = tdata.addMulti<float>(branchName,"eta",0);
	i_phi       = tdata.addMulti<float>(branchName,"phi",0);
	i_charge    = tdata.addMulti<int8>(branchName,"charge",0);

	if(options & TMB_TRUTH){
		i_csc_nHits        = tdata.addMulti<size8>(branchName,"csc_nHits",0);
		i_csc_tpNumber     = tdata.addMulti<size8>(branchName,"csc_tpNumber",0);
		i_csc_detID        = tdata.addMulti<size>(branchName ,"csc_detID",0);
		i_csc_strip        = tdata.addMulti<float>(branchName,"csc_strip",0);
		i_csc_wg           = tdata.addMulti<size8>(branchName,"csc_wg",0);
		i_csc_w            = tdata.addMulti<size>(branchName,"csc_w",0);
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


	      if(options & TMB_TRUTH){
	    	  int nTracks = 0;
	    	  int nHits = 0;
	    	  std::vector<const SimTrack*> strks = getSimTracks(TPCollectionH,&p);
	    	  for(const auto* strk :strks){
	    		  for (const auto& simHit : (*csc_SH)) {
	    		    if(simHit.eventId() != strk->eventId()) continue;
	    		    if(simHit.trackId() != strk->trackId()) continue;
	    		    const CSCDetId detID = simHit.detUnitId();
	    		    if(!tmbFiller->outputTMB(detID.station(),detID.ring())) continue;
	    		    const CSCLayerGeometry * geom = getLayerGeometry(detID);
	    		    auto lp = simHit.localPosition();
	    		    int nearestWire=geom->nearestWire(lp);
	    		    int wiregroup = geom->wireGroup(nearestWire);
	    		    float strip = geom->strip(lp);
	    		    tdata.fillMulti(i_csc_tpNumber,size8(nTracks));
	    		    tdata.fillMulti(i_csc_detID,size(detID.rawId()));
	    		    tdata.fillMulti(i_csc_strip,strip);
	    		    tdata.fillMulti(i_csc_wg,size8(wiregroup));
	    		    tdata.fillMulti(i_csc_w,size(nearestWire));
	    		    nHits++;
	    		    if(nHits > 254) break;
	    		  }
	    		  if(nHits) nTracks++;
	    		  if(nHits > 254) break;
	    	  }
	    	  if(nHits) tdata.fillMulti(i_csc_nHits,size8(nHits));
	      }
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
