#!/usr/bin/env python
#################################################################################
## This file contains a class for initiating a Z axis scan of the VELO         ##
## using the radiation length tool and firing a particle gun at different      ##
## positions within the VELO volume.                                           ##
## Only Scoring Plane 1 located after the VELO is activated.                   ##
##                                                                             ##
## This test can be run using the command:                                     ##
##                                                                             ##
##   python $SIMCHECKSROOT/rad_length_scan_velo_z.py                           ##
##                                                                             ##
## from within Gauss with the optional arguments:                              ##
##                                                                             ##
##  --use-giga-geo        Use GiGaGeo for Geometry reading                     ##
##  --use-gauss-geo       Use GaussGeo for Geometry reading                    ##
##  --debug               Run with higher verbosity                            ##
##                                                                             ##
## Twiki at: https://twiki.cern.ch/twiki/bin/view/LHCb/RadLengthStudies        ##
##                                                                             ##
##  @author : K.Zarebski                                                       ##
##  @date   : last modified on 2017-10-09                                      ##
#################################################################################

import sys
import os
import subprocess
from tempfile import NamedTemporaryFile
from ROOT import *
import logging
import argparse

class rad_length_velo_scan:
    '''RadLength Velo Scan: Particle Gun is Positioned at Various Positions Along the Z axis. The Cumulative Radiation
Lengths are Recorded.

        Args:
              output_dir (string)                    :     the location where output files should be written to

                        x (float)                    :     constant co-ordinate in 'x' plane for the particle gun

                        y (float)                    :     constant co-ordinate in 'y' plane for the particle gun

            z (float, float, int)                    :     tuple containing - minimum  and maximum 'z' values and number of increments

                      nevts (int)                    :     number of events Gauss should generate for the scan

           debug ('INFO'/'DEBUG')                    :     debug mode, default is 'INFO'

           use_geo ('Default'/'GaussGeo'/'GiGaGeo')  :     use either GaussGeo or GiGaGeo to read geometry, default 'Default'
                                                           is to use whichever is the default option in the version of Gauss

    '''
    _simchecks_version = os.environ['SIMCHECKSROOT']
    _rad_length_opts   = os.path.join(_simchecks_version, 'options', 'RadLength')
    _no_LHCb_phys_opts = os.path.join(os.environ['APPCONFIGOPTS'], 'Gauss', 'G4PL_FTFP_BERT_EmNoCuts_noLHCbphys.py')
    _velo_ana_opts     = os.path.join(_rad_length_opts, 'RadLengthAna_VELO.py')
    _gauss_job_opts    = os.path.join(_rad_length_opts, 'Gauss-Job.py')
    _part_gun_template = '''from Gaudi.Configuration import *
from Configurables import ParticleGun
from GaudiKernel.SystemOfUnits import *
from Configurables import MomentumRange

ParticleGun = ParticleGun("ParticleGun")
#ParticleGun.EventType = 53210205
ParticleGun.EventType = 53210168

from Configurables import MaterialEval
ParticleGun.addTool(MaterialEval, name="MaterialEval")
ParticleGun.ParticleGunTool = "MaterialEval"

from Configurables import FlatNParticles

ParticleGun.addTool(FlatNParticles, name="FlatNParticles")
ParticleGun.NumberOfParticlesTool = "FlatNParticles"
ParticleGun.FlatNParticles.MinNParticles = 1
ParticleGun.FlatNParticles.MaxNParticles = 1
ParticleGun.MaterialEval.PdgCode = 14
ParticleGun.MaterialEval.ModP = 50 * GeV
ParticleGun.MaterialEval.EtaPhi = True
ParticleGun.MaterialEval.Xorig = {x:.2f}*mm
ParticleGun.MaterialEval.Yorig = {y:.2f}*mm
ParticleGun.MaterialEval.Zorig = {z:.2f}*mm
ParticleGun.MaterialEval.ZPlane = 19.89*m

from Configurables import GiGa, GiGaStepActionSequence, RadLengthColl
giga = GiGa()
giga.addTool( GiGaStepActionSequence("StepSeq") , name = "StepSeq" )
giga.StepSeq.addTool( RadLengthColl )
giga.StepSeq.RadLengthColl.orig_x = {x:.2f}
giga.StepSeq.RadLengthColl.orig_y = {y:.2f}
giga.StepSeq.RadLengthColl.orig_z = {z:.2f}


NTupleSvc().Output = ["FILE2 DATAFILE='{output}/Rad_{x}_{y}_{z}.root' TYP='ROOT' OPT='NEW'"]

from Gauss.Configuration import *
LHCbApp().EvtMax = {nevts}
    '''
    _gauss_cmd = 'gaudirun.py {physoff} {anavelo} {job} {extraopts}'.format(physoff=_no_LHCb_phys_opts,
                                                                anavelo=_velo_ana_opts,
                                                                job=_gauss_job_opts + ' {tmp_opts}',
                                                                extraopts='--option="{extraopts}"')

    def __init__(self, output_dir, x, y, z, nevts, debug='INFO', use_geo='Default'):
        self._gauss_geo_opts = ''
        if use_geo == 'GaussGeo':
           self._gauss_geo_opts = 'from Configurables import Gauss; Gauss().UseGaussGeo = True'
        elif use_geo == 'GiGaGeo':
           self._gauss_geo_opts = 'from Configurables import Gauss; Gauss().UseGaussGeo = False'
        self._root_file_dir = os.path.join(output_dir, 'RadLengthVeloScan', 'root_files')
        self._root_out_file = os.path.join(self._root_file_dir, 'Rad_VELO.root')
        self._root_plot_file = os.path.join(self._root_file_dir, 'RadLengthVeloScan.root')
        sys.path.append(os.path.join(self._simchecks_version, 'python'))
        self._logger = logging.getLogger('RadLengthVELOScan')
        logging.basicConfig()
        self._logger.setLevel('{}'.format('DEBUG' if args.debug else 'INFO'))
        self._out_dir  = output_dir
        self._x_coord  = x
        self._y_coord  = y
        self._z_coords = [z[0]] if isinstance(z,float) else [float(n) for n in range(int(z[0]), int(z[1])+int(z[2]), int(z[2]))]
        self._max_evts = nevts

        # Set Up the Directory Structure
        if not os.path.exists(self._root_file_dir):  #Let's Be Safe!
            os.makedirs(self._root_file_dir)

    def _runScanAt(self, pgun_origin):
        '''Create Temporary Gauss script using coordinates and options then run Gauss using these settings

    pgun_origin <coordinate tuple>   Coordinates in the form (x,y,z) for the position of the particle gun
        '''

        # Generate a Gauss Options File for Each Coordinate and Run Gauss
        with NamedTemporaryFile(suffix='.py') as tmp:
            tmp.write(self._part_gun_template.format(pwd=self._out_dir,
                                                     x=pgun_origin[0],
                                                     y=pgun_origin[1],
                                                     z=pgun_origin[2],
                                                     nevts=self._max_evts,
                                                     output=self._root_file_dir))
            tmp.flush()
            self._logger.debug("Running RadLengthColl for Particle Gun at (%s, %s, %s)", *pgun_origin)
            subprocess.check_call(self._gauss_cmd.format(tmp_opts=tmp.name, extraopts=self._gauss_geo_opts), shell=True)

    def _mergeOutput(self):
        '''Merge All ROOT files Produced During the Scan Into a Single File "Rad_VELO.root"'''
        self._logger.debug("Merging ROOT Files")
        merge_command = 'hadd -f {output} {rootfiles}'.format(output=self._root_out_file, rootfiles=os.path.join(self._root_file_dir,'Rad_*.*_*.*_*.*.root'))
        subprocess.check_call(merge_command, shell=True)
        subprocess.check_call('rm {component_files}'.format(component_files=os.path.join(self._root_file_dir, 'Rad_*.*_*.*_*.*.root')), shell=True)


    def _makePlots(self, make_pdfs):
        '''Construct Plots of Test Results from Data Contained within the Output ROOT File'''
        result_table='''
x = {x_coord}, y= {y_coord}

z\t\Average\Error
-----\t-------\t-------
{results}
-----\t-------\t-------
        '''.format(x_coord=self._x_coord, y_coord=self._y_coord, results='{results}')
        root_file = TFile(self._root_out_file)
        rad_tree  = root_file.Get("RadLengthColl/tree")

        # Check Tree Exists Before Continuing
        try:
           rad_tree.GetEntries()
        except:
           self._logger.error("Could not Find Data Tree! Plot Generation Failed.")
           raise AssertionError

        # Create TGraphErrors using Information from the Cumulative Radiation Histograms
        hout  = TGraphErrors()
        scan_results = ''
        for i, co_ord in enumerate(self._z_coords):
            rad_tree.Draw("cumradlgh>>h", "(TMath::Abs(origz - %f) < 1e-5)" % co_ord)
            h = gPad.GetPrimitive("h")
            try:
              avg = h.GetMean()
              avg_err = h.GetMeanError()
            except:
              self._logger.error("Could Not Get Z Scan Information for (%s, %s, %s) Data", self._x_coord, self._y_coord, co_ord)
              raise AssertionError
            scan_results+='{0:.3f}\t{1:.5f}\t{2:.5f}\n'.format(co_ord, avg, avg_err)
            hout.SetPoint(i, co_ord, avg)
            hout.SetPointError(i, 0., avg_err)

        self._logger.info(result_table.format(results=scan_results))
        hout.GetXaxis().SetTitle("z [mm]")
        hout.SetName("Average_Radiation_Length_{}{}Z".format(self._x_coord, self._y_coord))
        hout.SetTitle("Average Radiation Length at ({},{},z)".format(self._x_coord, self._y_coord))
        hout.GetYaxis().SetTitle("#left<X_{0}#right>_{VELO}")
        hout.SetMarkerStyle(20)
        hout.SetMarkerColor(1)
        hout.SetMarkerSize(1.)

        # Only Produce Output as Pdfs if Required (Default is off for LHCbPR)
        if make_pdfs:
            if not os.path.exists(os.path.join(self._out_dir, 'RadLengthVeloScan', 'pdf_files')):  #Let's Be Safe!
                os.makedirs(os.path.join(self._out_dir, 'RadLengthVeloScan', 'pdf_files'))
            c = TCanvas()
            hout.Draw("AP")
            c.Print(os.path.join(self._out_dir, 'RadLengthVeloScan', 'pdf_files', 'Avg_rad_length_vs_z.pdf'))

        output_file = TFile.Open(self._root_plot_file, 'NEW')
        hout.Write()
        output_file.Close()


    def startVeloScan(self, make_pdfs=False):
        '''Initiate the Scan Across the VELO using the Interval Requested'''
        self._logger.info("Starting Scan, Particle Gun will be Positioned at Points:\n %s", [(self._x_coord, self._y_coord, z) for z in self._z_coords])
        for Z in self._z_coords:
            self._runScanAt((self._x_coord,self._y_coord,Z))
        self._mergeOutput()
        self._makePlots(make_pdfs)


if __name__ in "__main__":

    #-------------------------------ARGUMENT PARSER---------------------------------#
    #               Run Test in 'Debug' mode if '--debug' flag set                  #
    #               Options to use GaussGeo or GiGaGeo                              #
    #-------------------------------------------------------------------------------#

    parser = argparse.ArgumentParser('RadLengthArgs')
    parser.add_argument('--debug', action='store_true', help='Run in Debug Mode')
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

    scan_z0_only = rad_length_velo_scan(os.getcwd(), 0., 0., (0,100,10), 2000, args.debug, geo_type)
    scan_z0_only.startVeloScan()
