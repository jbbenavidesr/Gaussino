#!/usr/bin/env python
#################################################################################
## This file contains a class for initiating sub-detector scan of LHCb         ##
## using the radiation length tool to obtain information on interaction and    ##
## radiation lengths.                                                          ##
##                                                                             ##
## This test can be run using the command:                                     ##
##                                                                             ##
##   python $SIMCHECKSROOT/rad_length_scan.py                                  ##
##                                                                             ##
## from within Gauss with the optional arguments:                              ##
##                                                                             ##
##  --use-giga-geo        Use GiGaGeo for Geometry reading                     ##
##  --use-gauss-geo       Use GaussGeo for Geometry reading                    ##
##  --no-plots            Run Test Only, No Plotting (for debugging)           ##
##  --plot-only           Repeat ploting (for debugging)                       ##
##  --debug               Run with higher verbosity                            ##
##                                                                             ##
## Twiki at: https://twiki.cern.ch/twiki/bin/view/LHCb/RadLengthStudies        ##
##                                                                             ##
##  @author : K.Zarebski                                                       ##
##  @date   : last modified on 2017-10-09                                      ##
#################################################################################

import sys
import os
import logging
import subprocess

from RadLengthMakePlots import makePlots
from tempfile import NamedTemporaryFile

class rad_length_detector_scan:
    '''Class for Performing a Scan of SubDetectors within LHCb to extract Information on Radiation
and Interaction Lengths within the Different Regions using the Implemented Scoring Planes.

        Args:

            output_dir (string)                       :    the location where output files should be written to
          
            pdfs_dir (string)                         :    if specified, produce Pdfs of plots and save to this location
  
            text_data_dir (string)                    :    if specified, produce LaTeX tables and JSON output of results

            debug ('INFO'/'DEBUG')                    :    debug mode, default is 'INFO'

            use_geo ('Default'/'GaussGeo'/'GiGaGeo')  :    use either GaussGeo or GiGaGeo to read geometry, default 'Default'
                                                           is to use whichever is the default option in the version of Gauss
    '''
    _simchecks_version = os.environ['SIMCHECKSROOT']
    _radlength_opts    = os.path.join(_simchecks_version, 'options', 'RadLength')
    _lhcb_nophys_opts  = os.path.join(os.environ['APPCONFIGOPTS'], 'Gauss', 'G4PL_FTFP_BERT_EmNoCuts_noLHCbphys.py')
    _ana_opts          = os.path.join(_radlength_opts, 'RadLengthAna.py')
    _ana_velo_opts     = os.path.join(_radlength_opts, 'RadLengthAna_VELO.py')
    _mat_eval_opts     = os.path.join(_radlength_opts, 'MaterialEvalGun.py')
    _job_opts          = os.path.join(_radlength_opts, 'Gauss-Job.py')
    _gauss_cmd         = 'gaudirun.py {physoff} {mateval} {job}'.format(physoff=_lhcb_nophys_opts,
                                                                              mateval=_mat_eval_opts,
                                                                              job=_job_opts)
    def __init__(self, output_dir, pdfs_dir=None, text_data_dir=None, debug='INFO', use_geo='Default'):            
        self._gauss_geo_opts = ''
        if use_geo == 'GaussGeo':
           self._gauss_geo_opts = 'from Configurables import Gauss; Gauss().UseGaussGeo = True'
        elif use_geo == 'GiGaGeo':
           self._gauss_geo_opts = 'from Configurables import Gauss; Gauss().UseGaussGeo = False'
        self._logger = logging.getLogger('RadLengthDetectorScan')
        logging.basicConfig()
        self._logger.setLevel(debug)
        sys.path.append(os.path.join(self._simchecks_version, 'python'))
        self._root_file_dir = os.path.join(output_dir, 'RadLengthDetectorScan', 'root_files')
        self._root_out_file = os.path.join(self._root_file_dir, 'Rad_merged.root')
        self.pdf_directory  = pdfs_dir
        self.text_data_directory = text_data_dir

        # Set Up the Directory Structure
        self._logger.info('Creating Directories for Test Output')
        folders = ['root_files']
        if self.pdf_directory:
           self._debug('Creating Folder for Pdfs')
           folders.append('pdf_files')
        if self.text_data_directory:
           self._debug('Creating Folder for JSON Data Tables')
           folders.append('data_tables')

        for folder in folders:
            if not os.path.exists(os.path.join(output_dir, 'RadLengthDetectorScan', folder)): #Let's Be Safe!
                os.makedirs(os.path.join(output_dir, 'RadLengthDetectorScan', folder))

    def _mergeOutput(self):
        '''Merge All ROOT files Produced During the Scan Into a Single File "Rad_merged.root"'''
        self._logger.debug("Merging ROOT Files")
        _files_to_merge = [os.path.join(self._root_file_dir, file_) for file_ in ['Rad.root', 'Rad_VELO.root']]
        merge_command = 'hadd -f {output} {rootfiles}'.format(output=self._root_out_file, rootfiles=' '.join(_files_to_merge))
        subprocess.check_call(merge_command, shell=True)

    def runScan(self, output_plots=True):
        '''Perform the Per Sub Detector Material Scan and Construct Radiation and Interaction Length Maps


               Args:
                      output_plots (True/False)    :   produce plots whilst running test, default is 'True'
        '''

        # Run Gauss using options for All Detectors but the VELO, then for the VELO itself
        for options, filename in zip([self._ana_opts, self._ana_velo_opts], ['Rad.root', 'Rad_VELO.root']):
            output_opts='''
from Gaudi.Configuration import *
# --- Save ntuple with hadronic cross section information
ApplicationMgr().ExtSvc += ["NTupleSvc"]
NTupleSvc().Output = ["FILE2 DATAFILE='{outfile}' TYP='ROOT' OPT='NEW'" ]
            '''.format(outfile=os.path.join(self._root_file_dir, filename))

            with NamedTemporaryFile(suffix='.py') as tmp:
                tmp.write(output_opts)
                tmp.flush()
                self._logger.debug('Running Gauss with Options: %s %s %s --option="%s"',self._gauss_cmd, options, tmp.name, self._gauss_geo_opts)
                subprocess.check_call('{core} {opts} {outfile_opts} --option="{gaussgeo}"'.format(core=self._gauss_cmd, 
                                                                                       opts=options, 
										       gaussgeo=self._gauss_geo_opts,
                                                                                       outfile_opts=tmp.name), shell=True)

        # Merge the ROOT Files
        self._mergeOutput()

        # Run Plot Creation
        if output_plots:
            self._runPlotting()
 
        else:
            self._logger.warning("Option to Produce Plots upon Test Completion set to 'False'")

    def _runPlotting(self):
        '''Create Plots from the Results of the SubDetector Scans Including 2d Maps'''
        # Make the Plots using RadLengthMakePlots
        self._logger.info('Test Complete. Creating Plots using RadLengthMakePlots')
        makePlots(self._root_out_file, plot_type="rad",
                  output_dir=self._root_file_dir,
                  pdfs_dir=self.pdf_directory,
                  data_dir=self.text_data_directory,
                  debug=self._logger.getEffectiveLevel())

        makePlots(self._root_out_file, plot_type="inter",
                  output_dir=self._root_file_dir,
                  pdfs_dir=self.pdf_directory,
                  data_dir=self.text_data_directory,
                  debug=self._logger.getEffectiveLevel())

        inter_and_rad_plots = [os.path.join(self._root_file_dir, file_) for file_ in ['Interaction_Length_Plots.root', 'Radiation_Length_Plots.root']]

        self._logger.debug('Merging Plot Component Files')
        subprocess.check_call('hadd -f {output} {components}'.format(output=os.path.join(self._root_file_dir, 'RadLengthSubDetectorPlots.root'),
                              components=' '.join(inter_and_rad_plots)), shell=True)

        self._logger.debug('Deleting Plot Component Files')
        subprocess.check_call('rm {}'.format(' '.join(inter_and_rad_plots)), shell=True)

if __name__ == "__main__":

    #-------------------------------ARGUMENT PARSER---------------------------------#
    #               Run Test in 'Debug' mode if '--debug' flag set                  #
    #               Options to use GaussGeo or GiGaGeo                              #
    #               Choose whether to run plotting (and/or) test                    #
    #-------------------------------------------------------------------------------#

    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument('--debug', action='store_true', help='Run in Debug Mode')
    parser.add_argument('--plot-only', action='store_true', help='Run Plotting Section of Test Only')
    parser.add_argument('--no-plots', action='store_true', help='Run Test without Making Plots')
    parser.add_argument('--gauss-geo', action='store_true', help='Run Test using GaussGeo')
    parser.add_argument('--giga-geo', action='store_true', help='Run Test using GiGaGeo')
    args = parser.parse_args()

    #-------------------------------CHOOSE GEO READER-------------------------------#

    geo_type = 'Default'

    if args.giga_geo:
       geo_type = 'GiGaGeo'

    if args.gauss_geo:
       geo_type = 'GaussGeo'

    #-------------------------------------------------------------------------------#

    rad_length_scan = rad_length_detector_scan(os.getcwd(), debug=args.debug, use_geo=geo_type)

    #-----------------------------CHECK PLOTTING OPTIONS----------------------------#

    if args.plot_only:
       for type_ in ['Interaction', 'Radiation']:
           if os.path.exists(os.path.join(os.getcwd(), 'RadLengthDetectorScan', 'root_files', '{}_Length_Plots.root'.format(type_))):
              subprocess.check_call('rm {}'.format(os.path.join(os.getcwd(), 'RadLengthDetectorScan', 'root_files', '{}_Length_Plots.root'.format(type_))), shell=True)
       rad_length_scan._runPlotting()
    elif args.no_plots:
       rad_length_scan.runScan(False)
    else:
       rad_length_scan.runScan()
