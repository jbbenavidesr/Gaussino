"""
Configuration tools for GaussCherenkov

"""
from Gaudi.Configuration import *

import GaudiKernel.ProcessJobOptions
from GaudiKernel import SystemOfUnits


from Configurables import LHCbConfigurableUser, LHCbApp


from Configurables import ( GiGa )
from Configurables import ( GiGaPhysConstructorOpCkv,  GiGaPhysConstructorPhotoDetector)

class GaussCherenkovConf (LHCbConfigurableUser):
    
    #steering options

    __slots__ ={
         "RichUpgradeDataYear"                   : "2022"
        ,"ActivateCherenkovProcess"              : True
        ,"ActivateCherenkovScint"                : True
        ,"CkvRichExtendedInfo"                   : False
        ,"CkvRichExtendedinfoClasses"            : True
        ,"ActivateStorePmtStepInfo"              : False
        ,"ActivateCkvTrackActionFlags"           : True
        ,"ActivateCkvTrackActionRich2Debug"      : False
        ,"ActivateCkvTrackActionRich2ScintSup"   : False 
        ,"CkvFullAnalysis"                       : False
        ,"CkvActivateRecon"                      : False
        ,"CkvExtraFullAnalysis"                  : False
        ,"CkvEvActionVerboseLevel"               : 0
        ,"ActivateCkvEfficiencyConfig"           : False
        ,"ActivateCkvSmearingConfig"             : False
        ,"MakeCkvG4Ntuple"                       : False
        ,"MakeCkvG4MonitorHistoSet1"             : False
        ,"CkvG4MonitorHistoSet1Type"             : 0
        ,"MakeCkvG4MonitorHistoSet2"             : True
        ,"MakeCkvG4MonitorHistoSet3"             : False
        ,"MakeCkvG4MonitorHistoSet4"             : False
        ,"MakeCkvG4MonitorHistoSet5"             : False
        ,"ActivateCkvStepAnalysisCounters"       : True
        ,"ActivateCkvRich2PhotSuppressSimDebug"  : False
        ,"ActivateCkvStepAnaPhotDir"             : False
        ,"ActivateCkvStep8AnaDebug"              : False
        ,"ActivateCkvStep9AnaDebug"              : False
        ,"UpgradeRichPmtDetectorExist"           : True
        ,"SkipGeant4RichCkvFlag"                 : False 
        #,"CkvBinaryReadoutSetup"                : 0
        }

        
  
    def InitializeGaussCherenkovConfiguration(self):
        from Configurables import Gauss
        Gauss=Gauss()


        print "Gauss run option selected for Upgraded RICH  =  ", Gauss.getProp("UpgradeRICHSimRunOption")
        #log.info( "Gauss run option selected for Upgraded RICH  =  %s" %(Gauss.getProp("UpgradeRICHSimRunOption")))

        if ((Gauss.getProp("UpgradeRICHSimRunOption")) == "Formula1"):
            #run configuration for very fast runs
            self.setProp("CkvRichExtendedinfoClasses", False)
            self.setProp("MakeCkvG4MonitorHistoSet2",False)
        elif ((Gauss.getProp("UpgradeRICHSimRunOption")) == "GTB"):
            #standard run configuration for fast productions. This is the default configuration
            self.setProp("CkvRichExtendedinfoClasses",True)
            
        elif ((Gauss.getProp("UpgradeRICHSimRunOption")) == "SUV"):
            #Run configuration for RICH detector related studies using RICH reconstruction in Brunel
            self.setProp("CkvRichExtendedinfoClasses", True)
            self.setProp("CkvRichExtendedInfo",True)
            self.setProp("ActivateStorePmtStepInfo", True)
            self.setProp("ActivateCkvStepAnalysisCounters", True)
            
        elif ((Gauss.getProp("UpgradeRICHSimRunOption")) == "HGV"):
            #Run configuration for detailed Simulation studies using Gauss
            self.setProp("CkvFullAnalysis",True)
            self.setProp("CkvActivateRecon",True)
            self.setProp("MakeCkvG4MonitorHistoSet4",True)
            self.setProp("MakeCkvG4MonitorHistoSet5",True)
            self.setProp("CkvRichExtendedinfoClasses",True)
            self.setProp("CkvRichExtendedInfo",True)
            self.setProp("ActivateStorePmtStepInfo",True)
            self.setProp("ActivateCkvStepAnalysisCounters",True)
        elif ((Gauss.getProp("UpgradeRICHSimRunOption")) == "FareFiasco"):
            # Run configuration with Rich Simulation switched off
            self.setProp("ActivateCherenkovProcess", False)


        self.RichUpgradeDataYearConfig()
        print "Upgrade RICH simulation for the data in year = ", self.getProp("RichUpgradeDataYear")
        #log.info( "Upgrade RICH simulation for the data in year =  %s" %(self.getProp("RichUpgradeDataYear")))
        
        #avoid annoying printouts with warnings about non-existent aerogel 
        from Configurables import SimulationSvc 
        SimulationSvc().SimulationDbLocation = "$GAUSSROOT/xml/SimulationRICHAerogelOff.xml"
        
        
    def setUpgradeRichDetExistFlag(self,UpgradeRichPmtDetectorExistFlag=True):
        self.setProp("UpgradeRichPmtDetectorExist", UpgradeRichPmtDetectorExistFlag)

    def setSkipUpgradeGeant4Flag(self, SkipUpgradeGeant4=False):
        self.setProp("SkipGeant4RichCkvFlag",  SkipUpgradeGeant4)

    def ApplyGaussCherenkovConfiguration(self, GiGa ):
        #Actual implementation of upgrade RICH simulation configuration
        
        if ( not ( self.getProp("UpgradeRichPmtDetectorExist"))) :
            self.setProp("ActivateCherenkovProcess",False)

        if ( self.getProp("SkipGeant4RichCkvFlag")):
            self.setProp("ActivateCherenkovProcess",False)

        # RICH Cherenkov Processes 
        self.CherenkovProcessActivate(GiGa)       
        print "Upgrade RICH simulation now configured"
        #log.info("Upgrade RICH simulation now configured")
               
    def RichUpgradeDataYearConfig(self):
        # for now unchanged.    
        self.setProp("RichUpgradeDataYear", "2022")
        
    def CherenkovProcessActivate(self,GiGa):
            
        GiGa.ModularPL.addTool( GiGaPhysConstructorOpCkv,name="GiGaPhysConstructorOpCkv" )
        GiGa.ModularPL.addTool( GiGaPhysConstructorPhotoDetector ,name="GiGaPhysConstructorPhotoDetector" )
        
        if  (self.getProp("ActivateCherenkovProcess")):            
            GiGa.ModularPL.GiGaPhysConstructorOpCkv.RichOpticalPhysicsProcessActivate = True
            GiGa.ModularPL.GiGaPhysConstructorPhotoDetector.RichPmtPhysicsProcessActivate = True

            
            self.CkvGeneralPhysProcessOptions(GiGa)
            
            self.CkvScintillationSetup(GiGa)        
            
            #self.CkvRandomHitsSetup(GiGa) # kept as place holder 


            if (self.getProp("CkvRichExtendedinfoClasses")):
                self.CkvExtendedInfoClassesSetup()    
              
            if (self.getProp("CkvRichExtendedInfo")):
                self.CkvExtendedInfoSetup(GiGa)

            self.CkvGeneralStepActionSetup(GiGa)
            self.CkvGeneralTrackActionSetup(GiGa)

            self.CkvAnalysisSetup(GiGa)

            #if (self.getProp("ActivateCkvSmearingConfig" )):
            #    self.RichSmearingConfig(GiGa)  #kept as a place holder
            # The following does not work. Hence commented out.
            # The flags from this works when activated from an options file
            #if  (self.getProp("CkvBinaryReadoutSetup") > 0) :
            #    self.CkvBinaryReadOutConfig(GiGaGeo)
            #    self.CkvBinaryReadOutConfig()
               
            
        else:
            
            GiGa.ModularPL.GiGaPhysConstructorOpCkv.RichOpticalPhysicsProcessActivate = False
            GiGa.ModularPL.GiGaPhysConstructorPhotoDetector.RichPmtPhysicsProcessActivate = False


    def CkvScintillationSetup(self,GiGa):
        if (self.getProp("ActivateCherenkovScint")):
            GiGa.ModularPL.GiGaPhysConstructorOpCkv.RichActivateCF4Scintillation=True
            GiGa.ModularPL.GiGaPhysConstructorOpCkv.RichApplyScintillationYieldScaleFactor=True
            GiGa.ModularPL.GiGaPhysConstructorOpCkv.RichScintillationYieldScaleFactor=0.07
            
        else:            
            GiGa.ModularPL.GiGaPhysConstructorOpCkv.RichActivateCF4Scintillation=False

    def CkvGeneralPhysProcessOptions(self,GiGa):
        
        # When RichApplyMaxNumCkvPhotPerStepPerRadiator is False, the global value
        # as given by RichMaxNumPhotPerCherenkovStep is used for all radiators.
        # Otherwise the individual value as listed below for each radiator is used 
        GiGa.ModularPL.GiGaPhysConstructorOpCkv.RichApplyMaxNumCkvPhotPerStepPerRadiator=False
        GiGa.ModularPL.GiGaPhysConstructorOpCkv.RichMaxPhotonsPerCherenkovStepInRich1Gas=40
        GiGa.ModularPL.GiGaPhysConstructorOpCkv.RichMaxPhotonsPerCherenkovStepInRich2Gas=40
        GiGa.ModularPL.GiGaPhysConstructorOpCkv.RichMaxNumPhotPerCherenkovStep=40

    def CkvGeneralStepActionSetup(self,GiGa):
        
        if (self.getProp("ActivateStorePmtStepInfo")):
            GiGa.StepSeq.Members  += ["CherenkovG4StepAnalysis6/RichStepPmtRefl" ] 

        if (self.getProp("ActivateCkvStepAnalysisCounters")) :
            GiGa.StepSeq.Members  += ["RichG4StepAnalysis3/CkvStep3"]
            
        if (self.getProp("ActivateCkvStepAnaPhotDir")):
            GiGa.StepSeq.Members  += ["CherenkovStepAnalysis7/CkvStepPhotDir"]

        if (self.getProp("ActivateCkvStep8AnaDebug")):
            GiGa.StepSeq.Members  += ["CherenkovStepAnalysis8/CkvStep8"]
            
        if (self.getProp("ActivateCkvStep9AnaDebug")):
            GiGa.StepSeq.Members  += ["CkvStepAnalysis9/CkvStep9"]
            
            

    def CkvGeneralTrackActionSetup(self,GiGa):
        if  (self.getProp("ActivateCkvTrackActionFlags")):
            from Configurables import CkvG4TrackActionPhotOpt
            GiGa.TrackSeq.addTool(CkvG4TrackActionPhotOpt , name="CkvG4PhotOpt" )
            #if self.getProp("ActivateRichCkvEfficiencyConfig" ):
            #    self.CkvEfficiencyConfig(GiGa)   # kept as a place holer
            
            GiGa.TrackSeq.Members      += [ "CkvG4TrackActionPhotOpt/CkvG4PhotOpt"]
            
            if( self.getProp("ActivateCkvTrackActionRich2ScintSup")):
                
                from Configurables import RichG4TrackActionRich2Photon
                GiGa.TrackSeq.addTool(RichG4TrackActionRich2Photon, name="Rich2ScintPhotonSup")
                if (self.getProp("ActivateCkvTrackActionRich2Debug")):
                    GiGa.TrackSeq.Rich2ScintPhotonSup.RichG4TrackActionRich2PhotonDebugMode=1                
                GiGa.TrackSeq.Members      += [ "RichG4TrackActionRich2Photon/Rich2ScintPhotonSup"]
             
        if  (self.getProp("ActivateCkvRich2PhotSuppressSimDebug")):
            from Configurables import RichG4TrackActionRich2DbgPhotonTrack            
            GiGa.TrackSeq.addTool (RichG4TrackActionRich2DbgPhotonTrack, name="Rich2DbgSuppressPhoton")
            GiGa.TrackSeq.Rich2DbgSuppressPhoton.DeActiavteRich2PhotonTrackForDebug=True             
            GiGa.TrackSeq.Members      += ["RichG4TrackActionRich2DbgPhotonTrack/Rich2DbgSuppressPhoton"]



    def  CkvExtendedInfoClassesSetup(self):
        if (self.getProp("CkvRichExtendedinfoClasses")):
            from Configurables import GiGaGetHitsAlg
            GiGaGetHitsAlg().RichOpticalPhotons = "/Event/MC/Rich/OpticalPhotons"
            GiGaGetHitsAlg().RichSegments       = "/Event/MC/Rich/Segments"
            GiGaGetHitsAlg().RichTracks         = "/Event/MC/Rich/Tracks"
           
    def CkvExtendedInfoSetup(self, GiGa):
        GiGa.ModularPL.GiGaPhysConstructorOpCkv.RichActivateRichPhysicsProcVerboseTag=True;
        
        from Configurables import CkvG4StepAnalysis5
        GiGa.StepSeq.addTool (CkvG4StepAnalysis5, name="CkvStepMirrorRefl")
        GiGa.StepSeq.Members  += ["CkvG4StepAnalysis5/CkvStepMirrorRefl"]

        self.CkvExtendedInfoClassesSetup()
   
 
    def CkvAnalysisSetup(self, GiGa):
        from Configurables import CherenkovG4EventAction, CkvG4RunAction
        GiGa.EventSeq.addTool(CherenkovG4EventAction, name="CkvG4Event")
        GiGa.RunSeq.addTool(CkvG4RunAction, name="CkvG4Run" )
        GiGa.EventSeq.Members     += ["CherenkovG4EventAction/CkvG4Event"]
        GiGa.RunSeq.Members       += [ "CkvG4RunAction/CkvG4Run"]
            
            
        if (self.getProp("CkvFullAnalysis")):
                
            
            if (self.getProp("CkvActivateRecon")):
                
                GiGa.EventSeq.CkvG4Event.RichG4EventActivateCkvReconstruction=True
                GiGa.EventSeq.CkvG4Event.RichG4EventHitReconUseSaturatedHit=True;
                GiGa.EventSeq.CkvG4Event.RichG4EventHitReconUseMidRadiator=False
            
                if (self.getProp("MakeCkvG4MonitorHistoSet4")) :
                    
                    GiGa.EventSeq.CkvG4Event.RichEventActionHistoFillSet4=True
                    GiGa.RunSeq.CkvG4Run.DefineRichG4HistoSet4=True
                    
                if  (self.getProp("MakeCkvG4MonitorHistoSet5")) :
                
                    GiGa.EventSeq.CkvG4Event.RichEventActionHistoFillSet5=True
                    GiGa.RunSeq.CkvG4Run.DefineRichG4HistoSet5=True

            self.CkvExtendedInfoSetup(GiGa)
            self.CkvGeneralStepActionSetup(GiGa)

            
            GiGa.EventSeq.CkvG4Event.RichEventActionVerbose = self.getProp("CkvEvActionVerboseLevel")
            self.setProp("MakeCkvG4MonitorHistoSet2" ,True)
            
            #GiGa.EventSeq.CkvG4Event.RichG4InputMonitorActivate=False            
            # The printing of each hit is suppressed with the verbose flag set to be 0. To print them, set it to be 3.
                       
            # The counting in the full acceptance deactivated by default.
            #GiGa.EventSeq.CkvG4Event.RichG4EventActivateCountingFullAcc=True;
            

        # basic monitoring histograms are in set2 histograms
        if (self.getProp("MakeCkvG4MonitorHistoSet2")):
            GiGa.EventSeq.CkvG4Event.RichG4EventActivateCounting=True
            GiGa.EventSeq.CkvG4Event.RichEventActionHistoFillSet2=True
            GiGa.RunSeq.CkvG4Run.DefineRichG4HistoSet2=True
        else:
            GiGa.EventSeq.CkvG4Event.RichG4EventActivateCounting=False
            GiGa.EventSeq.CkvG4Event.RichEventActionHistoFillSet2=False
            GiGa.RunSeq.CkvG4Run.DefineRichG4HistoSet2=False
            

            
        if (self.getProp( "MakeCkvG4Ntuple" )) :
            GiGa.EventSeq.CkvG4Event.RichG4QuartzWindowCkvHistoActivate=True
            
        if (self.getProp("CkvExtraFullAnalysis")):
            
            if (self.getProp("MakeCkvG4MonitorHistoSet1")):
                GiGa.RunSeq.CkvG4Run.DefineRichG4HistoSet1=True
                #GiGa.RunSeq.CkvG4Run.DefineRichG4HistoSet1Type=self.getProp("CkvG4MonitorHistoSet1Type")
                if (self.getProp("CkvG4MonitorHistoSet1Type") == 1)  :
                    GiGa.EventSeq.CkvG4Event.RichEventActionHistoFillSet1=True
                    
            if (self.getProp("CkvG4MakeMonitorHistoSet3")) :
                GiGa.RunSeq.CkvG4Run.DefineRichG4HistoSet3=True;
                GiGa.EventSeq.CkvG4Event.RichEventActionHistoFillSet3=True

            if (self.getProp("ActivateCkvStep8AnaDebug")):
                GiGa.RunSeq.CkvG4Run.ActivateRichG4HistoSet8=True



#    def CkvSmearingConfig (self,GiGa):
#        GiGa.ModularPL.GiGaPhysConstructorOpCkv.RichHitSmearingActivate=True
#        GiGa.ModularPL.GiGaPhysConstructorOpCkv.Rich1GasHitSmearValue=0.0
#        GiGa.ModularPL.GiGaPhysConstructorOp.Rich2GasHitSmearValue=0.0
#    def CkvEfficiencyConfig(self,GiGa):
#        GiGa.TrackSeq.CkvG4PhotOpt.RichPmtReadOutEffInRich1Gas =1.0
#        GiGa.TrackSeq.CkvG4PhotOpt.RichPmtReadOutEffInRich2Gas =1.0

#    The following does not work and hence commented out. One can  activate this
#    from an options file as in
#    $GAUSSCHERENKOVROOT/options/GaussCherenkovGiGaGeoOptions/GaussCherenkovBinaryReadout.opts
#
#    def CkvBinaryReadOutConfig(self):
#        from Configurables import GiGaGeo
#        GiGaGeo=GiGaGeo()
#        GiGaGeo.RichPMTAnodeSensDet.RichPmtFlagDuplicateHitsactivate = True
#        if  (self.getProp("CkvBinaryReadoutSetup") == 1) :
#            GiGaGeo.RichPMTAnodeSensDet.RichPmtAviodDuplicateHitsActivate = False
#        elif (self.getProp("CkvBinaryReadoutSetup") == 2) :
#            GiGaGeo.RichPMTAnodeSensDet.RichPmtAviodDuplicateHitsActivate = True
#        elif (self.getProp("CkvBinaryReadoutSetup") == 3) :
#            GiGaGeo.RichPMTAnodeSensDet.RichPmtFlagDuplicateHitsactivate = False
#            GiGaGeo.RichPMTAnodeSensDet.RichPmtAviodDuplicateHitsActivate = False
#
