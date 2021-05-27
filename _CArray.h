#pragma once
#include<Windows.h>
#include<math.h>

using namespace std;
class _declspec (dllexport) _CArray
{
public:
	_CArray();
	~_CArray();
	BOOL      _CArray::WriteArray(PUINT8 BufferData, ULONG_PTR BufferLength);   //д��
	ULONG_PTR _CArray::ReallocateArray(ULONG_PTR BufferLength);
	ULONG_PTR _CArray::GetArrayMaximumLength();
	ULONG_PTR _CArray::GetArrayLength();                           //�����Ч����
	PUINT8    _CArray::GetArray(ULONG_PTR Position = 0);           //��ȡ
	VOID      _CArray::ClearArray();                               //���
	ULONG_PTR _CArray::DeallocateArray(ULONG_PTR BufferLength);    //��������
	ULONG_PTR _CArray::ReadArray(PUINT8 BufferData, ULONG_PTR BufferLength);    //��ȡ
	ULONG_PTR _CArray::RemoveArray(ULONG_PTR BufferLength);        //�ƶ�����
private:
	PUINT8    m_BufferData;
	PUINT8    m_CheckPosition;
	ULONG_PTR m_MaximumLength;
	CRITICAL_SECTION m_CriticalSection;
};

