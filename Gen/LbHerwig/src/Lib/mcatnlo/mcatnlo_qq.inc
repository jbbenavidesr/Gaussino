C $Id: mcatnlo_qq.inc,v 1.1 2006-09-05 12:36:50 karl Exp $

C-----------------------------------------------------------------------
C
C Declarations and common blocks from
C PROGRAM MCATNLO_QQMAIN of MC@NLO 3.2
C
C Common block MCNLOPAR introduced to collect variables
C shared by new subroutines MCATNLO_QQINIT and MCATNLO_QQGEN
C
C Modifications: Karl Harrison
C
C-----------------------------------------------------------------------

      implicit none
      real * 8 
     #  value(20),ecmlst(100),
     #  xmlst(100),fh1lst(100),fh2lst(100),renlst(100),
     #  fh1mclst(100),fh2mclst(100),renmclst(100)
      real * 8 
     #  xm,xpdflam4,xpdflam5,tmp,xfh,xfh1,xfh2,ecm,xren,betfac,
     #  delta,deltas,deltac,dtot,avtot,ac1,ac2,xtotal,ytotal,av3a,
     #  av3nega,d3a,d3nega,av3b,av3negb,d3b,d3negb,ctime,pi,tmas,
     #  etacut,alsf,besf,alcl,becl,gacl,wgtaev,wgtbev,dummy,xmone,
     #  xfhmc,xrenmc,evfrac,evprcfrac,xares,yares,xbres,ybres
      integer 
     #  ih1,ih2,ndns1,ndns2,jloop,iseld,nlf,ncl3,jecm,
     #  loproc,maproc,iproc,iinput,iverbose,ichkmom,
     #  ibswrite,itmpih,itmpndns,idpdfset,ipdfih,ipdfgroup,ipdfndns,
     #  ifk88istrl,ifk88ih,ifk88ndns,maxevt,it1,it2,ifuntype,
     #  ndim,nwild,itd1,itd2,ibscall,iwgtnorm,iseed0,
     #  iseed,maxtrials,mode,lo,isubttype,iprespl,iwrong,iwrong1,
     #  ntotal,ndiff,nevts,ntrls,itot,ionshell,iunita,iunitb,
     #  ioutput,ii,iunit,i,itmpqq,itmpvv,mx_of_evta,mx_of_evtb,
     #  nlfp1sch,nlas,ia1ora2,iasmc,iassoft,ifk88seed,ip4,ip5,
     #  izero,ione
      character * 2 scheme,xproc(3)
      character * 4 part1,part2
      character * 20 parm(20),gname
      character * 80 fname,fnamea,fnameb,fname1,fnamev
      character * 80 pref,prefn,prefev,prefnev
      character * 70 strin,strout,lhapdf
      logical iphflag,evgen
      external sig5a,sig5b
      parameter (pi=3.14159265358979312D0)
      parameter (xmone=-1.d0)
      parameter (izero=0)
      parameter (ione=1)
      include 'hvqcblks.h'
c
c common /strfun0/ is only in strfun:
c ndns = pdf type
c ih1,ih2 = beam type (0=(p+n)/2, 1=p, -1=pbar, 2=n, -2=nbar)
      common/strfun0/ih1,ih2,ndns1,ndns2
c quark and gluon masses, used by Herwig. PDF labeling convention
      real*8 xmass(-5:21)
      common/parmass/xmass
c alsf and besf are the parameters that control gfunsoft
      common/cgfunsfp/alsf,besf
c alcl, becl and gacl are the parameters that control gfuncoll
      common/cgfunclp/alcl,becl,gacl
c iwgtnorm=0 for weight=+1/-1, iwgtnorm=1 otherwise
      common/ciwgtnorm/iwgtnorm
c wgtaev and wgtbev are the norms of weights for H and S events respectively
      common/cwgtev/wgtaev,wgtbev
c iprespl=0 ==> preserves rapidity
c iprespl=1 ==> preserves longitudinal momentum
      common/ciprespl/iprespl
c ichkmom=0 --> enables checks on kinematics
      common/cichkmom/ichkmom
c----------------------------------------------------------
c Variables that control the integrations
c
      common/cisubttype/isubttype
      common/betfac/betfac,delta
      common/pmerge/deltas,deltac
c etacut is the maximum allowed for [2*kt(gluon)/sqrt(shat)]^2
      common/cetacut/etacut
      integer nsamp
      common/samp/nsamp
c----------------------------------------------------------
c nlfp1sch=0 --> use nl light flavours, nlfp1sch=1 --> nl+1 scheme
      common/cnlfp1sch/nlfp1sch
c nlas is the number of light flavours used in the computation of alpha_S
      common/cnlas/nlas
c Heavy quarks identities according to MC particle numbering scheme; used 
c when writing the event file
      common/ci2part/ip4,ip5
c----------------------------------------------------------
c The following refer to the computation of MC subtraction terms
c ia1ora2=1 -> full invariants, ia1ora2=2 -> simplified invariants
      common/cia1ora2/ia1ora2
c iasmc=1 -> as_nlo**3, iasmc=2 -> as_mc**3, iasmc=3 -> as_mc*as_nlo**2
      common/ciasmc/iasmc
c iassoft=0 -> as_nlo(hard), iassoft=1 -> as_nlo(soft)
      common/ciassoft/iassoft
c----------------------------------------------------------
c Subprocesses: prc = 'gg', 'qq', 'qg', corresponding to jproc=1,2,3
c and equal to xproc(jproc)
      common/cxproc/xproc
c In the integration routines, loproc<=jproc<=maproc
      common/cwchproc/loproc,maproc
c Number of failures in flavour determination
      common/ciwrong/iwrong,iwrong1
c----------------------------------------------------------
c ifuntype=1 for sig5a, ifuntype=2 for sig5b
      common/cifuntype/ifuntype
c Flag to put partons on shell, according to Herwig list of masses
      common/cionshell/ionshell
c Common blocks for general MC@NLO routines
c common block for internal rnd number generation, independent of bases
      common/cifk88seed/ifk88seed
c common block fk88ipdfs is filled by our interface to MLMPDF
      common/fk88ipdfs/ifk88ih,ifk88ndns
c common block w50511 and w50512 are filled by PDFLIB 
      common/w50511/ipdfih,ipdfgroup,ipdfndns,mode,nlf,lo,tmas
      common/w50512/xpdflam4,xpdflam5
c----------------------------------------------------------
c 
      common/mcnlopar/ac1,ac2,ecm,ecmlst,fh1lst,fh1mclst,fh2lst,
     &  fh2mclst,renlst,renmclst,xfh,xfhmc,xm,xmlst,xren,xrenmc,
     &  ibswrite,idpdfset,iproc,iseed,iseed0,iseld,it1,it2,itmpvv,
     &  iverbose,jecm,maxevt,maxtrials,ncl3,
     &  evgen,
     &  gname,part1,part2,pref,prefev,scheme
