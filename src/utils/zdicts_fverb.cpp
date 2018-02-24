/************************************************************************

   Размножение фразовых глаголов
   
   ../../dicts/z_knowlege/perevod/base_enru_fverb_s -> ../../dicts/english_russian/base_enru_fverb
   
   ----------------------------------------------------------------------   
   (Пример, когда изощренным программированием пытаются заткнуть дыры в
   идеологии.)

     Должны быть следующие явления
     1) фразовые глаголы  VV:v1
     2) связки            LL:b1
     3) be+const          BB:b1
     4) have+const        HH:h1
     5) фразовые глаголы  VW:v1 c нестандартными хвостами

     И переходят они в следующее явление
     1) классическое ядро
     2) повествовательное ядро
     3) ядро whether
     4) глагольная конструкция
     0) ну и собственно сам глагол     

************************************************************************/
# include <string.h>
# include <unistd.h>
# include <mylib.h>
# include <mylib.cpp>

struct t_Struct1
{   char *s1,*s2 ;
};
struct t_S1 
{  char Tag[10] ;
   char Str[90] ;
};

long  FileToMass( char *File, char **_Mass );
long  FileToString( char *File, char ***_Str );
short str_parse( char *S, t_S1 *S1 );
void  print_struct( FILE *fw, char *Tag1, t_S1 *S1, short n_S, char *Tail, char f=0 );
void  print_struct1( FILE *fw, char *Tag1, t_S1 *S1, short n_S, char *Tail );
void  reduce( char *wFileName );

char DicDir[300]="../../dicts/z_knowlege/perevod" ;

/************************************************************************/
//     argv[1] - каталог, где находятся английские словари
/************************************************************************/
int main( int argc, char *argv[] )
{
   long  i,i1,n1,n2,N ;
   char *S, **Str, Tag[100], Tag1[100], S1[1000] ;
   t_S1  Sou[10],Dst[10] ;
   char  FileStr[300] ;
   
   if( 1<argc )
   {   strcpy( DicDir,argv[1] );
   }

   sprintf( FileStr,"%s/base_enru_fverb_s",DicDir );
   N=FileToString( FileStr,&Str );

   sprintf( FileStr,"%s/base_fverb1",DicDir );
   FILE *fw=Fopen( FileStr,"w" );
   sprintf( FileStr,"%s/base_fverb.err",DicDir );
   FILE *fw1=Fopen( FileStr,"w" );

   for( i=0 ; i<N ; i++ )
   {
      S=Str[i] ;
      strcpy( S1,S );
      for( i1=0 ; i1<200 && S[i1]!='=' && S[i1]!=0 ; i1++ ) ;
      if( i1==0 ) continue ;
      S[i1]=0 ;
      n1=str_parse( S     , Sou );
      n2=str_parse( S+i1+1, Dst );
      
      Tag[0]=0 ;
      for( i1=0 ; i1<n1 ; i1++ )
      {  strcat( Tag,Sou[i1].Tag );
	 strcat( Tag," " );
      }
      Tag1[0]=0 ;
      for( i1=0 ; i1<n2 ; i1++ )
      {  strcat( Tag1,Dst[i1].Tag );
	 strcat( Tag1," " );
      }

      // ------ фразовые глаголы ----------
      if( 0==strcmp( Tag,"VV V1 TT~ @0 " ) )
      {
         fprintf( fw,"\nV1:v1[%s] TT~ @0[%s]=V1:",Sou[1].Str,Sou[3].Str );
	    print_struct( fw,"v1",Dst,n2,"TT~" );
         fprintf( fw,"\nV1:v1[%s] @0[%s] TT~=V1:",Sou[1].Str,Sou[3].Str );
	    print_struct( fw,"v1",Dst,n2,"TT~" );

         fprintf( fw,"\nV2:v2[%s] TT~ @0[%s]=V2:",Sou[1].Str,Sou[3].Str );
	    print_struct( fw,"v2",Dst,n2,"TT~" );
         fprintf( fw,"\nV2:v2[%s] @0[%s] TT~=V2:",Sou[1].Str,Sou[3].Str );
	    print_struct( fw,"v2",Dst,n2,"TT~" );

         fprintf( fw,"\nV3:v3[%s] TT~ @0[%s]=V3:",Sou[1].Str,Sou[3].Str );
	    print_struct( fw,"v3",Dst,n2,"TT~" );
         fprintf( fw,"\nV3:v3[%s] @0[%s] TT~=V3:",Sou[1].Str,Sou[3].Str );
	    print_struct( fw,"v3",Dst,n2,"TT~" );

         fprintf( fw,"\nV4:v4[%s](Ф) TT~ @0[%s]",Sou[1].Str,Sou[3].Str );

         if( 0==strcmp( Tag1,"VV V1 " ) )
         {   fprintf( fw, "=VA:"); goto M_Ok1a ; }
         if( 0==strcmp( Tag1,"VV V1 @0 " ) )
         {   fprintf( fw, "=VB:"); goto M_Ok1a ; }
         if( 0==strcmp( Tag1,"VV @0 V1 " ) )
         {   fprintf( fw, "=VC:"); goto M_Ok1a ; }
         if( 0==strcmp( Tag1,"VV @0 V1 @0 " ) )
         {   fprintf( fw, "=VD:"); goto M_Ok1a ; }

	 fprintf( fw,"=V4:");
	 print_struct1( fw,"v4",Dst,n2,"TT~" );

M_Ok1a:  print_struct( fw,"v4",Dst,n2,"TT~" );

         fprintf( fw,"\nV4:v4[%s](Ф) @0[%s] TT~",Sou[1].Str,Sou[3].Str );

         if( 0==strcmp( Tag1,"VV V1 " ) )
         {   fprintf( fw, "=VA:"); goto M_Ok1b ; }
         if( 0==strcmp( Tag1,"VV V1 @0 " ) )
         {   fprintf( fw, "=VB:"); goto M_Ok1b ; }
         if( 0==strcmp( Tag1,"VV @0 V1 " ) )
         {   fprintf( fw, "=VC:"); goto M_Ok1b ; }
         if( 0==strcmp( Tag1,"VV @0 V1 @0 " ) )
         {   fprintf( fw, "=VD:"); goto M_Ok1b ; }
         
	 fprintf( fw,"=V4:");
	 print_struct1( fw,"v4",Dst,n2,"TT~" );
	 continue ;

M_Ok1b : print_struct( fw,"v4",Dst,n2,"TT~" );
         continue ;
      }
      // ------ фразовые глаголы c "нестандартными хвостами" -------
      if( 0==strcmp( Tag,"VV V1 @0 " ) || 0==strcmp( Tag,"VV V1 " ) ||
          0==strcmp( Tag,"VV V1 @0 SS~ " ) || 0==strcmp( Tag,"VV V1 SS~ @0 " ) ||
	  0==strcmp( Tag,"VV V1 SS~ " ) || 0==strcmp( Tag,"VV V1 RR~ " ) ||
	  0==strcmp( Tag,"VV V1 @0 @0 " ) || 0==strcmp( Tag,"VV V1 SS~ @0 SS~ ") )
      {
         fprintf( fw,"\nV1:"); print_struct( fw,"v1",Sou,n1,"" );
	 fprintf( fw, "=V1:"); print_struct( fw,"v1",Dst,n2,"" );

         fprintf( fw,"\nV2:"); print_struct( fw,"v2",Sou,n1,"" );
	 fprintf( fw, "=V2:"); print_struct( fw,"v2",Dst,n2,"" );

         fprintf( fw,"\nV3:"); print_struct( fw,"v3",Sou,n1,"" );
	 fprintf( fw, "=V3:"); print_struct( fw,"v3",Dst,n2,"" );

         fprintf( fw,"\nV4:"); print_struct( fw,"v4",Sou,n1,"",1 );

         if( 0==strcmp( Tag1,"VV V1 " ) )
         {   fprintf( fw, "=Va:"); goto M_Ok2 ; }
         if( 0==strcmp( Tag1,"VV V1 @0 " ) )
         {   fprintf( fw, "=Vb:"); goto M_Ok2 ; }
         if( 0==strcmp( Tag1,"VV @0 V1 " ) )
         {   fprintf( fw, "=Vc:"); goto M_Ok2 ; }
         if( 0==strcmp( Tag1,"VV @0 V1 @0 " ) )
         {   fprintf( fw, "=Vd:"); goto M_Ok2 ; }
         if( 0==strcmp( Tag1,"VV V1 SS~(В) " ) || 
	     0==strcmp( Tag1,"VV V1 SS~(Д) " ) ||
	     0==strcmp( Tag1,"VV V1 SS~(Р) " ) ||
	     0==strcmp( Tag1,"VV V1 SS~(Т) " ))
         {   fprintf( fw, "=Ve:"); goto M_Ok2 ; }
         if( 0==strcmp( Tag1,"VV V1 @0 SS~(В) " ) || 
	     0==strcmp( Tag1,"VV V1 @0 SS~(Д) " ) ||
	     0==strcmp( Tag1,"VV V1 @0 SS~(Р) " ) ||
	     0==strcmp( Tag1,"VV V1 @0 SS~(Т) " ) ||
	     0==strcmp( Tag1,"VV V1 @0 SS~(П) " ))
         {   fprintf( fw, "=Vf:"); goto M_Ok2 ; }
         if( 0==strcmp( Tag1,"VV V1 SS~(В) @0 " ) || 
	     0==strcmp( Tag1,"VV V1 SS~(Д) @0 " ) ||
	     0==strcmp( Tag1,"VV V1 SS~(Р) @0 " ) ||
	     0==strcmp( Tag1,"VV V1 SS~(Т) @0 " ))
         {   fprintf( fw, "=Vg:"); goto M_Ok2 ; }
/*
         if( 0==strcmp( Tag1,"VV V1 SS~(В) SS~(Д) ") ||
	     0==strcmp( Tag1,"VV V1 SS~(Р) SS~(Т) "))
         {   fprintf( fw, "=Vh:"); goto M_Ok1 ; }
/*
         if( 0==strcmp( Tag1,"VV @0 V1 SS~(В) " ) || 
	     0==strcmp( Tag1,"VV @0 V1 SS~(Д) " ) ||
	     0==strcmp( Tag1,"VV @0 V1 SS~(Р) " ) ||
	     0==strcmp( Tag1,"VV @0 V1 SS~(Т) " ))
         {   fprintf( fw, "=Vi:"); goto M_Ok1 ; }
*/
         //fprintf( fw1,"\n%s\n",S1 );
         fprintf( fw,"=V4:");
	 print_struct1( fw,"v4",Dst,n2,"" );
	 
	 continue ;
M_Ok2 :
	 print_struct( fw,"v4",Dst,n2,"" );
	 continue ;
      }

      // ------ связки -------------------
      if( 0==strcmp( Tag,"LL V1 " ) )
      {
         fprintf( fw,"\nb[%s]=b[%s]",Sou[1].Str,Dst[1].Str );

         fprintf( fw,"\nL1:b1[%s] TB~=L1:",Sou[1].Str );
	    print_struct( fw,"b1",Dst,n2,"TB~" );
         fprintf( fw,"\nL2:b2[%s] TB~=L2:",Sou[1].Str );
	    print_struct( fw,"b2",Dst,n2,"TB~" );
         fprintf( fw,"\nL3:b3[%s] TB~=L3:",Sou[1].Str );
	    print_struct( fw,"b3",Dst,n2,"TB~" );
         fprintf( fw,"\nL4:b4[%s] TB~=L4:",Sou[1].Str );
	    print_struct( fw,"b4",Dst,n2,"TB~" );
         // ----- сказуемые я похерил, но если очень хочется, можно добавить
	 continue ;
      }
      // ------ be+const -------------------
      if( 0==strcmp( Tag,"BB V1 @0 " ) )
      {

         fprintf( fw,"\nB1:b1[%s] @0[%s]=B1:",Sou[1].Str,Sou[2].Str );
	    print_struct( fw,"b1",Dst,n2,"" );
         fprintf( fw,"\nB2:b2[%s] @0[%s]=B2:",Sou[1].Str,Sou[2].Str );
	    print_struct( fw,"b2",Dst,n2,"" );
         fprintf( fw,"\nB3:b3[%s] @0[%s]=B3:",Sou[1].Str,Sou[2].Str );
	    print_struct( fw,"b3",Dst,n2,"" );
         fprintf( fw,"\nB4:b4[%s] @0[%s]=B4:",Sou[1].Str,Sou[2].Str );
	    print_struct( fw,"b4",Dst,n2,"" );
         continue ;
      }
      // ------ have+const -------------------
      if( 0==strcmp( Tag,"HH V1 @0 " ) || 0==strcmp( Tag,"HH V1 @0 SS~ " ) ||
          0==strcmp( Tag,"HH V1 SS~ @0 " ) )
      {
         fprintf( fw,"\nH1:"); print_struct( fw,"h1~",Sou,n1,"" );
	 fprintf( fw, "=H1:"); print_struct( fw,"h1",Dst,n2,"" );

         fprintf( fw,"\nH2:"); print_struct( fw,"h2~",Sou,n1,"" );
	 fprintf( fw, "=H2:"); print_struct( fw,"h2",Dst,n2,"" );

         fprintf( fw,"\nH3:"); print_struct( fw,"h3~",Sou,n1,"" );
	 fprintf( fw, "=H3:"); print_struct( fw,"h3",Dst,n2,"" );

         fprintf( fw,"\nH4:"); print_struct( fw,"h4~",Sou,n1,"" );

         if( 0==strcmp( Tag1,"HH V1 " ) )
         {   fprintf( fw, "=Va:"); goto M_Ok4 ; }
         if( 0==strcmp( Tag1,"HH V1 @0 " ) )
         {   fprintf( fw, "=Vb:"); goto M_Ok4 ; }
         if( 0==strcmp( Tag1,"HH @0 V1 " ) )
         {   fprintf( fw, "=Vc:"); goto M_Ok4 ; }
         if( 0==strcmp( Tag1,"HH @0 V1 @0 " ) )
         {   fprintf( fw, "=Vd:"); goto M_Ok4 ; }
         if( 0==strcmp( Tag1,"HH V1 SS~(В) " ) || 
	     0==strcmp( Tag1,"HH V1 SS~(Д) " ) ||
	     0==strcmp( Tag1,"HH V1 SS~(Р) " ) ||
	     0==strcmp( Tag1,"HH V1 SS~(Т) " ))
         {   fprintf( fw, "=Ve:"); goto M_Ok4 ; }
         if( 0==strcmp( Tag1,"HH V1 @0 SS~(В) " ) || 
	     0==strcmp( Tag1,"HH V1 @0 SS~(Д) " ) ||
	     0==strcmp( Tag1,"HH V1 @0 SS~(Р) " ) ||
	     0==strcmp( Tag1,"HH V1 @0 SS~(Т) " ) ||
	     0==strcmp( Tag1,"HH V1 @0 SS~(П) " ))
         {   fprintf( fw, "=Vf:"); goto M_Ok4 ; }

         fprintf( fw,"=H4:");
         print_struct1( fw,"h4",Dst,n2,"" );
	 continue ;
	 M_Ok4 :
	 print_struct( fw,"h4",Dst,n2,"" );
	 
         continue ;
      }
      fprintf( fw1,"\n%s",S );
   }
   Fclose( fw );
   Fclose( fw1 );

   reduce( "../../dicts/english_russian/base_enru_fverb" );

   sprintf( FileStr,"%s/base_fverb1",DicDir );
   unlink( FileStr );

   return 0 ;
}
/***************************************************************************/
long FileToMass( char *File, char **_Mass )
{
   char *Mass ; long L ; FILE *fr ;
   
   fr=Fopen( File,"r" );
   L=FileLength( fr );
   Mass=(char *)Calloc( L,1 );
   fread( Mass,L,1,fr );
   Fclose(fr);

   *_Mass=Mass ;
   return L ;
}
/***************************************************************************/
long FileToString( char *File, char ***_Str )
{  long  i,j,L ;
   char *Mass,**ss ;

   L=FileToMass( File, &Mass );
   for( i=j=0 ; i<L ; i++ )
      if( Mass[i]=='\n' ) j++ ;
   ss=(char **)Calloc( j+1,sizeof(char *) );
   ss[0]=Mass ;
   for( i=0,j=1 ; i<L ; i++ )
      if( Mass[i]=='\n' )
      {   ss[j++]=Mass+i+1 ; Mass[i]=0 ;  }
   *_Str=ss ;
   return j ;
}
/************************************************************************/
short str_parse( char *S, t_S1 *S1 )
{  char Str[200] ;
   long i,i1,j,j1 ;


   for( i=0,j=0 ; i<200 && S[i]!=':' ; i++ )
      Str[j++]=S[i] ;
   Str[j]=0 ;
   strcpy( S1[0].Tag,Str );
   
   j1=1 ;
   while( 1 )
   {  for( j=0,i++ ; i<200 && S[i]!='[' ; i++ )
      {  if( S[i]==' ' ) continue ;
         if( S[i]==0 ) goto M1 ;
	 Str[j++]=S[i] ;
      }
      Str[j]=0 ;
      strcpy( S1[j1].Tag,Str );

      for( j=0,i++ ; i<200 && S[i]!=']' ; i++ )
      {  if( S[i]==0 ) goto M1 ;
         Str[j++]=S[i] ;
      }
      Str[j]=0 ;
      strcpy( S1[j1].Str,Str );
      j1++ ;
   }
M1:return j1 ;
}
/************************************************************************/
void print_struct( FILE *fw, char *Tag1, t_S1 *S1, short n_S, char *Tail, char f )
{  short i ;
   char  f_tilda=0 ;

   for( i=0 ; Tag1[i]!=0 ; i++ )
      if( Tag1[i]=='~' ) f_tilda=1 ;

   for( i=1 ; i<n_S ; i++ )
   {  if( 0==strcmp( S1[i].Tag,"V1" ))
               fprintf( fw,Tag1 );
	  else fprintf( fw,S1[i].Tag );
      if( S1[i].Str[0]!=0 && (f_tilda==0 || 0!=strcmp( S1[i].Tag,"V1" )) )
          fprintf( fw,"[%s]",S1[i].Str );
      fprintf( fw," ");
      if( f==1 && 0==strcmp( S1[i].Tag,"V1" ) )
          fprintf( fw,"(Ф)");
   }
   fprintf( fw,Tail );
}
/************************************************************************/
void print_struct1( FILE *fw, char *Tag1, t_S1 *S1, short n_S, char *Tail )
{  short i ;

   for( i=1 ; i<n_S ; i++ )
   {  if( 0==strcmp( S1[i].Tag,"V1" ) )
               fprintf( fw,Tag1 );
	  else fprintf( fw,S1[i].Tag );
      if( S1[i].Str[0]!=0 )
               fprintf( fw,"[%s] ",S1[i].Str );
	  else fprintf( fw," " );
      if( 0==strcmp( S1[i].Tag,"V1" ) )
          fprintf( fw,"( S,Ф,@0,Ч,В,Р,З,П ) " );
   }
   fprintf( fw,Tail );
}
/*******************************************************************/
int strcmp1( const void *a, const void *b )
{   return strcmp( *((char **)a),*((char **)b) );
}
/*******************************************************************/
//       Склеить похожие строки типа
//   HH:h1 @0[a bottle]=HH:h1[пить]
//   HH:h1 @0[a bottle]=HH:h1[выпивать]
/*******************************************************************/
void reduce( char *wFileName )
{
   long  i,i1,N ;
   char **Str ;
   t_Struct1 *SS ;
   char FileStr[300] ;

   sprintf( FileStr,"%s/base_fverb1",DicDir );
   N =FileToString( FileStr,&Str  );
   
   qsort( Str,N,sizeof(char *), strcmp1 );
   SS=(t_Struct1 *)Calloc( N,sizeof(t_Struct1) );
   for( i=0 ; i<N ; i++ )
   {  SS[i].s1=Str[i] ;
      for( i1=0 ; Str[i][i1]!=0 ; i1++ )
      {  if( Str[i][i1]=='=' ) 
	 {   Str[i][i1]=0 ;
	     SS[i].s2=Str[i]+i1+1 ;
	     break ;
	 }
      }
   }
   
   FILE *fw=Fopen( wFileName,"w" );

   for( i=0 ; i<N ; i++ )
   {
      if( SS[i].s1[0]==0 ) continue ;
      fprintf( fw,"\n%s=%s",SS[i].s1,SS[i].s2 );
      for( i1=i+1 ; i1<N ; i1++ )
      {  if( 0!=strcmp(SS[i1].s1,SS[i].s1) ) break ;
	 fprintf( fw,";%s",SS[i1].s2 );
	 SS[i1].s1[0]=0 ;
      }
   }
   Fclose( fw );
}
