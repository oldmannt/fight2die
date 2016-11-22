#pragma once

#include "T3DLIB1.h" // game library includes
#include "T3DLIB2.h"
#include "T3DLIB3.h"

const float fTextSpeed = 1.5;

extern BITMAP_IMAGE g_imgBegingBack;	// ���汳��ͼƬ
extern BOB g_bobStory;		// ��Ļ����

// ��ʼ��������ʾ
void StroyInit(void)
{
	// ��ȡ����ͼƬ
	Load_Bitmap_File(&bitmap16bit, "SKYSCROLL24.BMP");
	Create_Bitmap(&g_imgBegingBack,0,0,800,600,16);
	Load_Image_Bitmap16(&g_imgBegingBack, &bitmap16bit,0,0,BITMAP_EXTRACT_MODE_ABS);
	Unload_Bitmap_File(&bitmap16bit);

}// end StoryInit

// ����
void StroyBegin(void)
{
	// draw background
	Draw_Bitmap16(&g_imgBegingBack, back_buffer, back_lpitch,0);
}// end StroyBegin