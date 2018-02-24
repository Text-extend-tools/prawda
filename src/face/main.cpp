/***************************************************************************/
//              "��������"-"������" - open source ����������
//          ���������������� � ����������� � ��������� GNU v 2.0
//
//                                 main()
//     �������� �.�.                                         �������
/***************************************************************************/

# include <string.h>
# include <unistd.h>
# include <time.h>
# include <ctype.h>
# include "video.h"
# include "core.h"
# include "face.h"
# include "edit.h"
# include "edit1.h"
# include "resource1.h"

extern t_Option    Option ;
extern t_Edit      ErrorView ;
extern t_EditTrans Edit1 ;
extern char        MercuryDir[] ;
// ------- �����, ��� ����� ����������� ������� � ��. ������ ----------
char *MercuryDir00[]={ "/usr/share/prawda",
                       "/usr/local/share/prawda/",
                        NULL
		     };
// ------- �����, ��� ��� ������������� ����������� -------------------
char *MercuryDir0 ;
// ------- �����, ��� ����� ����������� ��������� ----------
char *HelpDir00[]={ "/usr/share/doc/prawda/help",
                    "/usr/local/share/doc/prawda/help",
                     NULL
                  };
// ------- �����, ��� ��� ������������� ����������� -------------------
char  HelpDir0[200] ;
char *HelpDir ;

void null_menu( void ) ;
char is_number( char *Str );
void preproc_rc( char *File );
void language_rc( char *File );

/***************************************************************************/
void spez_figna( char *Str1 )
{  char Str[200] ;

   Strcpy( Str,Option.get("font"),200 );
   if( Str[0]!=0 ) s_set_font( Str );
   s_begin_schone( );

   figna( Str1 );
}
/***************************************************************************/
void spez_figna1( char *Str )
{  
//   if( s_x_version() )
//        printf( Str );
//   else message( Str );
        printf( Str );
}
/***************************************************************************/
//               ���� �� ����� ������ ������ "������"
/***************************************************************************/
char old_version_present( char *Path, long Ver )
{
   char *Mass ;
   long Ver1 ;
   char Str[300] ;
   
   Strcpy( Str,Path,300 );
   if( 0>access( Str,R_OK ) ) return 0 ; //���������� �� ������� "������"
   
   Strcat( Str,"/version",300 );
   if( 0>access( Str ,R_OK ) ) return 1 ; //���������� �� ���� ������
   
   // ---- ������ �� ������� ������ ��� ���������? -----
   FILE *fr=Fopen( Str,"r" );
   if( EOF==fscanf( fr,"%ld",&Ver1 ) ) return 1 ;
   Fclose( fr );
   if( 0<(Ver-Ver1)/100 ) return 1 ;
   
   return 0 ;
}
/***************************************************************************/
//         ����������� ������� �������� � �������� �������
/***************************************************************************/
char copy_mercury_dir( char *h, char Clear, char rc_file )
{  int   r0,r1,r2,r3 ;
   char Cmd[1000] ;

   spez_figna1("\n ���������� � ������� ������� \"������\". �����...\n" );
   if( Clear )
   {   sprintf( Cmd,"rm -r %s",MercuryDir );
       system( Cmd );
   }
   sprintf( Cmd,"cp -r %s %s",MercuryDir0,MercuryDir );
   r1=system( Cmd );
   if( rc_file )
   {  sprintf( Cmd,"cp %s/prawdarc.local %s/.prawdarc",MercuryDir0,h );
      r2=system( Cmd );
   }
   if( r1<0 || r2<0 )
   {   spez_figna1("\n ����������� �� ���������" );
       return -1 ;
   }
   return 0 ;
}
/***************************************************************************/
//                      ���������� MercuryDir
/***************************************************************************/
void set_mercury_dir( char *h )
{
   short i,j ;
   char Str[200] ;
   
   Strcpy( Str,Option.get("path"),200 );
   if( Str[0]!=0 )
   {  for( i=j=0 ; i<199 && Str[i]!=0 ; i++ )
      {  if( Str[i]=='~' )
         {   MercuryDir[j++]=0 ;
             if( h!=NULL ) 
	     {  Strcat( MercuryDir,h,200 ); j=strlen( MercuryDir );  }
         }
         else
             MercuryDir[j++]=Str[i] ;
      }
      MercuryDir[j++]=0 ;
   }
   else
   {
      Strcpy( MercuryDir,h,200 );
      Strcat( MercuryDir,"/prawda",200 );
   }

}
/***************************************************************************/
//                �������� ����������� ������ ������ 
//               (������ ����, ��� �� ���� ������ ���������)
/***************************************************************************/
void check_install( )
{   
   int  i ;
   int r0 ; // ������������ ����������� ���� "~/.prawdarc"
   int r1 ; // ������������ "������� ������������" "~/prawda"
   int r2 ; // ������������ "�������� �������" "/usr/share/prawda"
   int r3 ; // ������������ ���������� "������� ������������" "~/prawda"
   char *h,Str[200]="" ;
      
   h=getenv( "HOME" );
   if( h!=NULL ) {  Strcpy( Str,h,200 );  }
   Strcat( Str,"/.prawdarc",200 );

   // ----- ������, � ��� � ��� "/usr/share/prawda" ---------
   MercuryDir0=MercuryDir00[0] ;
   for( i=0 ; MercuryDir00[i]!=NULL ; i++ )
   {  if( 0==access( MercuryDir00[i],R_OK ) )
      {   MercuryDir0=MercuryDir00[i] ; break ;  }
   }
   
   r0=access( Str,R_OK );

   if( 0==r0 ) 
   {  // ������������ ����������� ���� "~/.prawdarc
      // --------- �������� ����������� ���� ----------
      int r=Option.read( Str );
      if( 0>r )
      {   printf("\n ���� %s ����������� ��� �����������.",Str ); exit(-1) ;  }

      set_mercury_dir( h );
      r1=access( MercuryDir,R_OK );
      r2=access( MercuryDir0,R_OK );

      if( 0==r1 ) 
      {  // ������������ "~/prawda"
         r3=old_version_present( MercuryDir,300 );
	 if( 1==r3 ) // ������������ ���������� "������� ������������" "~/prawda"
	 {   
             if( r2<0 ) // �� ������������ "�������� �������" "/usr/share/prawda"
                 goto M_OldVersion ;
	     Strcpy( Str,Option.get("font"),200 );
	     if( Str[0]!=0 ) s_set_font( Str );
	     s_begin_schone( );

             if( 1==fragen("���������� ������ ������ ������\n ���������� ��? 1-��(�������������) 0-��� ") )
	        copy_mercury_dir( h, 1, 0 );
	     else goto M_OldVersion ;
	 }
      }
      else
      {  // �� ������������ "~/prawda"
         if( 0==r2 ) // ������������ "�������� �������" "/usr/share/prawda" 
	      copy_mercury_dir( h, 0, 0 ); // � ������� MercuryDir ��� ~/.prawdarc
	 else goto M_Dir0_Absend ;
      }
   }
   else
   {  // �� ������������ ����������� ���� "~/.prawdarc      
      set_mercury_dir( h );
      r1=access( MercuryDir,R_OK );
      r2=access( MercuryDir0,R_OK );
      if( 0>r2 ) // �� ������������ "/usr/share/prawda"
          goto M_Dir0_Absend ;
      copy_mercury_dir( h, 0==r1, 1 ); // ������ � ~/.prawdarc
      int r=Option.read( Str );
      if( 0>r )
      {   printf("\n ���� %s ����������� ��� �����������.",Str ); exit(-1) ;  }
      set_mercury_dir( h );
   }
   return ;

   M_Dir0_Absend:
      sprintf( Str,"��� ������ (��������) � ��������\n %s",MercuryDir0 );
      spez_figna( Str );
      exit(-1);
   M_OldVersion:
      sprintf( Str,"������ ������ � �������� \n%s\n �� ������������� ������ ���������\n",MercuryDir );
      printf("\n%s",Str );
      spez_figna( Str );
      exit(-1);
}
/***************************************************************************/

/***************************************************************************/
int main( short argc, char *argv[] )
{  char  r,r1 ;
   char  Str[200]="" ;
   short sx=80,sy=50 ;

try
{  
   check_install( );
   // --------- ���������� ������� � � -------------
   Strcpy( Str,Option.get("size_x"),200 );
   if( is_number( Str ) && Str[0]!=0 ) sx=atoi(Str);
   Strcpy( Str,Option.get("size_y"),200 );
   if( is_number( Str ) && Str[0]!=0 ) sy=atoi(Str);
   // --------- ���������� ������������ ------------
   Strcpy( Str,Option.get("rus"),200 );
   if( Str[0]!=0 && 0!=strcmp(Str,"No") )
   {  short Komb=0 ;
      if( NULL!=strstr(Str,"Shift_R") ) Komb|=S_Shift_R ;
      if( NULL!=strstr(Str,"Shift_L") ) Komb|=S_Shift_L ;
      if( NULL!=strstr(Str,"Ctrl_R")  ) Komb|=S_Ctrl_R ;
      if( NULL!=strstr(Str,"Ctrl_L")  ) Komb|=S_Ctrl_L ;
      if( NULL!=strstr(Str,"Alt_R")  )  Komb|=S_Alt_R ;
      if( NULL!=strstr(Str,"Alt_L")  )  Komb|=S_Alt_L ;
   
      s_set_ruskomb( Komb );   
   }   
   // --------- ���������� ����� -------------------
   Strcpy( Str,Option.get("font"),200 );
   if( Str[0]!=0 ) s_set_font( Str );
   s_begin_schone( );
   s_set_size( sx,sy );
   s_nacht( );

   // --------- ���������� HelpDir -----------------
   HelpDir=HelpDir0 ;
   Strcpy( HelpDir0,MercuryDir ,200 );
   Strcat( HelpDir0,"/doc/help",200 );
   for( long i=0 ; HelpDir00[i]!=NULL ; i++ )
   {  if( 0==access( HelpDir,R_OK ) ) break ;
      HelpDir=HelpDir00[i] ;
   }
   // --------- ���������� ������������� -----------
   Strcpy( Str,MercuryDir,200 );
   Strcat( Str,"/z_preproc.rc",200 );
   preproc_rc( Str );

   // --------- ���������� ����������� �������� ----
   Strcpy( Str,MercuryDir,200 );
   Strcat( Str,"/language.rc",200 );   
   language_rc( Str );

   // --------- ���������� ������ ------------------
   Windows.init( );
   Windows.set_pos( );
   null_menu( );
   message("���� �������� ������.\n        �����...");
   s_refresh();
# ifndef __WIN__
   HelpData.make();
# endif

   r=read_grammar( MercuryDir,&DirTrans[0] );
   if( r<0 ) 
   {  ErrorView.text()->read_file("mercury.err");
      error_add("���������� ��� ������� �� ���������!!!\n");
      error_add("���������� ������ ����������!!!");
      figna( error_get() );
      ErrorView.main( );
   }

   if( 0==strcmp( "Yes",Option.get("RTFM") ) )
   {   time_t      tm0=time(NULL);
       struct tm  *tm1=localtime( &tm0 );
       sprintf( Str,"tip%d.html",1+(tm1->tm_yday)%8 );

       Windows.set_pos();
       null_menu( );
       Window_help( Str );
   }
   if( 0==strcmp( "Yes",Option.get("Figna") ) )
       Core.f_Figna=1 ;
   null_menu( );
   Windows.main_loop( ) ;

   return 0 ;
}
catch( int E )
{
   s_end_schone();
   return -1 ;
}
}
