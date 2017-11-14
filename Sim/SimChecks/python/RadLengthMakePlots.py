#################################################################################
## Python script for creation of plots as part of Radiation Length tests       ##
##                                                                             ##
##  @author : K. Zarebski                                                      ##
##  @date   : last modified on 2017-06-09                                      ##
#################################################################################

import ROOT
import os
import json
import logging
logger = logging.getLogger('RadLengthMakePlots')
logging.basicConfig()
logger.setLevel('INFO')

pwd = os.getcwd()

#detectros = ["Velo", "Rich1", "Magnet", "OT1", "OT2", "OT3", "Rich2", "Detached muon", "Ecal", "Hcal", "Muon"]
#name = dicts( zip(range(1,len(detectors)),detectors) )
name = {1: "Velo",
         2: "Rich1",
         3: "Magnet",
         4: "OT1",
         5: "OT2",
         6: "OT3",
         7: "Rich2",
         8: "Detached muon",
         9: "Ecal",
         10: "Hcal",
         11: "Muon"}


def getErrorRMS(hist):
    logger.debug("Calculating Error for Histogram '%s'", hist.GetName())
    return hist.GetRMS() / ROOT.TMath.Sqrt(hist.GetEntries())


def makePlots(fileName="Rad_merged.root", output_dir="plots", plot_type="rad", pdfs_dir=None, data_dir=None, debug='INFO'):

    logger.setLevel(debug)
    ROOT.gROOT.Reset()

    logger.debug("Opening file '%s' for reading", fileName)
    f = ROOT.TFile.Open(fileName)

    logger.debug("Checking for tree")
    tree = f.Get("RadLengthColl/tree")


    try:
        assert tree, "Error Reading Tree from DataFile"
    except:
        logger.error("Could not find tree 'RadLengthColl/tree' in file!")
        raise Exception

    try:
        logger.debug("Creating Directories for output...")
        os.mkdirs(output_dir)
    except:
        logger.debug("Directory exists, continuing...")
	pass


    if plot_type == "rad":
        nplot_type = "Radiation"
        if data_dir:
            logger.debug("Creating LaTeX tables in text files")
            txtfile = open(os.path.join(data_dir, "{}LengthOut.txt".format(plot_type)), "w")
            txtfile.write("Position    \t& n_{X0}^{tot} \\\\ \n")
    elif plot_type == "inter":
        nplot_type = "Interaction"
        if data_dir:
            logger.debug("Creating LaTeX tables in text files")
            txtfile = open(os.path.join(data_dir, "{}LengthOut.txt".format(plot_type)), "w")
            txtfile.write("Position    \t& lambda_{I}^{tot} \\\\ \n")
    else:
        assert False, "Could not write Data Tables to Text File"

    logger.debug("Creating Output ROOT files")
    graphsOut = ROOT.TFile.Open(os.path.join(output_dir, "{}_Length_Plots.root".format(nplot_type)), "NEW")
    try:
       assert graphsOut, "Failed to Create File"
    except:
       logging.error("Could not create ROOT file '%s'", os.path.join(output_dir, "{}_Length_Plots.root".format(nplot_type)))
       raise Exception
    graphsOut.cd()

    nplanes = 11

    logger.debug("Creating new ROOT.TCanvas")
    c = ROOT.TCanvas("RadCanvas", "RadLength Canvas")

    logger.debug("Creating TGraphErrors objects")
    cumul = ROOT.TGraphErrors()
    cumulZ = ROOT.TGraphErrors()
    p2p = ROOT.TGraphErrors()

    p = 0


    p2p_list = []
    cumul_listZ = []
    cumul_list = []


    for i in range(1, 12):

        c.SetLogy()

        logger.debug("Creating Plots for Plane '%s'", name[i])
        select = "ID == {}".format(i)
        namehisto = "Cumulative_{}_Length_{}".format(nplot_type, name[i])
        var = "cum{}lgh>>{}".format(plot_type, namehisto)
        logger.debug("Drawing from Tree: '%s'", var) 
        tree.Draw(var, select)

        logger.debug("Retrieving Histogram")
        h1 = ROOT.gDirectory.Get(namehisto)
        try:
            assert h1.GetEntries() > 0, "Failed to get Histogram"
        except:
            if not h1:
               logger.error("Could not find histogram object '%s'", var)
            elif h1.GetEntries() < 1:
               logger.error("Histogram Contains no Entries!")
            raise Exception

        logger.debug("Setting Histogram Labels")

        if plot_type == "rad":
            h1.GetXaxis().SetTitle("n_{X0}^{tot}")
        else:
            h1.GetXaxis().SetTitle("#lambda_{I}^{tot}")

        h1.GetYaxis().SetTitle("N_{evt}")
        h1.SetTitle("Cumulative {} Length ({})".format(nplot_type, name[i]))

        if pdfs_dir:
            namefile = os.path.join(pdfs_dir, '{}.pdf'.format(namehisto))
            c.Print(namefile)


        logger.debug("Adding Data Point to TGraph: %s %s %s %s %s", p, i, h1.GetMean(), 0, h1.GetMeanError())
        cumul.SetPoint(p, i, h1.GetMean())
        cumul.SetPointError(p, 0, h1.GetMeanError())
        if data_dir:
            logger.debug("Writing Result to Text File")
            txtfile.write('{0:13}'.format(name[i]) + "\t& " + '{:5.4f} \\pm {:5.4f}'.format(h1.GetMean(), h1.GetMeanError()) + " \t \\\\ \n")

        cumul_list.append([i, h1.GetMean(), h1.GetMeanError()])
        namehisto = "Zposition_{}_{}".format(nplot_type, name[i])
        var = "Zpos>>{}".format(namehisto)
        logger.debug("Drawing from Tree: '%s'", var) 
        tree.Draw(var, select)
        logger.debug("Retrieving Histogram")
        hZ = ROOT.gDirectory.Get(namehisto)
        try:
            assert hZ.GetEntries() > 0, "Failed to get Histogram"
        except:
            if not hZ:
               logger.error("Could not find histogram object '%s'", var)
            elif hZ.GetEntries() < 1:
               logger.error("Histogram Contains no Entries!")
            raise Exception
        logger.debug("Adding Data Point to TGraph: %s %s %s %s %s", p, hZ.GetMean(), h1.GetMean(), getErrorRMS(hZ), getErrorRMS(h1))
        cumulZ.SetPoint(p, hZ.GetMean(), h1.GetMean())
        cumulZ.SetPointError(p, getErrorRMS(hZ), getErrorRMS(h1))

        cumul_listZ.append([hZ.GetMean(), h1.GetMean(), getErrorRMS(hZ), getErrorRMS(h1)])
        namehisto = "Plane2Plane_{}_{}".format(nplot_type, name[i])
        var = "p2p{}lgh >> {}".format(plot_type, namehisto)
        logger.debug("Drawing from Tree: '%s'", var) 
        tree.Draw(var, select)
        logger.debug("Retrieving Histogram")
        h2 = ROOT.gDirectory.Get(namehisto)
        try:
            assert h2.GetEntries() > 0, "Failed to get Histogram"
        except:
            if not h2:
               logger.error("Could not find histogram object '%s'", var)
            elif h2.GetEntries() < 1:
               logger.error("Histogram Contains no Entries!")
            raise Exception
        if(plot_type == "rad"):
            h2.GetXaxis().SetTitle("n_{X0}^{p2p}")
        else:
            h2.GetXaxis().SetTitle("#lambda_{I}^{p2p}")
        h2.GetYaxis().SetTitle("N_{evt}")
        h2.SetTitle("Plane-to-Plane {} Length ({})".format(nplot_type, name[i]))
        p2p.SetPoint(p, i, h2.GetMean())
        p2p.SetPointError(p, 0, getErrorRMS(h2))
        logger.debug("Adding Data Point to TGraph: %s %s %s %s %s", p, i, h2.GetMean(), 0, getErrorRMS(h2))
        if pdfs_dir:
          namefile = pdfs_dir + namehisto + ".pdf"
          c.Print(namefile)
        p2p_list.append([i, h2.GetMean(), getErrorRMS(h2)])

        c.SetLogy(0)
        ROOT.gStyle.SetOptStat(0)
        namehisto = 'Cumulative_{}_Length_2DScan_EtaPhi_{}'.format(nplot_type, name[i])
        var = "cum{}lgh:eta:phi>>{}(100,-3.3,3.3,100,2.,5.)".format(plot_type, namehisto)
        logger.debug("Drawing from Tree: '%s'", var) 
        tree.Draw(var, select, "profs")
        logger.debug("Retrieving Histogram")
        hh = ROOT.gDirectory.Get(namehisto)
        try:
            assert hh.GetEntries() > 0, "Failed to get Histogram"
        except:
            if not hh:
               logger.error("Could not find histogram object '%s'", var)
            elif hh.GetEntries() < 1:
               logger.error("Histogram Contains no Entries!")
            raise Exception
        logger.debug("Setting Histogram Labels")
        hh.GetXaxis().SetTitle("#phi")
        hh.GetYaxis().SetTitle("#eta")
        hh.SetTitle("Cumulative {} Length ({})".format(nplot_type, name[i]))
        hh.Draw("colz")
        namehisto = 'Plane2Plane_{}_Length_Profile_{}'.format(nplot_type, name[i])
        var = "p2p{}lgh:eta:phi>>{}(100,-3.3,3.3,100,2.,5.)".format(plot_type, namehisto)
        logger.debug("Drawing from Tree: '%s'", var) 
        tree.Draw(var, select, "profs")
        logger.debug("Retrieving Histogram")
        hh2 = ROOT.gDirectory.Get(namehisto)
        try:
            assert hh2.GetEntries() > 0, "Failed to get Histogram"
        except:
            if not hh2:
               logger.error("Could not find histogram object '%s'", var)
            elif hh2.GetEntries() < 1:
               logger.error("Histogram Contains no Entries!")
            raise Exception
        logger.debug("Setting Histogram Labels")
        hh2.GetXaxis().SetTitle("#phi")
        hh2.GetYaxis().SetTitle("#eta")
        hh2.SetTitle("Plane-to-Plane {} Length ({})".format(nplot_type, name[i]))
        hh2.Draw("colz")
        if pdfs_dir:
            namefile = os.path.join(pdfs_dir, '{}.pdf'.format(namehisto))
            c.Print(namefile)
        ROOT.gStyle.SetOptStat(0)

        logger.debug("Writing Histograms")
        hh.Write()
        h2.Write()
        hh2.Write()
        h1.Write()
        hZ.Write()

        p += 1

    if data_dir:
        logger.debug("Writing JSON strings")
        file_p2p = open(os.path.join(data_dir, 'p2p_{}length.json'.format(plot_type)), 'w')
        file_p2p.write(json.dumps(p2p_list))

        file_cumulz = open(os.path.join(data_dir, 'cumulz_{}length.json'.format(plot_type)), 'w')
        file_cumulz.write(json.dumps(cumul_listZ))
        file_cumul = open(os.path.join(data_dir,'cumul_{}length.json'.format(plot_type)), 'w')
        file_cumul.write(json.dumps(cumul_list))
    ROOT.gStyle.SetOptStat(0)
    c.SetLogy()

    logger.debug("Drawing from Tree: '%s' with cut '%s'", "cum{}lgh:eta>>hh1(100,2,5)".format(plot_type), 'ID == 11') 
    tree.Draw("cum{}lgh:eta>>hh1(100,2,5)".format(plot_type), "ID == 11", "prof")
    radlgh_eta = ROOT.gDirectory.Get("hh1")
    try:
       assert radlgh_eta.GetEntries() > 0, "Failed to get Histogram"
    except:
       if not radlgh_eta:
          logger.error("Could not find histogram object '%s'", "Drawing from Tree: '%s' with cut '%s'", "cum{}lgh:eta>>hh1(100,2,5)".format(plot_type))
       elif radlgh_eta.GetEntries() < 1:
          logger.error("Histogram Contains no Entries!")
       raise Exception
    radlgh_eta.SetName("Cumulative_{}_Length_vs_Eta".format(nplot_type))
    logger.debug("Setting TGraphErrors Labels")
    cumul.SetName("Cumulative_{}_Length_vs_ScoringPlaneID".format(nplot_type))
    cumul.GetXaxis().SetTitle("Scoring Plane ID")
    cumul.GetYaxis().SetTitle("<n_{X0}^{tot}>")
    cumul.SetTitle("Cumulative {} Length".format(nplot_type))
    cumulZ.SetName("Cumulative_{}_Length_vs_Zpos".format(nplot_type))
    cumulZ.GetXaxis().SetTitle("Z/mm")
    cumulZ.GetYaxis().SetTitle("<n_{X0}^{tot}>")
    cumulZ.SetTitle("Cumulative {} Length".format(nplot_type))
    p2p.GetXaxis().SetTitle("ID plane")
    p2p.SetName("Plane2Plane_{}_Length_vs_ScoringPlaneID".format(nplot_type))
    p2p.GetYaxis().SetTitle("<n_{X0}^{p2p}>")
    p2p.SetTitle("Plane-to-Plane {} Length".format(nplot_type))
    radlgh_eta.GetXaxis().SetTitle("#eta")
    radlgh_eta.GetYaxis().SetTitle("<n_{X0}^{tot}>")
    radlgh_eta.SetTitle("Cumulative {} Length ".format(nplot_type))
    cumul.SetMarkerStyle(22)
    cumul.SetMarkerSize(0.8)
    cumul.SetMarkerColor(1)
    cumulZ.SetMarkerStyle(22)
    cumulZ.SetMarkerSize(0.8)
    cumulZ.SetMarkerColor(1)
    p2p.SetMarkerStyle(22)
    p2p.SetMarkerSize(0.8)
    p2p.SetMarkerColor(1)
    radlgh_eta.SetMarkerStyle(22)
    radlgh_eta.SetMarkerSize(0.8)
    radlgh_eta.SetMarkerColor(1)
    if pdfs_dir:
        p2p.Draw("AP")
        c.Print(os.path.join(pdfs_dir, "p2p_" + plot_type + "Length.pdf"))
        cumul.Draw("AP")
        c.Print(os.path.join(pdfs_dir, "cum" + plot_type + "Length.pdf"))
        cumulZ.Draw("AP")
        c.Print(os.path.join(pdfs_dir, "cum" + plot_type + "Length_vs_Z.pdf"))
        radlgh_eta.Draw()
        c.Print(os.path.join(pdfs_dir, "cum" + plot_type + "Length_vs_eta.pdf"))


    logger.debug("Writing Graphs")
    p2p.Write()
    radlgh_eta.Write()
    cumulZ.Write()
    cumul.Write()

    graphsOut.Write()
    graphsOut.Close()

if __name__ == "__main__":

    import sys

    fileName = "Rad_merged.root"
    outpath = "plots/"
    plot_type = "rad"

    args = 0
    for ag in sys.argv:
        if(ag == "-inter"):
            plot_type = "inter"
            args += 1
        if(ag == "-f"):
            args += 2
            if(os.path.isfile(sys.argv[args])):
                fileName = sys.argv[args]
            else:
                print "File", sys.argv[args], "not found!"
                sys.exit()
        if(ag == "-p"):
            args += 2
            outpath = sys.argv[args]

    makePlots(fileName, outpath, plot_type)
