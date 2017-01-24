#ifndef CSCGEMINT_TREEFILLERS_SimMuonFiller_H
#define CSCGEMINT_TREEFILLERS_SimMuonFiller_H

#include "DataFormats/Common/interface/DetSetVector.h"
#include "SimDataFormats/TrackerDigiSimLink/interface/StripDigiSimLink.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"
#include "BaseFiller.h"

class CSCLayerGeometry;
class CSCGeometry;


namespace CSCGEMInt{

class TMBFiller;

class SimMuonFiller : public BaseFiller {
	enum Options {NONE = 0, TMB_TRUTH = (1 << 0), SIMDIGI_LINK = (1 << 1) };


public:
	SimMuonFiller(const edm::ParameterSet& cfg, edm::ConsumesCollector && cc, const TMBFiller * tmbFillerPtr = 0, bool addSimDigiLinks = false);
	virtual ~SimMuonFiller();
	virtual void setup(TreeWrapper * wrapper);
	virtual void reset();
	virtual void processRun(const edm::Event& iEvent, const edm::EventSetup& iSetup);

private:
	std::vector<const SimTrack*> getSimTracks(edm::Handle<TrackingParticleCollection>  TPCollectionH, const reco::GenParticle * p) const;
	const CSCLayerGeometry * getLayerGeometry(int detId) const;
int options = NONE;

// std
edm::EDGetTokenT<reco::GenParticleCollection> t_genParticles;
size i_pt       = 0;
size i_eta      = 0;
size i_phi      = 0;
size i_charge   = 0;

//TMB Truth
const TMBFiller * tmbFiller = 0;
const CSCGeometry      * theCSCGeometry = 0;
edm::EDGetTokenT<TrackingParticleCollection> t_trackingParticles;
edm::EDGetTokenT<std::vector<PSimHit>> t_csc_sh;

edm::EDGetTokenT<edm::DetSetVector<StripDigiSimLink>> t_csc_stlink;
edm::EDGetTokenT<edm::DetSetVector<StripDigiSimLink>> t_csc_wrlink;

size i_csc_nHits         = 0;
size i_csc_tpNumber      = 0;
size i_csc_detID         = 0;
size i_csc_strip         = 0;
size i_csc_wg            = 0;
size i_csc_w             = 0;
size i_csc_stLink_nHits  =0;
size i_csc_stLink_detId  =0;
size i_csc_stLink_strip  =0;
size i_csc_wrLink_nHits  =0;
size i_csc_wrLink_detId  =0;
size i_csc_wrLink_wire   =0;






};





}

#endif
