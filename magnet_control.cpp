#include <iostream>
#include <string>
#include <cstdio>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

#define RECVSIZE 1
#define SENDSIZE 1

using namespace std;

int openDevice( string dev_name ){
  int fd;
  struct termios oldtio , newtio;

  fd = open( dev_name.c_str() , O_RDWR | O_NOCTTY );
  if( fd < 0 ){
    return fd;
  }
  tcgetattr( fd , &oldtio );
  newtio = oldtio;
  newtio.c_cflag = CS8 | CLOCAL | CREAD;
  cfsetspeed( &newtio , B115200 );
  newtio.c_iflag = IGNPAR;
  newtio.c_oflag = 0;
  newtio.c_lflag = 0;
  newtio.c_cc[VTIME] = 30;
  newtio.c_cc[VMIN]  = 0;
  tcflush( fd , TCIFLUSH );
  tcsetattr( fd , TCSANOW , &newtio );
  return fd;
}

int recvData( int fd , char *buf ){
  int ret;
  ret = read( fd , buf , RECVSIZE );
  if( ret != RECVSIZE ){
    return -1;
  }else{
    return ret;
  }
}

int sendData( int fd , short *ctr ){
  int ret , i;
  char buf;
  buf  = 0;
  for( i = 0 ; i < 4 ; i++ ){
    if( ctr[i] ){
      buf += (0x01<<(i));
    }
  }
  ret = write( fd , &buf , SENDSIZE );
  return ret;
}

typedef struct{
  short           *ctr;
  int              exitf;
  pthread_mutex_t  mutex;
  pthread_cond_t   cond;
  string           wsh_path;
}global_info;

void pattern( FILE *wsh , global_info *gi ){
  short pat[4][20];
  int   i , j = -1;
  for( i = 0 ; i < 20 ; i++ ){
    fscanf( wsh , "%hd %hd %hd %hd" 
	    , &pat[0][i] , &pat[1][i] , &pat[2][i] , &pat[3][i] );
    //fprintf( wsh , "ok\n" );
    fflush( wsh );
  }
  for( i = 0 ; i < 800 ; i++ ){
    if( i%40 == 0 ){
      j++;
    }
    pthread_mutex_lock( &gi->mutex );
    pthread_cond_wait( &gi->cond , &gi->mutex );
    pthread_mutex_unlock( &gi->mutex );
    gi->ctr[0] = pat[0][j];
    gi->ctr[1] = pat[1][j];
    gi->ctr[2] = pat[2][j];
    gi->ctr[3] = pat[3][j];
  }
  pthread_mutex_lock( &gi->mutex );
  pthread_cond_wait( &gi->cond , &gi->mutex );
  pthread_mutex_unlock( &gi->mutex );
  gi->ctr[0] = 0;
  gi->ctr[1] = 0;
  gi->ctr[2] = 0;
  gi->ctr[3] = 0;
}

void *graphicaluserinterface( void *arg ){
  global_info *gi;
  char         buf[1024];
  FILE *wsh;

  gi = (global_info *)arg;
  wsh = popen( "wish magnet_control.tcl" , "r" );
  if( wsh == NULL ){
    fprintf( stderr , "popen failed\n" );
    gi->exitf = 1;
    return NULL;
  }

  while( 1 ){
    fscanf( wsh , "%s" , buf );
    if( strcmp( buf , "exit" ) == 0 ){
      gi->exitf = 1;
      break;
    }else if( strcmp( buf , "set" ) == 0 ){
      fscanf( wsh , "%hd %hd %hd %hd" 
	      , &gi->ctr[0] , &gi->ctr[1] , &gi->ctr[2] , &gi->ctr[3] );
    }else if( strcmp( buf , "pattern" ) == 0 ){
      pattern( wsh , gi );
    }
    //fprintf( wsh , "ok\n" );
    fflush( wsh );
  }
 
  pclose( wsh );
  return NULL;
}

void *commanduserinterface( void *arg ){
  global_info *gi;
  char         buf[1024];
  int          size;

  gi = (global_info *)arg;

  while( 1 ){
    scanf( "%s" , buf );
    if( strcmp( buf , "exit" ) == 0 ){
      gi->exitf = 1;
      break;
    }else if( strcmp( buf , "set" ) == 0 ){
      scanf( "%hd %hd %hd" , &gi->ctr[0] , &gi->ctr[1] , &gi->ctr[2] );
    }
  }
  return NULL;
}

void signalhandler( int a ){
}

int main( int ac , char **av ){
  int         fd;
  short       ctr[] = {0,0,0,0};
  char        buf;
  int         signo , i , j;
  sigset_t    ss;
  global_info gi;
  pthread_t   cui;
  struct sigaction alm;
  struct itimerval tim;

  gi.ctr   = ctr;
  gi.exitf = 0;
  pthread_mutex_init( &gi.mutex , NULL );
  pthread_cond_init( &gi.cond , NULL );

  sigaddset( &ss , SIGALRM );
  sigprocmask( SIG_BLOCK , &ss , NULL );

  memset( &alm , 0 , sizeof(struct sigaction) );
  alm.sa_handler = signalhandler;
  alm.sa_flags   = SA_RESTART;
  sigaction( SIGALRM , &alm , NULL );
  tim.it_value.tv_sec = tim.it_interval.tv_sec = 0;
  tim.it_value.tv_usec = tim.it_interval.tv_usec = 5000;
  setitimer( ITIMER_REAL , &tim , NULL );
  
  if( ac < 2 ){
    cerr << "Usage: " << av[0] << " [device name]" << endl;
    return 0;
  }
  if( ac > 2 ){
    gi.wsh_path = (string)av[2] + " magnet_control.tcl";
  }else{
    gi.wsh_path = "wish magnet_control.tcl";
  }
  fd = openDevice( av[1] );
  if( fd < 0 ){
    return 0;
  }

  pthread_create( &cui , NULL , graphicaluserinterface , (void *)&gi );

  while( 1 ){
    sigwait( &ss , &signo );
    sendData( fd , ctr );
    recvData( fd , &buf );
    pthread_cond_broadcast( &gi.cond );
    if( gi.exitf ){
      break;
    }
  }
  
  pthread_join( cui , NULL );
  close( fd );
}
