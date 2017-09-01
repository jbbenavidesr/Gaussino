## ############################################################################
## # File for running Gauss with Upgrade database with baseline as agreed in
## # Upgrade TDRs as of May 2015 and updated with the FT neutron shield in
## # June 2017
## #
## # Syntax is:
## #   gaudirun.py Gauss-Upgrade-Baseline.py <someInputJobConfiguration>.py
## ############################################################################

from Configurables import Gauss, CondDB

CondDB().Upgrade     = True

Gauss().DetectorGeo  = { "Detectors": ['VP', 'UT', 'FT', 'Rich1Pmt', 'Rich2Pmt', 'Ecal', 'Hcal', 'Muon', 'Magnet' ] }
Gauss().DetectorSim  = { "Detectors": ['VP', 'UT', 'FT', 'Rich1Pmt', 'Rich2Pmt', 'Ecal', 'Hcal', 'Muon', 'Magnet' ] }
Gauss().DetectorMoni = { "Detectors": ['VP', 'UT', 'FT', 'Rich1Pmt', 'Rich2Pmt', 'Ecal', 'Hcal', 'Muon', 'Magnet' ] }

Gauss().DataType     = "Upgrade" 

# Activate the neutron shield
CondDB().AllLocalTagsByDataType=["FT_NeutronShieldActivate_TypeA"] 

from Gaudi.Configuration import *
def NeutronShieldActivate(): 
    from Configurables import GiGaInputStream 
    Geo = GiGaInputStream("Geo") 
    Geo.StreamItems += ["/dd/Structure/LHCb/DownstreamRegion/NeutronShielding"] 


appendPostConfigAction( NeutronShieldActivate )

