#ifdef __cplusplus
    #include <cstdlib>
#else
    #include <stdlib.h>
#endif

#include <stdio.h>
#include <ctime>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
#define jpotongan 16

int data[jpotongan]={8, 7, 6, 7, 6, 5, 4, 1, 4, 3, 5, 2, 3, 1, 2, 8};
int buka[jpotongan]={0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int utama(SDL_Event *evt);
SDL_Surface* screenSurface = NULL;
Mix_Chunk *suara=NULL;

struct status{
    unsigned int lpetak=0;
    unsigned int tpetak=0;
    unsigned int tunda=0;
    unsigned int x1=4;
    unsigned int y1=4;
    unsigned int x2=4;
    unsigned int y2=4;
    SDL_Surface* gambar = NULL;
    SDL_Surface* menang = NULL;
}stat;

int tukar(int *a, int i, int j){
int tmp;
    tmp=data[i];
    a[i]=a[j];
    a[j]=tmp;
    return 0;
}

int acak(int *a){
int i, n, total=0;
    for(i=0;i<jpotongan;i++){
        srand((int)time(0));
        n=(rand()+SDL_GetTicks())%jpotongan;
        tukar(a, i, n);
    }
    return total;
}

int terbuka(int *a){
int i, total=0;
    for(i=0;i<jpotongan;i++){
        if(a[i]!=0){
            total++;
        }
    }
    return total;
}

int tampilkan_gambar(SDL_Surface *gambar, int x, int y, int pos){
SDL_Rect rcd, rct;
        rcd.x=pos*stat.lpetak;
        rcd.y=0;
        rcd.w=stat.lpetak;
        rcd.h=stat.tpetak;

        rct.x=x*stat.lpetak;
        rct.y=y*stat.tpetak;
        rct.w=stat.lpetak;
        rct.h=stat.tpetak;

        SDL_BlitSurface(stat.gambar, &rcd, screenSurface, &rct);
        return 0;
}

int tampilkan_semua(SDL_Surface *latar, SDL_Surface *gambar){
int x, y, lebar, tinggi;
SDL_Rect rct, rcd;
int i=0;
    lebar=gambar->w/10;
    tinggi=gambar->h;
    for(y=0;y<4;y++){
        for(x=0;x<4;x++){
            rcd.x=x*lebar;
            rcd.y=y*tinggi;
            rcd.w=lebar;
            rcd.h=tinggi;
            rct.x=data[i]*lebar;
            rct.y=0;
            rct.w=lebar;
            rct.h=tinggi;
            if(buka[i]!=0){
                SDL_BlitSurface(gambar, &rct, latar, &rcd);
            }else{
                rct.x=0;
                SDL_BlitSurface(gambar, &rct, latar, &rcd);
            }
            i++;
        }
    }
    return 0;
}

int main( int argc, char* args[] ){
	SDL_Window *window = NULL;
	SDL_Surface *gambar1=NULL;
	TTF_Font *huruf=NULL;
	SDL_Color warna;
    SDL_Rect rcd;
	unsigned int pos1, pos2;

    warna.r=0xFF;
    warna.g=0xFF;
    warna.b=0x00;

    acak(data);
	if(SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 && IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) <0){
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
	}else{
		window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );

		if( window != NULL ){
            SDL_Event sevent;

			//Get window surface
			Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
			TTF_Init();
			screenSurface = SDL_GetWindowSurface( window );
            SDL_FillRect( screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0x00, 0x00, 0xFF));

            suara=Mix_LoadWAV("efek.wav");
            gambar1=IMG_Load("gambar.png");
            huruf=TTF_OpenFont("FreeSans.ttf", 48);
            if(gambar1!=NULL && suara!=NULL && huruf!=NULL){
                stat.gambar=SDL_ConvertSurface(gambar1, screenSurface->format, 0);
                stat.menang=TTF_RenderText_Solid(huruf, "You Win", warna);
                stat.lpetak=gambar1->w/10;
                stat.tpetak=gambar1->h;

                stat.x1=4;
                stat.y1=4;
                stat.tunda=0;

                SDL_FreeSurface(gambar1);

                tampilkan_semua(screenSurface, stat.gambar);
                while(sevent.type!=SDL_QUIT){
                    while(SDL_PollEvent(&sevent)){
                        if(stat.tunda>0 && stat.tunda<SDL_GetTicks()){
                            stat.tunda=0;
                            pos1=stat.y1*4+stat.x1;
                            pos2=stat.y2*4+stat.x2;
                            if(data[pos1]==data[pos2]){
                                buka[pos1]=1;
                                buka[pos2]=1;
                                if(terbuka(buka)>=jpotongan){
                                    rcd.x=(stat.gambar->w/5)-(stat.menang->w/2);
                                    rcd.y=(stat.gambar->h*2)-(stat.menang->h/2);
                                    rcd.w=stat.menang->w;
                                    rcd.h=stat.menang->h;
                                    SDL_BlitSurface(stat.menang, 0, screenSurface, &rcd);
                                }
                            }else{
                                tampilkan_gambar(stat.gambar, stat.x1, stat.y1, 0);
                                tampilkan_gambar(stat.gambar, stat.x2, stat.y2, 0);
                            }
                            stat.x1=4;
                        }else{
                            utama(&sevent);
                        }
                        SDL_UpdateWindowSurface(window);
                    }
                }

                SDL_FreeSurface(stat.gambar);
                Mix_FreeChunk(suara);
                Mix_CloseAudio();
                IMG_Quit();
                TTF_Quit();
            }
            SDL_DestroyWindow( window );
		}
	}

	//Quit SDL subsystems
	SDL_Quit();

	return 0;
}

int utama(SDL_Event *evt){
unsigned int x, y, pos;
    switch(evt->type){
        case SDL_MOUSEBUTTONDOWN:
            if(stat.tunda==0){
                x=evt->button.x;
                x/=stat.lpetak;

                y=evt->button.y;
                y/=stat.tpetak;

                pos=y*4+x;

                if(x<4 && y<4 && buka[pos]==0){
                    tampilkan_gambar(stat.gambar, x, y, data[pos]);

                    if(stat.x1<4 && stat.y1<4 && pos!=(stat.y1*4+stat.x1)){
                        stat.x2=stat.x1;
                        stat.y2=stat.y1;
                        stat.tunda=SDL_GetTicks()+500;
                        stat.x1=4;
                    }

                    stat.x1=x;
                    stat.y1=y;
                    Mix_Pause(1);
                    Mix_PlayChannel(1, suara, 0);
                }
            }
            break;
        default:
            break;
    }
    return 0;
}
