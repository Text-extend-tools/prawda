/*******************************************************************/
//              "��������"-"������" - open source ����������
//          ���������������� � ����������� � ��������� GNU v 2.0
//
//                 ���������, �������, ������ � �.�.
//     �������� �.�.                               �������
/*******************************************************************/

# ifndef RESOURCE1_H
# define RESOURCE1_H

# include <list.h>
# include <lexer.h>

struct t_OptionParam
{  char Tag[50] ;
   char Value[200] ;
};
DECLARE_LIST_TYPE( t_OptionParam,t_OptionParamList );

struct t_Option
{
   t_OptionParamList  Param ;

   int    read( char *File );
   char  *get( char *Tag );
};

# endif
