/**\class Candidates Candidates.cc Analysis/Ntuplizer/src/Candidates.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Roberval Walsh Bastos Rangel
//         Created:  Mon, 20 Oct 2014 14:24:08 GMT
//
//

// system include files
// 
#include <regex>
#include <boost/core/demangle.hpp>

// user include files
#include "FWCore/Framework/interface/Event.h"
// 
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"

#include "DataFormats/L1Trigger/interface/L1JetParticle.h"
#include "DataFormats/L1Trigger/interface/L1JetParticleFwd.h"

#include "DataFormats/L1Trigger/interface/L1MuonParticle.h"
#include "DataFormats/L1Trigger/interface/L1MuonParticleFwd.h"

#include "DataFormats/L1Trigger/interface/Jet.h"
#include "DataFormats/L1Trigger/interface/Muon.h"

#include "DataFormats/RecoCandidate/interface/RecoChargedCandidate.h"
#include "DataFormats/RecoCandidate/interface/RecoChargedCandidateFwd.h"

#include "DataFormats/JetReco/interface/CaloJet.h"
#include "DataFormats/JetReco/interface/CaloJetCollection.h"

#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"

#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/MET.h"

#include "DataFormats/JetReco/interface/GenJet.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"

#include "CommonTools/Utils/interface/PtComparator.h"

#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerTypeDefs.h"
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"

#include "Analysis/Ntuplizer/interface/Candidates.h"

#include "FWCore/Framework/interface/ESHandle.h"

#include "DataFormats/Common/interface/TriggerResults.h"


#include "TTree.h"


//
// class declaration
//

using namespace analysis;
using namespace analysis::ntuple;

// member functions specialization - needed to be declared in the same namespace as the class
namespace analysis {
   namespace ntuple {
      template <> void Candidates<pat::TriggerObject>::ReadFromEvent(const edm::Event& event);
      template <> void Candidates<pat::TriggerObject>::TriggerObjectType(const std::string& trigobj_type);
      template <> void Candidates<pat::Jet>::JECRecord(const std::string & jr);
      template <> void Candidates<trigger::TriggerObject>::ReadFromEvent(const edm::Event& event);
      template <> void Candidates<l1t::Jet>::ReadFromEvent(const edm::Event& event);
      template <> void Candidates<l1t::Muon>::ReadFromEvent(const edm::Event& event);
      template <> void Candidates<trigger::TriggerObject>::Kinematics();
 }
}   
//
// constructors and destructor
//
template <typename T>
Candidates<T>::Candidates()
{
   // default constructor
}

template <typename T>
Candidates<T>::Candidates(const edm::InputTag& tag, TTree* tree, const bool & mc, float minPt, float maxEta ) :
      minPt_(minPt), maxEta_(maxEta)
{
   input_collection_ = tag;
   tree_ = tree;
   
   is_mc_              = mc;
   is_l1jet_           = std::is_same<T,l1extra::L1JetParticle>::value;
   is_l1muon_          = std::is_same<T,l1extra::L1MuonParticle>::value;
   is_calojet_         = std::is_same<T,reco::CaloJet>::value;
   is_pfjet_           = std::is_same<T,reco::PFJet>::value;
   is_patjet_          = std::is_same<T,pat::Jet>::value;
   is_patmuon_         = std::is_same<T,pat::Muon>::value;
   is_patmet_          = std::is_same<T,pat::MET>::value;
   is_genjet_          = std::is_same<T,reco::GenJet>::value;
   is_genparticle_     = std::is_same<T,reco::GenParticle>::value;
   is_trigobject_      = std::is_same<T,pat::TriggerObject>::value;
   is_trigobject_reco_ = std::is_same<T,trigger::TriggerObject>::value;
   is_l1tjet_          = std::is_same<T,l1t::Jet>::value;
   is_l1tmuon_         = std::is_same<T,l1t::Muon>::value;
   is_chargedcand_     = std::is_same<T,reco::RecoChargedCandidate>::value;
   
//   do_kinematics_ = ( is_l1jet_ || is_l1muon_ || is_calojet_ || is_pfjet_ || is_patjet_ || is_patmuon_ || is_genjet_ || is_genparticle_ );
   do_kinematics_ = true;
   do_generator_  = ( is_mc_ && is_genparticle_ );
   
   higgs_pdg_ = 36;
   
   
   std::string title = boost::core::demangle(typeid(T).name()) + " | " + tree_->GetTitle();
   
   tree_->SetTitle(title.c_str());
   
   // definitions
   // jetid
   id_vars_.clear();
   id_vars_.push_back({"neutralHadronEnergyFraction", "id_nHadFrac"});
   id_vars_.push_back({"neutralEmEnergyFraction",     "id_nEmFrac" });
   id_vars_.push_back({"neutralMultiplicity",         "id_nMult"   });
   id_vars_.push_back({"chargedHadronEnergyFraction", "id_cHadFrac"});
   id_vars_.push_back({"chargedEmEnergyFraction",     "id_cEmFrac" });
   id_vars_.push_back({"chargedMultiplicity",         "id_cMult"   });
   id_vars_.push_back({"muonEnergyFraction",          "id_muonFrac"});
   
   // init
   btag_vars_.clear();
   
   // JEC info default
   jecRecord_ = "";
   jecFile_   = "";
   
   // trigger object split
   trigobj_type_ = "";
    
}

template <typename T>
Candidates<T>::~Candidates()
{
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)
}


//
// member functions
//

// ------------ method called for each event  ------------
template <typename T>
void Candidates<T>::ReadFromEvent(const edm::Event& event)
{
   using namespace edm;

   // Candidates
   candidates_.clear();
   edm::Handle<std::vector<T> > handler;
   event.getByLabel(input_collection_, handler);
   candidates_ = *(handler.product());
}
// Specialization for l1t jets
template <>
void Candidates<l1t::Jet>::ReadFromEvent(const edm::Event& event)
{
   using namespace edm;

   // Candidates
   candidates_.clear();
   edm::Handle<l1t::JetBxCollection> handler;
   event.getByLabel(input_collection_, handler);
   BXVector<l1t::Jet> l1jets = *(handler.product());
   for ( auto l1jet : l1jets )
   {
      candidates_.push_back(l1jet);
   }
   
}
// Specialization for l1t muons
template <>
void Candidates<l1t::Muon>::ReadFromEvent(const edm::Event& event)
{
   using namespace edm;

   // Candidates
   candidates_.clear();
   edm::Handle<l1t::MuonBxCollection> handler;
   event.getByLabel(input_collection_, handler);
   BXVector<l1t::Muon> l1muons = *(handler.product());
   for ( auto l1muon : l1muons )
   {
      candidates_.push_back(l1muon);
   }
}

// Specialization for trigger objects (pat)
template <>
void Candidates<pat::TriggerObject>::ReadFromEvent(const edm::Event& event)
{
   using namespace edm;
   
   Handle<TriggerResults> trhandler;
   event.getByLabel(triggerresults_collection_, trhandler);
   const TriggerResults & trgres = *(trhandler.product());
   
   candidates_.clear();
   // The stand alone collection
   edm::Handle<pat::TriggerObjectStandAloneCollection> handler;
   event.getByLabel(input_collection_, handler);
   
   const std::string treename = tree_ -> GetName(); // using the label to name the tree
   const std::string delimiter = "_";
   std::string label = treename.substr(0, treename.find(delimiter));

   for ( auto ito : *handler )
   {
      ito.unpackFilterLabels(event,trgres);
      if ( ito.filter(label) )
      {
         if ( trigobj_type_ != "" )
         {
            int type = ito.triggerObject().triggerObjectTypes().at(0);
            if ( trigobj_type_ == "l1muon" )
            {
               if ( type == trigger::TriggerL1Mu )               candidates_.push_back(ito.triggerObject());
            }
            else if ( trigobj_type_ == "l1jet" )
            {
               if ( type == trigger::TriggerL1Jet  )             candidates_.push_back(ito.triggerObject());
            }
            else if ( trigobj_type_ == "hltmuon" )
            {
               if ( type == trigger::TriggerMuon )               candidates_.push_back(ito.triggerObject());
            }
            else if ( trigobj_type_ == "hltjet" )
            {
               if ( type == trigger::TriggerJet )                candidates_.push_back(ito.triggerObject());
            }
            else if ( trigobj_type_ == "hltbjet" )
            {
               if ( type == trigger::TriggerBJet )               candidates_.push_back(ito.triggerObject());
            }
         }
         else
         {
            candidates_.push_back(ito.triggerObject());
         }
      }
   }
   
   // Sort the objects by pt
   NumericSafeGreaterByPt<pat::TriggerObject> triggerObjectGreaterByPt;
   std::sort (candidates_.begin(), candidates_.end(),triggerObjectGreaterByPt);
}
template <>
void Candidates<pat::TriggerObject>::TriggerObjectType(const std::string& trigobj_type)
{
   trigobj_type_ = trigobj_type;
}

// Specialization for trigger objects (trigger - reco)
template <>
void Candidates<trigger::TriggerObject>::ReadFromEvent(const edm::Event& event)
{
   using namespace edm;
   
   candidates_.clear();
   // The stand alone collection
   
   edm::Handle<trigger::TriggerEvent> handler;
   event.getByLabel(input_collection_, handler);
   
   const std::string treename = tree_ -> GetName(); // using the label to name the tree
   const std::string delimiter = "_";
   const std::string processName(handler->usedProcessName());
   std::string label = treename.substr(0, treename.find(delimiter));
   const unsigned int filterIndex(handler->filterIndex(InputTag(label,"",processName)));
   if ( filterIndex < handler->sizeFilters() )
   {
      const trigger::Keys& keys(handler->filterKeys(filterIndex));
      const trigger::TriggerObjectCollection & triggerObjects = handler->getObjects();
      for ( auto & key : keys )
      {
         candidates_.reserve(candidates_.size()+keys.size()); 
         candidates_.push_back(triggerObjects[key]);
      }
   }
   
   // Sort the objects by pt
   NumericSafeGreaterByPt<trigger::TriggerObject> triggerObjectGreaterByPt;
   std::sort (candidates_.begin(), candidates_.end(),triggerObjectGreaterByPt);
}

template <typename T>
void Candidates<T>::Kinematics()
{
   using namespace edm;
   
   int n = 0;
   for ( size_t i = 0 ; i < candidates_.size(); ++i )
   {
      if ( n >= maxCandidates ) break;
      
      if ( minPt_  >= 0. && candidates_[i].pt()  < minPt_  ) continue;
      if ( maxEta_ >= 0. && fabs(candidates_[i].eta()) > maxEta_ ) continue;

      if ( is_genparticle_ )
      {
         reco::GenParticle * gp = dynamic_cast<reco::GenParticle*> (&candidates_[i]);
         int pdg    = gp -> pdgId();
         int status = gp -> status();  // any status selection?
         if ( abs(pdg) > 38 ) continue;
         pdg_[n]   = pdg;
         status_[n]= status;
         lastcopy_[n] = gp -> isLastCopy();
         const reco::Candidate * mother = gp->mother(0);
         higgs_dau_[n] = false;
         if ( mother != NULL )  // initial protons are orphans
         {
            if ( mother->pdgId() == higgs_pdg_ )
               higgs_dau_[n] = true;
         }
      }
            
      pt_[n]  = candidates_[i].pt();
      eta_[n] = candidates_[i].eta();
      phi_[n] = candidates_[i].phi();
      px_[n]  = candidates_[i].px();
      py_[n]  = candidates_[i].py();
      pz_[n]  = candidates_[i].pz();
      e_[n]   = candidates_[i].energy();
      et_[n]  = candidates_[i].et();
      q_[n]   = candidates_[i].charge();
      
      // PAT MUONS
      if ( is_patmuon_ )
      {
         pat::Muon * muon = dynamic_cast<pat::Muon*> (&candidates_[i]);
         
         isPFMuon_       [n] = muon->isPFMuon()     ;
         isGlobalMuon_   [n] = muon->isGlobalMuon() ;
         isTrackerMuon_  [n] = muon->isTrackerMuon();
         
         isLooseMuon_    [n] = muon->isLooseMuon() ; 
         isMediumMuon_   [n] = muon->isMediumMuon(); 
          
         // default values
         validFraction_          [n] = -1.;
         segmentCompatibility_   [n] = -1.;
         trkKink_                [n] = 9999.;
         normChi2_               [n] = 9999.;
         chi2LocalPos_           [n] = 9999.;
         
         if ( isPFMuon_[n] && ( isGlobalMuon_[n] || isTrackerMuon_[n] ) )
         {
        	  //inner tracker
           validFraction_          [n] = muon->innerTrack()->validFraction();  
           segmentCompatibility_   [n] = muon->segmentCompatibility()       ; 
            
           //global tracker - only for GlobalMuons
           if ( isGlobalMuon_[n] ) 
           {
              normChi2_    [n] = muon->normChi2();                         
              trkKink_     [n] = muon->combinedQuality().trkKink;          
              chi2LocalPos_[n] = muon->combinedQuality().chi2LocalPosition;
           }
        }
     }

      // PAT JETS
      if ( is_patjet_ )
      {
         pat::Jet * jet = dynamic_cast<pat::Jet*> (&candidates_[i]);
         
//         std::string sv = "pfSecondaryVertexTagInfos";
//         std::cout << "oioi   " << sv << std::endl;
//         const reco::SecondaryVertexTagInfo * svTI = jet->tagInfoSecondaryVertex("secondaryVertex");
//         std::cout << "oioi   " << svTI << std::endl;

//          //          
//          std::vector<std::string> tagNames = jet -> userIntNames();
//          std::cout << "Jet has " << tagNames.size() << " tags" << std::endl;
//          for ( size_t it = 0 ; it < tagNames.size() ; ++it ) 
//          {
//             std::cout << "    Tag Name = " << tagNames[it] << std::endl;
//          }
         
         
         for ( size_t it = 0 ; it < btag_vars_.size() ; ++it )
         {
            btag_[it][n] = jet->bDiscriminator(btag_vars_[it].title);
         }
         
         if ( jet -> isPFJet() || jet -> isJPTJet() ) 
         {
            jetid_[0][n] = jet->neutralHadronEnergyFraction();
            jetid_[1][n] = jet->neutralEmEnergyFraction();
            jetid_[2][n] = (float)jet->neutralMultiplicity();
            jetid_[3][n] = jet->chargedHadronEnergyFraction();
            jetid_[4][n] = jet->chargedEmEnergyFraction();
            jetid_[5][n] = (float)jet->chargedMultiplicity();
            jetid_[6][n] = jet->muonEnergyFraction();
         }
         else  // set some dummy values
         {
            for ( size_t ii = 0; ii < 7; ++ii )  jetid_[ii][n] = -1.;
         }
         flavour_        [n] = 0;
         hadronFlavour_  [n] = 0;
         partonFlavour_  [n] = 0;
         physicsFlavour_ [n] = 0;
         if ( is_mc_ )
         {
            flavour_       [n]  = jet->hadronFlavour();
            hadronFlavour_ [n]  = jet->hadronFlavour();
            partonFlavour_ [n]  = jet->partonFlavour();
            if (jet->genParton())
            physicsFlavour_[n] = jet->genParton()->pdgId();
         }
         
         // JEC Uncertainties
         if ( jecRecord_ != "" )
         {
            jecUnc_->setJetEta(eta_[n]);
            jecUnc_->setJetPt(pt_[n]);
            jecUncert_[n] = jecUnc_->getUncertainty(true);
         }
         else
         {
            jecUncert_[n] = -1.;
         }
         //JER
         if( jerRecord_ != "" )
         {
          // SetUp Jet parameters
            JME::JetParameters jerParamRes;
            jerParamRes.setJetPt(pt_[n]);
            jerParamRes.setJetEta(eta_[n]);
            jerParamRes.setRho(rho_);
            
            // Return JER
            jerResolution_[n]    = res_.getResolution(jerParamRes);

            JME::JetParameters jerParamSF;
            jerParamSF.set(JME::Binning::JetEta, eta_[n]);
            jerParamSF.set(JME::Binning::Rho, rho_);

            jerSF_[n]       = res_sf_.getScaleFactor(jerParamSF);
            jerSFUp_[n]     = res_sf_. getScaleFactor(jerParamSF,Variation::UP);
            jerSFDown_[n]   = res_sf_. getScaleFactor(jerParamSF,Variation::DOWN);
            
         }
         else
         {
            jerResolution_[n] = -1;
            jerSF_[n]         = -1;
            jerSFUp_[n]       = -1;
            jerSFDown_[n]     = -1;
         } 
         
         // user floats
         bool hasQG = false;
         for ( auto & it : jet -> userFloatNames() )
         { 
            if ( it == "QGTagger:qgLikelihood" ) hasQG = true;
         }
         
         // quark-gluon likelihood
         qgLikelihood_[n] = -1.;
         if ( hasQG ) qgLikelihood_[n] = jet->userFloat("QGTagger:qgLikelihood");
         
         
      }
      if ( is_pfjet_ )
      {
         reco::PFJet * jet = dynamic_cast<reco::PFJet*> (&candidates_[i]);
         jetid_[0][n] = jet->neutralHadronEnergyFraction();
         jetid_[1][n] = jet->neutralEmEnergyFraction();
         jetid_[2][n] = (float)jet->neutralMultiplicity();
         jetid_[3][n] = jet->chargedHadronEnergyFraction();
         jetid_[4][n] = jet->chargedEmEnergyFraction();
         jetid_[5][n] = (float)jet->chargedMultiplicity();
         jetid_[6][n] = jet->muonEnergyFraction();
      }
      if ( is_patmet_ )
      {
         pat::MET * met = dynamic_cast<pat::MET*> (&candidates_[i]);
         sigxx_[n] = met->getSignificanceMatrix()(0,0);
         sigxy_[n] = met->getSignificanceMatrix()(0,1);
         sigyx_[n] = met->getSignificanceMatrix()(1,0);
         sigyy_[n] = met->getSignificanceMatrix()(1,1);
         if ( is_mc_ )
         {
            const reco::GenMET * genMET = met->genMET();
            gen_px_[n] = genMET->px();;
            gen_py_[n] = genMET->py();;
            gen_pz_[n] = genMET->pz();;
         }
      }
      
      if ( is_trigobject_ )
      {
         pat::TriggerObject * to = dynamic_cast<pat::TriggerObject*> (&candidates_[i]);
         type_[n] = 0;
         if ( to->triggerObjectTypes().size() > 0 )
            type_[n] = to->triggerObjectTypes().at(0);
      }
      if ( is_trigobject_reco_ )
      {
         trigger::TriggerObject * to = dynamic_cast<trigger::TriggerObject*> (&candidates_[i]);
         type_[n] = to -> id();
      }
      
      ++n;
   }
   
   n_ = n;

}

template <>
void Candidates<trigger::TriggerObject>::Kinematics()
{
   using namespace edm;

   int n = 0;
   for ( size_t i = 0 ; i < candidates_.size(); ++i )
   {
      if ( n >= maxCandidates ) break;
      
      if ( minPt_  >= 0. && candidates_[i].pt()  < minPt_  ) continue;
      if ( maxEta_ >= 0. && fabs(candidates_[i].eta()) > maxEta_ ) continue;

      pt_[n]  = candidates_[i].pt();
      eta_[n] = candidates_[i].eta();
      phi_[n] = candidates_[i].phi();
      px_[n]  = candidates_[i].px();
      py_[n]  = candidates_[i].py();
      pz_[n]  = candidates_[i].pz();
      e_[n]   = candidates_[i].energy();
      et_[n]  = candidates_[i].et();
      q_[n]   = 0;
      type_[n] = candidates_[i].id();
      
      ++n;
   }
   n_ = n;

}


template <typename T>
void Candidates<T>::JECRecord(const std::string& jr)
{
   jecRecord_ = "";
}

template <>
void Candidates<pat::Jet>::JECRecord(const std::string& jr)
{
   jecRecord_ = jr;
}


template <typename T>
void Candidates<T>::MinPt(const float& minPt)
{
   minPt_ = minPt;
}

template <typename T>
void Candidates<T>::MaxEta(const float& maxEta)
{
   maxEta_ = maxEta;
}


template <typename T>
void Candidates<T>::Fill()
{
   tree_->Fill();
}

template <typename T>
void Candidates<T>::Fill(const edm::Event& event)
{
   ReadFromEvent(event);
   if ( do_kinematics_ ) Kinematics();
   Fill();
}

template <typename T>
void Candidates<T>::Fill(const edm::Event& event, const edm::EventSetup& setup)
{
   if ( jecRecord_ != "" )
   {
      if ( jecFile_ != "" )
      {
         jecUnc_ = std::unique_ptr<JetCorrectionUncertainty>(new JetCorrectionUncertainty(jecFile_));
      }
      else
      {
         edm::ESHandle<JetCorrectorParametersCollection> JetCorParColl;
         setup.get<JetCorrectionsRecord>().get(jecRecord_,JetCorParColl); 
         JetCorrectorParameters const & JetCorPar = (*JetCorParColl)["Uncertainty"];
         jecUnc_ = std::unique_ptr<JetCorrectionUncertainty>(new JetCorrectionUncertainty(JetCorPar));
      }
   }

   if (jerRecord_ != "" )
   {
      if(jerFile_ != "" && jersfFile_ != "")
      {
         res_    = JME::JetResolution(jerFile_);
         res_sf_ = JME::JetResolutionScaleFactor(jersfFile_);
      }
      else
      {
         std::string label_pt = jerRecord_ + "_pt";
         res_    = JME::JetResolution::get(setup,label_pt);
         std::string label_sf = jerRecord_;
         res_sf_    = JME::JetResolutionScaleFactor::get(setup,label_sf);
      }

      edm::Handle<double> rhoHandler;
      event.getByLabel(rho_collection_, rhoHandler);
      rho_ = *(rhoHandler.product());
            
   }
      
   Fill(event);
}

// ------------ method called once each job just before starting event loop  ------------

template <typename T>
void Candidates<T>::Branches()
{
   // kinematics output info
   if ( do_kinematics_ )
   {
      tree_->Branch("n",   &n_,  "n/I");
      tree_->Branch("pt",  pt_,  "pt[n]/F");
      tree_->Branch("eta", eta_, "eta[n]/F");
      tree_->Branch("phi", phi_, "phi[n]/F");
      tree_->Branch("px",  px_,  "px[n]/F");
      tree_->Branch("py",  py_,  "py[n]/F");
      tree_->Branch("pz",  pz_,  "pz[n]/F");
      tree_->Branch("e",   e_,   "e[n]/F");
      tree_->Branch("et",  et_,  "et[n]/F");
      tree_->Branch("q",   q_,   "q[n]/I");
      
      if ( is_genparticle_ )
      {
         tree_->Branch("pdg",   pdg_,   "pdg[n]/I");
         tree_->Branch("status",status_,"status[n]/I");
         tree_->Branch("last_copy",lastcopy_,"last_copy[n]/O");
         tree_->Branch("higgs_dau",higgs_dau_,"higgs_dau[n]/O");
      }

      if ( is_patmuon_ )
        {
          tree_->Branch("isPFMuon",     isPFMuon_,     "isPFMuon[n]/O");
          tree_->Branch("isGlobalMuon", isGlobalMuon_, "isGlobalMuon[n]/O");
          tree_->Branch("isTrackerMuon",isTrackerMuon_,"isTrackerMuon[n]/O");
          tree_->Branch("isLooseMuon",  isLooseMuon_,  "isLooseMuon[n]/O");
          tree_->Branch("isMediumMuon", isMediumMuon_, "isMediumMuon[n]/O");

          
          tree_->Branch("validFraction",          validFraction_,          "validFraction[n]/F");
          tree_->Branch("segmentCompatibility",   segmentCompatibility_,   "segmentCompatibility[n]/F");

          tree_->Branch("normChi2",     normChi2_,     "normChi2[n]/F");
          tree_->Branch("trkKink",      trkKink_,      "trkKink[n]/F");
          tree_->Branch("chi2LocalPos", chi2LocalPos_, "chi2LocalPos[n]/F");

        }

      if ( is_patjet_ )
      {
         for ( size_t it = 0 ; it < btag_vars_.size() ; ++it )
         {
            std::string title = btag_vars_[it].title;
            if (title.find(":") != std::string::npos)
            {
               title = std::regex_replace(title, std::regex("\\:"), "_");
            }
            tree_->Branch(btag_vars_[it].alias.c_str(), btag_[it], (title+"[n]/F").c_str());
//            tree_->Branch(btag_vars_[it].alias.c_str(), btag_[it], (btag_vars_[it].title+"[n]/F").c_str());
//            std::cout << btag_vars_[it].alias.c_str() << "   " << title.c_str() << std::endl;
         }
            
         tree_->Branch("flavour",        flavour_,         "flavour[n]/I");
         tree_->Branch("hadronFlavour",  hadronFlavour_,   "hadronFlavour[n]/I" );
         tree_->Branch("partonFlavour",  partonFlavour_,   "partonFlavour[n]/I" );
         tree_->Branch("physicsFlavour", physicsFlavour_,  "physicsFlavour[n]/I");
         
//         if ( jecRecord_ != "" )
//         {
             tree_->Branch("jecUncert", jecUncert_, "jecUncert[n]/F");
             tree_->Branch("jerResolution",jerResolution_,"jerResolution[n]/F");
             tree_->Branch("jerSF",jerSF_,"jerSF[n]/F");
             tree_->Branch("jerSFUp",jerSFUp_,"jerSFUp[n]/F");
             tree_->Branch("jerSFDown",jerSFDown_,"jerSFDown[n]/F");
             tree_->Branch("Rho",&rho_,"Rho/D");
//         }
          tree_->Branch("qgLikelihood", qgLikelihood_, "qgLikelihood[n]/F");
         
      }
      if ( is_pfjet_ || is_patjet_ )
      {
         for ( size_t it = 0 ; it < id_vars_.size() ; ++it )
            tree_->Branch(id_vars_[it].alias.c_str(), jetid_[it], (id_vars_[it].title+"[n]/F").c_str());
      }
      if ( is_patmet_ )
      {
         tree_->Branch("sigxx",  sigxx_,  "sigxx[n]/F");
         tree_->Branch("sigxy",  sigxy_,  "sigxy[n]/F");
         tree_->Branch("sigyx",  sigyx_,  "sigyx[n]/F");
         tree_->Branch("sigyy",  sigyy_,  "sigyy[n]/F");
         if ( is_mc_ )
         {
            tree_->Branch("gen_px",  gen_px_,  "gen_px[n]/F");
            tree_->Branch("gen_py",  gen_py_,  "gen_py[n]/F");
            tree_->Branch("gen_pz",  gen_pz_,  "gen_pz[n]/F");
        }
      }
      
      if ( is_trigobject_ || is_trigobject_reco_ )
      {
         // there may be more than one type for an object, one has to be careful depending on the trigger
         // for now only the first entry is used.
         // definitions in DataFormats/HLTReco/interface/TriggerTypeDefs.h
         tree_->Branch("type", type_, "type[n]/I");
      }
      
      
   }
      
   
}

// Initialisation

template <typename T>
void Candidates<T>::Init()
{
   Branches();
}

template <typename T>
void Candidates<T>::Init( const std::vector<TitleAlias> & btagVars )
{
   btag_vars_ = btagVars;
   if ( btag_vars_.size() > 15 )
      btag_vars_.erase(btag_vars_.begin()+15,btag_vars_.end());
   Init();
   
}

template <typename T>
void Candidates<T>::UseTriggerResults(edm::InputTag& tr)
{
   triggerresults_collection_ = tr;
}



template <typename T>
void Candidates<T>::AddJecInfo( const std::string & jec )
{
   // Will use confDB
   jecRecord_ = jec;
}

template <typename T>
void Candidates<T>::AddJecInfo( const std::string & jec , const std::string & jec_file )
{
   // Will use txt file
   jecRecord_ = jec;
   jecFile_   = jec_file;
}

template <typename T>
void Candidates<T>::AddJerInfo( const std::string & jer, const edm::InputTag & rho )
{
   // Will use confDB
   jerRecord_ = jer;
   rho_collection_ = rho;
}

template <typename T>
void Candidates<T>::AddJerInfo(const std::string & jer, const std::string & res_file, const std::string & sf_file, const edm::InputTag & rho)
{
   // Will use txt file
   jerRecord_ = jer;
   jerFile_   = res_file;
   jersfFile_ = sf_file;
   rho_collection_ = rho;
   
}

// Need to declare all possible template classes here
template class Candidates<l1extra::L1JetParticle>;
template class Candidates<l1extra::L1MuonParticle>;
template class Candidates<reco::CaloJet>;
template class Candidates<reco::PFJet>;
template class Candidates<pat::Jet>;
template class Candidates<pat::Muon>;
template class Candidates<pat::MET>;
template class Candidates<reco::GenJet>;
template class Candidates<reco::GenParticle>;
template class Candidates<pat::TriggerObject>;
template class Candidates<trigger::TriggerObject>;
template class Candidates<l1t::Jet>;
template class Candidates<l1t::Muon>;
template class Candidates<reco::RecoChargedCandidate>;
