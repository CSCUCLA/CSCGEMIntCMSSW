#ifndef CSCGEMINT_TREEFILLERS_BASEFILLER_H
#define CSCGEMINT_TREEFILLERS_BASEFILLER_H
#include<iostream>
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"

#include "../../AnalysisSupport/interface/TreeWriterData.h"

namespace CSCGEMInt{
typedef   char                 int8  ;
typedef   short int            int16 ;
typedef   unsigned char        size8 ; // 8 bit 0->255
typedef   unsigned short int   size16; //16 bit 0->65536
typedef   unsigned int         size  ; //32 bit 0->4294967296

template<typename Target, typename Source>
Target convertTo(Source source, const char name[], bool lenient = false, bool* good = 0)
{
  Target            converted = static_cast<Target>(source);

  if (static_cast<Source>(converted) != source) {
    const Target    lowest    = !std::numeric_limits<Target>::is_signed
                              ? 0
                              : std::numeric_limits<Target>::has_infinity
                              ? -std::numeric_limits<Target>::infinity()
                              :  std::numeric_limits<Target>::min()
                              ;

    std::string problem = "convertTo(): Source value " + std::to_string((double)  source) + " outside of target range "
                         +"["+std::to_string((double)  lowest)+","+std::to_string((double)  std::numeric_limits<Target>::max())+"]"
                         + " for '" + name +"'";

    if (good)      *good      = false;
    if (lenient) {
      std::cerr << "WARNING: " << problem << std::endl;
      return  ( source > static_cast<Source>(std::numeric_limits<Target>::max())
              ? std::numeric_limits<Target>::max()
              : lowest
              );
    }
    throw std::range_error( problem);
  }

  return converted;
}

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
