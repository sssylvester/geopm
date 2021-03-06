!
! MVAPICH2
!
! Copyright 2003-2016 The Ohio State University.
! Portions Copyright 1999-2002 The Regents of the University of
! California, through Lawrence Berkeley National Laboratory (subject to
! receipt of any required approvals from U.S. Dept. of Energy).
! Portions copyright 1993 University of Chicago.
!
! Redistribution and use in source and binary forms, with or without
! modification, are permitted provided that the following conditions are
! met:
!
! (1) Redistributions of source code must retain the above copyright
! notice, this list of conditions and the following disclaimer.
!
! (2) Redistributions in binary form must reproduce the above copyright
! notice, this list of conditions and the following disclaimer in the
! documentation and/or other materials provided with the distribution.
!
! (3) Neither the name of The Ohio State University, the University of
! California, Lawrence Berkeley National Laboratory, The University of
! Chicago, Argonne National Laboratory, U.S. Dept. of Energy nor the
! names of their contributors may be used to endorse or promote products
! derived from this software without specific prior written permission.
!
! THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
! "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
! LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
! A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
! OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
! SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
! LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
! DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
! THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
! (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
! OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
!

! This file created from test/mpi/f77/datatype/hindex1f.f with f77tof90
! -*- Mode: Fortran; -*-
!
!
!  (C) 2011 by Argonne National Laboratory.
!      See COPYRIGHT in top-level directory.
!
      program main
      use mpi
      integer errs, ierr, intsize
      integer i, displs(10), counts(10), dtype
      integer bufsize
      parameter (bufsize=100)
      integer inbuf(bufsize), outbuf(bufsize), packbuf(bufsize)
      integer position, len, psize
!
!     Test for hindexed;
!
      errs = 0
      call mtest_init( ierr )

      call mpi_type_size( MPI_INTEGER, intsize, ierr )

      do i=1, 10
         displs(i) = (10-i)*intsize
         counts(i) = 1
      enddo
      call mpi_type_hindexed( 10, counts, displs, MPI_INTEGER, dtype, &
      &     ierr )
      call mpi_type_commit( dtype, ierr )
!
      call mpi_pack_size( 1, dtype, MPI_COMM_WORLD, psize, ierr )
      if (psize .gt. bufsize*intsize) then
         errs = errs + 1
      else
         do i=1,10
            inbuf(i)  = i
            outbuf(i) = -i
         enddo
         position = 0
         call mpi_pack( inbuf, 1, dtype, packbuf, psize, position, &
      &        MPI_COMM_WORLD, ierr )
!
         len      = position
         position = 0
         call mpi_unpack( packbuf, len, position, outbuf, 10, &
      &        MPI_INTEGER, MPI_COMM_WORLD, ierr )
!
         do i=1, 10
            if (outbuf(i) .ne. 11-i) then
               errs = errs + 1
               print *, 'outbuf(',i,')=',outbuf(i),', expected ', 10-i
            endif
         enddo
      endif
!
      call mpi_type_free( dtype, ierr )
!
      call mtest_finalize( errs )
      call mpi_finalize( ierr )
      end
