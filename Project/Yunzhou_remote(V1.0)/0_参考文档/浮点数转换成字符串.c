
// ������ת�����ַ���
// ������data
// ���أ�str
void float_to_string(double data, u8 *str)
{
  int i,j,k;
  long temp,tempoten;
  u8 intpart[20], dotpart[20];  // �����������ֺ�С������

  //1.ȷ������λ
  if(data<0) {str[0]='-';data=-data;}
  else str[0]='+';

  //2.ȷ����������
  temp=(long)data;

  i=0;
  tempoten=temp/10;
  while(tempoten!=0)
  {
    intpart[i]=temp-10*tempoten+48; //to ascii code
    temp=tempoten;
    tempoten=temp/10;
    i++;
  }
  intpart[i]=temp+48;

  //3.ȷ��С������,ȡ��12λС��
  data=data-(long)data;
  for(j=0;j<12;j++)
  {
    dotpart[j]=(int)(data*10)+48;
    data=data*10.0;
    data=data-(long)data;
  }

  //4.������װ
  for(k=1;k<=i+1;k++) str[k]=intpart[i+1-k];
  str[i+2]='.';
  for(k=i+3;k<i+j+3;k++) str[k]=dotpart[k-i-3];
  str[i+j+3]=0x0D;

}