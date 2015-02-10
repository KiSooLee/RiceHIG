#include "../interface/ChargeDepCorrAnalyzerSP2.h"
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TMath.h>
#include <TRandom.h>
#include <TTree.h>
#include <TFile.h>
#include <TList.h>
#include <TIterator.h>
#include <TLorentzVector.h>
#include <TClonesArray.h>
#include <TString.h>
#include <TVector3.h>
#include <vector> 
#include <iostream>
#include "Math/Vector3D.h"

using namespace std;

ChargeDepCorrAnalyzerSP2::ChargeDepCorrAnalyzerSP2(const edm::ParameterSet& iConfig) :
  DiHadronCorrelationMultiBase(iConfig)
{
/*
  for(double mult = 0; mult < 4; mult += 1) multBins.push_back(mult);
  for(double mult = 4; mult < 10; mult += 2) multBins.push_back(mult);
  for(double mult = 10; mult < 30; mult += 4) multBins.push_back(mult);
  for(double mult = 30; mult < 80; mult += 10) multBins.push_back(mult);
  for(double mult = 80; mult < 120; mult += 20) multBins.push_back(mult);
  for(double mult = 120; mult < 200; mult += 40) multBins.push_back(mult);
  multBins.push_back(200.);
*/
/*
  for(double mult = 0; mult < 10; mult += 1) multBins.push_back(mult);
  for(double mult = 10; mult < 50; mult += 4) multBins.push_back(mult);
  for(double mult = 50; mult < 130; mult += 16) multBins.push_back(mult);
  for(double mult = 130; mult < 200; mult += 70) multBins.push_back(mult);
  for(double mult = 200; mult < 400; mult += 200) multBins.push_back(mult);
*/
  multBins.push_back(0);
  multBins.push_back(1000);

  cutPara.etagap = iConfig.getParameter<double>("etagap");
  cutPara.IsReCenter = iConfig.getParameter<bool>("IsReCenter");
/*
  dndetadphi_filename = new TString(iConfig.getParameter<string>("dNdetadphiFileName"));
  if(dndetadphi_filename->Length()) 
  {
    edm::FileInPath fip2(Form("FlowCorrAna/DiHadronCorrelationAnalyzer/data/%s",dndetadphi_filename->Data()));
    fdndetadphi = new TFile(fip2.fullPath().c_str(),"READ");
  }
  else fdndetadphi = 0;
*/
  if(cutPara.IsReCenter) return;
  recenter_filename = new TString(iConfig.getParameter<string>("ReCenterFileName"));
  recenter_dirname = new TString(iConfig.getParameter<string>("ReCenterDirName"));
  if(!recenter_filename->Length()) {frecenter = 0; return;}
  edm::FileInPath fip(Form("FlowCorrAna/DiHadronCorrelationAnalyzer/data/%s",recenter_filename->Data()));
  frecenter = new TFile(fip.fullPath().c_str(),"READ");
}

ChargeDepCorrAnalyzerSP2::~ChargeDepCorrAnalyzerSP2() 
{}

void ChargeDepCorrAnalyzerSP2::beginJob()
{
  DiHadronCorrelationMultiBase::beginJob();
   
//  double scale_factor = 500.0;
/*
  double etaMin   = 0.0-ETATRGBINWIDTHPT/2;
  double etaMax   = 4.8-ETATRGBINWIDTHPT/2;
  std::vector<double> etaBins;
  for(double eta = etaMin; eta < etaMax + ETATRGBINWIDTHPT/2; eta += ETATRGBINWIDTHPT)
    etaBins.push_back(eta);
*/
  hdNdEtadPhiPos = theOutputs->make<TH2F>("dndetadphipos",";#eta;#phi",MAXETATRGBINSPT,-2.4,2.4,128,-PI,PI);
  hdNdEtadPhiNeg = theOutputs->make<TH2F>("dndetadphineg",";#eta;#phi",MAXETATRGBINSPT,-2.4,2.4,128,-PI,PI);
/*
  if(fdndetadphi)
  {
    hdNdEtadPhiPosWeight = (TH2F*)fdndetadphi->Get("dNdetadphi_weight_pos");
    hdNdEtadPhiNegWeight = (TH2F*)fdndetadphi->Get("dNdetadphi_weight_neg");
  }
  else 
  {
    hdNdEtadPhiPosWeight = 0;
    hdNdEtadPhiNegWeight = 0; 
  }
*/
  if(!cutPara.IsReCenter)
  {
    hSumNpairs_pp = theOutputs->make<TH2F>("sumnpairs_pp",";#eta_{1};#eta_{2};npairs",MAXETATRGBINSPT,-2.4,2.4,MAXETATRGBINSPT,-2.4,2.4);
    hSumNpairs_nn = theOutputs->make<TH2F>("sumnpairs_nn",";#eta_{1};#eta_{2};npairs",MAXETATRGBINSPT,-2.4,2.4,MAXETATRGBINSPT,-2.4,2.4);
    hSumNpairs_pn = theOutputs->make<TH2F>("sumnpairs_pn",";#eta_{1};#eta_{2};npairs",MAXETATRGBINSPT,-2.4,2.4,MAXETATRGBINSPT,-2.4,2.4);
    hSumNpairs_np = theOutputs->make<TH2F>("sumnpairs_np",";#eta_{1};#eta_{2};npairs",MAXETATRGBINSPT,-2.4,2.4,MAXETATRGBINSPT,-2.4,2.4);

    hSumCos_pp_for = theOutputs->make<TH2F>("sumcos_pp_for",";#eta_{1};#eta_{2};cos(#phi_{a}+#phi_{b}-2#phi_{c})",MAXETATRGBINSPT,-2.4,2.4,MAXETATRGBINSPT,-2.4,2.4);
    hSumCos_nn_for = theOutputs->make<TH2F>("sumcos_nn_for",";#eta_{1};#eta_{2};cos(#phi_{a}+#phi_{b}-2#phi_{c})",MAXETATRGBINSPT,-2.4,2.4,MAXETATRGBINSPT,-2.4,2.4);
    hSumCos_pn_for = theOutputs->make<TH2F>("sumcos_pn_for",";#eta_{1};#eta_{2};cos(#phi_{a}+#phi_{b}-2#phi_{c})",MAXETATRGBINSPT,-2.4,2.4,MAXETATRGBINSPT,-2.4,2.4);
    hSumCos_np_for = theOutputs->make<TH2F>("sumcos_np_for",";#eta_{1};#eta_{2};cos(#phi_{a}+#phi_{b}-2#phi_{c})",MAXETATRGBINSPT,-2.4,2.4,MAXETATRGBINSPT,-2.4,2.4);
    hSumCos_pp_bak = theOutputs->make<TH2F>("sumcos_pp_bak",";#eta_{1};#eta_{2};cos(#phi_{a}+#phi_{b}-2#phi_{c})",MAXETATRGBINSPT,-2.4,2.4,MAXETATRGBINSPT,-2.4,2.4);
    hSumCos_nn_bak = theOutputs->make<TH2F>("sumcos_nn_bak",";#eta_{1};#eta_{2};cos(#phi_{a}+#phi_{b}-2#phi_{c})",MAXETATRGBINSPT,-2.4,2.4,MAXETATRGBINSPT,-2.4,2.4);
    hSumCos_pn_bak = theOutputs->make<TH2F>("sumcos_pn_bak",";#eta_{1};#eta_{2};cos(#phi_{a}+#phi_{b}-2#phi_{c})",MAXETATRGBINSPT,-2.4,2.4,MAXETATRGBINSPT,-2.4,2.4);
    hSumCos_np_bak = theOutputs->make<TH2F>("sumcos_np_bak",";#eta_{1};#eta_{2};cos(#phi_{a}+#phi_{b}-2#phi_{c})",MAXETATRGBINSPT,-2.4,2.4,MAXETATRGBINSPT,-2.4,2.4);
    hSumCos2PcorrP_pp = theOutputs->make<TH2F>("sumcos2pcorrp_pp",";#eta_{1};#eta_{2};cos(#phi_{a}+#phi_{b})",MAXETATRGBINSPT,-2.4,2.4,MAXETATRGBINSPT,-2.4,2.4);
    hSumCos2PcorrP_nn = theOutputs->make<TH2F>("sumcos2pcorrp_nn",";#eta_{1};#eta_{2};cos(#phi_{a}+#phi_{b})",MAXETATRGBINSPT,-2.4,2.4,MAXETATRGBINSPT,-2.4,2.4);
    hSumCos2PcorrM_pp = theOutputs->make<TH2F>("sumcos2pcorrm_pp",";#eta_{1};#eta_{2};cos(#phi_{a}-#phi_{b})",MAXETATRGBINSPT,-2.4,2.4,MAXETATRGBINSPT,-2.4,2.4);
    hSumCos2PcorrM_nn = theOutputs->make<TH2F>("sumcos2pcorrm_nn",";#eta_{1};#eta_{2};cos(#phi_{a}-#phi_{b})",MAXETATRGBINSPT,-2.4,2.4,MAXETATRGBINSPT,-2.4,2.4);
    hSumCos2PcorrP_pn = theOutputs->make<TH2F>("sumcos2pcorrp_pn",";#eta_{1};#eta_{2};cos(#phi_{a}+#phi_{b})",MAXETATRGBINSPT,-2.4,2.4,MAXETATRGBINSPT,-2.4,2.4);
    hSumCos2PcorrP_np = theOutputs->make<TH2F>("sumcos2pcorrp_np",";#eta_{1};#eta_{2};cos(#phi_{a}+#phi_{b})",MAXETATRGBINSPT,-2.4,2.4,MAXETATRGBINSPT,-2.4,2.4);
    hSumCos2PcorrM_pn = theOutputs->make<TH2F>("sumcos2pcorrm_pn",";#eta_{1};#eta_{2};cos(#phi_{a}-#phi_{b})",MAXETATRGBINSPT,-2.4,2.4,MAXETATRGBINSPT,-2.4,2.4);
    hSumCos2PcorrM_np = theOutputs->make<TH2F>("sumcos2pcorrm_np",";#eta_{1};#eta_{2};cos(#phi_{a}-#phi_{b})",MAXETATRGBINSPT,-2.4,2.4,MAXETATRGBINSPT,-2.4,2.4);

    for(int ietabin = 1; ietabin<=MAXETATRGBINSPT; ietabin++)
      for(int jetabin = ietabin; jetabin<=MAXETATRGBINSPT; jetabin++)
      {
        npairs_pp_total[ietabin-1][jetabin-1] = 0;
        npairs_nn_total[ietabin-1][jetabin-1] = 0;
        npairs_pn_total[ietabin-1][jetabin-1] = 0;
        npairs_np_total[ietabin-1][jetabin-1] = 0;

        hSignalCos_pp_for[ietabin-1][jetabin-1] = new TH1F(Form("signalcos_pp_for_n%d_n%d",ietabin-1,jetabin-1),";cos(#phi_{a}+#phi_{b}-2#phi_{c})",5000,-50,50);
        hSignalCos_nn_for[ietabin-1][jetabin-1] = new TH1F(Form("signalcos_nn_for_n%d_n%d",ietabin-1,jetabin-1),";cos(#phi_{a}+#phi_{b}-2#phi_{c})",5000,-50,50);
        hSignalCos_pn_for[ietabin-1][jetabin-1] = new TH1F(Form("signalcos_pn_for_n%d_n%d",ietabin-1,jetabin-1),";cos(#phi_{a}+#phi_{b}-2#phi_{c})",5000,-50,50);
        hSignalCos_np_for[ietabin-1][jetabin-1] = new TH1F(Form("signalcos_np_for_n%d_n%d",ietabin-1,jetabin-1),";cos(#phi_{a}+#phi_{b}-2#phi_{c})",5000,-50,50);
        hSignalCos_pp_bak[ietabin-1][jetabin-1] = new TH1F(Form("signalcos_pp_bak_n%d_n%d",ietabin-1,jetabin-1),";cos(#phi_{a}+#phi_{b}-2#phi_{c})",5000,-50,50);
        hSignalCos_nn_bak[ietabin-1][jetabin-1] = new TH1F(Form("signalcos_nn_bak_n%d_n%d",ietabin-1,jetabin-1),";cos(#phi_{a}+#phi_{b}-2#phi_{c})",5000,-50,50);
        hSignalCos_pn_bak[ietabin-1][jetabin-1] = new TH1F(Form("signalcos_pn_bak_n%d_n%d",ietabin-1,jetabin-1),";cos(#phi_{a}+#phi_{b}-2#phi_{c})",5000,-50,50);
        hSignalCos_np_bak[ietabin-1][jetabin-1] = new TH1F(Form("signalcos_np_bak_n%d_n%d",ietabin-1,jetabin-1),";cos(#phi_{a}+#phi_{b}-2#phi_{c})",5000,-50,50);

        hSignalCos2PcorrP_pp[ietabin-1][jetabin-1] = new TH1F(Form("signalcos2pcorrp_pp_n%d_n%d",ietabin-1,jetabin-1),";cos(#phi_{a}+#phi_{b})",5000,-50,50);
        hSignalCos2PcorrP_nn[ietabin-1][jetabin-1] = new TH1F(Form("signalcos2pcorrp_nn_n%d_n%d",ietabin-1,jetabin-1),";cos(#phi_{a}+#phi_{b})",5000,-50,50);
        hSignalCos2PcorrM_pp[ietabin-1][jetabin-1] = new TH1F(Form("signalcos2pcorrm_pp_n%d_n%d",ietabin-1,jetabin-1),";cos(#phi_{a}-#phi_{b})",5000,-50,50);
        hSignalCos2PcorrM_nn[ietabin-1][jetabin-1] = new TH1F(Form("signalcos2pcorrm_nn_n%d_n%d",ietabin-1,jetabin-1),";cos(#phi_{a}-#phi_{b})",5000,-50,50);
        hSignalCos2PcorrP_pn[ietabin-1][jetabin-1] = new TH1F(Form("signalcos2pcorrp_pn_n%d_n%d",ietabin-1,jetabin-1),";cos(#phi_{a}+#phi_{b})",5000,-50,50);
        hSignalCos2PcorrP_np[ietabin-1][jetabin-1] = new TH1F(Form("signalcos2pcorrp_np_n%d_n%d",ietabin-1,jetabin-1),";cos(#phi_{a}+#phi_{b})",5000,-50,50);
        hSignalCos2PcorrM_pn[ietabin-1][jetabin-1] = new TH1F(Form("signalcos2pcorrm_pn_n%d_n%d",ietabin-1,jetabin-1),";cos(#phi_{a}-#phi_{b})",5000,-50,50);
        hSignalCos2PcorrM_np[ietabin-1][jetabin-1] = new TH1F(Form("signalcos2pcorrm_np_n%d_n%d",ietabin-1,jetabin-1),";cos(#phi_{a}-#phi_{b})",5000,-50,50);
      }

    hQ2AssCos = theOutputs->make<TH1F>("q2asscos",";Q_{2}^{2}",20000,-10000.0,10000.0);
    hSumNpairs_hf = theOutputs->make<TH1F>("sumnpairs_hf",";Npairs_for",1,0,1);
  }
  else
  {
    for(int kk=0;kk<MAXZVTXBINS;kk++)
    {
      for(int ll=0;ll<MAXETATRGBINSPT;ll++)
      {
        hSumCosTrgPos[kk][ll] = theOutputs->make<TH2F>(Form("sumcostrgpos_vtx%d_eta%d",kk,ll),";Q_{2};n",400,-1.0,1.0,multBins.size()-1, &multBins[0]);
        hSumSinTrgPos[kk][ll] = theOutputs->make<TH2F>(Form("sumsintrgpos_vtx%d_eta%d",kk,ll),";Q_{2};n",400,-1.0,1.0,multBins.size()-1, &multBins[0]);
        hSumCosTrgNeg[kk][ll] = theOutputs->make<TH2F>(Form("sumcostrgneg_vtx%d_eta%d",kk,ll),";Q_{2};n",400,-1.0,1.0,multBins.size()-1, &multBins[0]);
        hSumSinTrgNeg[kk][ll] = theOutputs->make<TH2F>(Form("sumsintrgneg_vtx%d_eta%d",kk,ll),";Q_{2};n",400,-1.0,1.0,multBins.size()-1, &multBins[0]);
        hSumCos2TrgPos[kk][ll] = theOutputs->make<TH2F>(Form("sumcos2trgpos_vtx%d_eta%d",kk,ll),";Q_{2};n",400,-1.0,1.0,multBins.size()-1, &multBins[0]);
        hSumSin2TrgPos[kk][ll] = theOutputs->make<TH2F>(Form("sumsin2trgpos_vtx%d_eta%d",kk,ll),";Q_{2};n",400,-1.0,1.0,multBins.size()-1, &multBins[0]);
        hSumCos2TrgNeg[kk][ll] = theOutputs->make<TH2F>(Form("sumcos2trgneg_vtx%d_eta%d",kk,ll),";Q_{2};n",400,-1.0,1.0,multBins.size()-1, &multBins[0]);
        hSumSin2TrgNeg[kk][ll] = theOutputs->make<TH2F>(Form("sumsin2trgneg_vtx%d_eta%d",kk,ll),";Q_{2};n",400,-1.0,1.0,multBins.size()-1, &multBins[0]);
      }

      hSumAssFor[kk] = theOutputs->make<TH1F>(Form("sumassfor_vtx%d",kk),";Q_{2}",10000,0.0,10000.0);
      hSumAssBak[kk] = theOutputs->make<TH1F>(Form("sumassbak_vtx%d",kk),";Q_{2}",10000,0.0,10000.0);
      hSumCosAssFor[kk] = theOutputs->make<TH1F>(Form("sumcosassfor_vtx%d",kk),";Q_{2}",10000,-500.0,500.0);
      hSumSinAssFor[kk] = theOutputs->make<TH1F>(Form("sumsinassfor_vtx%d",kk),";Q_{2}",10000,-500.0,500.0);
      hSumCosAssBak[kk] = theOutputs->make<TH1F>(Form("sumcosassbak_vtx%d",kk),";Q_{2}",10000,-500.0,500.0);
      hSumSinAssBak[kk] = theOutputs->make<TH1F>(Form("sumsinassbak_vtx%d",kk),";Q_{2}",10000,-500.0,500.0);
      hSumCos2AssFor[kk] = theOutputs->make<TH1F>(Form("sumcos2assfor_vtx%d",kk),";Q_{2}",10000,-500.0,500.0);
      hSumSin2AssFor[kk] = theOutputs->make<TH1F>(Form("sumsin2assfor_vtx%d",kk),";Q_{2}",10000,-500.0,500.0);
      hSumCos2AssBak[kk] = theOutputs->make<TH1F>(Form("sumcos2assbak_vtx%d",kk),";Q_{2}",10000,-500.0,500.0);
      hSumSin2AssBak[kk] = theOutputs->make<TH1F>(Form("sumsin2assbak_vtx%d",kk),";Q_{2}",10000,-500.0,500.0);
    }
  }

  for(int kk=0;kk<MAXZVTXBINS;kk++)
  {
    if(frecenter)
    {
/*
      hSumNCosTrgPos[kk] = (TH2F*)frecenter->Get(Form("sumncostrgpos_vtx%d",kk));
      hSumNCosTrgNeg[kk] = (TH2F*)frecenter->Get(Form("sumncostrgneg_vtx%d",kk));
      hSumNSinTrgPos[kk] = (TH2F*)frecenter->Get(Form("sumnsintrgpos_vtx%d",kk));
      hSumNSinTrgNeg[kk] = (TH2F*)frecenter->Get(Form("sumnsintrgneg_vtx%d",kk));
      hSumNCos2TrgPos[kk] = (TH2F*)frecenter->Get(Form("sumncos2trgpos_vtx%d",kk));
      hSumNCos2TrgNeg[kk] = (TH2F*)frecenter->Get(Form("sumncos2trgneg_vtx%d",kk));
      hSumNSin2TrgPos[kk] = (TH2F*)frecenter->Get(Form("sumnsin2trgpos_vtx%d",kk));
      hSumNSin2TrgNeg[kk] = (TH2F*)frecenter->Get(Form("sumnsin2trgneg_vtx%d",kk));
*/
      hSumNCosTrgPos[kk] = (TH2F*)frecenter->Get(Form("sumncostrgpos_vtx%d_all",kk));
      hSumNCosTrgNeg[kk] = (TH2F*)frecenter->Get(Form("sumncostrgneg_vtx%d_all",kk));
      hSumNSinTrgPos[kk] = (TH2F*)frecenter->Get(Form("sumnsintrgpos_vtx%d_all",kk));
      hSumNSinTrgNeg[kk] = (TH2F*)frecenter->Get(Form("sumnsintrgneg_vtx%d_all",kk));
      hSumNCos2TrgPos[kk] = (TH2F*)frecenter->Get(Form("sumncos2trgpos_vtx%d_all",kk));
      hSumNCos2TrgNeg[kk] = (TH2F*)frecenter->Get(Form("sumncos2trgneg_vtx%d_all",kk));
      hSumNSin2TrgPos[kk] = (TH2F*)frecenter->Get(Form("sumnsin2trgpos_vtx%d_all",kk));
      hSumNSin2TrgNeg[kk] = (TH2F*)frecenter->Get(Form("sumnsin2trgneg_vtx%d_all",kk));
      if(!hSumNCosTrgPos[kk] || !hSumNCosTrgNeg[kk] || !hSumNSinTrgPos[kk] || !hSumNSinTrgNeg[kk] || !hSumNCos2TrgPos[kk] || !hSumNCos2TrgNeg[kk] || !hSumNSin2TrgPos[kk] || !hSumNSin2TrgNeg[kk]) cout<<"correction histograms missing!"<<endl;
    }
    else
    {
      hSumNCosTrgPos[kk] = 0; 
      hSumNCosTrgNeg[kk] = 0;
      hSumNSinTrgPos[kk] = 0;
      hSumNSinTrgNeg[kk] = 0;
      hSumNCos2TrgPos[kk] = 0;
      hSumNCos2TrgNeg[kk] = 0;
      hSumNSin2TrgPos[kk] = 0;
      hSumNSin2TrgNeg[kk] = 0;
      cout<<"No acceptance correction applied!"<<endl;
    }
  }
}

void ChargeDepCorrAnalyzerSP2::endJob()
{
  DiHadronCorrelationMultiBase::endJob();
  
  unsigned int neventsArr = eventcorrArray.size();
  unsigned int nevents = hMultRawAll->GetEntries();

  cout<< "Start running CME analysis!" << endl;
  cout<< "There are "<<neventsArr<<" and "<<nevents<<" events selected!"<<endl;

  if(cutPara.IsReCenter) 
  {
    for(unsigned int i=0;i<neventsArr;i++)
    {
      if( i % 1000 == 0 ) cout << "Recenter: processing " << i << "th event" << endl;
      int zvtxbin = (int)((eventcorrArray[i].zvtx+15.)/30.0*MAXZVTXBINS);
      FillHistsQVectorTrk(eventcorrArray[i],zvtxbin);
      FillHistsQVectorHF(eventcorrArray[i],zvtxbin);
      Recenter(zvtxbin);
    }
  }
  else
  {
    for(unsigned int i=0;i<neventsArr;i++)
    {
      int zvtxbin = (int)((eventcorrArray[i].zvtx+15.)/30.0*MAXZVTXBINS);

      FillHistsQVectorHF(eventcorrArray[i],zvtxbin);
      if(npairs_ass_for && npairs_ass_bak) 
      {
        hQ2AssCos->Fill(sumcosn_ass_for*sumcosn_ass_bak+sumsinn_ass_for*sumsinn_ass_bak);
        hSumNpairs_hf->SetBinContent(1,npairs_ass_for*npairs_ass_bak); 
      }

      FillHistsQVectorTrk(eventcorrArray[i],zvtxbin);       
      for(int ietabin = 1; ietabin<=MAXETATRGBINSPT; ietabin++)
        for(int jetabin = ietabin; jetabin<=MAXETATRGBINSPT; jetabin++)
          FillHistsCorrelators(ietabin-1,jetabin-1);
    }

    for(int ietabin = 1; ietabin<=MAXETATRGBINSPT; ietabin++)
      for(int jetabin = ietabin; jetabin<=MAXETATRGBINSPT; jetabin++)
      {
        double sigcos_pp_for = hSignalCos_pp_for[ietabin-1][jetabin-1]->GetMean();
        double sigcos_pp_bak = hSignalCos_pp_bak[ietabin-1][jetabin-1]->GetMean();
        double sigcos2pcorrp_pp = hSignalCos2PcorrP_pp[ietabin-1][jetabin-1]->GetMean();
        double sigcos2pcorrm_pp = hSignalCos2PcorrM_pp[ietabin-1][jetabin-1]->GetMean();
        double sigcos_nn_for = hSignalCos_nn_for[ietabin-1][jetabin-1]->GetMean();
        double sigcos_nn_bak = hSignalCos_nn_bak[ietabin-1][jetabin-1]->GetMean();
        double sigcos2pcorrp_nn = hSignalCos2PcorrP_nn[ietabin-1][jetabin-1]->GetMean();
        double sigcos2pcorrm_nn = hSignalCos2PcorrM_nn[ietabin-1][jetabin-1]->GetMean();

        double nevts_sigcos_pp_for = hSignalCos_pp_for[ietabin-1][jetabin-1]->GetEntries();
        double nevts_sigcos_pp_bak = hSignalCos_pp_bak[ietabin-1][jetabin-1]->GetEntries();
        double nevts_sigcos2pcorrp_pp = hSignalCos2PcorrP_pp[ietabin-1][jetabin-1]->GetEntries();
        double nevts_sigcos2pcorrm_pp = hSignalCos2PcorrM_pp[ietabin-1][jetabin-1]->GetEntries();
        double nevts_sigcos_nn_for = hSignalCos_nn_for[ietabin-1][jetabin-1]->GetEntries();
        double nevts_sigcos_nn_bak = hSignalCos_nn_bak[ietabin-1][jetabin-1]->GetEntries();
        double nevts_sigcos2pcorrp_nn = hSignalCos2PcorrP_nn[ietabin-1][jetabin-1]->GetEntries();
        double nevts_sigcos2pcorrm_nn = hSignalCos2PcorrM_nn[ietabin-1][jetabin-1]->GetEntries();

        double sigcos_pp_for_err = hSignalCos_pp_for[ietabin-1][jetabin-1]->GetMeanError();
        double sigcos_pp_bak_err = hSignalCos_pp_bak[ietabin-1][jetabin-1]->GetMeanError();
        double sigcos2pcorrp_pp_err = hSignalCos2PcorrP_pp[ietabin-1][jetabin-1]->GetMeanError();
        double sigcos2pcorrm_pp_err = hSignalCos2PcorrM_pp[ietabin-1][jetabin-1]->GetMeanError();
        double sigcos_nn_for_err = hSignalCos_nn_for[ietabin-1][jetabin-1]->GetMeanError();
        double sigcos_nn_bak_err = hSignalCos_nn_bak[ietabin-1][jetabin-1]->GetMeanError();
        double sigcos2pcorrp_nn_err = hSignalCos2PcorrP_nn[ietabin-1][jetabin-1]->GetMeanError();
        double sigcos2pcorrm_nn_err = hSignalCos2PcorrM_nn[ietabin-1][jetabin-1]->GetMeanError();

        double sigcos_pn_for = hSignalCos_pn_for[ietabin-1][jetabin-1]->GetMean();
        double sigcos_pn_bak = hSignalCos_pn_bak[ietabin-1][jetabin-1]->GetMean();
        double sigcos2pcorrp_pn = hSignalCos2PcorrP_pn[ietabin-1][jetabin-1]->GetMean(); 
        double sigcos2pcorrm_pn = hSignalCos2PcorrM_pn[ietabin-1][jetabin-1]->GetMean();
        double sigcos_np_for = hSignalCos_np_for[ietabin-1][jetabin-1]->GetMean();
        double sigcos_np_bak = hSignalCos_np_bak[ietabin-1][jetabin-1]->GetMean();
        double sigcos2pcorrp_np = hSignalCos2PcorrP_np[ietabin-1][jetabin-1]->GetMean(); 
        double sigcos2pcorrm_np = hSignalCos2PcorrM_np[ietabin-1][jetabin-1]->GetMean();

        double nevts_sigcos_pn_for = hSignalCos_pn_for[ietabin-1][jetabin-1]->GetEntries();
        double nevts_sigcos_pn_bak = hSignalCos_pn_bak[ietabin-1][jetabin-1]->GetEntries();
        double nevts_sigcos2pcorrp_pn = hSignalCos2PcorrP_pn[ietabin-1][jetabin-1]->GetEntries();
        double nevts_sigcos2pcorrm_pn = hSignalCos2PcorrM_pn[ietabin-1][jetabin-1]->GetEntries();
        double nevts_sigcos_np_for = hSignalCos_np_for[ietabin-1][jetabin-1]->GetEntries();
        double nevts_sigcos_np_bak = hSignalCos_np_bak[ietabin-1][jetabin-1]->GetEntries();
        double nevts_sigcos2pcorrp_np = hSignalCos2PcorrP_np[ietabin-1][jetabin-1]->GetEntries();
        double nevts_sigcos2pcorrm_np = hSignalCos2PcorrM_np[ietabin-1][jetabin-1]->GetEntries();

        double sigcos_pn_for_err = hSignalCos_pn_for[ietabin-1][jetabin-1]->GetMeanError();
        double sigcos_pn_bak_err = hSignalCos_pn_bak[ietabin-1][jetabin-1]->GetMeanError();
        double sigcos2pcorrp_pn_err = hSignalCos2PcorrP_pn[ietabin-1][jetabin-1]->GetMeanError();
        double sigcos2pcorrm_pn_err = hSignalCos2PcorrM_pn[ietabin-1][jetabin-1]->GetMeanError();
        double sigcos_np_for_err = hSignalCos_np_for[ietabin-1][jetabin-1]->GetMeanError();
        double sigcos_np_bak_err = hSignalCos_np_bak[ietabin-1][jetabin-1]->GetMeanError();
        double sigcos2pcorrp_np_err = hSignalCos2PcorrP_np[ietabin-1][jetabin-1]->GetMeanError();
        double sigcos2pcorrm_np_err = hSignalCos2PcorrM_np[ietabin-1][jetabin-1]->GetMeanError();

        hSignalCos_pp_for[ietabin-1][jetabin-1]->Reset();
        hSignalCos_pp_bak[ietabin-1][jetabin-1]->Reset();
        hSignalCos2PcorrP_pp[ietabin-1][jetabin-1]->Reset();
        hSignalCos2PcorrM_pp[ietabin-1][jetabin-1]->Reset();
        hSignalCos_nn_for[ietabin-1][jetabin-1]->Reset();
        hSignalCos_nn_bak[ietabin-1][jetabin-1]->Reset();
        hSignalCos2PcorrP_nn[ietabin-1][jetabin-1]->Reset();
        hSignalCos2PcorrM_nn[ietabin-1][jetabin-1]->Reset();
        hSignalCos_pn_for[ietabin-1][jetabin-1]->Reset();
        hSignalCos_pn_bak[ietabin-1][jetabin-1]->Reset();
        hSignalCos2PcorrP_pn[ietabin-1][jetabin-1]->Reset();
        hSignalCos2PcorrM_pn[ietabin-1][jetabin-1]->Reset();
        hSignalCos_np_for[ietabin-1][jetabin-1]->Reset();
        hSignalCos_np_bak[ietabin-1][jetabin-1]->Reset();
        hSignalCos2PcorrP_np[ietabin-1][jetabin-1]->Reset();
        hSignalCos2PcorrM_np[ietabin-1][jetabin-1]->Reset();

        hSumCos_pp_for->SetBinContent(ietabin,jetabin,hSumCos_pp_for->GetBinContent(ietabin,jetabin)+sigcos_pp_for*nevts_sigcos_pp_for);
        hSumCos_pn_for->SetBinContent(ietabin,jetabin,hSumCos_pn_for->GetBinContent(ietabin,jetabin)+sigcos_pn_for*nevts_sigcos_pn_for);
        hSumCos_pp_bak->SetBinContent(ietabin,jetabin,hSumCos_pp_bak->GetBinContent(ietabin,jetabin)+sigcos_pp_bak*nevts_sigcos_pp_bak);
        hSumCos_pn_bak->SetBinContent(ietabin,jetabin,hSumCos_pn_bak->GetBinContent(ietabin,jetabin)+sigcos_pn_bak*nevts_sigcos_pn_bak);
        hSumCos2PcorrP_pp->SetBinContent(ietabin,jetabin,hSumCos2PcorrP_pp->GetBinContent(ietabin,jetabin)+sigcos2pcorrp_pp*nevts_sigcos2pcorrp_pp);
        hSumCos2PcorrP_pn->SetBinContent(ietabin,jetabin,hSumCos2PcorrP_pn->GetBinContent(ietabin,jetabin)+sigcos2pcorrp_pn*nevts_sigcos2pcorrp_pn);
        hSumCos2PcorrM_pp->SetBinContent(ietabin,jetabin,hSumCos2PcorrM_pp->GetBinContent(ietabin,jetabin)+sigcos2pcorrm_pp*nevts_sigcos2pcorrm_pp);
        hSumCos2PcorrM_pn->SetBinContent(ietabin,jetabin,hSumCos2PcorrM_pn->GetBinContent(ietabin,jetabin)+sigcos2pcorrm_pn*nevts_sigcos2pcorrm_pn);

        double err = hSumCos_pp_for->GetBinError(ietabin,jetabin)*hSumCos_pp_for->GetBinError(ietabin,jetabin)+sigcos_pp_for_err*sigcos_pp_for_err*nevts_sigcos_pp_for*nevts_sigcos_pp_for;
        if(err) hSumCos_pp_for->SetBinError(ietabin,jetabin,sqrt(err));
        err = hSumCos_pn_for->GetBinError(ietabin,jetabin)*hSumCos_pn_for->GetBinError(ietabin,jetabin)+sigcos_pn_for_err*sigcos_pn_for_err*nevts_sigcos_pn_for*nevts_sigcos_pn_for;
        if(err) hSumCos_pn_for->SetBinError(ietabin,jetabin,sqrt(err));
        err = hSumCos_pp_bak->GetBinError(ietabin,jetabin)*hSumCos_pp_bak->GetBinError(ietabin,jetabin)+sigcos_pp_bak_err*sigcos_pp_bak_err*nevts_sigcos_pp_bak*nevts_sigcos_pp_bak;
        if(err) hSumCos_pp_bak->SetBinError(ietabin,jetabin,sqrt(err));
        err = hSumCos_pn_bak->GetBinError(ietabin,jetabin)*hSumCos_pn_bak->GetBinError(ietabin,jetabin)+sigcos_pn_bak_err*sigcos_pn_bak_err*nevts_sigcos_pn_bak*nevts_sigcos_pn_bak;
        if(err) hSumCos_pn_bak->SetBinError(ietabin,jetabin,sqrt(err));
        err = hSumCos2PcorrP_pp->GetBinError(ietabin,jetabin)*hSumCos2PcorrP_pp->GetBinError(ietabin,jetabin)+sigcos2pcorrp_pp_err*sigcos2pcorrp_pp_err*nevts_sigcos2pcorrp_pp*nevts_sigcos2pcorrp_pp;
        if(err) hSumCos2PcorrP_pp->SetBinError(ietabin,jetabin,sqrt(err));
        err = hSumCos2PcorrP_pn->GetBinError(ietabin,jetabin)*hSumCos2PcorrP_pn->GetBinError(ietabin,jetabin)+sigcos2pcorrp_pn_err*sigcos2pcorrp_pn_err*nevts_sigcos2pcorrp_pn*nevts_sigcos2pcorrp_pn;
        if(err) hSumCos2PcorrP_pn->SetBinError(ietabin,jetabin,sqrt(err));
        err = hSumCos2PcorrM_pp->GetBinError(ietabin,jetabin)*hSumCos2PcorrM_pp->GetBinError(ietabin,jetabin)+sigcos2pcorrm_pp_err*sigcos2pcorrm_pp_err*nevts_sigcos2pcorrm_pp*nevts_sigcos2pcorrm_pp;
        if(err) hSumCos2PcorrM_pp->SetBinError(ietabin,jetabin,sqrt(err));
        err = hSumCos2PcorrM_pn->GetBinError(ietabin,jetabin)*hSumCos2PcorrM_pn->GetBinError(ietabin,jetabin)+sigcos2pcorrm_pn_err*sigcos2pcorrm_pn_err*nevts_sigcos2pcorrm_pn*nevts_sigcos2pcorrm_pn;
        if(err) hSumCos2PcorrM_pn->SetBinError(ietabin,jetabin,sqrt(err));

        hSumCos_nn_for->SetBinContent(ietabin,jetabin,hSumCos_nn_for->GetBinContent(ietabin,jetabin)+sigcos_nn_for*nevts_sigcos_nn_for);
        hSumCos_np_for->SetBinContent(ietabin,jetabin,hSumCos_np_for->GetBinContent(ietabin,jetabin)+sigcos_np_for*nevts_sigcos_np_for);
        hSumCos_nn_bak->SetBinContent(ietabin,jetabin,hSumCos_nn_bak->GetBinContent(ietabin,jetabin)+sigcos_nn_bak*nevts_sigcos_nn_bak);
        hSumCos_np_bak->SetBinContent(ietabin,jetabin,hSumCos_np_bak->GetBinContent(ietabin,jetabin)+sigcos_np_bak*nevts_sigcos_np_bak);
        hSumCos2PcorrP_nn->SetBinContent(ietabin,jetabin,hSumCos2PcorrP_nn->GetBinContent(ietabin,jetabin)+sigcos2pcorrp_nn*nevts_sigcos2pcorrp_nn);
        hSumCos2PcorrP_np->SetBinContent(ietabin,jetabin,hSumCos2PcorrP_np->GetBinContent(ietabin,jetabin)+sigcos2pcorrp_np*nevts_sigcos2pcorrp_np);
        hSumCos2PcorrM_nn->SetBinContent(ietabin,jetabin,hSumCos2PcorrM_nn->GetBinContent(ietabin,jetabin)+sigcos2pcorrm_nn*nevts_sigcos2pcorrm_nn);
        hSumCos2PcorrM_np->SetBinContent(ietabin,jetabin,hSumCos2PcorrM_np->GetBinContent(ietabin,jetabin)+sigcos2pcorrm_np*nevts_sigcos2pcorrm_np);

        err = hSumCos_nn_for->GetBinError(ietabin,jetabin)*hSumCos_nn_for->GetBinError(ietabin,jetabin)+sigcos_nn_for_err*sigcos_nn_for_err*nevts_sigcos_nn_for*nevts_sigcos_nn_for;
        if(err) hSumCos_nn_for->SetBinError(ietabin,jetabin,sqrt(err));
        err = hSumCos_np_for->GetBinError(ietabin,jetabin)*hSumCos_np_for->GetBinError(ietabin,jetabin)+sigcos_np_for_err*sigcos_np_for_err*nevts_sigcos_np_for*nevts_sigcos_np_for;
        if(err) hSumCos_np_for->SetBinError(ietabin,jetabin,sqrt(err));
        err = hSumCos_nn_bak->GetBinError(ietabin,jetabin)*hSumCos_nn_bak->GetBinError(ietabin,jetabin)+sigcos_nn_bak_err*sigcos_nn_bak_err*nevts_sigcos_nn_bak*nevts_sigcos_nn_bak;
        if(err) hSumCos_nn_bak->SetBinError(ietabin,jetabin,sqrt(err));
        err = hSumCos_np_bak->GetBinError(ietabin,jetabin)*hSumCos_np_bak->GetBinError(ietabin,jetabin)+sigcos_np_bak_err*sigcos_np_bak_err*nevts_sigcos_np_bak*nevts_sigcos_np_bak;
        if(err) hSumCos_np_bak->SetBinError(ietabin,jetabin,sqrt(err));
        err = hSumCos2PcorrP_nn->GetBinError(ietabin,jetabin)*hSumCos2PcorrP_nn->GetBinError(ietabin,jetabin)+sigcos2pcorrp_nn_err*sigcos2pcorrp_nn_err*nevts_sigcos2pcorrp_nn*nevts_sigcos2pcorrp_nn;
        if(err) hSumCos2PcorrP_nn->SetBinError(ietabin,jetabin,sqrt(err));
        err = hSumCos2PcorrP_np->GetBinError(ietabin,jetabin)*hSumCos2PcorrP_np->GetBinError(ietabin,jetabin)+sigcos2pcorrp_np_err*sigcos2pcorrp_np_err*nevts_sigcos2pcorrp_np*nevts_sigcos2pcorrp_np;
        if(err) hSumCos2PcorrP_np->SetBinError(ietabin,jetabin,sqrt(err));
        err = hSumCos2PcorrM_nn->GetBinError(ietabin,jetabin)*hSumCos2PcorrM_nn->GetBinError(ietabin,jetabin)+sigcos2pcorrm_nn_err*sigcos2pcorrm_nn_err*nevts_sigcos2pcorrm_nn*nevts_sigcos2pcorrm_nn;
        if(err) hSumCos2PcorrM_nn->SetBinError(ietabin,jetabin,sqrt(err));
        err = hSumCos2PcorrM_np->GetBinError(ietabin,jetabin)*hSumCos2PcorrM_np->GetBinError(ietabin,jetabin)+sigcos2pcorrm_np_err*sigcos2pcorrm_np_err*nevts_sigcos2pcorrm_np*nevts_sigcos2pcorrm_np;
        if(err) hSumCos2PcorrM_np->SetBinError(ietabin,jetabin,sqrt(err));

//if(npairs_pp_total[ietabin-1][jetabin-1]) cout<<nn<<" "<<mm<<" "<<ietabin<<" "<<jetabin<<" "<<npairs_pp_total[ietabin-1][jetabin-1]<<endl;
        hSumNpairs_pp->SetBinContent(ietabin,jetabin,hSumNpairs_pp->GetBinContent(ietabin,jetabin)+npairs_pp_total[ietabin-1][jetabin-1]);
        hSumNpairs_nn->SetBinContent(ietabin,jetabin,hSumNpairs_nn->GetBinContent(ietabin,jetabin)+npairs_nn_total[ietabin-1][jetabin-1]);
        hSumNpairs_pn->SetBinContent(ietabin,jetabin,hSumNpairs_pn->GetBinContent(ietabin,jetabin)+npairs_pn_total[ietabin-1][jetabin-1]);
        hSumNpairs_np->SetBinContent(ietabin,jetabin,hSumNpairs_np->GetBinContent(ietabin,jetabin)+npairs_np_total[ietabin-1][jetabin-1]);
        hSumNpairs_pp->SetBinError(ietabin,jetabin,0);
        hSumNpairs_nn->SetBinError(ietabin,jetabin,0);
        hSumNpairs_pn->SetBinError(ietabin,jetabin,0);
        hSumNpairs_np->SetBinError(ietabin,jetabin,0);

        npairs_pp_total[ietabin-1][jetabin-1] = 0;
        npairs_nn_total[ietabin-1][jetabin-1] = 0;
        npairs_pn_total[ietabin-1][jetabin-1] = 0;
        npairs_np_total[ietabin-1][jetabin-1] = 0;
      }
  }

  cout<< "Finish running CME analysis!" << endl;
}

void ChargeDepCorrAnalyzerSP2::Recenter(int zvtxbin)
{
  if(npairs_ass_for)
  {
    hSumAssFor[zvtxbin]->Fill(npairs_ass_for);
    hSumCosAssFor[zvtxbin]->Fill(sumcosn_ass_for);
    hSumSinAssFor[zvtxbin]->Fill(sumsinn_ass_for);
    hSumCos2AssFor[zvtxbin]->Fill(sumcos2n_ass_for);
    hSumSin2AssFor[zvtxbin]->Fill(sumsin2n_ass_for);
  }
  if(npairs_ass_bak)
  {
    hSumAssBak[zvtxbin]->Fill(npairs_ass_bak);
    hSumCosAssBak[zvtxbin]->Fill(sumcosn_ass_bak);
    hSumSinAssBak[zvtxbin]->Fill(sumsinn_ass_bak);
    hSumCos2AssBak[zvtxbin]->Fill(sumcos2n_ass_bak);
    hSumSin2AssBak[zvtxbin]->Fill(sumsin2n_ass_bak);
  }

  for(int i=0;i<MAXETATRGBINSPT;i++)
  {
    if(npairs_trg_pos[i]!=0)
    {
      hSumCosTrgPos[zvtxbin][i]->Fill(sumcosn_trg_pos[i]/npairs_trg_pos[i],npairs_trg_pos[i]);
      hSumCos2TrgPos[zvtxbin][i]->Fill(sumcos2n_trg_pos[i]/npairs_trg_pos[i],npairs_trg_pos[i]);
      hSumSinTrgPos[zvtxbin][i]->Fill(sumsinn_trg_pos[i]/npairs_trg_pos[i],npairs_trg_pos[i]);
      hSumSin2TrgPos[zvtxbin][i]->Fill(sumsin2n_trg_pos[i]/npairs_trg_pos[i],npairs_trg_pos[i]);
    }
    if(npairs_trg_neg[i]!=0)
    {
      hSumCosTrgNeg[zvtxbin][i]->Fill(sumcosn_trg_neg[i]/npairs_trg_neg[i],npairs_trg_neg[i]);
      hSumCos2TrgNeg[zvtxbin][i]->Fill(sumcos2n_trg_neg[i]/npairs_trg_neg[i],npairs_trg_neg[i]);
      hSumSinTrgNeg[zvtxbin][i]->Fill(sumsinn_trg_neg[i]/npairs_trg_neg[i],npairs_trg_neg[i]);
      hSumSin2TrgNeg[zvtxbin][i]->Fill(sumsin2n_trg_neg[i]/npairs_trg_neg[i],npairs_trg_neg[i]);
    }
  }
}

void ChargeDepCorrAnalyzerSP2::FillHistsQVectorHF(const DiHadronCorrelationEvent& eventcorr, int zvtxbin)
{
  sumcosn_ass_for=0.0;
  sumsinn_ass_for=0.0;
  sumcos2n_ass_for=0.0;
  sumsin2n_ass_for=0.0;
  npairs_ass_for=0.0;
  sumcosn_ass_bak=0.0;
  sumsinn_ass_bak=0.0;
  sumcos2n_ass_bak=0.0;
  sumsin2n_ass_bak=0.0;
  npairs_ass_bak=0.0;

  unsigned int nasssize = eventcorr.pVect_ass[0].size();
  for(unsigned int nass=0;nass<nasssize;nass++)
  {
    TLorentzVector pvector_ass = (eventcorr.pVect_ass[0])[nass];
    double effweight_ass = (eventcorr.effVect_ass[0])[nass];
    double eta_ass = pvector_ass.Eta()-cutPara.etacms;
    double phi_ass = pvector_ass.Phi();

    if(eta_ass>0)
    {
      sumcosn_ass_for = sumcosn_ass_for + cos(2*phi_ass)/effweight_ass;
      sumsinn_ass_for = sumsinn_ass_for + sin(2*phi_ass)/effweight_ass;
      sumcos2n_ass_for = sumcos2n_ass_for + cos(4*phi_ass)/effweight_ass/effweight_ass;
      sumsin2n_ass_for = sumsin2n_ass_for + sin(4*phi_ass)/effweight_ass/effweight_ass;
      npairs_ass_for += 1.0/effweight_ass;
    }
    if(eta_ass<0)
    {
      sumcosn_ass_bak = sumcosn_ass_bak + cos(2*phi_ass)/effweight_ass;
      sumsinn_ass_bak = sumsinn_ass_bak + sin(2*phi_ass)/effweight_ass;
      sumcos2n_ass_bak = sumcos2n_ass_bak + cos(4*phi_ass)/effweight_ass/effweight_ass;
      sumsin2n_ass_bak = sumsin2n_ass_bak + sin(4*phi_ass)/effweight_ass/effweight_ass;
      npairs_ass_bak += 1.0/effweight_ass;
    }
  } 

  //Applying corrections
  if(npairs_ass_for)
  {
//    sumcosn_ass_for /= npairs_ass_for;
//    sumsinn_ass_for /= npairs_ass_for;
    if(hSumNCosTrgPos[zvtxbin] && hSumNSinTrgPos[zvtxbin] && hSumNSin2TrgPos[zvtxbin] && hSumNCos2TrgPos[zvtxbin]) 
    {
      sumcosn_ass_for -= npairs_ass_for * hSumNCosTrgPos[zvtxbin]->GetBinContent(1,2);
      sumsinn_ass_for -= npairs_ass_for * hSumNSinTrgPos[zvtxbin]->GetBinContent(1,2);

      double lambda_p = - hSumNSin2TrgPos[zvtxbin]->GetBinContent(1,2)/(1+hSumNCos2TrgPos[zvtxbin]->GetBinContent(1,2));
      double lambda_n = hSumNSin2TrgPos[zvtxbin]->GetBinContent(1,2)/(1-hSumNCos2TrgPos[zvtxbin]->GetBinContent(1,2));
      sumcosn_ass_for = (sumcosn_ass_for-lambda_n*sumsinn_ass_for)/(1-lambda_p*lambda_n);
      sumsinn_ass_for = (sumsinn_ass_for-lambda_p*sumcosn_ass_for)/(1-lambda_p*lambda_n);

      sumcosn_ass_for /= 1+hSumNCos2TrgPos[zvtxbin]->GetBinContent(1,2);
      sumsinn_ass_for /= 1-hSumNCos2TrgPos[zvtxbin]->GetBinContent(1,2);
    }
  }

  if(npairs_ass_bak) 
  {
//    sumcosn_ass_bak /= npairs_ass_bak;
//    sumsinn_ass_bak /= npairs_ass_bak;
    if(hSumNCosTrgPos[zvtxbin] && hSumNSinTrgPos[zvtxbin] && hSumNSin2TrgPos[zvtxbin] && hSumNCos2TrgPos[zvtxbin]) 
    {
      sumcosn_ass_bak -= npairs_ass_bak * hSumNCosTrgPos[zvtxbin]->GetBinContent(MAXETATRGBINSPT+2,2);
      sumsinn_ass_bak -= npairs_ass_bak * hSumNSinTrgPos[zvtxbin]->GetBinContent(MAXETATRGBINSPT+2,2);

      double lambda_p = - hSumNSin2TrgPos[zvtxbin]->GetBinContent(MAXETATRGBINSPT+2,2)/(1+hSumNCos2TrgPos[zvtxbin]->GetBinContent(MAXETATRGBINSPT+2,2));
      double lambda_n = hSumNSin2TrgPos[zvtxbin]->GetBinContent(MAXETATRGBINSPT+2,2)/(1-hSumNCos2TrgPos[zvtxbin]->GetBinContent(MAXETATRGBINSPT+2,2));
      sumcosn_ass_bak = (sumcosn_ass_bak-lambda_n*sumsinn_ass_bak)/(1-lambda_p*lambda_n);
      sumsinn_ass_bak = (sumsinn_ass_bak-lambda_p*sumcosn_ass_bak)/(1-lambda_p*lambda_n);

      sumcosn_ass_bak /= 1+hSumNCos2TrgPos[zvtxbin]->GetBinContent(MAXETATRGBINSPT+2,2);
      sumsinn_ass_bak /= 1-hSumNCos2TrgPos[zvtxbin]->GetBinContent(MAXETATRGBINSPT+2,2);
    }
  }
}

void ChargeDepCorrAnalyzerSP2::FillHistsQVectorTrk(const DiHadronCorrelationEvent& eventcorr, int zvtxbin)
{
  for(int i=0;i<MAXETATRGBINSPT;i++)
  {
    sumcosn_trg_pos[i]=0.0;
    sumsinn_trg_pos[i]=0.0;
    sumcos2n_trg_pos[i]=0.0;
    sumsin2n_trg_pos[i]=0.0;
    npairs_trg_pos[i]=0.0;
    sumcosn_trg_neg[i]=0.0;
    sumsinn_trg_neg[i]=0.0;
    sumcos2n_trg_neg[i]=0.0;
    sumsin2n_trg_neg[i]=0.0;
    npairs_trg_neg[i]=0.0;
  }
/*
  sumcosn_trg_pos.clear();
  sumsinn_trg_pos.clear();
  sumcos2n_trg_pos.clear();
  sumsin2n_trg_pos.clear();
  npairs_trg_pos.clear();
  sumcosn_trg_neg.clear();
  sumsinn_trg_neg.clear();
  sumcos2n_trg_neg.clear();
  sumsin2n_trg_neg.clear();
  npairs_trg_neg.clear();
*/  
  unsigned int ntrgsize = eventcorr.pVect_trg[0].size();
  for(unsigned int ntrg=0;ntrg<ntrgsize;ntrg++)
  {
    TLorentzVector pvector_trg = (eventcorr.pVect_trg[0])[ntrg];
    double effweight_trg = (eventcorr.effVect_trg[0])[ntrg];
    double charge_trg = (eventcorr.chgVect_trg[0])[ntrg];
    double eta_trg = pvector_trg.Eta()-cutPara.etacms;
    double phi_trg = pvector_trg.Phi();

    int ietabin = (int)((eta_trg+2.4)/ETATRGBINWIDTHPT);

    if(charge_trg>0)
    {
      hdNdEtadPhiPos->Fill(eta_trg,phi_trg,1.0/effweight_trg);
      sumcosn_trg_pos[ietabin] += cos(phi_trg)/effweight_trg;
      sumsinn_trg_pos[ietabin] += sin(phi_trg)/effweight_trg;
      sumcos2n_trg_pos[ietabin] += cos(2*phi_trg)/effweight_trg/effweight_trg;
      sumsin2n_trg_pos[ietabin] += sin(2*phi_trg)/effweight_trg/effweight_trg;
      npairs_trg_pos[ietabin] += 1.0/effweight_trg;
    }
    if(charge_trg<0)
    {
      hdNdEtadPhiNeg->Fill(eta_trg,phi_trg,1.0/effweight_trg);
      sumcosn_trg_neg[ietabin] += cos(phi_trg)/effweight_trg;
      sumsinn_trg_neg[ietabin] += sin(phi_trg)/effweight_trg;
      sumcos2n_trg_neg[ietabin] += cos(2*phi_trg)/effweight_trg/effweight_trg;
      sumsin2n_trg_neg[ietabin] += sin(2*phi_trg)/effweight_trg/effweight_trg;
      npairs_trg_neg[ietabin] += 1.0/effweight_trg;
    }
  }

  // Applying corrections
  for(int i=0;i<MAXETATRGBINSPT;i++)
  {
    if(npairs_trg_pos[i]!=0 && hSumNCosTrgPos[zvtxbin] && hSumNSinTrgPos[zvtxbin] && hSumNSin2TrgPos[zvtxbin] && hSumNCos2TrgPos[zvtxbin])
    {
//cout<<"applying correction "<<i<<" "<<npairs_trg_pos[i]<<endl;
      // recenter
//if(i==2 && npairs_trg_pos[i]==6) cout<<"before recentering:"<<sumcosn_trg_pos[i]<<endl;
      sumcosn_trg_pos[i] -= npairs_trg_pos[i]*hSumNCosTrgPos[zvtxbin]->GetBinContent(i+2,hSumNCosTrgPos[zvtxbin]->GetYaxis()->FindBin(npairs_trg_pos[i]+0.001));
//if(i==2 && npairs_trg_pos[i]==6) cout<<"after recentering:"<<sumcosn_trg_pos[i]<<endl;
      sumsinn_trg_pos[i] -= npairs_trg_pos[i]*hSumNSinTrgPos[zvtxbin]->GetBinContent(i+2,hSumNSinTrgPos[zvtxbin]->GetYaxis()->FindBin(npairs_trg_pos[i]+0.001));
      sumcos2n_trg_pos[i] -= npairs_trg_pos[i]*hSumNCos2TrgPos[zvtxbin]->GetBinContent(i+2,hSumNCos2TrgPos[zvtxbin]->GetYaxis()->FindBin(npairs_trg_pos[i]+0.001));
      sumsin2n_trg_pos[i] -= npairs_trg_pos[i]*hSumNSin2TrgPos[zvtxbin]->GetBinContent(i+2,hSumNSin2TrgPos[zvtxbin]->GetYaxis()->FindBin(npairs_trg_pos[i]+0.001));

      // twist
      double lambda_p = - hSumNSin2TrgPos[zvtxbin]->GetBinContent(i+2,hSumNSin2TrgPos[zvtxbin]->GetYaxis()->FindBin(npairs_trg_pos[i]+0.001))/(1+hSumNCos2TrgPos[zvtxbin]->GetBinContent(i+2,hSumNCos2TrgPos[zvtxbin]->GetYaxis()->FindBin(npairs_trg_pos[i]+0.001)));  
      double lambda_n = hSumNSin2TrgPos[zvtxbin]->GetBinContent(i+2,hSumNSin2TrgPos[zvtxbin]->GetYaxis()->FindBin(npairs_trg_pos[i]+0.001))/(1-hSumNCos2TrgPos[zvtxbin]->GetBinContent(i+2,hSumNCos2TrgPos[zvtxbin]->GetYaxis()->FindBin(npairs_trg_pos[i]+0.001)));

      sumcosn_trg_pos[i] = (sumcosn_trg_pos[i]-lambda_n*sumsinn_trg_pos[i])/(1-lambda_p*lambda_n);
      sumsinn_trg_pos[i] = (sumsinn_trg_pos[i]-lambda_p*sumcosn_trg_pos[i])/(1-lambda_p*lambda_n);

      // rescale
      sumcosn_trg_pos[i] /= 1+hSumNCos2TrgPos[zvtxbin]->GetBinContent(i+2,hSumNCos2TrgPos[zvtxbin]->GetYaxis()->FindBin(npairs_trg_pos[i]+0.001));
      sumsinn_trg_pos[i] /= 1-hSumNCos2TrgPos[zvtxbin]->GetBinContent(i+2,hSumNCos2TrgPos[zvtxbin]->GetYaxis()->FindBin(npairs_trg_pos[i]+0.001));
/*
      hSumCosTrgPos[i]->Fill(sumcosn_trg_pos[i]/npairs_trg_pos[i],npairs_trg_pos[i]);
      hSumSinTrgPos[i]->Fill(sumsinn_trg_pos[i]/npairs_trg_pos[i],npairs_trg_pos[i]);
      hSumCos2TrgPos[i]->Fill(sumcos2n_trg_pos[i]/npairs_trg_pos[i],npairs_trg_pos[i]);
      hSumSin2TrgPos[i]->Fill(sumsin2n_trg_pos[i]/npairs_trg_pos[i],npairs_trg_pos[i]);
*/
    }

    if(npairs_trg_neg[i]!=0 && hSumNCosTrgNeg[zvtxbin] && hSumNSinTrgNeg[zvtxbin] && hSumNSin2TrgNeg[zvtxbin] && hSumNCos2TrgNeg[zvtxbin]) 
    {
      sumcosn_trg_neg[i] -= npairs_trg_neg[i]*hSumNCosTrgNeg[zvtxbin]->GetBinContent(i+2,hSumNCosTrgNeg[zvtxbin]->GetYaxis()->FindBin(npairs_trg_neg[i]+0.001));
      sumsinn_trg_neg[i] -= npairs_trg_neg[i]*hSumNSinTrgNeg[zvtxbin]->GetBinContent(i+2,hSumNSinTrgNeg[zvtxbin]->GetYaxis()->FindBin(npairs_trg_neg[i]+0.001));
      sumcos2n_trg_neg[i] -= npairs_trg_neg[i]*hSumNCos2TrgNeg[zvtxbin]->GetBinContent(i+2,hSumNCos2TrgNeg[zvtxbin]->GetYaxis()->FindBin(npairs_trg_neg[i]+0.001));
      sumsin2n_trg_neg[i] -= npairs_trg_neg[i]*hSumNSin2TrgNeg[zvtxbin]->GetBinContent(i+2,hSumNSin2TrgNeg[zvtxbin]->GetYaxis()->FindBin(npairs_trg_neg[i]+0.001));

      double lambda_p = - hSumNSin2TrgNeg[zvtxbin]->GetBinContent(i+2,hSumNSin2TrgNeg[zvtxbin]->GetYaxis()->FindBin(npairs_trg_neg[i]+0.001))/(1+hSumNCos2TrgNeg[zvtxbin]->GetBinContent(i+2,hSumNCos2TrgNeg[zvtxbin]->GetYaxis()->FindBin(npairs_trg_neg[i]+0.001)));
      double lambda_n = hSumNSin2TrgNeg[zvtxbin]->GetBinContent(i+2,hSumNSin2TrgNeg[zvtxbin]->GetYaxis()->FindBin(npairs_trg_neg[i]+0.001))/(1-hSumNCos2TrgNeg[zvtxbin]->GetBinContent(i+2,hSumNCos2TrgNeg[zvtxbin]->GetYaxis()->FindBin(npairs_trg_neg[i]+0.001)));
      sumcosn_trg_neg[i] = (sumcosn_trg_neg[i]-lambda_n*sumsinn_trg_neg[i])/(1-lambda_p*lambda_n);
      sumsinn_trg_neg[i] = (sumsinn_trg_neg[i]-lambda_p*sumcosn_trg_neg[i])/(1-lambda_p*lambda_n);

      sumcosn_trg_neg[i] /= 1+hSumNCos2TrgNeg[zvtxbin]->GetBinContent(i+2,hSumNCos2TrgNeg[zvtxbin]->GetYaxis()->FindBin(npairs_trg_neg[i]+0.001));
      sumsinn_trg_neg[i] /= 1-hSumNCos2TrgNeg[zvtxbin]->GetBinContent(i+2,hSumNCos2TrgNeg[zvtxbin]->GetYaxis()->FindBin(npairs_trg_neg[i]+0.001));
/*
      hSumCosTrgNeg[i]->Fill(sumcosn_trg_neg[i]/npairs_trg_neg[i],npairs_trg_neg[i]);
      hSumSinTrgNeg[i]->Fill(sumsinn_trg_neg[i]/npairs_trg_neg[i],npairs_trg_neg[i]);
      hSumCos2TrgNeg[i]->Fill(sumcos2n_trg_neg[i]/npairs_trg_neg[i],npairs_trg_neg[i]);
      hSumSin2TrgNeg[i]->Fill(sumsin2n_trg_neg[i]/npairs_trg_neg[i],npairs_trg_neg[i]);
*/
    }
  }
}

void ChargeDepCorrAnalyzerSP2::FillHistsCorrelators(int i, int j)
{
  if(npairs_trg_pos[i]!=0.0 && npairs_trg_pos[j]!=0.0)
  {
    double Qabx_pp = (sumcosn_trg_pos[i]*sumcosn_trg_pos[j]-sumsinn_trg_pos[i]*sumsinn_trg_pos[j]);
    double Qaby_pp = (sumcosn_trg_pos[i]*sumsinn_trg_pos[j]+sumsinn_trg_pos[i]*sumcosn_trg_pos[j]);
    double Qabxx_pp = (sumcosn_trg_pos[i]*sumcosn_trg_pos[j]+sumsinn_trg_pos[i]*sumsinn_trg_pos[j]);
    double nabpairs_pp = npairs_trg_pos[i]*npairs_trg_pos[j];
    if(j==i)
    {
      Qabx_pp -= sumcos2n_trg_pos[i];
      Qaby_pp -= sumsin2n_trg_pos[i];
      Qabxx_pp -= npairs_trg_pos[i];
      nabpairs_pp -= npairs_trg_pos[i];
    }
/*
    Qabx_pp = Qabx_pp / nabpairs_pp;
    Qaby_pp = Qaby_pp / nabpairs_pp;
    Qabxx_pp = Qabxx_pp / nabpairs_pp;
*/
    double Qabc_pp_for = Qabx_pp*sumcosn_ass_for+Qaby_pp*sumsinn_ass_for;
    double Qabc_pp_bak = Qabx_pp*sumcosn_ass_bak+Qaby_pp*sumsinn_ass_bak;

    hSignalCos_pp_for[i][j]->Fill(Qabc_pp_for);
    hSignalCos_pp_bak[i][j]->Fill(Qabc_pp_bak);
    hSignalCos2PcorrP_pp[i][j]->Fill(Qabx_pp);
    hSignalCos2PcorrM_pp[i][j]->Fill(Qabxx_pp);
    npairs_pp_total[i][j] += nabpairs_pp;
  }

  if(npairs_trg_neg[i]!=0.0 && npairs_trg_neg[j]!=0.0)
  {
    double Qabx_nn = (sumcosn_trg_neg[i]*sumcosn_trg_neg[j]-sumsinn_trg_neg[i]*sumsinn_trg_neg[j]);
    double Qaby_nn = (sumcosn_trg_neg[i]*sumsinn_trg_neg[j]+sumsinn_trg_neg[i]*sumcosn_trg_neg[j]);
    double Qabxx_nn = (sumcosn_trg_neg[i]*sumcosn_trg_neg[j]+sumsinn_trg_neg[i]*sumsinn_trg_neg[j]);
    double nabpairs_nn = npairs_trg_neg[i]*npairs_trg_neg[j];
    if(j==i)
    {
      Qabx_nn -= sumcos2n_trg_neg[i];
      Qaby_nn -= sumsin2n_trg_neg[i];
      Qabxx_nn -= npairs_trg_neg[i];
      nabpairs_nn -= npairs_trg_neg[i];
    }
/*
    Qabx_nn = Qabx_nn / nabpairs_nn;
    Qaby_nn = Qaby_nn / nabpairs_nn;
    Qabxx_nn = Qabxx_nn / nabpairs_nn;
*/
    double Qabc_nn_for = Qabx_nn*sumcosn_ass_for+Qaby_nn*sumsinn_ass_for;
    double Qabc_nn_bak = Qabx_nn*sumcosn_ass_bak+Qaby_nn*sumsinn_ass_bak;
    
    hSignalCos_nn_for[i][j]->Fill(Qabc_nn_for);
    hSignalCos_nn_bak[i][j]->Fill(Qabc_nn_bak);
    hSignalCos2PcorrP_nn[i][j]->Fill(Qabx_nn);
    hSignalCos2PcorrM_nn[i][j]->Fill(Qabxx_nn);
    npairs_nn_total[i][j] += nabpairs_nn;
  }

  if(npairs_trg_pos[i]!=0.0 && npairs_trg_neg[j]!=0.0)
  {
    double Qabx_pn = (sumcosn_trg_pos[i]*sumcosn_trg_neg[j]-sumsinn_trg_pos[i]*sumsinn_trg_neg[j]);
    double Qaby_pn = (sumcosn_trg_pos[i]*sumsinn_trg_neg[j]+sumsinn_trg_pos[i]*sumcosn_trg_neg[j]);
    double Qabxx_pn = (sumcosn_trg_pos[i]*sumcosn_trg_neg[j]+sumsinn_trg_pos[i]*sumsinn_trg_neg[j]);
    double nabpairs_pn = npairs_trg_pos[i]*npairs_trg_neg[j];

/*
    Qabx_pn = Qabx_pn / nabpairs_pn;
    Qaby_pn = Qaby_pn / nabpairs_pn;
    Qabxx_pn = Qabxx_pn / nabpairs_pn;
*/
    double Qabc_pn_for = Qabx_pn*sumcosn_ass_for+Qaby_pn*sumsinn_ass_for;
    double Qabc_pn_bak = Qabx_pn*sumcosn_ass_bak+Qaby_pn*sumsinn_ass_bak;

    hSignalCos_pn_for[i][j]->Fill(Qabc_pn_for);
    hSignalCos_pn_bak[i][j]->Fill(Qabc_pn_bak);
    hSignalCos2PcorrP_pn[i][j]->Fill(Qabx_pn);
    hSignalCos2PcorrM_pn[i][j]->Fill(Qabxx_pn);
    npairs_pn_total[i][j] += nabpairs_pn;
  }

  if(npairs_trg_neg[i]!=0.0 && npairs_trg_pos[j]!=0.0)
  {
    double Qabx_np = (sumcosn_trg_neg[i]*sumcosn_trg_pos[j]-sumsinn_trg_neg[i]*sumsinn_trg_pos[j]);
    double Qaby_np = (sumcosn_trg_neg[i]*sumsinn_trg_pos[j]+sumsinn_trg_neg[i]*sumcosn_trg_pos[j]);
    double Qabxx_np = (sumcosn_trg_neg[i]*sumcosn_trg_pos[j]+sumsinn_trg_neg[i]*sumsinn_trg_pos[j]);
    double nabpairs_np = npairs_trg_neg[i]*npairs_trg_pos[j];
/*
    Qabx_np = Qabx_np / nabpairs_np;
    Qaby_np = Qaby_np / nabpairs_np;
    Qabxx_np = Qabxx_np / nabpairs_np;
*/
    double Qabc_np_for = Qabx_np*sumcosn_ass_for+Qaby_np*sumsinn_ass_for;
    double Qabc_np_bak = Qabx_np*sumcosn_ass_bak+Qaby_np*sumsinn_ass_bak;

    hSignalCos_np_for[i][j]->Fill(Qabc_np_for);
    hSignalCos_np_bak[i][j]->Fill(Qabc_np_bak);
    hSignalCos2PcorrP_np[i][j]->Fill(Qabx_np);
    hSignalCos2PcorrM_np[i][j]->Fill(Qabxx_np);
    npairs_np_total[i][j] += nabpairs_np;
  }
}

int ChargeDepCorrAnalyzerSP2::GetNMultBin(int nmult)
{
  for(int i=0;i<(int)(multBins.size()-1);i++)
  {
    if(nmult >= multBins[i] && nmult < multBins[i+1]) return i;
  }
  return 0;
}
