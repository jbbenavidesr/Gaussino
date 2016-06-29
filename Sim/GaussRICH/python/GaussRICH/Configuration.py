"""
Configuration tools for GaussRICH

"""

from Gaudi.Configuration import *
from Gauss.Configuration import *
from Configurables import Gauss

import GaudiKernel.ProcessJobOptions
from GaudiKernel import SystemOfUnits


from Configurables import LHCbConfigurableUser, LHCbApp


from Configurables import ( GiGa )
from Configurables import (GiGaPhysConstructorOp, GiGaPhysConstructorHpd)

 

class GaussRICHConf (LHCbConfigurableUser):
    
    #steering options
    
    __slots__= {
         "RichDataYear"                        : "2012"
        ,"ActivateRichProcess"                 : True
        ,"ActivateRichScint"                   : True
        ,"ActivateAerogel"                     : True
        ,"RichExtendedInfo"                    : False
        ,"RichExtendedinfoClasses"             : True
        ,"ActivateStoreHpdReflStepInfo"        : False
        ,"ActivateTrackActionFlags"            : True
        ,"ActivateTrackActionRich2Debug"       : False   
        ,"RichFullAnalysis"                    : False
        ,"RichActivateRecon"                   : False
        ,"RichExtraFullAnalysis"               : False
        ,"RichEvActionVerboseLevel"            : 0
        ,"ActivateRichEfficiencyConfig"        : True
        ,"ActivateRichSmearingConfig"          : True
        ,"MakeRichG4Ntuple"                    : False
        ,"MakeRichG4MonitorHistoSet1"          : False
        ,"RichG4MonitorHistoSet1Type"          : 0
        ,"MakeRichG4MonitorHistoSet2"          : True
        ,"MakeRichG4MonitorHistoSet3"          : False
        ,"MakeRichG4MonitorHistoSet4"          : False
        ,"MakeRichG4MonitorHistoSet5"          : False
        ,"ActivateStepAnalysisCounters"        : False
        ,"ActivateRich2PhotSuppressSimDebug"   : False
        ,"RichBinaryReadoutSetup"              : 0
        ,"Run1Run2RichDetectorExist"           : True
        ,"SkipGeant4RichFlag"                  : False  
        }


        

    def InitializeGaussRICHConfiguration(self):
        from Configurables import Gauss
        Gauss = Gauss()
        print "Gauss run option selected for RICH  =  ", Gauss.getProp("CurrentRICHSimRunOption")
        

        if ((Gauss.getProp("CurrentRICHSimRunOption")) == 'Formula1'):
            #run configuration for very fast runs
            self.setProp("RichExtendedinfoClasses",False)
            self.setProp("MakeRichG4MonitorHistoSet2",False)
            
        elif ((Gauss.getProp("CurrentRICHSimRunOption")) == 'GTB' ):
            #standard run configuration for fast productions. This is the default configuration
            self.setProp("RichExtendedinfoClasses",True)
            self.setProp("MakeRichG4MonitorHistoSet2",True)
            
        elif ((Gauss.getProp("CurrentRICHSimRunOption")) == 'SUV'):
            #Run configuration for RICH detector related studies using RICH reconstruction in Brunel
            self.setProp("RichExtendedinfoClasses",True)
            self.setProp("RichExtendedInfo",True)
            self.setProp("ActivateStoreHpdReflStepInfo",True)
            self.setProp("ActivateStepAnalysisCounters",True)

        elif ((Gauss.getProp("CurrentRICHSimRunOption")) == 'HGV'):
            #Run configuration for detailed Simulation studies using Gauss
            self.setProp("RichFullAnalysis",True)
            self.setProp("RichActivateRecon",True)
            self.setProp("MakeRichG4MonitorHistoSet4",True)
            self.setProp("MakeRichG4MonitorHistoSet5",True)
            self.setProp("RichExtendedinfoClasses",True)
            self.setProp("RichExtendedInfo",True)
            self.setProp("ActivateStoreHpdReflStepInfo",True)
            self.setProp("ActivateStepAnalysisCounters",True)
            
        elif ((Gauss.getProp("CurrentRICHSimRunOption")) == "FareFiasco"):
            #Run configuration with RICH simulation switched off
            self.setProp("ActivateRichProcess",False)
            self.setProp("ActivateAerogel",False)
                        

         
        self.RichDataYearConfig()
        print "RICH simulation for the data in year = ", self.getProp("RichDataYear")
        
         

    def setRichDetectorExistFlag(self, Run1Run2RichDetector=True):
        self.setProp("Run1Run2RichDetectorExist",Run1Run2RichDetector)
        
        
    def setSkipGeant4RichFlag(self, SkipGeant4=False):
        self.setProp("SkipGeant4RichFlag",SkipGeant4)
    
    def ApplyGaussRICHConfiguration(self,GiGa):
        #Actual implementation of current RICH simulation configuration

                 
        if ( not (self.getProp("Run1Run2RichDetectorExist"))) :
            self.setProp("ActivateRichProcess",False)
            self.setProp("ActivateAerogel",False)

            
        if ( self.getProp("SkipGeant4RichFlag")) :
            self.setProp("ActivateRichProcess",False)
            self.setProp("ActivateAerogel",False)
            
        # Aerogel configuration
        self.RichAerogelConfig(GiGa)
            
        # RICH Processes
        self.RichProcessActivate(GiGa)
        print "RICH simulation now configured "        
        
    def RichDataYearConfig(self):
        from Configurables import Gauss
        Gauss=Gauss()
        if Gauss.getProp("DataType") in  Gauss.getProp("Run1DataTypes"):
            self.setProp("RichDataYear", Gauss.getProp("DataType") )
            
        if Gauss.getProp("DataType") in  Gauss.getProp("Run2DataTypes"):
            self.setProp("RichDataYear", Gauss.getProp("DataType") )



        
    def RichProcessActivate(self,GiGa):
        
      GiGa.ModularPL.addTool( GiGaPhysConstructorOp,name="GiGaPhysConstructorOp" )
      GiGa.ModularPL.addTool( GiGaPhysConstructorHpd,name="GiGaPhysConstructorHpd" )
      
      if (self.getProp("ActivateRichProcess")):
          
          GiGa.ModularPL.GiGaPhysConstructorOp.RichOpticalPhysicsProcessActivate=True
          GiGa.ModularPL.GiGaPhysConstructorHpd.RichHpdPhysicsProcessActivate=True
            
          self.RichGeneralPhysProcessOptions(GiGa)
          self.RichScintillationSetup(GiGa)        
            
          self.RichRandomHitsSetup(GiGa)


          if (self.getProp("RichExtendedinfoClasses")):
              self.RichExtendedInfoClassesSetup()
              
          if (self.getProp("RichExtendedInfo")):
              self.RichExtendedInfoSetup(GiGa)
               
               
          self.RichGeneralStepActionSetup(GiGa)
          self.RichGeneralTrackActionSetup(GiGa)

          self.RichAnalysisSetup(GiGa)

          if (self.getProp("ActivateRichSmearingConfig")):
              self.RichSmearingConfig(GiGa)

          if  (self.getProp("RichBinaryReadoutSetup") > 0) :
              self.RichBinaryReadOutConfig(GiGa)

              
      else:
          
          GiGa.ModularPL.GiGaPhysConstructorOp.RichOpticalPhysicsProcessActivate=False
          GiGa.ModularPL.GiGaPhysConstructorHpd.RichHpdPhysicsProcessActivate=False
            
          
    def RichScintillationSetup(self,GiGa):
        if (self.getProp("ActivateRichScint")):
            GiGa.ModularPL.GiGaPhysConstructorOp.RichActivateCF4Scintillation=True
            GiGa.ModularPL.GiGaPhysConstructorOp.RichApplyScintillationYieldScaleFactor=True
            # In 2011 there was too much CO2 and 2012 the CO2 was reduced. Hence
            # the scintillation in 2011 was 0.1 of normal scintillation.
            # and the scintillation in 2012 was 0.6 of normal scinitillation
            # In 2015 onwards the scinitillation is expected to a bit reduced wrt to that in 2012.
            # But the precise values are not known. So kept default as that in 2012 for now.
            if  self.getProp("RichDataYear") == "2011" :
                GiGa.ModularPL.GiGaPhysConstructorOp.RichScintillationYieldScaleFactor=0.1
            elif self.getProp("RichDataYear") == "2012":
                GiGa.ModularPL.GiGaPhysConstructorOp.RichScintillationYieldScaleFactor=0.6
            elif self.getProp("RichDataYear") == "2013":
                GiGa.ModularPL.GiGaPhysConstructorOp.RichScintillationYieldScaleFactor=0.6
            elif  self.getProp("RichDataYear") == "2015":  
                GiGa.ModularPL.GiGaPhysConstructorOp.RichScintillationYieldScaleFactor=0.6
            elif  self.getProp("RichDataYear") == "2016":  
                GiGa.ModularPL.GiGaPhysConstructorOp.RichScintillationYieldScaleFactor=0.6
            elif  self.getProp("RichDataYear") == "2010":
                GiGa.ModularPL.GiGaPhysConstructorOp.RichScintillationYieldScaleFactor=0.7
            elif  self.getProp("RichDataYear")  == "2009":
                GiGa.ModularPL.GiGaPhysConstructorOp.RichScintillationYieldScaleFactor=0.7
            else:
                GiGa.ModularPL.GiGaPhysConstructorOp.RichScintillationYieldScaleFactor=0.5
        else:
            GiGa.ModularPL.GiGaPhysConstructorOp.RichActivateCF4Scintillation=False

    def RichAerogelConfig(self,GiGa):

        if (self.getProp("RichDataYear") in ( "2015", "2016", "2017", "2018", "2019", "2020", "2021", "2022")):
            self.setProp("ActivateAerogel",False)
            
        from Configurables import Gauss    
        Gauss=Gauss()
        
        if (self.getProp("RichDataYear") in Gauss.getProp("Run2DataTypes")):
            self.setProp("ActivateAerogel",False)
           
        #print "Current rich data year : ",  self.getProp("RichDataYear")
        from Configurables import SimulationSvc

        if (self.getProp("ActivateAerogel")):
            GiGa.TrackSeq.Members      +=  ["Rich1G4TrackActionUpstrPhoton/Rich1UpstrPhotonTrack" ]
            GiGa.TrackSeq.Members      +=  ["RichG4TrackActionAerogelPhoton/RichG4AgelPhot" ]
            SimulationSvc().SimulationDbLocation = "$GAUSSROOT/xml/Simulation.xml"
            #print "now added Rich1G4TrackActionUpstrPhoton " ,str(GiGa.TrackSeq.Members).translate(None)
            #print "Aerogel activated for this run "
        else:
            if (self.getProp("Run1Run2RichDetectorExist")) :
                importOptions("$APPCONFIGOPTS/Conditions/RichAerogelRemoveInDB.py")

            SimulationSvc().SimulationDbLocation = "$GAUSSROOT/xml/SimulationRICHAerogelOff.xml"
            
            if( "Rich1G4TrackActionUpstrPhoton/Rich1UpstrPhotonTrack"  in GiGa.TrackSeq.Members ):
                GiGa.TrackSeq.Members.remove( "Rich1G4TrackActionUpstrPhoton/Rich1UpstrPhotonTrack" )
                
            if ( "RichG4TrackActionAerogelPhoton/RichG4AgelPhot"  in  GiGa.TrackSeq.Members ):
                GiGa.TrackSeq.Members.remove("RichG4TrackActionAerogelPhoton/RichG4AgelPhot")
                            
            #print "Aerogel de-activated for this run "

    def RichGeneralPhysProcessOptions(self,GiGa):
        
        # When RichApplyMaxNumCkvPhotPerStepPerRadiator is False, the global value
        # as given by RichMaxNumPhotPerCherenkovStep is used for all radiators.
        # Otherwise the individual value listed below for each radiator is used. 
        GiGa.ModularPL.GiGaPhysConstructorOp.RichApplyMaxNumCkvPhotPerStepPerRadiator=False
        GiGa.ModularPL.GiGaPhysConstructorOp.RichMaxPhotonsPerCherenkovStepInRich1Agel=900
        GiGa.ModularPL.GiGaPhysConstructorOp.RichMaxPhotonsPerCherenkovStepInRich1Gas=40
        GiGa.ModularPL.GiGaPhysConstructorOp.RichMaxPhotonsPerCherenkovStepInRich2Gas=40
        GiGa.ModularPL.GiGaPhysConstructorOp.RichMaxNumPhotPerCherenkovStep=40
 
            
    def RichGeneralStepActionSetup(self,GiGa):
        
        if (self.getProp("ActivateStoreHpdReflStepInfo")):
            GiGa.StepSeq.Members  += ["RichG4StepAnalysis6/RichStepHpdRefl" ] 

        if (self.getProp("ActivateStepAnalysisCounters")) :
            GiGa.StepSeq.Members  += ["RichG4StepAnalysis3/RichStep"]
            
 


    def RichGeneralTrackActionSetup(self,GiGa):
        #print "Now in track action setup "
        if  (self.getProp("ActivateTrackActionFlags")):
            from Configurables import RichG4TrackActionPhotOpt,RichG4TrackActionRich2Photon 
            GiGa.TrackSeq.addTool(RichG4TrackActionPhotOpt , name="RichG4PhotOpt" )
            if (self.getProp("ActivateRichEfficiencyConfig")):
                self.RichEfficiencyConfig(GiGa)
            
            GiGa.TrackSeq.addTool(RichG4TrackActionRich2Photon, name="Rich2ScintPhotonSup")
            if (self.getProp("ActivateTrackActionRich2Debug")):
                GiGa.TrackSeq.Rich2ScintPhotonSup.RichG4TrackActionRich2PhotonDebugMode=1
                
            GiGa.TrackSeq.Members      += ["RichG4TrackActionPhotOpt/RichG4PhotOpt" ]
            GiGa.TrackSeq.Members      += ["RichG4TrackActionRich2Photon/Rich2ScintPhotonSup" ]
            #print "now added RichG4PhotOpt " ,str(GiGa.TrackSeq.Members).translate(None)
        if  (self.getProp("ActivateRich2PhotSuppressSimDebug")):
            from Configurables import RichG4TrackActionRich2DbgPhotonTrack            
            GiGa.TrackSeq.addTool (RichG4TrackActionRich2DbgPhotonTrack, name="Rich2DbgSuppressPhoton")
            GiGa.TrackSeq.Rich2DbgSuppressPhoton.DeActiavteRich2PhotonTrackForDebug=True             
            GiGa.TrackSeq.Members      += ["RichG4TrackActionRich2DbgPhotonTrack/Rich2DbgSuppressPhoton"]
            
    def RichRandomHitsSetup(self, GiGa):
        
        GiGa.ModularPL.GiGaPhysConstructorOp.Rich2BackgrHitsActivate = False
        from Configurables import Gauss
        Gauss = Gauss()
        if  (Gauss.getProp("RICHRandomHits")) :
            GiGa.ModularPL.GiGaPhysConstructorOp.Rich2BackgrHitsActivate = True
            GiGa.ModularPL.GiGaPhysConstructorOp.Rich2BackgrHitsProbabilityFactor =0.6

    def RichEfficiencyConfig(self,GiGa):
        GiGa.TrackSeq.RichG4PhotOpt.RichHpdReadOutEffInAerogel = 0.844
        GiGa.TrackSeq.RichG4PhotOpt.RichHpdReadOutEffInRich1Gas =0.775
        GiGa.TrackSeq.RichG4PhotOpt.RichHpdReadOutEffInRich2Gas=0.776
        
    def RichSmearingConfig (self,GiGa):
        GiGa.ModularPL.GiGaPhysConstructorOp.RichHitSmearingActivate=True
        GiGa.ModularPL.GiGaPhysConstructorOp.RichAerogelHitSmearValue=0.76
        GiGa.ModularPL.GiGaPhysConstructorOp.Rich1GasHitSmearValue=0.085
        #GiGa.ModularPL.GiGaPhysConstructorOp.Rich2GasHitSmearValue=0.0
        
    def RichBinaryReadOutConfig(self,GiGa):
        
        GiGaGeo.HpdSiliconSensDet.RichFlagDuplicateHitsactivate=True
       
        if  (self.getProp("RichBinaryReadoutSetup") == 1) :
            GiGaGeo.HpdSiliconSensDet.RichAviodDuplicateHitsActivate=False
        elif (self.getProp("RichBinaryReadoutSetup") == 2) :
            GiGaGeo.HpdSiliconSensDet.RichAviodDuplicateHitsActivate=True

    def  RichExtendedInfoClassesSetup(self):
        if (self.getProp("RichExtendedinfoClasses")):
            from Configurables import GiGaGetHitsAlg
            GiGaGetHitsAlg().RichOpticalPhotons = "/Event/MC/Rich/OpticalPhotons"
            GiGaGetHitsAlg().RichSegments       = "/Event/MC/Rich/Segments"
            GiGaGetHitsAlg().RichTracks         = "/Event/MC/Rich/Tracks"
           
    def RichExtendedInfoSetup(self, GiGa):
        GiGa.ModularPL.GiGaPhysConstructorOp.RichActivateRichPhysicsProcVerboseTag=True

        if (self.getProp("ActivateAerogel")):
            GiGa.StepSeq.Members  += ["RichG4StepAnalysis4/RichStepAgelExit"]

        GiGa.StepSeq.Members  += ["RichG4StepAnalysis5/RichStepMirrorRefl"]

        self.RichExtendedInfoClassesSetup()

        
    def RichAnalysisSetup(self, GiGa):
        from Configurables import RichG4EventAction, RichG4RunAction
        GiGa.EventSeq.addTool(RichG4EventAction, name="RichG4Event")
        GiGa.RunSeq.addTool(RichG4RunAction, name="RichG4Run" )
        GiGa.EventSeq.Members     += [ "RichG4EventAction/RichG4Event" ]
        GiGa.RunSeq.Members       += [ "RichG4RunAction/RichG4Run" ]
            
            
        if (self.getProp("RichFullAnalysis")):     
            
            if (self.getProp("RichActivateRecon")):                
                GiGa.EventSeq.RichG4Event.RichG4EventActivateCkvReconstruction=True
                GiGa.EventSeq.RichG4Event.RichG4EventHitReconUseSaturatedHit=True
                GiGa.EventSeq.RichG4Event.RichG4EventHitReconUseMidRadiator=False
            
                if (self.getProp("MakeRichG4MonitorHistoSet4")) :
                    GiGa.EventSeq.RichG4Event.RichEventActionHistoFillSet4=True
                    GiGa.RunSeq.RichG4Run.DefineRichG4HistoSet4=True
                    
                if  (self.getProp("MakeRichG4MonitorHistoSet5")) :                    
                    GiGa.EventSeq.RichG4Event.RichEventActionHistoFillSet5=True
                    GiGa.RunSeq.RichG4Run.DefineRichG4HistoSet5=True

            self.RichExtendedInfoSetup(GiGa)
            self.RichGeneralStepActionSetup(GiGa)

            
            GiGa.EventSeq.RichG4Event.RichEventActionVerbose = self.getProp("RichEvActionVerboseLevel")
            self.setProp("MakeRichG4MonitorHistoSet2",True)
            
            #GiGa.EventSeq.RichG4Event.RichG4InputMonitorActivate=False            
            # The printing of each hit is suppressed with the verbose flag set to be 0. To print them, set it to be 3.
                       
            # The counting in the full acceptance deactivated by default.
            #GiGa.EventSeq.RichG4Event.RichG4EventActivateCountingFullAcc=True


        # basic monitoring histograms are in set2 histograms
        if (self.getProp("MakeRichG4MonitorHistoSet2")):
            GiGa.EventSeq.RichG4Event.RichG4EventActivateCounting=True
            GiGa.EventSeq.RichG4Event.RichEventActionHistoFillSet2=True
            GiGa.RunSeq.RichG4Run.DefineRichG4HistoSet2=True
        else:
            GiGa.EventSeq.RichG4Event.RichG4EventActivateCounting=False
            GiGa.EventSeq.RichG4Event.RichEventActionHistoFillSet2=False
            GiGa.RunSeq.RichG4Run.DefineRichG4HistoSet2=False
            

            
        if (self.getProp("MakeRichG4Ntuple")) :
            GiGa.EventSeq.RichG4Event.RichG4QuartzWindowCkvHistoActivate=True
            
        if (self.getProp("RichExtraFullAnalysis")):
            
            if (self.getProp("MakeRichG4MonitorHistoSet1")):
                GiGa.RunSeq.RichG4Run.DefineRichG4HistoSet1=True
                GiGa.RunSeq.RichG4Run.DefineRichG4HistoSet1Type=mRichG4MonitorHistoSet1Type
                if (self.getProp("RichG4MonitorHistoSet1Type") == 1)  :
                    GiGa.EventSeq.RichG4Event.RichEventActionHistoFillSet1=True
                    
            if (self.getProp("RichG4MakeMonitorHistoSet3")) :
                GiGa.RunSeq.RichG4Run.DefineRichG4HistoSet3=True
                GiGa.EventSeq.RichG4Event.RichEventActionHistoFillSet3=True
       
         
