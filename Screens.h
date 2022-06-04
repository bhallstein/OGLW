#ifndef WScreens_h
#define WScreens_h

namespace W {
	int numberOfScreens();
	void getScreenPosition(int screen, int *x, int *y);
	void getScreenSize(int screen, int *w, int *h);
}

#endif
