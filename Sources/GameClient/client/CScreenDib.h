/*-------------------------------------------------------------------
CScreenDib.h
-------------------------------------------------------------------*/
#pragma once

/*-------------------------------------------------------------------
DIB�� ����� GDI�� ��ũ�� ����.
�������� HDC �� DIB �� ����Ͽ� �׸��� ����ش�.

1. ������ : ���� �ػ� �� �÷� ����
2. ������ : ����� �´� �޸� ���� �Ҵ�
3. ������ : DIBȰ���� ���� BITMAPINFOHEADER ����
4. �ܺ� : �޸� ���� ������ ���� Getter �Լ� ���
5. �ܺ� : �׸� ��� (�̹��� ����)
6. �޸� ȭ���� DC�� ���

���� ���������� 4, 5, 6�� �� �����Ӹ��� �ݺ� ����ȴ�.
-------------------------------------------------------------------*/
class CScreenDib
{
public:
	CScreenDib(const int iWidth, const int iHeight, const int iColorBit);
	//----------------------------------------
	// ������ - ��ü ������� �÷� ��Ʈ�� �޴´�.
	// 
	// 
	//----------------------------------------

	virtual ~CScreenDib();

protected:
	void CreateDibBuffer(const int iWidth, const int iHeight, const int iColorBit);
	//----------------------------------------
	// �޸� ���ۿ� DIB�� �����Ѵ�. �����ڿ��� ȣ��
	// 
	// 
	//----------------------------------------


	void ReleaseDibBuffer(void);
	//----------------------------------------
	// ������ �޸� ���۸� �����Ѵ�. �ı��ڿ��� ȣ��
	// 
	// 
	//----------------------------------------

public:
	void Flip(const HWND hWnd, const int iX = 0, const int iY = 0);
	//----------------------------------------
	// �����ʿ��� ���ۿ� �׸��� ��� �׸� �� ȭ������ ��� (FLIP)
	// 
	// 
	//----------------------------------------


	BYTE* GetDibBuffer(void);
	//----------------------------------------
	// �޸� ���� ������ ����
	// 
	// 
	//----------------------------------------


	int GetWidth(void);
	//----------------------------------------
	// �޸� ���� Pixel �ʺ� ����
	// 
	// 
	//----------------------------------------


	int GetHeight(void);
	//----------------------------------------
	// �޸� ���� Pixel ���� ����
	// 
	// 
	//----------------------------------------


	int GetPitch(void);
	//----------------------------------------
	// �޸� ���� Picth ����
	// 
	// 
	//----------------------------------------

private:
	BITMAPINFO	m_stDibInfo;		// DIB Ȱ���� ���� BITMAPINFOHEADER
	BYTE*		m_bypBuffer;		// �޸� ������ ������
	int			m_iWidth;			// �޸� ������ ���� �� �� Pixel �ʺ�	
	int			m_iHeight;			// �޸� ������ ���� �� �� Pixel ����
	int			m_iPitch;			// �޸� ������ ���� �� �� byte ������ ����
	int			m_iColorBit;		// �� ������ �̹��� �÷� Bit
	int			m_iBufferSize;		// �޸� ������ ��ü ������
};