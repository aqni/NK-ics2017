#include <assert.h>
#include <stdlib.h>
#include <ndl.h>

int main() {
  printf("line pass:%s\n",__LINE__);
  NDL_Bitmap *bmp = (NDL_Bitmap*)malloc(sizeof(NDL_Bitmap));
  printf("line pass:%s\n",__LINE__);
  NDL_LoadBitmap(bmp, "/share/pictures/projectn.bmp");
  printf("line pass:%s\n",__LINE__);
  assert(bmp->pixels);
  printf("line pass:%s\n",__LINE__);
  NDL_OpenDisplay(bmp->w, bmp->h);
  printf("line pass:%s\n",__LINE__);
  NDL_DrawRect(bmp->pixels, 0, 0, bmp->w, bmp->h);
  printf("line pass:%s\n",__LINE__);
  NDL_Render();
  printf("line pass:%s\n",__LINE__);
  NDL_CloseDisplay();
  printf("line pass:%s\n",__LINE__);
  while (1);
  return 0;
}
