typedef int  (fSORTFUNC)(const void *val1,const void *val2);

int Qfind( void *Data, long Num, long Size, fSORTFUNC *cmpfunc, void *Object )
{
  int cur;
  int cmp_res;
  int top,bottom ;

  bottom=0 ;
  top   =Num ;

  while( bottom <= top )
  {
     cur=(top+bottom)>>1;

     // Compare to val.
     if((cmp_res=cmpfunc(Object,(char *)Data+cur*Size)) == 0) return cur;

     if( cmp_res > 0)
          bottom=cur+1 ;
     else top=cur-1;
  }
  return -1;        // Not found.
}
