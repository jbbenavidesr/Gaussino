###########################################################################################################
#
#  Tool to extract histograms from rootfiles produced by the EMGaussMoni and BremVeloCheck. The histograms
#  are massaged in order to be used in LHCbPR. RootFiles that hold the full infromation are deleted. Only
#  rootFiles that are about to be used by LHCbPR are stored.
#
#  Georgios Chatzikonstantinidis 12/05/2017 
#  email:georgios.chatzikonstantinidis@cern.ch
#
###########################################################################################################

import sys,os
sys.path.append(os.environ["SIMCHECKSROOT"]+"/scripts/EmValidation_LHCbPR/")
from os import remove
from glob import glob
from ROOT import TFile, TTree, TH1F, gDirectory, gROOT
from configurations import config

files=glob(os.environ["SIMCHECKSROOT"]+"/scripts/EmValidation_LHCbPR/*.root")
histos=[]



for file in files:
    rootOld = TFile(file,"READ")
    # massage rootfiles for the bream test. Output root files will be feed to LHCbPR. Original rootfiles
    # will be deleted.
    if(config()['testType']=='bream' or config()['testType']=='both'):
        rootOld.cd("photon energy")
        histos.append(gROOT.FindObject("1").Clone("photon_energy"))
        histos[0].SetDirectory(0)
        histos[0].GetXaxis().SetTitle("Photon energy [eV/c^{2}]")
        histos[0].GetYaxis().SetTitle("Number of photons")
        histos[0].SetTitle("")
        rootOld.cd()

        rootOld.cd("number of photons per track")        
        histos.append(gROOT.FindObject("1").Clone("number_of_photons_per_track"))
        histos[1].SetDirectory(0)
        histos[1].GetXaxis().SetTitle("Number of photons")
        histos[1].GetYaxis().SetTitle("Number of Geant4 tracks")
        histos[1].SetTitle("")
        rootOld.cd()

    # massage rootfiles for the dedx test. Output root files will be fed to LHCbPR. Original rootfiles
    # will be deleted.
    if(config()['testType']=='dedx' or config()['testType']=='both'):
        rootOld.cd("EMGaussMoniVeloGaussMoni")
        histos.append(gROOT.FindObject("1002").Clone("energy_deposit"))
        histos[2].SetDirectory(0)
        histos[2].GetXaxis().SetTitle("Energy deposit in thin layer of SI, [eV/c^{2}]")
        histos[2].GetYaxis().SetTitle("Particle hits")
        histos[2].SetTitle("")
        rootOld.cd()
        
    rootOld.Close()
   
    #save the histograms in the rootfile that will be feed to LHCbPR
    rootNew = TFile(file[:-5]+"_LHCbPR.root","RECREATE")
    for hist in histos:hist.Write()
    os.remove(file)
    del histos[:]



