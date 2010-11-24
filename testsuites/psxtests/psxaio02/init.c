/*
 * Copyright 2010, Alin Rus <alin.codejunkie@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 *
 * $Id$
 */

#define CONFIGURE_INIT
#include "system.h"
#include <rtems.h>
#include "tmacros.h"
#include <rtems/posix/aio_misc.h>
#include <aio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sched.h>
#include <fcntl.h>
#include <rtems/chain.h>

#define BUFSIZE 32
#define MAX 7

struct aiocb *
create_aiocb (int fd)
{
  struct aiocb *aiocbp;

  aiocbp = malloc (sizeof (struct aiocb));
  memset (aiocbp, 0, sizeof (struct aiocb));
  aiocbp->aio_buf = malloc (BUFSIZE * sizeof (char));
  aiocbp->aio_nbytes = BUFSIZE;
  aiocbp->aio_offset = 0;
  aiocbp->aio_reqprio = 0;
  aiocbp->aio_fildes = fd;

  return aiocbp;
}

void
free_aiocb (struct aiocb *aiocbp)
{
  free ((char*) aiocbp->aio_buf);
  free (aiocbp);
}

void *
POSIX_Init (void *argument)
{
  int fd[MAX];
  struct aiocb *aiocbp[MAX+2];
  int status, i;
  char filename[BUFSIZE];

  status = rtems_aio_init ();
  rtems_test_assert (status == 0);
 
  status = mkdir ("/tmp", S_IRWXU);
  rtems_test_assert (!status);
  
  puts ("\n\n*** POSIX AIO TEST 02 ***");
  
  puts (" Init: Open files ");

  for (i=0; i<MAX; i++)
    {
      sprintf (filename, "/tmp/aio_fildes%d",i);
      fd[i] = open (filename, O_RDWR|O_CREAT, S_IRWXU|S_IRWXG|S_IRWXO);
      rtems_test_assert ( fd[i] != -1);
    }
  
  puts (" Init: [WQ] aio_write on 1st file ");
  aiocbp[0] = create_aiocb (fd[0]);
  status = aio_write (aiocbp[0]);
  rtems_test_assert (status != -1);

  puts (" Init: [WQ] aio_write on 2nd file ");
  aiocbp[1] = create_aiocb (fd[1]);
  status = aio_write (aiocbp[1]);
  rtems_test_assert (status != -1);
  
  puts (" Init: [WQ] aio_read on 2nd file add by priority ");
  aiocbp[2] = create_aiocb (fd[1]);
  status = aio_read (aiocbp[2]);
  rtems_test_assert (status != -1);
  
  puts (" Init: [WQ] aio_write on 3rd file "); 
  aiocbp[3] = create_aiocb (fd[2]);
  status = aio_write (aiocbp[3]);
  rtems_test_assert (status != -1);
  
  puts (" Init: [WQ] aio_write on 4th file ");
  aiocbp[4] = create_aiocb (fd[3]);
  status = aio_write (aiocbp[4]);
  rtems_test_assert (status != -1);
  
  puts (" Init: [WQ] aio_write on 5th file  -- [WQ] full ");
  aiocbp[5] = create_aiocb (fd[4]);
  status = aio_write (aiocbp[5]);
  rtems_test_assert (status != -1);
  
  puts (" Init: [IQ] aio_write on 6th file ");
  aiocbp[6] = create_aiocb (fd[5]);
  status = aio_write (aiocbp[6]);
  rtems_test_assert (status != -1);
  
  puts (" Init: [IQ] aio_write on 7th file ");
  aiocbp[7] = create_aiocb (fd[6]);
  status = aio_write (aiocbp[7]);
  rtems_test_assert (status != -1);

  puts (" Init: [IQ] aio_read on 7th file add by priority ");
  aiocbp[8] = create_aiocb (fd[6]);
  status = aio_read (aiocbp[8]);
  rtems_test_assert (status != -1);

  puts ("\n\n*** POSIX AIO TEST 02 ***");

  puts ("*** END OF POSIX AIO TEST 01 ***");

  for (i = 0; i < MAX; i++)
    {
      close (fd[i]);
      free_aiocb (aiocbp[i]);      
    }
  free_aiocb (aiocbp[i]);
  free_aiocb (aiocbp[i+1]);
  rtems_test_exit (0);

  return NULL;

}