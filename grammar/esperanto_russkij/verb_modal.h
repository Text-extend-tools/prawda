@�������
{  @���������  �����������_���� =
	       _Ne ������[povi]( %������, &�����, @0, @0 ) ;
   @���������  �����������_����( ���� &���� , ����� &�����, ��� &��� ) =
               _Ne ������[����]( &��, %������, ����, �����, &�����, ���, %�����, @0 ) ;

   @�������( ����� ����� = ����� �����, ������������� �� )
   {  
      �������   = �������   ��� ;
      ��������� = ��������� ����� ;
      �������   = �������   ��� ;
   }
}
@�������
{  @���������  �����������_������ =
	       _Ne ������[voli]( %������, &�����, @0, @0 ) ;
   @���������  �����������_������( ���� &���� , ����� &�����, ��� &��� ) =
               _Ne ������[������]( &��, %������, ����, �����, &�����, ���, %�����, @0 ) ;

   @�������( ����� ����� = ����� �����, ������������� �� )
   {  
      �������   = �������   ��� ;
      ��������� = ��������� ����� ;
      �������   = �������   ��� ;
   }
}
@�������
{  @���������  �����������_������ =
	       _Ne ������[devi]( %������, &�����, @0, @0) ;
   @���������  �����������_������( ���� &���� , ����� &�����, ��� &��� ) =
               _Ne ������[����]( &��, %������, ����, �����, &�����, ���, %�����, @0 ) 
	       ������[������]( %�����, %������, @0, �����, @0, ���, %�����, @0 ) ;

   @�������( ����� ����� = ����� �����, ������������� �� )
   {  
      �������   = �������   ��� ;
      ��������� = ��������� ����� ;
      �������   = �������   ��� ;
   }
}
@�������
{
   @�����      �����������_������1 =
               @0[povus]
	       @0[volus]
	       @0[devus]
	       ;
   @�����      �����������_������1( ���� &����, ����� &�����, ��� &��� ) =
	       ������[����]  ( %�����, %������, ����, �����, %�������, ���, %�����, @0 )
	       ������[������]( %�����, %������, ����, �����, %�������, ���, %�����, @0 )
	       ������[������]( %�����, %������, @0  , �����, @0      , ���, %�����, @0 )
               ;
}
@�������
{
   @���������  �����������_������ = _Ne �����������_������1 ;
   @���������  �����������_������( ���� &����, ����� &�����, ��� &��� ) =
              _Ne  �����������_������1( ����, �����, ��� )  @0[��] ;
}
@�������
{
   @�����      ����������� =
               �����������_����
	       �����������_������
	       �����������_������
	       �����������_������
	       ;
   @�����      �����������( ���� &����, ����� &�����, ��� &��� ) =
               �����������_����  ( ����, �����, ��� )
	       �����������_������( ����, �����, ��� )
	       �����������_������( ����, �����, ��� )
	       �����������_������( ����, �����, ��� )
	       ;
}
// ----------------------------------------------------------------------------------
@�������
{
   @���������  ���������_esti_��������� = 
               ����������� ������( %Inf,@0, @0, @0 ) _�������������� ;
   @���������  ���������_esti_���������( ���� &���� , ����� &�����, ��� &��� ) = 
               �����������( ����, �����, ��� ) 
	       ������( %�����, %Inf,@0,@0,@0,@0,@0  )
	       _�������������� ;
}
// ----------------------------------------------------------------------------------
@�������
{  @���������  ���������_�����_��������� = 
               ����������� ������( %Inf,@0, @0, @0 ) _�������������� ;
   @���������  ���������_�����_���������( ���� &���� , ����� &�����, ��� &���, ����� &����� ) = 
               �����������( ����, �����, ��� ) 
	       ������( %�����, %Inf,@0,@0,@0,@0,@0  )
	       _�������������� ;
   @�������( @����� = ����� ����� )
   {  @0 = � ;
   }
}
@�������
{  @���������  ���������_havi_��������� = 
               ����������� @0[havi] _�������������� ;
   @���������  ���������_havi_���������( ���� &���� , ����� &�����, ��� &��� ) = 
               �����������( ����, �����, ��� ) @0[�����] _�������������� ;
}
