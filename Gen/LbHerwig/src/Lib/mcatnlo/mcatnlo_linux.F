C $Id: mcatnlo_linux.F,v 1.1 2006-09-05 12:36:50 karl Exp $

C-----------------------------------------------------------------------
C
C  Modified version of file linux.f from MC@NLO 3.2
C
C  Subrouine renamed to avoid clash with Jimmy:
C  delete -> mcatnlo_delete
C
C  Modification: Karl Harrison
C
C-----------------------------------------------------------------------

      subroutine sysdep
c use newver='NEW' for vaxes, 'UNKNOWN' in other machines,
      character * 7 newver
      common/newver/newver
      newver = 'UNKNOWN'
      end

C-- KH modification: start
C-- Name of subroutine changed to avoid clash with Jimmy
C     subroutine delete(fname)
      subroutine mcatnlo_delete(fname)
C-- KH modification: end
      character * 80 str
      character * (*) fname
      l = len(fname)
      k = 1
      dowhile(fname(k:k).eq.' '.and.k.lt.l)
         k = k+1
      enddo
      dowhile(fname(l:l).eq.' '.and.l.gt.k)
         l = l-1
      enddo
      if(l-k.gt.70) then
         write(*,*) 'delete: filename > 70 chars not allowed'
         stop
      endif
      if(l.eq.k) then
         write(*,*) 'delete: void filename'
         stop
      endif
      str(1:) = 'rm '
      str(7:) = fname(k:l)
      call system(str)
      end

c      subroutine idate(i1,i2,i3)
c      common/slate/isl(40)
c      call datime(id,it)
c      i3=id/10000
c      i1=(id-i3*10000)/100
c      i2= id-i3*10000-i1*100
c      end

c      subroutine time(ctime)
c      character * 8 ctime,cday
c      call datimh(cday,ctime)
c      end

