#################################################################################
## Python script for creation of plots as part of Radiation Length tests       ##
##                                                                             ##
##  @author : K. Zarebski                                                      ##
##  @date   : last modified on 2017-06-09                                      ##
#################################################################################

from ROOT import *
import os
import json
import logging
logger = logging.getLogger('RADLENGTHMAKEPLOTS')
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
    return hist.GetRMS() / TMath.Sqrt(hist.GetEntries())


def makePlots(fileName="Rad_merged.root", path="plots/", type_="rad"):

    gROOT.Reset()

    logger.debug("Opening file '%s' for reading", fileName)
    f = TFile.Open(fileName)

    logger.debug("Checking for tree")
    tree = f.Get("RadLengthColl/tree")


    try:
        assert tree, "Error Reading Tree from DataFile"
    except:
        logger.error("Could not find tree 'RadLengthColl/tree' in file!")
        raise Exception

    try:
        logger.debug("Creating Directories for output...")
        os.mkdirs(path)
    except:
        logger.debug("Directory exists, continuing...")
	pass

    logger.debug("Creating LaTeX tables in text files")
    txtfile = open("{}/Rad_length/data_tables/{}LengthOut.txt".format(pwd, type_), "w")

    logger.debug("Creating Output ROOT files")
    graphsOut = TFile.Open(os.path.join(pwd,"Rad_length/root_files", "{}RadPlots.root".format(type_)), "recreate")
    try:
       assert graphsOut, "Failed to Create File"
    except:
       logging.error("Could not create ROOT file '%s'", os.path.join(pwd,"Rad_length/root_files", "{}RadPlots.root".format(type_)))
       raise Exception
    graphsOut.cd()

    nplanes = 11

    logger.debug("Creating new TCanvas")
    c = TCanvas("RadCanvas", "RadLength Canvas")

    logger.debug("Creating TGraphErrors objects")
    cumul = TGraphErrors()
    cumulZ = TGraphErrors()
    p2p = TGraphErrors()

    p = 0


    if type_ == "rad":
        ntype_ = " radiation "
        txtfile.write("Position    \t& n_{X0}^{tot} \\\\ \n")
    elif type_ == "inter":
        ntype_ = " interaction "
        txtfile.write("Position    \t& lambda_{I}^{tot} \\\\ \n")
    else:
        assert False

    p2p_list = []
    cumul_listZ = []
    cumul_list = []


    for i in range(1, 12):

        c.SetLogy()

        logger.debug("Creating Plots for Plane '%s'", name[i])
        select = "ID == {}".format(i)
        namehisto = "cumulative_{}length_{}".format('radiation' if type_ == 'rad' else 'interaction', name[i])
        var = "cum{}lgh>>{}".format(type_, namehisto)
        logger.debug("Drawing from Tree: '%s'", var) 
        tree.Draw(var, select)

        logger.debug("Retrieving Histogram")
        h1 = gDirectory.Get(namehisto)
        try:
            assert h1.GetEntries() > 0, "Failed to get Histogram"
        except:
            if not h1:
               logger.error("Could not find histogram object '%s'", var)
            elif h1.GetEntries() < 1:
               logger.error("Histogram Contains no Entries!")
            raise Exception

        logger.debug("Setting Histogram Labels")

        if type_ == "rad":
            h1.GetXaxis().SetTitle("n_{X0}^{tot}")
        else:
            h1.GetXaxis().SetTitle("#lambda_{I}^{tot}")

        h1.GetYaxis().SetTitle("N_{evt}")
        h1.SetTitle("Cumulative {} length ({})".format(ntype_, name[i]))

        namefile = os.path.join(path, '{}.pdf'.format(namehisto))



        logger.debug("Adding Data Point to TGraph: %s %s %s %s %s", p, i, h1.GetMean(), 0, h1.GetMeanError())
        cumul.SetPoint(p, i, h1.GetMean())
        cumul.SetPointError(p, 0, h1.GetMeanError())
        logger.debug("Writing Result to Text File")
        txtfile.write('{0:13}'.format(name[i]) + "\t& " + '{:5.4f} \\pm {:5.4f}'.format(h1.GetMean(), h1.GetMeanError()) + " \t \\\\ \n")

        cumul_list.append([i, h1.GetMean(), h1.GetMeanError()])
        namehisto = "Z_" + type_ + "_ID" + str(i)
        var = "Zpos>>" + namehisto
        logger.debug("Drawing from Tree: '%s'", var) 
        tree.Draw(var, select)
        logger.debug("Retrieving Histogram")
        hZ = gDirectory.Get(namehisto)
        try:
            assert hZ.GetEntries() > 0, "Failed to get Histogram"
        except:
            if not hZ:
               logger.error("Could not find histogram object '%s'", var)
            elif hZ.GetEntries() < 1:
               logger.error("Histogram Contains no Entries!")
            raise Exception
        namehisto = "Z_" + type_ + "_ID" + str(i)
        var = "Zpos>>" + namehisto
        logger.debug("Drawing from Tree: '%s'", var) 
        tree.Draw(var, select)
        logger.debug("Retrieving Histogram")
        hZ = gDirectory.Get(namehisto)
        logger.debug("Adding Data Point to TGraph: %s %s %s %s %s", p, hZ.GetMean(), h1.GetMean(), getErrorRMS(hZ), getErrorRMS(h1))
        cumulZ.SetPoint(p, hZ.GetMean(), h1.GetMean())
        cumulZ.SetPointError(p, getErrorRMS(hZ), getErrorRMS(h1))

        cumul_listZ.append([hZ.GetMean(), h1.GetMean(), getErrorRMS(hZ), getErrorRMS(h1)])
        namehisto = "p2p_" + type_ + "lgh_ID" + str(i)
        var = "p2p" + type_ + "lgh>>" + namehisto
        logger.debug("Drawing from Tree: '%s'", var) 
        tree.Draw(var, select)
        logger.debug("Retrieving Histogram")
        h2 = gDirectory.Get(namehisto)
        try:
            assert h2.GetEntries() > 0, "Failed to get Histogram"
        except:
            if not h2:
               logger.error("Could not find histogram object '%s'", var)
            elif h2.GetEntries() < 1:
               logger.error("Histogram Contains no Entries!")
            raise Exception
        if(type_ == "rad"):
            h2.GetXaxis().SetTitle("n_{X0}^{p2p}")
        else:
            h2.GetXaxis().SetTitle("#lambda_{I}^{p2p}")
        h2.GetYaxis().SetTitle("N_{evt}")
        h2.SetTitle("Plane-to-plane " + ntype_ + " length (" + name[i] + ")")
        p2p.SetPoint(p, i, h2.GetMean())
        p2p.SetPointError(p, 0, getErrorRMS(h2))
        logger.debug("Adding Data Point to TGraph: %s %s %s %s %s", p, i, h2.GetMean(), 0, getErrorRMS(h2))
        namefile = path + namehisto + ".pdf"
        p2p_list.append([i, h2.GetMean(), getErrorRMS(h2)])

        c.SetLogy(0)
        gStyle.SetOptStat(0)
        namehisto = 'cumul_{}_length_profile_{}'.format('radiation' if type_ == 'rad' else 'interaction', name[i])
        var = "cum" + type_ + "lgh:eta:phi>>" + namehisto + "(100,-3.3,3.3,100,2.,5.)"
        logger.debug("Drawing from Tree: '%s'", var) 
        tree.Draw(var, select, "profs")
        logger.debug("Retrieving Histogram")
        hh = gDirectory.Get(namehisto)
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
        hh.SetTitle("Cumulative {} Length ({})".format('Radiation' if ntype_ == 'rad' else 'Interaction', name[i]))
        hh.Draw("colz")
        namehisto = 'p2p_{}_length_profile_{}'.format('radiation' if type_ == 'rad' else 'interaction', name[i])
        var = "p2p" + type_ + "lgh:eta:phi>>" + namehisto + "(100,-3.3,3.3,100,2.,5.)"
        logger.debug("Drawing from Tree: '%s'", var) 
        tree.Draw(var, select, "profs")
        logger.debug("Retrieving Histogram")
        hh2 = gDirectory.Get(namehisto)
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
        hh2.SetTitle("Plane-to-Plane {} Length ({})".format('Radiation' if ntype_ == 'rad' else 'Interaction', name[i]))
        hh2.Draw("colz")
        namefile = os.path.join(path, '{}.pdf'.format(namehisto))
        c.Print(namefile)
        gStyle.SetOptStat(0)

        logger.debug("Writing Histograms")
        hh.Write()
        h2.Write()
        hh2.Write()
        h1.Write()
        hZ.Write()

        p += 1

    logger.debug("Writing JSON strings")
    file_p2p = open('%s/Rad_length/data_tables/p2p_%slength.json' % (pwd, type_), 'w')
    file_p2p.write(json.dumps(p2p_list))

    file_cumulz = open('%s/Rad_length/data_tables/cumulz_%slength.json' % (pwd, type_), 'w')
    file_cumulz.write(json.dumps(cumul_listZ))
    file_cumul = open('%s/Rad_length/data_tables/cumul_%slength.json' % (pwd, type_), 'w')
    file_cumul.write(json.dumps(cumul_list))
    gStyle.SetOptStat(0)
    c.SetLogy()

    logger.debug("Drawing from Tree: '%s' with cut '%s'", "cum" + type_ + "lgh:eta>>hh1(100,2,5)", 'ID == 11') 
    tree.Draw("cum" + type_ + "lgh:eta>>hh1(100,2,5)", "ID == 11", "prof")
    radlgh_eta = gDirectory.Get("hh1")
    try:
       assert radlgh_eta.GetEntries() > 0, "Failed to get Histogram"
    except:
       if not radlgh_eta:
          logger.error("Could not find histogram object '%s'", "Drawing from Tree: '%s' with cut '%s'", "cum" + type_ + "lgh:eta>>hh1(100,2,5)")
       elif radlgh_eta.GetEntries() < 1:
          logger.error("Histogram Contains no Entries!")
       raise Exception
    radlgh_eta.SetName("cumulative_{}_length_vs_Eta".format(ntype_))
    logger.debug("Setting TGraphErrors Labels")
    cumul.SetName("cumulative_{}_length_vs_ID".format(ntype_))
    cumul.GetXaxis().SetTitle("ID plane")
    cumul.GetYaxis().SetTitle("<n_{X0}^{tot}>")
    cumul.SetTitle("Cumulative " + ntype_ + " length")
    cumulZ.SetName("cumulative_{}_length_vs_Z".format(ntype_))
    cumulZ.GetXaxis().SetTitle("Z (mm)")
    cumulZ.GetYaxis().SetTitle("<n_{X0}^{tot}>")
    cumulZ.SetTitle("Cumulative " + ntype_ + " length")
    p2p.GetXaxis().SetTitle("ID plane")
    p2p.SetName("p2p_{}_length_vs_ID".format(ntype_))
    p2p.GetYaxis().SetTitle("<n_{X0}^{p2p}>")
    p2p.SetTitle("Plane-to-plane " + ntype_ + " length")
    radlgh_eta.GetXaxis().SetTitle("#eta")
    radlgh_eta.GetYaxis().SetTitle("<n_{X0}^{tot}>")
    radlgh_eta.SetTitle("Cumulative " + ntype_ + " length")
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
    p2p.Draw("AP")
    c.Print(os.path.join(path, "p2p_" + type_ + "Length.pdf"))
    cumul.Draw("AP")
    c.Print(os.path.join(path, "cum" + type_ + "Length.pdf"))
    cumulZ.Draw("AP")
    c.Print(os.path.join(path, "cum" + type_ + "Length_vs_Z.pdf"))
    radlgh_eta.Draw()
    c.Print(os.path.join(path, "cum" + type_ + "Length_vs_eta.pdf"))


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
    type_ = "rad"

    args = 0
    for ag in sys.argv:
        if(ag == "-inter"):
            type_ = "inter"
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

    makePlots(fileName, outpath, type_)
