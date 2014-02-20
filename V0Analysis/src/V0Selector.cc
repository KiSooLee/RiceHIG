// -*- C++ -*-
//
// Package:    V0Selector
// Class:      V0Selector
// 
/**\class V0Selector V0Selector.cc RiceHIG/V0Analysis/src/V0Selector.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author: Wei Li 
//
//


// system include files
#include <memory>

#include "RiceHIG/V0Analysis/interface/V0Selector.h"

// Constructor
V0Selector::V0Selector(const edm::ParameterSet& iConfig) 
{
  using std::string;

  vertexCollName_ = iConfig.getParameter<edm::InputTag>("vertexCollName");
  v0CollName_     = iConfig.getParameter<string>("v0CollName");
  v0IDName_       = iConfig.getParameter<string>("v0IDName");
  etaCutMin_      = iConfig.getParameter<double>("etaCutMin");
  etaCutMax_      = iConfig.getParameter<double>("etaCutMax");
  dxySigCut1_     = iConfig.getParameter<double>("dxySigCut1");
  dxySigCut2_     = iConfig.getParameter<double>("dxySigCut2");
  dzSigCut1_      = iConfig.getParameter<double>("dzSigCut1");
  dzSigCut2_      = iConfig.getParameter<double>("dzSigCut2");
  vtxChi2Cut_     = iConfig.getParameter<double>("vtxChi2Cut");
  cosThetaCut_    = iConfig.getParameter<double>("cosThetaCut");
  decayLSigCut_   = iConfig.getParameter<double>("decayLSigCut");

  // Trying this with Candidates instead of the simple reco::Vertex
  produces< reco::VertexCompositeCandidateCollection >(v0IDName_);

}

// (Empty) Destructor
V0Selector::~V0Selector() {

}

//
// Methods
//

// Producer Method
void V0Selector::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {

//   using std::vector;
   using namespace edm;
//   using namespace reco;
    
    // select on requirement of valid vertex
   edm::Handle<reco::VertexCollection> vertices;
   iEvent.getByLabel(vertexCollName_,vertices);
   double bestvz=-999.9, bestvx=-999.9, bestvy=-999.9;
   double bestvzError=-999.9, bestvxError=-999.9, bestvyError=-999.9;
   const reco::Vertex & vtx = (*vertices)[0];
   bestvz = vtx.z(); bestvx = vtx.x(); bestvy = vtx.y();
   bestvzError = vtx.zError(); bestvxError = vtx.xError(); bestvyError = vtx.yError();
   if(bestvz < -15.0 || bestvz>15.0) return;

   edm::Handle<reco::VertexCompositeCandidateCollection> v0candidates;
   iEvent.getByLabel(v0CollName_,v0IDName_,v0candidates);
   if(!v0candidates.isValid()) return;

   // Create auto_ptr for each collection to be stored in the Event
   std::auto_ptr< reco::VertexCompositeCandidateCollection > 
     theNewV0Cands( new reco::VertexCompositeCandidateCollection() );

   for( reco::VertexCompositeCandidateCollection::const_iterator v0cand = v0candidates->begin();
         v0cand != v0candidates->end();
         v0cand++) {

       double secvz=-999.9, secvx=-999.9, secvy=-999.9;

       const reco::Candidate * d1 = v0cand->daughter(0);
       const reco::Candidate * d2 = v0cand->daughter(1);

       reco::TrackRef dau1 = d1->get<reco::TrackRef>();
       reco::TrackRef dau2 = d2->get<reco::TrackRef>();

       //pt,mass
       double eta = v0cand->eta();
       double pt = v0cand->pt();
       double px = v0cand->px();
       double py = v0cand->py();
       double pz = v0cand->pz();
       double mass = v0cand->mass();

       if(eta > etaCutMax_ || eta < etaCutMin_) continue;

       secvz = v0cand->vz(); secvx = v0cand->vx(); secvy = v0cand->vy();

       //trkNHits
       double nhit1 = dau1->numberOfValidHits();
       double nhit2 = dau2->numberOfValidHits();

       //algo
       double algo1 = dau1->algo();
       double algo2 = dau2->algo();

       //dau eta
       double eta1 = dau1->eta();
       double eta2 = dau2->eta();

       //DCA
       math::XYZPoint bestvtx(bestvx,bestvy,bestvz);

       double dzbest1 = dau1->dz(bestvtx);
       double dxybest1 = dau1->dxy(bestvtx);
       double dzerror1 = sqrt(dau1->dzError()*dau1->dzError()+bestvzError*bestvzError);
       double dxyerror1 = sqrt(dau1->d0Error()*dau1->d0Error()+bestvxError*bestvyError);
       double dzos1 = dzbest1/dzerror1;
       double dxyos1 = dxybest1/dxyerror1;
       if(fabs(dzos1) < dzSigCut1_ || fabs(dxyos1) < dxySigCut1_) continue;

       double dzbest2 = dau2->dz(bestvtx);
       double dxybest2 = dau2->dxy(bestvtx);
       double dzerror2 = sqrt(dau2->dzError()*dau2->dzError()+bestvzError*bestvzError);
       double dxyerror2 = sqrt(dau2->d0Error()*dau2->d0Error()+bestvxError*bestvyError);
       double dzos2 = dzbest2/dzerror2;
       double dxyos2 = dxybest2/dxyerror2;
       if(fabs(dzos2) < dzSigCut2_ || fabs(dxyos2) < dxySigCut2_) continue;

       //vtxChi2
       double vtxChi2 = v0cand->vertexChi2();
       if(vtxChi2 > vtxChi2Cut_ ) continue;

       //PAngle
       TVector3 ptosvec(secvx-bestvx,secvy-bestvy,secvz-bestvz);
       TVector3 secvec(px,py,pz);
       double agl = cos(secvec.Angle(ptosvec));
       if(agl < cosThetaCut_) continue;

       //Decay length
       typedef ROOT::Math::SMatrix<double, 3, 3, ROOT::Math::MatRepSym<double, 3> > SMatrixSym3D;
       typedef ROOT::Math::SVector<double, 3> SVector3;
       SMatrixSym3D totalCov = vtx.covariance() + v0cand->vertexCovariance();
       SVector3 distanceVector(secvx-bestvx,secvy-bestvy,secvz-bestvz);
       double dl = ROOT::Math::Mag(distanceVector);
       double dlerror = sqrt(ROOT::Math::Similarity(totalCov, distanceVector))/dl;
       double dlos = dl/dlerror;
       if(dlos < decayLSigCut_) continue;

       theNewV0Cands->push_back( *v0cand );
   }

   // Write the collections to the Event
   iEvent.put( theNewV0Cands, std::string(v0IDName_) );
}


void V0Selector::beginJob() {
}


void V0Selector::endJob() {
}
