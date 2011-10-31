#include "wxIncludes.h"
#include "globals.h"

void MakeScreenShot(std::string screenshot_dst)
{
	glReadBuffer(GL_BACK);
	unsigned char *dt0 = new unsigned char [width*height*3];
	unsigned char *dt = new unsigned char [width*height*3];
	glReadPixels(0, 0, width,height, GL_RGB,GL_UNSIGNED_BYTE,dt0);
	for(int i=0;i<height;i++)
		memcpy(dt+(height-i-1)*width*3,dt0+i*width*3,width*3);
	wxImage img(width,height,dt,true);

	wxBitmap bp(img);
	bp.SaveFile(screenshot_dst,wxBITMAP_TYPE_PNG);
	delete[]dt;
	delete[]dt0;
	
}