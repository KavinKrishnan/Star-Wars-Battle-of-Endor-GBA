#include "myLib.h"

unsigned short *videoBuffer = (unsigned short *)0x6000000;

void setPixel(int r, int c, unsigned short color)
{
	videoBuffer[OFFSET(r, c, 240)] = color;
}

void drawRect(int row, int col, int height, int width, unsigned short color)
{
	for(int r=0; r<height; r++)
	{
		if (r + row >= 0) {
			DMA[3].src = &color;
			DMA[3].dst = &videoBuffer[OFFSET(row+r, col, 240)];
			DMA[3].cnt = width | DMA_ON | DMA_SOURCE_FIXED;
		}
	}
}

void drawImage3(int x, int y, int width, int height, const unsigned short *image) {

    //int p = 0;   //first element of array
    for (int r = 0; r < height; r++) {
        // for (int c = 0; c < width; c++) {
            // videoBuffer[(y + r) * 240 + (c + x)] = image[p];     //set current pixel to rgb value of array
            // p++;    //get rgb value of next element
			if (y + r >= -1 && (y + r) < 149) {
       DMA[3].src = image + r * width;
       DMA[3].dst = videoBuffer + OFFSET(r + y, x, 240);
       DMA[3].cnt = width | DMA_DESTINATION_INCREMENT | DMA_SOURCE_INCREMENT | DMA_ON;
		 	}
        // }
    }
}

void drawSprite3(int x, int y, int width, int height, const unsigned short *image) {

    int p = 0;   //first element of array
    for (int r = 0; r < height; r++) {
        for (int c = 0; c < width; c++) {
						if (image[p] != BLACK) {
	            setPixel(y + r, c + x, image[p]);
						}    //set current pixel to rgb value of array
	          p++;							//get rgb value of next element
        }
    }
}

void drawChar(int row, int col, char ch, u16 color)
{
	for(int r=0; r<8; r++)
	{
		for(int c=0; c<6; c++)
		{
			if(fontdata_6x8[OFFSET(r, c, 6) + ch*48])
			{
				setPixel(row+r, col+c, color);
			}
		}
	}
}

void drawString(int row, int col, char *str, u16 color)
{
	while(*str)
	{
		drawChar(row, col, *str++, color);
		col += 6;
	}
}


void delay(int n)
{
	volatile int x = 0;
	for(int i=0; i<n*10000; i++)
	{
		x++;
	}
}

void waitForVblank()
{
	while(SCANLINECOUNTER > 160);
	while(SCANLINECOUNTER < 160);
}

void fillScreen(volatile u16 color)
{
	DMA[3].src = &color;
	DMA[3].dst = videoBuffer;
	DMA[3].cnt = 38400 | DMA_ON | DMA_SOURCE_FIXED;
}
