#pragma once

#include "T3DLIB1.h" // game library includes
#include "T3DLIB2.h"
#include "T3DLIB3.h"

const float fTextSpeed = 1.5;

extern BITMAP_IMAGE g_imgBegingBack;	// 保存背景图片
extern BOB g_bobStory;		// 字幕滚动

// 初始化剧情显示
void StroyInit(void)
{
	// 读取背景图片
	Load_Bitmap_File(&bitmap16bit, "SKYSCROLL24.BMP");
	Create_Bitmap(&g_imgBegingBack,0,0,800,600,16);
	Load_Image_Bitmap16(&g_imgBegingBack, &bitmap16bit,0,0,BITMAP_EXTRACT_MODE_ABS);
	Unload_Bitmap_File(&bitmap16bit);

}// end StoryInit

// 开场
void StroyBegin(void)
{
	// draw background
	Draw_Bitmap16(&g_imgBegingBack, back_buffer, back_lpitch,0);
}// end StroyBegin