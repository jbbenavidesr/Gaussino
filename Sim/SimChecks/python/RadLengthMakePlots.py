#################################################################################
## Python script for creation of plots as part of Radiation Length tests       ##
##                                                                             ##
##  @author : K. Zarebski                                                      ##
##  @date   : last modified on 2016-12-15                                      ##
#################################################################################

from ROOT import *
import os
import json

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
    return hist.GetRMS() / TMath.Sqrt(hist.GetEntries())


def makePlots(fileName="Rad_merged.root", path="plots/", type="rad"):

    gROOT.Reset()

    f = TFile.Open(fileName)

    tree = f.Get("RadLengthColl/tree")

    assert tree, "Error Reading Tree from DataFile"
    #    fout = TFile.Open("%s/Rad_length/root_files/RadLengthOut.root" % pwd,"recreate")

    os.mkdirs(path)

    txtfile = open("{}/Rad_length/data_tables/{}LengthOut.txt".format(pwd, type), "w")

    graphsOut = TFile.Open("{}/Rad_length/root_files/{}RadPlots.root".format(pwd, type), "recreate")
    graphsOut.cd()

    nplanes = 11

    c = TCanvas("RadCanvas", "RadLength Canvas")

    cumul = TGraphErrors()
    cumulZ = TGraphErrors()
    p2p = TGraphErrors()

    p = 0


    if type == "rad":
        ntype = " radiation "
        txtfile.write("Position    \t& n_{X0}^{tot} \\\\ \n")
    elif type == "inter":
        ntype = " interaction "
        txtfile.write("Position    \t& lambda_{I}^{tot} \\\\ \n")
    else:
        assert False

    p2p_list = []
    cumul_listZ = []
    cumul_list = []


    for i in range(1, 12):

    #    fout.cd()
        c.SetLogy()

        select = "ID == {}".format(i)
        namehisto = "cum_{}lgh_ID{}".format(type, i)
        var = "cum{}lgh>>{}".format(type, namehisto)
        tree.Draw(var, select)

        h1 = gPad.GetPrimitive(namehisto)

        if type == "rad":
            h1.GetXaxis().SetTitle("n_{X0}^{tot}")
        else:
            h1.GetXaxis().SetTitle("#lambda_{I}^{tot}")

        h1.GetYaxis().SetTitle("N_{evt}")
        h1.SetTitle("Cumulative {} length ({})".format(ntype, name[i]))
        namefile = os.path.join(path, namehisto + ".pdf")

        cumul.SetName("cuml_{}_lengthID".format(ntype))
        cumul.SetPoint(p, i, h1.GetMean())
        cumul.SetPointError(p, 0, h1.GetMeanError())
        txtfile.write('{0:13}'.format(name[i]) + "\t& " + '{:5.4f} \\pm {:5.4f}'.format(h1.GetMean(), h1.GetMeanError()) + " \t \\\\ \n")

        cumul_list.append([i, h1.GetMean(), h1.GetMeanError()])
        namehisto = "Z_" + type + "_ID" + str(i)
        var = "Zpos>>" + namehisto
        tree.Draw(var, select)
        hZ = gPad.GetPrimitive(namehisto)
        cumulZ.SetName("cuml_{}_lengthZ".format(ntype))
        cumulZ.SetPoint(p, hZ.GetMean(), h1.GetMean())
        cumulZ.SetPointError(p, getErrorRMS(hZ), getErrorRMS(h1))

        cumul_listZ.append([hZ.GetMean(), h1.GetMean(), getErrorRMS(hZ), getErrorRMS(h1)])
        namehisto = "p2p_" + type + "lgh_ID" + str(i)
        var = "p2p" + type + "lgh>>" + namehisto
        tree.Draw(var, select)
        h2 = gPad.GetPrimitive(namehisto)
        if(type == "rad"):
            h2.GetXaxis().SetTitle("n_{X0}^{p2p}")
        else:
            h2.GetXaxis().SetTitle("#lambda_{I}^{p2p}")
        h2.GetYaxis().SetTitle("N_{evt}")
        h2.SetTitle("Plane-to-plane " + ntype + " length (" + name[i] + ")")
        p2p.SetName("p2p_{}_lengthID".format(ntype))
        p2p.SetPoint(p, i, h2.GetMean())
        p2p.SetPointError(p, 0, getErrorRMS(h2))
        namefile = path + namehisto + ".pdf"
        p2p_list.append([i, h2.GetMean(), getErrorRMS(h2)])
        #c.Print(namefile)

        c.SetLogy(0)
        gStyle.SetOptStat(0)
        namehisto = type + "lgh_prof_ID" + str(i)
        var = "cum" + type + "lgh:eta:phi>>" + namehisto + "(100,-3.3,3.3,100,2.,5.)"
        tree.Draw(var, select, "profs")
        hh = gPad.GetPrimitive(namehisto)
        hh.GetXaxis().SetTitle("#phi")
        hh.GetYaxis().SetTitle("#eta")
        hh.SetTitle("Cumulative " + ntype + " length (" + name[i] + ")")
        hh.Draw("colz")
        namefile = path + namehisto + ".pdf"
        c.Print(namefile)
        gStyle.SetOptStat(0)

        hh.Write()
        h2.Write()
        h1.Write()
        hZ.Write()
        #namehisto = "etaphi_ID"+str(i)
        #var = "eta:phi>>" + namehisto# + "(80,-3.3,3.3,80,2.,5.)"
        #tree.Draw(var,select,"colz")
        #hhetaphi = gPad.GetPrimitive(namehisto)
        #hhetaphi.SetTitle(name[i])
        #hhetaphi.GetXaxis().SetTitle("#phi")
        #hhetaphi.GetYaxis().SetTitle("#eta")
        #namefile = path + namehisto + ".pdf"
        #c.Print(namefile)

        p += 1

    file_p2p = open('%s/Rad_length/data_tables/p2p_%slength.json' % (pwd, type), 'w')
    file_p2p.write(json.dumps(p2p_list))

    file_cumulz = open('%s/Rad_length/data_tables/cumulz_%slength.json' % (pwd, type), 'w')
    file_cumulz.write(json.dumps(cumul_listZ))
    file_cumul = open('%s/Rad_length/data_tables/cumul_%slength.json' % (pwd, type), 'w')
    file_cumul.write(json.dumps(cumul_list))
    gStyle.SetOptStat(0)
    c.SetLogy()

    tree.Draw("cum" + type + "lgh:eta>>hh1(100,2,5)", "ID == 11", "prof")
    radlgh_eta = gPad.GetPrimitive("hh1")
    radlgh_eta.SetName("cuml_{}_lengthEta".format(ntype))
    cumul.GetXaxis().SetTitle("ID plane")
    cumul.GetYaxis().SetTitle("<n_{X0}^{tot}>")
    cumul.SetTitle("Cumulative " + ntype + " length")
    cumulZ.GetXaxis().SetTitle("Z (mm)")
    cumulZ.GetYaxis().SetTitle("<n_{X0}^{tot}>")
    cumulZ.SetTitle("Cumulative " + ntype + " length")
    p2p.GetXaxis().SetTitle("ID plane")
    p2p.GetYaxis().SetTitle("<n_{X0}^{p2p}>")
    p2p.SetTitle("Plane-to-plane " + ntype + " length")
    radlgh_eta.GetXaxis().SetTitle("#eta")
    radlgh_eta.GetYaxis().SetTitle("<n_{X0}^{tot}>")
    radlgh_eta.SetTitle("Cumulative " + ntype + " length")
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
    c.Print(path + "p2p_" + type + "Length.pdf")
    cumul.Draw("AP")
    c.Print(path + "cum" + type + "Length.pdf")
    cumulZ.Draw("AP")
    c.Print(path + "cum" + type + "Length_vs_Z.pdf")
    radlgh_eta.Draw()
    c.Print(path + "cum" + type + "Length_vs_eta.pdf")

    p2p.Write()
    radlgh_eta.Write()
    cumulZ.Write()
    cumul.Write()

    graphsOut.Write()
    graphsOut.Close()
    #fout.Write()
    #fout.Close()





if __name__ == "__main__":

    import sys

    fileName = "Rad_merged.root"
    outpath = "plots/"
    type = "rad"

    args = 0
    for ag in sys.argv:
        if(ag == "-inter"):
            type = "inter"
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

    makePlots(fileName, outpath, type)
